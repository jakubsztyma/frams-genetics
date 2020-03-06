#ifndef _SSTRING_SIMPLE_H_
#define _SSTRING_SIMPLE_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class ExtValue;  //this include would result in recurrent inclusion: #include "extvalue.h"
class ExtObject;

class SString
{
private:
	char *txt;      ///< string buffer or NULL for empty string
	int size;       ///< allocated memory (including \0)
	int used;	///< string length
	int appending;  ///< append mode, changes can occur after character # 'appending'

	void initEmpty();
	void copyFrom(SString &from); ///< copy from SString
	void resize(int newsize);
	void ensureSize(int needed);
	const char* getPtr() const { return txt ? txt : ""; }

public:
	SString(); ///< make an empty string
	SString(const char*t, int t_len = -1); ///< make a string from char*
	SString(int x); ///< string with initial buffer allocated for x characters
	SString(const SString& from); ///< duplicate string
	SString(SString&& from);///< move
	~SString();

	void copyFrom(const char* ch, int chlen = -1); ///< copy string, length of -1 == unknown

	void* operator new(size_t s, void* mem){ return mem; }
#ifdef _MSC_VER
		void operator delete(void* mem, void* t) {}
#endif
	void* operator new(size_t s){ return malloc(sizeof(SString)); }
	void operator delete(void* mem) { free(mem); }

	int len() const { return used; } ///< get string length
	void shrink(); ///< free unnecessary buffer

	/// after this call, you can modify sstring directly.
	/// returned value is the pointer to the internal buffer.
	/// <B>ensuresize</B> is minimal value of bytes you need,
	/// the buffer will be resized as needed.
	/// all "direct" operations have to leave the buffer with trailing '\0'
	/// at the end. endWrite() will search for this value in order to determine
	/// new string length.
	/// <P>Sample:<CODE>
	/// SString t;
	/// sprintf(t.directWrite(50),"a=%d,b=%f",a,b);
	/// t.endWrite();</CODE>
	char *directWrite(int ensuresize = -1);
	//char *directWrite();
	/// like directWrite, but it returns the pointer to the first char after current string
	/// for easy appending. <B>maxappend</B> is minimum of character in buffer
	/// that can be appended after this call.
	/// <P>Sample:<CODE>
	/// SString t;
	/// sprintf(t.directAppend(10),"c=%d",c);
	/// t.endAppend();</CODE>
	char *directAppend(int maxappend = 0);
	/// update string length, after directWrite.
	/// you don't have to to call endWrite after directWrite if the string's length doesn't change.
	/// optional <B>newlength</B> parameter gives a chance to further optimize
	/// this operation if you know exact length of resulting string.
	/// <P>Sample:<CODE>
	/// SString t("samplestring");
	/// strncpy(t.directWrite(50),src,bytecount);
	/// t.endWrite(bytecount);</CODE>
	void endWrite(int newlength = -1);
	/// update string length, after directAppend.
	/// you will usually need to call endAppend (or endWrite) after directAppend,
	/// because the purpose of directAppend is to change string's length.
	/// optional <B>appendlength</B> parameter gives a chance to further optimize
	/// this operation if you know exact length of the appended string.
	/// <P>Sample:<CODE>
	/// SString t("samplestring");
	/// strncpy(t.directAppend(50),src,bytecount);
	/// t.endAppend(bytecount);</CODE>
	void endAppend(int appendlength = -1);

	void memoryHint(int howbig) { ensureSize(howbig); }
	int directMaxLen() { return size - 1; } ///< when called after directWrite: max number of characters allowed (can be more than requested)

	/// find a character in SString.
	/// return index if the character was found or -1 otherwise.
	int indexOf(int character, int start = 0) const;

	/// find a substring.
	/// return index if the substring was found or -1 otherwise.
	int indexOf(const char *substring, int start = 0) const;

	/// find a substring.
	/// return index if the substring was found or -1 otherwise.
	int indexOf(const SString & substring, int start = 0) const;

	const char* c_str() const { return getPtr(); } ///< get SString's readonly buffer
	void operator=(const char*t); ///< assign from const char*
	void operator=(const SString &s);

	void append(const char *t, int n);
	SString operator+(const SString &s) const;
	void operator+=(int x); ///< append x spaces after current string
	void operator+=(const char*); ///< append char* contents
	void operator+=(const SString&); ///< append other SString

	bool equals(const SString &s) const; ///< TRUE if equal
	bool operator==(const SString &s) const { return equals(s); } ///< TRUE if equal
	bool operator!=(const SString &s) const { return !equals(s); } ///< TRUE if not equal
	bool operator<(const SString &s) const { return strcmp(getPtr(), s.getPtr()) < 1; }
	const char* operator()(int p) const { return getPtr() + p; } ///< pointer to p'th character in SString
	char operator[](int i) const { return getPtr()[i]; } ///< get char like in array

	/// return a substring of the current string
	SString substr(int begin, int length = 1 << 30) const;

	/// simple tokenization:
	/// starting at <B>pos</B>, get next substring delimited by <B>separator</B> character
	/// and put it in output parameter <B>token</B>.
	/// <B>pos</B> is moved accordingly.
	/// returns <B>false</B> if no more tokens are available or <B>true</B> otherwise.
	bool getNextToken(int& pos, SString &token, char separator) const;

	void operator+=(char ch) { directAppend(1)[0] = ch; endAppend(1); } ///< append single character

	bool startsWith(const char *pattern) const;
	char charAt(int pos) const { return operator[](pos); }
	uint32_t hash() const;

	static SString valueOf(int);
	static SString valueOf(long);
	static SString valueOf(double);
	static SString valueOf(const SString&); //tylko do kompletu zeby mozna uzyc tej funkcji nie martwiac sie o typ argumentu
	static SString valueOf(const ExtValue&); //tylko do kompletu zeby mozna uzyc tej funkcji nie martwiac sie o typ argumentu
	static SString valueOf(const ExtObject&); //tylko do kompletu zeby mozna uzyc tej funkcji nie martwiac sie o typ argumentu
	static SString sprintf(const char* format, ...);

	static SString &empty();
};

#endif
