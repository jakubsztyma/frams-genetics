// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <stdio.h>
#include <ctype.h>

#include "param.h"
#include <frams/util/extvalue.h>
#include "common/log.h"
#include <frams/util/sstringutils.h>
#include <common/virtfile/stringfile.h>

#ifdef _DEBUG
//for sanityCheck - mutable param detection
#include "mutparamiface.h"
#endif

//#define SAVE_ALL_NAMES
#define SAVE_SELECTED_NAMES
#define WARN_MISSING_NAME

char MakeCodeGuardHappy;

ParamEntry empty_paramtab[] =
{ { "Empty", 1, 0, "Empty", }, { 0, 0, 0, }, };

/** return: true if tilde was found, false if finished at EOF */
static bool readUntilTilde(VirtFILE *f, SString &s)
{
	SString temp;
	int z;
	char last_char = 0;
	bool tilde_found = false;
	while ((z = f->Vgetc()) != EOF)
	{
		if (z == '~')
			if (last_char != '\\') { tilde_found = true; break; }
		last_char = (char)z;
		temp += last_char;
	}
	s = temp;
	return tilde_found;
}

static const char *strchrlimit(const char *t, int ch, const char *limit)
{
	if (limit < t) return NULL;
	return (const char*)memchr((const void*)t, ch, limit - t);
}

void ParamInterface::copyFrom(ParamInterface *src)
{
	int n = getPropCount();
	ExtValue v;
	int j;
	for (int i = 0; i < n; i++)
		if ((!(flags(i)&PARAM_READONLY))
			&& (*type(i) != 'p'))
		{
			j = src->findId(id(i));
			if (j < 0) continue;
			src->get(j, v);
			set(i, v);
		}
}

void ParamInterface::quickCopyFrom(ParamInterface *src)
{
	int n = getPropCount();
	ExtValue v;
	for (int i = 0; i < n; i++)
		if ((!(flags(i)&PARAM_READONLY))
			&& (*type(i) != 'p'))
		{
			src->get(i, v);
			set(i, v);
		}
}

int ParamInterface::getMinMaxInt(int prop, paInt& minumum, paInt& maximum, paInt &def)
{
	return getMinMaxIntFromTypeDef(type(prop), minumum, maximum, def);
}

int ParamInterface::getMinMaxDouble(int prop, double& minumum, double& maximum, double& def)
{
	return getMinMaxDoubleFromTypeDef(type(prop), minumum, maximum, def);
}

int ParamInterface::getMinMaxString(int prop, int& minumum, int& maximum, SString& def)
{
	return getMinMaxStringFromTypeDef(type(prop), minumum, maximum, def);
}

int ParamInterface::getMinMaxIntFromTypeDef(const char* t, paInt& minumum, paInt& maximum, paInt &def)
{
	while (*t) if (*t == ' ') break; else t++;
	return sscanf(t, PA_INT_SCANF " " PA_INT_SCANF " " PA_INT_SCANF, &minumum, &maximum, &def);
}

int ParamInterface::getMinMaxDoubleFromTypeDef(const char* t, double& minumum, double& maximum, double& def)
{
	while (*t) if (*t == ' ') break; else t++;
	return sscanf(t, "%lg %lg %lg", &minumum, &maximum, &def);
}

int ParamInterface::getMinMaxStringFromTypeDef(const char* t, int& minumum, int& maximum, SString& def)
{
	while (*t) if (*t == ' ') break; else t++;
	int ret = sscanf(t, "%d %d", &minumum, &maximum);
	def = SString::empty();
	if (ret == 2)
	{
		while (*t == ' ') t++;
		for (int skip_fields = 2; skip_fields > 0; skip_fields--)
		{
			while (*t) if (*t == ' ') break; else t++;
			while (*t == ' ') t++;
		}
		if (*t)
		{
			const char* end = strchr(t, '~');
			if (!end)
				end = t + strlen(t);
			while ((end > t) && (end[-1] == ' ')) end--;
			def = SString(t, end - t);
		}
		return 3;
	}
	else
		return ret;
}

void ParamInterface::setDefault()
{
	for (int i = 0; i < getPropCount(); i++)
		setDefault(i);
}

void ParamInterface::setMin()
{
	for (int i = 0; i < getPropCount(); i++)
		setMin(i);
}

void ParamInterface::setMax()
{
	for (int i = 0; i < getPropCount(); i++)
		setMax(i);
}

void ParamInterface::setDefault(int i)
{
	const char *t = type(i);
	switch (*t)
	{
	case 'f':
	{
		double mn = 0, mx = 0, def = 0;
		if (getMinMaxDoubleFromTypeDef(t, mn, mx, def) < 3) def = mn;
		setDouble(i, def);
	}
	break;
	case 'd':
	{
		paInt mn = 0, mx = 0, def = 0;
		if (getMinMaxIntFromTypeDef(t, mn, mx, def) < 3) def = mn;
		setInt(i, def);
	}
	break;
	case 's': case 'x':
	{
		int mn, mx; SString def;
		getMinMaxStringFromTypeDef(t, mn, mx, def);
		if (*t == 's')
			setString(i, def);
		else
		{
			if (def.len() > 0) setExtValue(i, ExtValue(def)); else setExtValue(i, ExtValue::empty());
		}
	}
	break;
	case 'o':
		setObject(i, ExtObject::empty());
		break;
	}
}

