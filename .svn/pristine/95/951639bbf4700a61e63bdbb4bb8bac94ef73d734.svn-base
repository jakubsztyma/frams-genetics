// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENCONV_H_
#define _GENCONV_H_

#include "geno.h"
#include <frams/param/param.h>
#include <frams/util/list.h>
#include <frams/util/sstring.h>

#include <string>
#include <vector>


class GenoConvManager;

class GenoConvParam : public Param
{
	GenoConvManager *gcm;
	std::vector<std::string> gcnames; //stores names of converters so that these names persist and pointers to these names can be safely used externally
	char tmp_id[20];
	void freetab();
public:
	GenoConvParam(GenoConvManager *g);
	~GenoConvParam();
	void *getTarget(int);
	const char* id(int i);
	void updatetab();
};

class MultiMap;

/// Base class for all Geno Converters.
/// In constructor you have to set public fields
/// indicating your identity and supported formats.
/// Each converter serves one in-out format pair.
/// Instance of your converter should be registered
/// in GenoConvManager.
class GenoConverter
{
public:
	const char *name;	//< converter name (short)
	char in_format,		//< input format, eg. '1'
		out_format;	//< output format, eg. '0'
	paInt enabled;	//< don't touch this! (used by configuration module)
	paInt mapsupport; //< set to 1 if your converter supports genotype mapping

	/// You have to reimplement this method.
	/// If your converter cannot do its job, return empty string - return SString();
	/// Any other return value is assumed to be output genotype.
	/// @param map if not null, mapping informaton is requested, converter should add conversion map to this object
	virtual SString convert(SString &i, MultiMap *map, bool using_checkpoints) = 0;

	virtual ~GenoConverter() {}
	/// Don't forget to set public fields in your constructor
	GenoConverter() :name(""), in_format(-1), out_format('0'), enabled(1), mapsupport(0) {}
};

/// This class gathers abilities of all converters and can
/// convert a genotype to any other one, provided there is
/// a path of GenoConverters between them.
/// In most cases you don't use this class directly,
/// Geno::getConverted(int) provides full converting functionality.
/// Explicit GenoConvManager object is only needed for registering
/// your GenoConverter.
/// Use DefaultGenoConvManager to register the standard genotype converters automatically.
class GenoConvManager
{
	friend class GenoConvParam;
	SList converters;
public:
	GenoConvManager();
	~GenoConvManager();
	class GenoConvParam param;
	/// make a genotype in other format. genotype will be invalid
	/// if GenoConvManager cannot convert it.
	Geno convert(Geno &in, char format, MultiMap *map = 0, bool using_checkpoints = false, bool *converter_missing = NULL);
	/// register GenoConverter, the added object will be automatically deleted when GenoConvManager is destructed (call removeConverter() if this is not desirable)
	void addConverter(GenoConverter *conv);
	/// unregister GenoConverter
	void removeConverter(GenoConverter *conv);

	char *getPath(char in, char out, char *path, int maxlen, int *mapavailable = 0);
	char *getFormatPath(char in, char out, char *path, int maxlen, int *mapavailable = 0);
	/// returns the list of converters meeting the specified criteria
	/// pass result=0 if you only need one result (by return value)
	/// default criteria values mean "don't care", pass anything else to narrow your search
	GenoConverter *findConverters(SListTempl<GenoConverter*>* result = 0, char in = -1, char out = -1, int enabled = -1, char* name = 0);
};

#endif
