// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "sstringutils.h"
#include <common/virtfile/virtfile.h>
#include <common/log.h>
#include <common/nonstd.h>
#include <common/Convert.h>

int loadSString(const char* filename, SString& s, const char* framsgmodule, const char* error, bool remove_cr)
{
	VirtFILE *f;
	int ret = 0;
	if (f = Vfopen(filename, FOPEN_READ_BINARY))
	{
		loadSString(f, s, remove_cr);
		ret = 1;
		delete f;
	}
	else if (framsgmodule)
		logPrintf(framsgmodule, "loadSString", LOG_WARN, error ? error : "Can't open file \"%s\"", filename);
	return ret;
}

void loadSString(VirtFILE *f, SString& s, bool remove_cr)
{
	char buf[1024];
	int len;
	while (!f->Veof())
	{
		len = f->Vread(buf, 1, sizeof(buf));
		s.append(buf, len);
	}
	if (remove_cr)
		removeCR(s);
}

//load single line, discarding any \r or \n found at the end, return false if nothing could be loaded (error or eof)
bool loadSStringLine(VirtFILE* f, SString& s)
{
	char buf[100];
	bool eolfound = false;
	bool ret = false;
	s = SString::empty();
	while (!eolfound)
	{
		char *r = f->Vgets(buf, sizeof(buf));
		if (r == NULL) break;
		ret = true;
		int d = strlen(r);
		if (d > 0)
		{
			if (r[d - 1] == '\n') { d--; eolfound = true; }
			if (d > 0) if (r[d - 1] == '\r') d--;
			s += SString(r, d);
		}
	}
	return ret;
}

//////////////////////////

/** "x~xx~xxx" -> "x\~xx\~xxx"  */
int quoteTilde(SString &target)
{
	const char* x = target.c_str();
	SString tmp;
	char *f;
	while (1)
	{
		f = strchr((char*)x, '~');
		if (f)
		{
			tmp.append(x, f - x);
			tmp += "\\~";
			x = f + 1;
		}
		else
		{
			if (tmp.len() == 0) return 0; // nothing was changed!
			tmp += x;
			target = tmp;
			return 1;
		}
	}
}

/** "x\~xx\~xxx" -> "x~xx~xxx"  */
int unquoteTilde(SString &target)
{
	const char* x = target.c_str();
	SString tmp;
	char *f;
	while (1)
	{
		f = strchr((char*)x, '\\');
		if (f)
		{
			tmp.append(x, f - x);
			if (f[1] == '~')
			{
				tmp += '~';
				x = f + 2;
			}
			else
			{
				tmp += "\\";
				x = f + 1;
			}
		}
		else
		{
			if (tmp.len() == 0) return 0; // nothing was changed!
			tmp += x;
			target = tmp;
			return 1;
		}
	}
}

/////////////////

bool strContainsOneOf(const char* str, const char* chars)
{
	while (*str)
	{
		if (strchr(chars, *str)) return 1;
		str++;
	}
	return 0;
}

//////////////

bool sstringQuote(SString& target)
{
	const char* x = target.c_str();
	bool changed = 0;
	SString tmp;
	tmp.memoryHint(target.len());
	while (*x)
	{
		switch (*x)
		{
		case '\n': tmp += "\\n"; changed = 1; break;
		case '\r': tmp += "\\r"; changed = 1; break;
		case '\t': tmp += "\\t"; changed = 1; break;
		case '\"': tmp += "\\\""; changed = 1; break;
		case '\\': tmp += "\\\\"; changed = 1; break;
		default: tmp += *x;
		}
		x++;
	}
	if (changed) target = tmp;
	return changed;
}

SString sstringDelimitAndShorten(const SString &in, int maxlen, bool show_length, const SString& before, const SString& after)
{
	SString out;
	if (in.len() > maxlen)
		out = in.substr(0, maxlen / 2) + "..." + in.substr(in.len() - maxlen + maxlen / 2);
	else
	{
		out = in; show_length = false;
	}
	sstringQuote(out);
	out = before + out + after;
	if (show_length)
		out += SString::sprintf(" (length %d)", in.len());
	return out;
}

const char* skipQuoteString(const char* txt, const char* limit)
{
	while (*txt)
	{
		if (*txt == '\"') return txt;
		if (*txt == '\\') txt++;
		txt++;
		if (txt == limit) break;
	}
	return txt;
}

int sstringUnquote(SString &target)
{
	const char* x = target.c_str();
	SString tmp;
	char *f;
	while (1)
	{
		f = strchr((char*)x, '\\');
		if (f)
		{
			tmp.append(x, f - x);
			switch (f[1])
			{
			case 'n': tmp += '\n'; break;
			case 'r': tmp += '\r'; break;
			case 't': tmp += '\t'; break;
			case '\"': tmp += '\"'; break;
			default: tmp += f[1];
			}
			x = f + 2;
		}
		else
		{
			if (tmp.len() == 0) return 0; // nothing was changed!
			tmp += x;
			target = tmp;
			return 1;
		}
	}
}