void ParamInterface::setMin(int i)
{
	const char *t = type(i);
	switch (*t)
	{
	case 'f':
	{
		double mn = 0, mx = 0, def = 0;
		getMinMaxDoubleFromTypeDef(t, mn, mx, def);
		setDouble(i, mn);
	}
	break;
	case 'd':
	{
		paInt mn = 0, mx = 0, def = 0;
		getMinMaxIntFromTypeDef(t, mn, mx, def);
		setInt(i, mn);
	}
	break;
	default: setFromString(i, "", false);
	}
}

void ParamInterface::setMax(int i)
{
	const char *t = type(i);
	switch (*t)
	{
	case 'f':
	{
		double mn = 0, mx = 0, def = 0;
		getMinMaxDoubleFromTypeDef(t, mn, mx, def);
		setDouble(i, mx);
	}
	break;
	case 'd':
	{
		paInt mn = 0, mx = 0, def = 0;
		getMinMaxIntFromTypeDef(t, mn, mx, def);
		setInt(i, mx);
	}
	break;
	default: setFromString(i, "", false);
	}
}

SString ParamInterface::getStringById(const char*prop)
{
	int i = findId(prop); if (i >= 0) return getString(i); else return SString();
}
paInt ParamInterface::getIntById(const char*prop)
{
	int i = findId(prop); if (i >= 0) return getInt(i); else return 0;
}
double ParamInterface::getDoubleById(const char*prop)
{
	int i = findId(prop); if (i >= 0) return getDouble(i); else return 0;
}
ExtObject ParamInterface::getObjectById(const char*prop)
{
	int i = findId(prop); if (i >= 0) return getObject(i); else return ExtObject();
}
ExtValue ParamInterface::getExtValueById(const char*prop)
{
	int i = findId(prop); if (i >= 0) return getExtValue(i); else return ExtValue();
}

int ParamInterface::setIntById(const char* prop, paInt v)
{
	int i = findId(prop); if (i >= 0) return setInt(i, v); else return PSET_NOPROPERTY;
}
int ParamInterface::setDoubleById(const char* prop, double v)
{
	int i = findId(prop); if (i >= 0) return setDouble(i, v); else return PSET_NOPROPERTY;
}
int ParamInterface::setStringById(const char* prop, const SString &v)
{
	int i = findId(prop); if (i >= 0) return setString(i, v); else return PSET_NOPROPERTY;
}
int ParamInterface::setObjectById(const char* prop, const ExtObject &v)
{
	int i = findId(prop); if (i >= 0) return setObject(i, v); else return PSET_NOPROPERTY;
}
int ParamInterface::setExtValueById(const char* prop, const ExtValue &v)
{
	int i = findId(prop); if (i >= 0) return setExtValue(i, v); else return PSET_NOPROPERTY;
}
int ParamInterface::setById(const char* prop, const ExtValue &v)
{
	int i = findId(prop); if (i >= 0) return set(i, v); else return PSET_NOPROPERTY;
}

int ParamInterface::saveMultiLine(VirtFILE* f, const char* altname, bool force)
{
	const char *p;
	SString ws;
	int err = 0, i;
	bool withname = false;
	if ((altname == NULL) || (altname[0] != 0))
	{
		err |= (f->Vputs(altname ? altname : getName()) == EOF);
		err |= (f->Vputs(":\n") == EOF);
		withname = true;
	}
	for (i = 0; p = id(i); i++)
		err |= saveprop(f, i, p, force);
	if (withname)
		err |= (f->Vputs("\n") == EOF);
	return err;
}

const char* ParamInterface::SERIALIZATION_PREFIX = "@Serialized:";

int ParamInterface::saveprop(VirtFILE* f, int i, const char* p, bool force)
{
	if ((flags(i)&PARAM_DONTSAVE) && (!force)) return 0;
	const char *typ = type(i);
	if (*typ == 'p') return 0;

	const char *t, *w;
	SString ws;
	int err = 0, cr;

	err |= (f->Vputs(p) == EOF); f->Vputc(':');
	cr = 0;
	if ((*typ == 'x') || (*typ == 'o'))
	{
		ExtValue ex;
		get(i, ex);
		ws = SString(SERIALIZATION_PREFIX) + ex.serialize(NativeSerialization);
	}
	else
		ws = get(i);
	quoteTilde(ws);
	w = ws.c_str();
	if (ws.len() > 50) cr = 1;
	else for (t = w; *t; t++) if ((*t == 10) || (*t == 13)) { cr = 1; break; }
	if (cr) f->Vputs("~\n");
	err |= (f->Vputs(w) == EOF);
	err |= (f->Vputs(cr ? "~\n" : "\n") == EOF);
	return err;
}


int SimpleAbstractParam::isequal(int i, void* defdata)
{ // defdata->member == object->member ?
	void *backup = object;
	switch (type(i)[0])
	{
	case 'd':
	{
		select(defdata);
		paInt x = getInt(i);
		select(backup);
		return x == getInt(i);
	}
	case 'f':
	{
		select(defdata);
		double x = getDouble(i);
		select(backup);
		return x == getDouble(i);
	}
	case 's':
	{
		select(defdata);
		SString x = getString(i);
		select(backup);
		return x == getString(i);
	}
	}
	return 1;
}

