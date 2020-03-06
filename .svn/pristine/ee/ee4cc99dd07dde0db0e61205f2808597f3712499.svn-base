#include "genotypemini.h"
#include <frams/vm/classes/collectionobj.h>
#include <common/log.h>

#define FIELDSTRUCT GenotypeMini
ParamEntry genotypemini_paramtab[] =
{
	{ "Genotype", 1, 31, "org", },

	{ "name", 0, 0, "Name", "s 0 40", FIELD(name), },
	{ "genotype", 0, 0, "Genotype", "s 1", FIELD(genotype), "Genes as a string of characters.", },

	{ "info_timestamp", 1, 0, "Last modified", "ft 0 -1 0", FIELD(info_timestamp), },
	{ "info_author", 1, 0, "Author name", "s 0 100", FIELD(info_author), },
	{ "info_author_ispublic", 1, 0, "Author name is public", "d 0 1 1", FIELD(info_author_ispublic), },
	{ "info_email", 1, 0, "Author email", "s 0 100", FIELD(info_email), },
	{ "info_email_ispublic", 1, 0, "Author email is public", "d 0 1 0", FIELD(info_email_ispublic), },
	{ "info", 1, 0, "Description", "s 1 1000", FIELD(info), "Short description of key features of this creature.", },
	{ "info_origin", 1, 0, "Origin", "d 0 4 0 ~Unspecified~Designed~Designed and evolved~Evolved under various conditions~Evolved using single, constant setup", FIELD(info_origin), "Declaration of how this genotype originated." },
	{ "info_how_created", 1, 0, "How created", "s 1 1000", FIELD(info_how_created), "Description of the process of designing and/or evolving this genotype." },
	{ "info_performance", 1, 0, "Performance notes", "s 1 1000", FIELD(info_performance), "Description of why this genotype is special/interesting and how it performs." },

	{ "energy0", 0, 0, "Starting energy", "f 0 -1 0", FIELD(energy0), },
	{ "numparts", 0, 0, "Body parts", "d", FIELD(numparts), },
	{ "numjoints", 0, 0, "Body joints", "d", FIELD(numjoints), },
	{ "numneurons", 0, 0, "Brain size", "d", FIELD(numneurons), },
	{ "numconnections", 0, 0, "Brain connections", "d", FIELD(numconnections), },

	{ "num", 0, 0, "Ordinal number", "d", FIELD(ordnumber), },
	{ "gnum", 0, 0, "Generation", "d", FIELD(generation), },

	{ "instances", 0, 0, "Instances", "d", FIELD(instances), "Copies of this genotype", },

	{ "lifespan", 0, 0, "Life span", "f", FIELD(lifespan), "Average life span", },
	{ "velocity", 0, 0, "Velocity", "f", FIELD(velocity), "Average velocity", },
	{ "distance", 0, 0, "Distance", "f", FIELD(distance), },
	{ "vertvel", 0, 0, "Vertical velocity", "f", FIELD(vertvel), },
	{ "vertpos", 0, 0, "Vertical position", "f", FIELD(vertpos), },

	{ "user1", 0, 0, "User field 1", "x", FIELD(user1), },
	{ "user2", 0, 0, "User field 2", "x", FIELD(user2), },
	{ "user3", 0, 0, "User field 3", "x", FIELD(user3), },
	{ "data", 3, PARAM_OBJECTSET, "Custom fields dictionary", "oDictionary", FIELD(data), },

	{ "is_valid", 0, 0, "Validity", "d -1 1 -1", FIELD(is_valid),
	"0 = invalid genotype\n"
	"1 = valid genotype\n"
	"-1 = validity is not known." },

	{ "uid", 0, 0, "#", "s", FIELD(uid), "Unique identifier" },

	{ "mutate", 0, 0, "Mutate", "p()", PROCEDURE(p_mutate), },

	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

void GenotypeMini::initData()
{
	DictionaryObject *d = new DictionaryObject;
	data = d->makeObject();
}

GenotypeMini::GenotypeMini()
{
	initData();
}

GenotypeMini::GenotypeMini(const GenotypeMini &src)
{
	initData();
	name = src.name; genotype = src.genotype; info = src.info; uid = src.uid; info_timestamp = src.info_timestamp; info_author = src.info_author; info_email = src.info_email; info_author_ispublic = src.info_author_ispublic; info_email_ispublic = src.info_email_ispublic; info_origin = src.info_origin; info_how_created = src.info_how_created; info_performance = src.info_performance; energy0 = src.energy0; lifespan = src.lifespan; velocity = src.velocity; distance = src.distance; vertvel = src.vertvel; vertpos = src.vertpos; numparts = src.numparts; numjoints = src.numjoints; numneurons = src.numneurons; numconnections = src.numconnections; ordnumber = src.ordnumber; generation = src.generation; instances = src.instances; is_valid = src.is_valid; user1 = src.user1; user2 = src.user2; user3 = src.user3;

	// special case for 'data' (Dictionary inside an ExtObject)
	// 'data=src.data' would copy the Dictionary object reference (very wrong!)
	// performing a shallow copy instead:
	DictionaryObject *d = DictionaryObject::fromObject(data);
	DictionaryObject *src_d = DictionaryObject::fromObject(src.data);
	d->copyFrom(src_d);
}

void GenotypeMini::clear()
{
	Param p(genotypemini_paramtab, this);
	p.setDefault();
}

static std::function<Geno(const Geno&)> genman_mutate;

void GenotypeMini::useGenManMutate(std::function<Geno(const Geno&)> gmm)
{
	genman_mutate = gmm;
}

void GenotypeMini::p_mutate(ExtValue *args, ExtValue *ret)
{
	if (ret != NULL) ret->setEmpty();
	if (!genman_mutate)
	{
		logPrintf("GenotypeMini", "mutate", LOG_ERROR, "No function for mutation provided. See GENOTYPEMINI_USE_GENMAN().");
		return;
	}
	Geno in(genotype.c_str(), -1, name.c_str(), info.c_str());
	Geno out = genman_mutate(in);
	genotype = out.getGenesAndFormat();
	name = out.getName();
	info = out.getComment();
}
