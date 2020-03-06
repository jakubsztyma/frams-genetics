// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENO_H_
#define _GENO_H_

#include <frams/util/sstring.h>
#include <frams/util/extvalue.h>

class MultiMap;
class Geno;
class GenoConvManager;

class GenoValidator
{
public:
	virtual int testGenoValidity(Geno& g) = 0;/// -1=no information  0=invalid  1=valid
	virtual ~GenoValidator() { };
};

/// Basic GenoValidator that works by building a Model from any Geno (by converting to f0).
/// Validation fails when the model can't be built or the genotype can't be converted.
class ModelGenoValidator : public GenoValidator
{
public:
	int testGenoValidity(Geno& g);
};

/// basic information about a single genotype.
class Geno : public DestrBase
{
	friend class Simulator;//needs to access validators directly
	SString gen;
	SString name;
	char format;
	SString txt;
	int isvalid; ///< <0 -> unknown   >=0 -> value for "isValid"

	Geno *f0gen;

	int mapinshift; /// number of characters in the initial comment
	int mapoutshift; /// number of characters in the output comment
	int multiline;

	void init(const SString& genstring, char genformat, const SString& genname, const SString& comment);
	void validate(void);

	void freeF0();

	bool isInvalid() { return isvalid == 0; }

	friend class Model;
	friend class GenoConvManager;

public:
	static const char INVALID_FORMAT = '!';
	typedef SListTempl<GenoValidator*> Validators;

	/// create a genotype object from primitives
	/// @param genstring pure genotype, without any comments
	/// @param genformat genotype format
	/// @param comment information about genotype (for genetic operators and "history")
	Geno(const char *genstring = 0, char genformat = -1, const char *genname = 0, const char *comment = 0);

	/// create a genotype object from primitives
	/// @param genstring pure genotype, wihtout any comments
	/// @param genformat genotype format
	/// @param name genotype name, new name will generated if needed
	/// @param comment information about genotype (for genetic operators and "history")
	Geno(const SString& genstring, char genformat, const SString& genname, const SString& comment);

	/// create object from full string, containing optional format and comment information
	Geno(const SString & fullstring);

	/// clone
	Geno(const Geno& src);

	void operator=(const Geno& src);

	~Geno();

	void setValid(int v) { isvalid = v; }
	int getValid() { return isvalid; }

	/// return string representation, with format comment at the beginning
	SString getGenesAndFormat(void) const;

	void setString(const SString& genewithcomments);

	/** @param genformat=-1 -> detect genotype format from genstring comment (like the constructor does), else specify the valid format in genformat and pure genes in genstring. */
	void setGenesAndFormat(const SString& genstring, char genformat = -1);
	/** g must be pure genes, without format. For the standard behavior use setGenesAndFormat() */
	void setGenesAssumingSameFormat(const SString& g);
	SString getGenes(void) const;

	SString getName(void) const;
	void setName(const SString&);
	char getFormat(void) const;

	SString getComment(void) const;
	void setComment(const SString&);

	/// invalid genotype cannot be used to build a creature
	bool isValid(void);

	/// make converted version of the genotype.
	/// @param converter_missing optional output parameter (ignored when NULL). Receives true if the conversion fails because of the lack of appropriate converter(s) (the returned Geno is always invalid in this case). Receives false if the genotype was converted by a converter or a converter chain (the returned Geno can be valid or invalid, depending on the converter's decision).
	Geno getConverted(char otherformat, MultiMap *m = 0, bool using_checkpoints = false, bool *converter_missing = NULL);

	/// @return -1 = before first char in the string
	/// @return -2 = after last char in the string
	int mapGenToString(int genpos) const;
	/// @return -1 = before first char in the genotype
	/// @return -2 = after last char in the genotype
	int mapStringToGen(int stringpos) const;

	int operator==(const Geno &g) { return (format == g.format) && (gen == g.gen); }

	void* owner;

	// managing global Geno-related objects (used for validation and conversion)
	static Validators* useValidators(Validators* val);
	static Validators* getValidators();

	static GenoConvManager* useConverters(GenoConvManager* gcm);
	static GenoConvManager* getConverters();
};

#ifndef NO_GENOCONVMANAGER
#include "genoconv.h"
#endif

#endif