void SimpleAbstractParam::saveSingleLine(SString& f, void *defdata, bool addcr, bool all_names)
{ // defdata!=NULL -> does not save default values
	const char *p;
	int i;
	int needlabel = 0;
	int first = 1;
	SString val;
	SString t;
	int fl;
	// t+=SString(getName()); t+=':';
	for (i = 0; p = id(i); i++)
		if (!((fl = flags(i))&PARAM_DONTSAVE))
		{
			if (defdata && isequal(i, defdata))
				needlabel = 1;
			else
			{
				if (!first) t += ", ";
#ifndef SAVE_ALL_NAMES
#ifdef SAVE_SELECTED_NAMES
				if (needlabel || all_names || !(fl & PARAM_CANOMITNAME))
#else
				if (needlabel)
#endif
#endif
				{
					t += p; t += "="; needlabel = 0;
				}
				if (type(i)[0] == 's')
				{ // string - special case
					SString str = getString(i);
					if (strContainsOneOf(str.c_str(), ", \\\n\r\t\""))
					{
						t += "\"";
						sstringQuote(str);
						t += str;
						t += "\"";
					}
					else
						t += str;
				}
				else
					t += get(i);
				first = 0;
			}
		}
	if (addcr)
		t += "\n";
	f += t;
}

static void closingTildeError(ParamInterface *pi, VirtFILE *file, int field_index)
{
	SString fileinfo;
	const char* fname = file->VgetPath();
	if (fname != NULL)
		fileinfo = SString::sprintf(" while reading from '%s'", fname);
	SString field;
	if (field_index >= 0)
		field = SString::sprintf("'%s.%s'", pi->getName(), pi->id(field_index));
	else
		field = SString::sprintf("unknown property of '%s'", pi->getName());
	logPrintf("ParamInterface", "load", LOG_WARN, "Closing '~' (tilde) not found in %s%s", field.c_str(), fileinfo.c_str());
}

template<typename T> void messageOnExceedRange(SimpleAbstractParam *pi, int i, int setflags, T valuetoset) ///< prints a warning when setflags indicates that allowed param range has been exceeded during set
{
	if (setflags & (PSET_HITMIN | PSET_HITMAX))
	{
		ExtValue v(valuetoset);
		pi->messageOnExceedRange(i, setflags, v);
	}
}

void SimpleAbstractParam::messageOnExceedRange(int i, int setflags, ExtValue& valuetoset) ///< prints a warning when setflags indicates that allowed param range has been exceeded during set
{
	if (setflags & (PSET_HITMIN | PSET_HITMAX))
	{
		SString svaluetoset = valuetoset.getString(); //converts any type to SString
		SString actual = get(i);
		bool s_type = type(i)[0] == 's';
		bool show_length = valuetoset.getType() == TString;
		const char* quote = (valuetoset.getType() == TString) ? "\"" : "'";
		logPrintf("Param", "set", LOG_WARN, "Setting %s.%s = %s exceeded allowed range (too %s). %s to %s.",
			getName(), id(i),
			::sstringDelimitAndShorten(svaluetoset, 30, show_length, quote, quote).c_str(),
			(setflags&PSET_HITMAX) ? (s_type ? "long" : "big") : "small", s_type ? "Truncated" : "Adjusted",
			::sstringDelimitAndShorten(actual, 30, show_length, quote, quote).c_str()
		);
	}
}

int ParamInterface::load(FileFormat format, VirtFILE* f, LoadOptions *options)
{
	LoadOptions default_options;
	if (options == NULL)
		options = &default_options;
	switch (format)
	{
	case FormatMultiLine:
		return loadMultiLine(f, *options);

	case FormatSingleLine:
	{
		StringFILE *sf = dynamic_cast<StringFILE*>(f);
		SString s;
		if (sf)
		{
			s = sf->getString().c_str();
			options->offset += sf->Vtell();
		}
		else
		{
			if (!loadSStringLine(f, s))
				return -1;
		}
		return loadSingleLine(s, *options);
	}
	}
	return -1;
}

int ParamInterface::load(FileFormat format, const SString &s, LoadOptions *options)
{
	LoadOptions default_options;
	if (options == NULL)
		options = &default_options;
	switch (format)
	{
	case FormatMultiLine:
	{
		string std_string(s.c_str());
		StringFILE f(std_string);
		return loadMultiLine(&f, *options);
	}

	case FormatSingleLine:
		return loadSingleLine(s, *options);
	}
	return -1;
}