int strFindField(const SString& txt, const SString& name, int &end)
{
	const char* t = txt.c_str(), *n;
	int pos = 0;
	while (1)
	{
		n = strchr(t + pos, ',');
		if ((!strncmp(t + pos, name.c_str(), name.len())) && (t[pos + name.len()] == '='))
		{
			if (n) end = n - t; else end = txt.len();
			return pos;
		}
		if (n) pos = n - t + 1; else break;
	}
	return -1;
}

SString strGetField(const SString& txt, const SString& name)
{
	int p, e;
	p = strFindField(txt, name, e);
	if (p < 0) return SString();
	p += name.len() + 1;
	return SString(txt.substr(p, e - p));
}

void strSetField(SString& txt, const SString& name, const SString& value)
{
	int p, e;
	p = strFindField(txt, name, e);
	if (p < 0)
	{
		if (!value.len()) return;
		char *t = txt.directAppend(1 + name.len() + value.len());
		char *b = t;
		if (txt.len()) *(t++) = ',';
		strcpy(t, name.c_str()); t += name.len();
		*(t++) = '=';
		strcpy(t, value.c_str()); t += value.len();
		txt.endAppend(t - b);
	}
	else
	{
		if (!value.len())
		{
			if (p > 0) p--; else if (e < txt.len()) e++;
			char *t = txt.directWrite(0);
			memmove(t + p, t + e, txt.len() - e);
			txt.endWrite(txt.len() + value.len() - (e - p));
		}
		else
		{
			p += name.len() + 1;
			char *t = txt.directWrite(txt.len() + value.len() - (e - p));
			memmove(t + p + value.len(), t + e, txt.len() - e);
			memmove(t + p, value.c_str(), value.len());
			txt.endWrite(txt.len() + value.len() - (e - p));
		}
	}
}

SString trim(const SString& s)
{
	const unsigned char*b = (const unsigned char*)s.c_str();
	const unsigned char*e = b + s.len();
	while ((b < e) && (*b <= ' ')) b++;
	while ((b < e) && (e[-1] <= ' ')) e--;
	if ((e - b) == s.len()) return s;
	SString newstring;
	char* t = newstring.directWrite(e - b);
	memmove(t, b, e - b);
	newstring.endWrite(e - b);
	return newstring;
}

SString concatPath(const SString& in1,const SString& in2)
{
	SString out=in1;
	if (out.len()>0 && out[out.len()-1]!=PATH_SEPARATOR_CHAR)
		out+=PATH_SEPARATOR_CHAR;
	out+=in2;
	return out;
}

bool removeCR(SString& s)
{
	const char* p = s.c_str();
	const char* cr = strchr(p, '\r');
	if (!cr) return false;
	char* begin = s.directWrite();
	char* src = begin + (cr - p), *dst = src;
	while (*src)
		if (*src == '\r')
			src++;
		else
			*(dst++) = *(src++);
	s.endWrite(dst - begin);
	return true;
}

bool matchWildcard(const SString& word, const SString& pattern)
{
	if (pattern.len() == 0)
		return word.len() == 0;
	int aster = pattern.indexOf('*');
	if (aster >= 0)
	{
		SString before = pattern.substr(0, aster);
		SString after = pattern.substr(aster + 1);
		if (!word.len()) return false;
		if (before.len()) if (!word.startsWith(before.c_str())) return false;
		if (after.len())
			if ((word.len() < after.len())
				|| (strcmp(after.c_str(), word.c_str() + word.len() - after.len())))
				return false;
		return true;
	}
	else
		return word == pattern;
}

bool matchWildcardList(const SString& word, const SString& patterns)
{
	if (patterns.len() == 0)
		return word.len() == 0;
	int pos = 0;
	SString pattern;
	while (patterns.getNextToken(pos, pattern, ','))
		if (matchWildcard(word, pattern))
			return true;
	return false;
}

SString getUIDString(uint64_t uid, char prefix)
{
	return SString::sprintf("%c" UINT64_FORMAT, prefix, uid);
}

bool parseUIDString(const char* str, char prefix, uint64_t &uid, bool err)
{
	if ((str[0] == prefix) && (isdigit(str[1])))
	{
		char* end;
		uid = strtoull(str + 1, &end, 10);
		if (end == (str + 1 + strlen(str + 1)))
			return true;
	}
	if (err)
		logPrintf("SString", "parseUIDString", LOG_ERROR, "Invalid uid: '%s'", str);
	return false;
}
