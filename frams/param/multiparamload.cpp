// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "multiparamload.h"
#include <frams/util/sstringutils.h>
#include "common/log.h"
#include <ctype.h>

void MultiParamLoader::init()
{
	file = 0; ownfile = 0;
	status = 0;
	reset();
}

void MultiParamLoader::reset()
{
	status = 0;
	breakcond = OnError;
	aborting = false;
	emptyparam.setParamTab(empty_paramtab);
	linenum = 0;
}

int MultiParamLoader::findObject(const ExtObject &o)
{
	for (int i = 0; i < objects.size(); i++)
		if ((*objects(i)) == o)
			return i;
	return -1;
}

void MultiParamLoader::removeObject(const ExtObject &o)
{
	int i = findObject(o);
	if (i >= 0)
	{
		delete objects(i);
		objects -= i;
	}
}

void MultiParamLoader::clearObjects()
{
	FOREACH(ExtObject*, o, objects)
		delete o;
	objects.clear();
}

void MultiParamLoader::load()
{
	clearstack();
	if (!file)
	{
		lasterror = "can't open file";
		status = OnError;
		return;
	}
	status = Loading;
	aborting = false;
}

void MultiParamLoader::abort()
{
	if (file && ownfile)
	{
		delete file;
		file = 0;
	}
	clearstack();
	status = Finished;
	aborting = true;
}

void MultiParamLoader::load(VirtFILE *f)
{
	abort();
	ownfile = 0;
	file = f;
	load();
}

void MultiParamLoader::load(const char* filename)
{
	abort();
	ownfile = 1;
	file = Vfopen(filename, FOPEN_READ_BINARY);
	load();
}

int MultiParamLoader::go()
{
	SString buf;
	if (status == OnError) return status;
	int unexpected_line = 0;
	while (!finished())
	{
		if ((status == BeforeObject) || ((status == BeforeUnknown) && !lastobject.isEmpty()))
		{
			Param tmp_param;
			ParamInterface *pi = lastobject.getParamInterface(tmp_param);
			ParamInterface::LoadOptions opts;
			opts.abortable = &aborting;
			opts.warn_unknown_fields = true;
			opts.linenum = &linenum;
			pi->load(ParamInterface::FormatMultiLine, file, &opts);
			if ((status != Finished) && maybeBreak(AfterObject))
				break;
			unexpected_line = 0;
			continue;
		}
		else if (status == BeforeUnknown)
		{
			logPrintf("MultiParamLoader", "go", LOG_WARN, "Skipping object '%s'", lastunknown.c_str());
			loadObjectNow(&emptyparam, false);
			continue;
		}
		if (!loadSStringLine(file, buf))
		{
			unexpected_line = 0;
			if (!returnFromIncluded())
			{
				abort();
				break;
			}
			else
				continue;
		}
		linenum++;
		if (buf[0] == '#')
		{
			unexpected_line = 0;
			if (buf.startsWith("#include"))
			{
				const char* t = strchr(buf.c_str(), '\"'), *t2 = 0;
				if (t)
					t2 = strchr(t + 1, '\"');
				if (t2)
				{
					SString filename(t + 1, t2 - t - 1);
					includeFile(filename);
				}
				else
				{
					const char* thisfilename = file->VgetPath();
					logPrintf("MultiParamLoader", "go", LOG_WARN, "invalid \"%s\"%s%s", buf.c_str(),
						(thisfilename ? " in " : ""), (thisfilename ? thisfilename : ""));
				}
				continue;
			}
			else if ((status != Finished) && maybeBreak(OnComment))
			{
				lastcomment = buf.substr(1);
				break;
			}
			continue;
		}
		buf = trim(buf);
		if (buf.len() == 0)
			unexpected_line = 0;
		else if ((buf.len() > 1) && (buf[buf.len() - 1] == ':'))
		{
			unexpected_line = 0;
			lastunknown = 0;
			lastunknown = buf.substr(0, buf.len() - 1);
			lastobject.setEmpty();
			FOREACH(ExtObject*, o, objects)
			{
				if (!strcmp(o->interfaceName(), lastunknown.c_str())) { lastobject = *o; break; }
			}
			if (!lastobject.isEmpty())
			{
				if (maybeBreak(BeforeObject))
					break;
			}
			else
			{
				if (maybeBreak(BeforeUnknown))
					break;
			}

		}
		else
		{
			switch (unexpected_line)
			{
			case 0:
			{
				const char* thisfilename = file->VgetPath();
				logPrintf("MultiParamLoader", "go", LOG_WARN, "Ignored unexpected line %d%s",
					linenum,
					thisfilename ? SString::sprintf(" while reading '%s'", thisfilename).c_str() : "");
			}
				break;

			case 1:
				logPrintf("MultiParamLoader", "go", LOG_WARN, "The following line(s) were also unexpected and were ignored");
				break;
			}
			unexpected_line++;
		}
	}
	return status;
}

bool MultiParamLoader::alreadyIncluded(const char* filename)
{
	int i;
	const char* t;
	for (i = 0; i < filestack.size(); i++)
	{
		t = filestack(i)->VgetPath();
		if (!t) continue;
		if (!strcmp(filename, t)) return true;
	}
	return false;
}

void MultiParamLoader::includeFile(SString& filename)
{
	const char* thisfilename = file->VgetPath();
	SString newfilename;
	const char* t = thisfilename ? strrchr(thisfilename, PATH_SEPARATOR_CHAR) : 0;

	if (thisfilename && t)
	{
		newfilename.append(thisfilename, t - thisfilename + 1);
		newfilename += filename;
	}
	else
		newfilename = filename;

	if (alreadyIncluded(newfilename.c_str()))
	{
		logPrintf("MultiParamLoader", "include", LOG_WARN, "circular reference ignored (\"%s\")",
			filename.c_str());
		return;
	}

	VirtFILE *f = Vfopen(newfilename.c_str(), FOPEN_READ_BINARY);
	if (!f)
	{
		logPrintf("MultiParamLoader", "include", LOG_WARN, "\"%s\" not found", newfilename.c_str());
	}
	else
	{
		filestack += file;
		file = f;
	}
}

VirtFILE* MultiParamLoader::popstack()
{
	if (!filestack.size()) return 0;
	VirtFILE* f = filestack(filestack.size() - 1);
	filestack.remove(filestack.size() - 1);
	return f;
}

void MultiParamLoader::clearstack()
{
	VirtFILE *f;
	while (f = popstack()) delete f;
}

bool MultiParamLoader::returnFromIncluded()
{
	if (!filestack.size()) return false;
	if (file) delete file;
	file = popstack();
	return true;
}

int MultiParamLoader::loadObjectNow(const ExtObject& o, bool warn_unknown_fields)
{
	Param tmp_param;
	ParamInterface *pi = o.getParamInterface(tmp_param);
	ParamInterface::LoadOptions opts;
	opts.abortable = &aborting;
	opts.warn_unknown_fields = warn_unknown_fields;
	opts.linenum = &linenum;
	pi->load(ParamInterface::FormatMultiLine, file, &opts);
	status = AfterObject;
	return 0;
}

int MultiParamLoader::run()
{
	int stat;
	breakOn(OnError);
	while (stat = go())
		if (stat == OnError)
		{
		abort();
		return 0;
		}
	return 1;
}

SString MultiParamLoader::currentFilePathForErrorMessage()
{
	const char* filename = getFile()->VgetPath();
	if (filename)
		return SString::sprintf(" in '%s'", filename);
	return SString::empty();
}