int ParamInterface::loadMultiLine(VirtFILE* f, LoadOptions &options)
{
	SString buf;
	int i;
	const char *p, *p0;
	int p_len;
	bool loaded;
	int fields_loaded = 0;
	int unexpected_line = 0;
	vector<bool> seen;
	seen.resize(getPropCount());
	if ((i = findId("beforeLoad")) >= 0)
		call(i, NULL, NULL);
	while (((!options.abortable) || (!*options.abortable)) && loadSStringLine(f, buf))
	{
		if (options.linenum) (*options.linenum)++;
		const char* t = buf.c_str();
		p0 = t; while (isblank(*p0)) p0++;
		if (!*p0) break;
		if (p0[0] == '#') { unexpected_line = 0; continue; }
		p = strchr(p0, ':');
		if (!p)
		{
			switch (unexpected_line)
			{
			case 0:
				logPrintf("ParamInterface", "load", LOG_WARN, "Ignored unexpected line %s while reading object '%s'",
					options.linenum ?
					SString::sprintf("%d", *options.linenum).c_str()
					: SString::sprintf("'%s'", p0).c_str(),
					getName());
				break;
			case 1:
				logPrintf("ParamInterface", "load", LOG_WARN, "The following line(s) were also unexpected and were ignored");
				break;
			}
			unexpected_line++;
			continue;
		}
		unexpected_line = 0;
		p_len = (int)(p - p0);
		loaded = false;
		if (p_len && ((i = findIdn(p0, p_len)) >= 0))
		{
			if (seen[i])
			{
				SString fileinfo;
				const char* fname = f->VgetPath();
				if (fname != NULL)
				{
					fileinfo = SString::sprintf(" while reading from '%s'", fname);
					if (options.linenum)
						fileinfo += SString::sprintf(" (line %d)", *options.linenum);
				}
				logPrintf("ParamInterface", "load", LOG_WARN, "Multiple '%s.%s' properties found%s", getName(), id(i), fileinfo.c_str());
			}
			else
				seen[i] = true;
			if (!(flags(i)&PARAM_DONTLOAD))
			{
				if (p0[p_len + 1] == '~')
				{
					SString s;
					if (!readUntilTilde(f, s))
						closingTildeError(this, f, i);
					int lfcount = 1;
					const char* tmp = s.c_str();
					while (tmp)
						if ((tmp = strchr(tmp, '\n')))
						{
							lfcount++; tmp++;
						}
					removeCR(s);
					int ch; while ((ch = f->Vgetc()) != EOF) if (ch == '\n') break;
					unquoteTilde(s);
					if (options.linenum && (flags(i)&PARAM_LINECOMMENT))
						s = SString::sprintf("@file %s\n@line %d\n", f->VgetPath(), *options.linenum + 1) + s;
					setFromString(i, s.c_str(), false);
					if (options.linenum)
						(*options.linenum) += lfcount;
				}
				else
				{
					SString s = SString(p0 + p_len + 1);
					unquoteTilde(s);
					setFromString(i, s.c_str(), false);
				}
				fields_loaded++;
				loaded = true;
			}
		}
		else if (options.warn_unknown_fields)
		{
			SString name(p0, p_len);
			logPrintf("ParamInterface", "load", LOG_WARN, "Ignored unknown property '%s.%s'", getName(), name.c_str());
		}

		if ((!loaded) && (p0[p_len + 1] == '~'))
		{ // eat unrecognized multiline field
			SString s;
			if (!readUntilTilde(f, s))
				closingTildeError(this, f, -1);
			if (options.linenum)
			{
				const char* tmp = s.c_str();
				int lfcount = 1;
				while (tmp)
					if ((tmp = strchr(tmp, '\n')))
					{
						lfcount++; tmp++;
					}
				(*options.linenum) += lfcount;
			}
			int ch; while ((ch = f->Vgetc()) != EOF) if (ch == '\n') break;
		}
	}
	if ((i = findId("afterLoad")) >= 0)
		call(i, NULL, NULL);
	return fields_loaded;
}


/*
SString SimpleAbstractParam::getString(int i)
{
char *t;
switch (*(t=type(i)))
{
case 'd':
{
for (i=atol(get(i));i>=0;i--) if (t) t=strchr(t+1,'~');
if (t)
{
t++;
char *t2=strchr(t,'~');
if (!t2) t2=t+strlen(t);
SString str;
strncpy(str.directWrite(t2-t),t,t2-t);
str.endWrite(t2-t);
return str;
}
}
}
return get(i);
}
*/

int ParamInterface::findId(const char* n)
{
	int i; const char *p;
	for (i = 0; p = id(i); i++) if (!strcmp(n, p)) return i;
	return -1;
}

int ParamInterface::findIdn(const char* naz, int n)
{
	int i; const char *p;
	for (i = 0; p = id(i); i++) if ((!strncmp(naz, p, n)) && (!p[n])) return i;
	return -1;
}

void ParamInterface::get(int i, ExtValue &ret)
{
	switch (type(i)[0])
	{
	case 'd':	ret.setInt(getInt(i)); break;
	case 'f':	ret.setDouble(getDouble(i)); break;
	case 's':	ret.setString(getString(i)); break;
	case 'o':	ret.setObject(getObject(i)); break;
	case 'x':	ret = getExtValue(i); break;
	default: logPrintf("ParamInterface", "get", LOG_ERROR, "'%s.%s' is not a property", getName(), id(i));
	}
}

int ParamInterface::setIntFromString(int i, const char* str, bool strict)
{
	paInt value;
	if (!ExtValue::parseInt(str, value, strict, true))
	{
		paInt mn, mx, def;
		if (getMinMaxInt(i, mn, mx, def) >= 3)
			return setInt(i, def) | PSET_PARSEFAILED;
		else
			return setInt(i, (paInt)0) | PSET_PARSEFAILED;
	}
	else
		return setInt(i, value);
}

int ParamInterface::setDoubleFromString(int i, const char* str)
{
	double value;
	if (!ExtValue::parseDouble(str, value, true))
	{
		double mn, mx, def;
		if (getMinMaxDouble(i, mn, mx, def) >= 3)
			return setDouble(i, def) | PSET_PARSEFAILED;
		else
			return setDouble(i, (double)0) | PSET_PARSEFAILED;
	}
	else
		return setDouble(i, value);
}

int ParamInterface::set(int i, const ExtValue &v)
{
	switch (type(i)[0])
	{
	case 'd':
		if ((v.type == TInt) || (v.type == TDouble)) return setInt(i, v.getInt());
		else
		{
			if (v.type == TObj)
			{
				logPrintf("ParamInterface", "set", LOG_ERROR, "Setting int '%s.%s' from object reference (%s)", getName(), id(i), v.getString().c_str());
				return 0;
			}
			else
				return setIntFromString(i, v.getString().c_str(), false);
		}
	case 'f':
		if ((v.type == TInt) || (v.type == TDouble)) return setDouble(i, v.getDouble());
		else
		{
			if (v.type == TObj)
			{
				logPrintf("ParamInterface", "set", LOG_ERROR, "Setting float '%s.%s' from object reference (%s)", getName(), id(i), v.getString().c_str());
				return 0;
			}
			else
				return setDoubleFromString(i, v.getString().c_str());
		}
	case 's': { SString t = v.getString(); return setString(i, t); }
	case 'o':
		if ((v.type != TUnknown) && (v.type != TObj))
			logPrintf("ParamInterface", "set", LOG_ERROR, "Setting object '%s.%s' from %s", getName(), id(i), v.typeAndValue().c_str());
		else
			return setObject(i, v.getObject());
		break;
	case 'x': return setExtValue(i, v);
	default: logPrintf("ParamInterface", "set", LOG_ERROR, "'%s.%s' is not a property", getName(), id(i));
	}
	return 0;
}

int ParamInterface::setFromString(int i, const char *v, bool strict)
{
	char typ = type(i)[0];
	switch (typ)
	{
	case 'd': return setIntFromString(i, v, strict);
	case 'f': return setDoubleFromString(i, v);
	case 's': { SString t(v); return setString(i, t); }
	case 'x': case 'o':
	{
		ExtValue e;
		const char* after;
		if (!strncmp(v, SERIALIZATION_PREFIX, strlen(SERIALIZATION_PREFIX)))
		{
			after = e.deserialize(v + strlen(SERIALIZATION_PREFIX));
			if ((after == NULL) || (*after))
			{
				logPrintf("ParamInterface", "set", LOG_ERROR, "serialization format mismatch in %s.%s", (getName() ? getName() : "<Unknown>"), id(i));
				e.setEmpty();
			}
		}
		else if ((after = e.parseNumber(v)) && (*after == 0)) //consumed the whole string
		{
			//OK!
		}
		else
		{
			e.setString(SString(v));
		}
		if (typ == 'x')
			return setExtValue(i, e);
		else
			return setObject(i, e.getObject());
	}
	}
	return 0;
}

SString ParamInterface::getText(int i) //find the current enum text or call get(i) if not enum
{
	const char *t;
	if (((*(t = type(i))) == 'd') && (strchr(t, '~') != NULL)) //type is int and contains enum labels
	{
		paInt mn, mx, def;
		int value = getInt(i);
		if (getMinMaxIntFromTypeDef(t, mn, mx, def) >= 2)
		{
			if (value > mx)
				return get(i);//unexpected value of out bounds (should never happen) -> fallback
			value -= mn;
		}
		if (value < 0) return get(i); //unexpected value of out bounds (should never happen) -> fallback
		// now value is 0-based index of ~text
		for (; value >= 0; value--) if (t) t = strchr(t + 1, '~'); else break;
		if (t) // found n-th ~text in type description (else: not enough ~texts in type description)
		{
			t++;
			const char *t2 = strchr(t, '~');
			if (!t2) t2 = t + strlen(t);
			return SString(t, (int)(t2 - t));
		}
	}
	return get(i); //fallback - return int value as string
}

SString ParamInterface::get(int i)
{
	switch (type(i)[0])
	{
	case 'd': return SString::valueOf(getInt(i));
	case 'f': return SString::valueOf(getDouble(i));
	case 's': return getString(i);
	}
	ExtValue v;
	get(i, v);
	return v.getString();
}

bool ParamInterface::isValidTypeDescription(const char* t)
{
	if (t == NULL) return false;
	if (*t == 0) return false;
	if (strchr("dfsoxp", *t) == NULL) return false;
	switch (*t)
	{
	case 'd':
	{
		paInt a, b, c;
		int have = getMinMaxIntFromTypeDef(t, a, b, c);
		if (have == 1) return false;
		if ((have >= 2) && (b < a) && (a != 0) && (b != -1)) return false; // max<min meaning 'undefined' is only allowed as "d 0 -1"
	}
	break;
	case 'f':
	{
		double a, b, c;
		int have = getMinMaxDoubleFromTypeDef(t, a, b, c);
		if (have == 1) return false;
		if ((have >= 2) && (b < a) && (a != 0) && (b != -1)) return false; // max<min meaning 'undefined' is only allowed as "f 0 -1"
	}
	break;
	case 's':
	{
		int a, b; SString c;
		int have = getMinMaxStringFromTypeDef(t, a, b, c);
		//if (have == 1) return false; //not sure?
		if ((have >= 1) && (!((a == 0) || (a == 1)))) return false; // 'min' for string (single/multi) can be only 0 or 1
		if ((have >= 2) && (b < 0)) return false; // max=0 means unlimited, max<0 is not allowed
	}
	break;
	}
	return true;
}

SString ParamInterface::friendlyTypeDescrFromTypeDef(const char* type)
{
	SString t;
	switch (type[0])
	{
	case 'd': t += "integer";
	{paInt a, b, c; int n = getMinMaxIntFromTypeDef(type, a, b, c); if ((n >= 2) && (b >= a)) t += SString::sprintf(" %d..%d", a, b); if (n >= 3) t += SString::sprintf(" (default %d)", c); }
	break;
	case 'f': t += "float";
	{double a, b, c; int n = getMinMaxDoubleFromTypeDef(type, a, b, c); if ((n >= 2) && (b >= a)) t += SString::sprintf(" %g..%g", a, b); if (n >= 3) t += SString::sprintf(" (default %g)", c); }
	break;
	case 's': t += "string";
	{int a, b; SString c; int n = getMinMaxStringFromTypeDef(type, a, b, c); if ((n >= 2) && (b > 0)) t += SString::sprintf(", max %d chars", b); if (n >= 3) t += SString::sprintf(" (default \"%s\")", c.c_str()); }
	break;
	case 'x': t += "untyped value"; break;
	case 'p': t += "function"; break;
	case 'o': t += "object"; if (type[1]) { t += " of class "; t += type + 1; } break;
	default: return "unknown type";
	}
	return t;
}

//////////////////////////////// PARAM ////////////////////////////////////

#ifdef _DEBUG
void SimpleAbstractParam::sanityCheck(int i)
{
	ParamEntry *pe = entry(i);

	const char* t = pe->type;
	const char* err = NULL;

	if (!isValidTypeDescription(t))
		err = "invalid type description";
	if (*t == 'p')
	{
		if (pe->fun1 == NULL)
		{
			MutableParamInterface *mpi = dynamic_cast<MutableParamInterface*>(this);
			if (mpi == NULL) // Avoid false positives for script-driven mutable params, like expdefs. This can't be reliably verified. Function pointer checking is meant for static param tables anyway so it's probably not a big deal.
				err = "no procedure defined";
		}
		if (pe->flags & PARAM_READONLY)
			err = "function can't be PARAM_READONLY";
	}
	else
	{
		if ((t[0] == 'o') && (t[1] == ' '))
		{
			err = "space after 'o'";
		}
		if (!(pe->flags & (PARAM_READONLY | PARAM_DONTSAVE | PARAM_USERREADONLY | PARAM_CONST | PARAM_DONTLOAD | PARAM_LINECOMMENT | PARAM_OBJECTSET)))
		{ //write access
			if ((pe->fun2 == NULL) && (pe->offset == PARAM_ILLEGAL_OFFSET))
				err = "no field defined (GETONLY without PARAM_READONLY?)";
		}
	}
	if (err != NULL)
		logPrintf("SimpleAbstractParam", "sanityCheck", LOG_ERROR,
			"Invalid ParamEntry for %s.%s (%s)", getName(), pe->id, err);
}
#endif

void *SimpleAbstractParam::getTarget(int i)
{
	return (void*)(((char*)object) + entry(i)->offset);
	//return &(object->*(entry(i)->fldptr));
}

///////// get

#ifdef _DEBUG
#define SANITY_CHECK(i) sanityCheck(i)
#else
#define SANITY_CHECK(i) 
#endif

paInt SimpleAbstractParam::getInt(int i)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->fun1)
	{
		(*(void(*)(void*, ExtValue*))pe->fun1)(object, &v);
		return v.getInt();
	}
	else
	{
		void *target = getTarget(i);
		return *((paInt*)target);
	}
}

double SimpleAbstractParam::getDouble(int i)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->fun1)
	{
		(*(void(*)(void*, ExtValue*))pe->fun1)(object, &v);
		return v.getDouble();
	}
	else
	{
		void *target = getTarget(i);
		return *((double*)target);
	}
}

SString SimpleAbstractParam::getString(int i)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->fun1)
	{
		(*(void(*)(void*, ExtValue*))pe->fun1)(object, &v);
		return v.getString();
	}
	else
	{
		void *target = getTarget(i);
		return *((SString*)target);
	}
}

ExtObject SimpleAbstractParam::getObject(int i)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->fun1)
	{
		(*(void(*)(void*, ExtValue*))pe->fun1)(object, &v);
		return v.getObject();
	}
	else
	{
		void *target = getTarget(i);
		return *((ExtObject*)target);
	}
}

ExtValue SimpleAbstractParam::getExtValue(int i)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->fun1)
	{
		(*(void(*)(void*, ExtValue*))pe->fun1)(object, &v);
		return v;
	}
	else
	{
		void *target = getTarget(i);
		return *((ExtValue*)target);
	}
}


//////// set

int SimpleAbstractParam::setInt(int i, paInt x)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->flags&PARAM_READONLY) return PSET_RONLY;
	paInt xcopy = x; //only needed for messageOnExceedRange(): retain original, requested value of x because it may be changed below
	paInt mn = 0, mx = 0, de = 0;
	int result = 0;
	if (getMinMaxIntFromTypeDef(pe->type, mn, mx, de) >= 2)
		if (mn <= mx) // else if mn>mx then the min/max constraint makes no sense and there is no checking
		{
			if (x < mn) { x = mn; result = PSET_HITMIN; }
			else if (x > mx) { x = mx; result = PSET_HITMAX; }
		}

	if (pe->fun2)
	{
		v.setInt(x);
		result |= (*(int(*)(void*, const ExtValue*))pe->fun2)(object, &v);
	}
	else
	{
		void *target = getTarget(i);
		if (dontcheckchanges || (*((paInt*)target) != x))
		{
			result |= PSET_CHANGED;
			*((paInt*)target) = x;
		}
	}
	::messageOnExceedRange(this, i, result, xcopy);
	return result;
}

int SimpleAbstractParam::setDouble(int i, double x)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->flags&PARAM_READONLY) return PSET_RONLY;
	double xcopy = x; //only needed for messageOnExceedRange(): retain original, requested value of x because it may be changed below
	double mn = 0, mx = 0, de = 0;
	int result = 0;
	if (getMinMaxDoubleFromTypeDef(pe->type, mn, mx, de) >= 2)
		if (mn <= mx) // else if mn>mx then the min/max constraint makes no sense and there is no checking
		{
			if (x < mn) { x = mn; result = PSET_HITMIN; }
			else if (x > mx) { x = mx; result = PSET_HITMAX; }
		}

	if (pe->fun2)
	{
		v.setDouble(x);
		result |= (*(int(*)(void*, const ExtValue*))pe->fun2)(object, &v);
	}
	else
	{
		void *target = getTarget(i);
		if (dontcheckchanges || (*((double*)target) != x))
		{
			result |= PSET_CHANGED;
			*((double*)target) = x;
		}
	}
	::messageOnExceedRange(this, i, result, xcopy);
	return result;
}

int SimpleAbstractParam::setString(int i, const SString& x)
{
	SANITY_CHECK(i);
	ExtValue v;
	SString vs;
	const SString *xx = &x;
	ParamEntry *pe = entry(i);
	if (pe->flags&PARAM_READONLY) return PSET_RONLY;
	SString xcopy = x; //only needed for messageOnExceedRange(): retain original, requested value of x because it may be changed below
	const char* t = pe->type + 1;
	while (*t) if (*t == ' ') break; else t++;
	int mn = 0, mx = 0;
	int result = 0;
	if (sscanf(t, "%d %d", &mn, &mx) == 2) //using getMinMax would also get default value, which is not needed here
	{
		if ((x.len() > mx) && (mx > 0))
		{
			vs = x.substr(0, mx);
			xx = &vs;
			result |= PSET_HITMAX;
		}
	}

	if (pe->fun2)
	{
		v.setString(*xx);
		result |= (*(int(*)(void*, const ExtValue*))pe->fun2)(object, &v);
	}
	else
	{
		void *target = getTarget(i);
		if (dontcheckchanges || (!(*((SString*)target) == *xx)))
		{
			result |= PSET_CHANGED;
			*((SString*)target) = *xx;
		}
	}
	::messageOnExceedRange(this, i, result, xcopy);
	return result;
}

int SimpleAbstractParam::setObject(int i, const ExtObject& x)
{
	SANITY_CHECK(i);
	ExtValue v;
	ParamEntry *pe = entry(i);
	if (pe->flags&PARAM_READONLY) return PSET_RONLY;
	if (pe->flags&PARAM_OBJECTSET)
	{
		ExtObject o = getObject(i);
		Param tmp;
		ParamInterface* oif = o.getParamInterface(tmp);
		int ass;
		if (oif && ((ass = oif->findId("assign")) >= 0))
		{
			ExtValue arg = x;
			oif->call(ass, &arg, &v);
		}
		else
			logPrintf("SimpleAbstractParam", "setObject", LOG_ERROR,
				"'%s.%s' is PARAM_OBJECTSET but no 'assign()' in %s", getName(), pe->id, o.interfaceName());
		return PSET_CHANGED;
	}
	ExtObject xcopy = x; //only needed for messageOnExceedRange(): retain original, requested value of x because it may be changed below
	if (pe->fun2)
	{
		v.setObject(x);
		int result = (*(int(*)(void*, const ExtValue*))pe->fun2)(object, &v);
		::messageOnExceedRange(this, i, result, xcopy);
		return result;
	}
	else
	{
		void *target = getTarget(i);
		*((ExtObject*)target) = x;
		return PSET_CHANGED;
	}
}

int SimpleAbstractParam::setExtValue(int i, const ExtValue& x)
{
	SANITY_CHECK(i);
	ParamEntry *pe = entry(i);
	if (pe->flags&PARAM_READONLY) return PSET_RONLY;
	ExtValue xcopy = x; //only needed for messageOnExceedRange(): retain original, requested value of x because it may be changed below
	if (pe->fun2)
	{
		int result = (*(int(*)(void*, const ExtValue*))pe->fun2)(object, &x);
		::messageOnExceedRange(this, i, result, xcopy);
		return result;
	}
	else
	{
		void *target = getTarget(i);
		*((ExtValue*)target) = x;
		return PSET_CHANGED;
	}
}

void SimpleAbstractParam::call(int i, ExtValue *args, ExtValue *ret)
{
	SANITY_CHECK(i);
	ParamEntry *pe = entry(i);
	if (!pe) return;
	if (pe->fun1 && (pe->type[0] == 'p'))
		(*(void(*)(void*, ExtValue*, ExtValue*))pe->fun1)(object, args, ret);
	else
	{
		logPrintf("SimpleAbstractParam", "call", LOG_ERROR,
			(*pe->type != 'p') ? "'%s.%s' is not a function" : "Internal error - undefined function pointer for '%s.%s'", getName(), pe->id);
		ret->setInvalid();
	}
}

void SimpleAbstractParam::setDefault()
{
	bool save = dontcheckchanges;
	dontcheckchanges = 1;
	ParamInterface::setDefault();
	dontcheckchanges = save;
}

void SimpleAbstractParam::setDefault(int i)
{
	bool save = dontcheckchanges;
	dontcheckchanges = 1;
	ParamInterface::setDefault(i);
	dontcheckchanges = save;
}

// Returns the address of the beginning of the line.
// len = line length (without \n).
// 0 may mean the line with length=0 or the end of the SString.
// poz is advanced to the beginning of the next line.
// A typical loop: for(poz=0;poz<s.d;) {line=getline(s,poz,len);...
static const char *getline(const SString &s, int &poz, int &len)
{
	const char *beg = s.c_str() + poz;
	if (poz >= s.len()) { poz = s.len(); len = 0; return s.c_str() + s.len(); }
	const char *lf = strchr(beg, '\n');
	if (!lf) { lf = s.c_str() + s.len() - 1; poz = s.len(); }
	else { poz = (int)(lf - s.c_str()) + 1; if (poz > s.len()) poz = s.len(); }
	while (lf >= beg) if ((*lf == '\n') || (*lf == '\r')) lf--; else break;
	len = (int)(lf - beg) + 1;
	return beg;
}

int ParamInterface::loadSingleLine(const SString &s, LoadOptions &options)
{
	int i; // the index number of the parameter
	int tmpi;
	int len;
	int ret;
	int fields_loaded = 0;
	const char *t, *lin, *end;
	const char *equals_sign, *field_end, *next_field;
	char remember;
	const char *quote, *quote2;
	const char *value, *valstop;
	SString tmpvalue;
	bool parse_failed = false;
	if (options.offset >= s.len()) return fields_loaded;
	t = s.c_str() + options.offset;

	lin = getline(s, options.offset, len); // all fields must be encoded in a single line
	if (!len) return fields_loaded; // empty line = end
	i = 0;
	end = lin + len;
	while (t < end)
	{
		// processing a single field
		// "p:name=field_value,  field_name=field_value  , name=value..."
		//                     ^ ^-t (after)           ^ ^_next_field
		//                     \_t (before)            \_field_end
		while (isspace(*t)) if (t < end) t++; else return fields_loaded;

		field_end = strchrlimit(t, ',', end); if (!field_end) field_end = end;
		next_field = field_end;
		while ((field_end > t) && isblank(field_end[-1])) field_end--;
		quote = strchrlimit(t, '\"', field_end);
		if (quote)
		{
			quote2 = skipQuoteString(quote + 1, end);
			if (quote2 > field_end)
			{
				field_end = strchrlimit(quote2 + 1, ',', end);
				if (!field_end) field_end = end;
				next_field = field_end;
			}
			equals_sign = strchrlimit(t, '=', quote);
		}
		else
		{
			equals_sign = strchrlimit(t, '=', field_end);
			quote2 = 0;
		}
		if (equals_sign == t) { t++; equals_sign = 0; }
		if (field_end == t)	// skip empty value
		{
			t++; i++;
			continue;
		}
		if (equals_sign) // have parameter name
		{
			tmpi = findIdn(t, (int)(equals_sign - t));
			i = tmpi;
			if (tmpi < 0)
			{
				SString name(t, (int)(equals_sign - t));
				logPrintf("Param", "loadSingleLine", LOG_WARN, "Unknown property '%s.%s' (ignored)", getName(), name.c_str());
			}
			t = equals_sign + 1; // t=value
		}
#ifdef WARN_MISSING_NAME
		else
#ifdef SAVE_SELECTED_NAMES
			if ((i >= getPropCount()) || !(flags(i)&PARAM_CANOMITNAME))
#endif
			{
				if (id(i))
					logPrintf("Param", "loadSingleLine", LOG_WARN, "Missing property name in '%s' (assuming '%s')", getName(), id(i));
				else
					logPrintf("Param", "loadSingleLine", LOG_WARN, "Value after the last property of '%s'", getName());
			}
#endif
		if ((i >= 0) && id(i))
		{
			value = t;
			if (quote)
			{
				tmpvalue.copyFrom(quote + 1, (int)(quote2 - quote) - 1);
				sstringUnquote(tmpvalue);
				value = tmpvalue.c_str();
				valstop = quote2;
			}
			else
				if (field_end < end) valstop = field_end; else valstop = end;

			remember = *valstop;
			*(char*)valstop = 0;
			ret = setFromString(i, value, true);
			fields_loaded++;
			if (ret&PSET_PARSEFAILED)
				parse_failed = true;
			*(char*)valstop = remember;
		}

		if (i >= 0) i++;
#ifdef __CODEGUARD__ 
		if (next_field < end - 1) t = next_field + 1; else return fields_loaded;
#else
		t = next_field + 1;
#endif
	}
	if (parse_failed) options.parse_failed = true;
	return fields_loaded;
}

int Param::grmember(int g, int a)
{
	if ((getGroupCount() < 2) && (!g))
		return (a < getPropCount()) ? a : -9999;

	ParamEntry *e = entry(0);
	int x = 0, i = 0;
	for (; e->id; i++, e++)
	{
		if (e->group == g)
			if (a == x) return i; else x++;
	}
	return -9999;
}
