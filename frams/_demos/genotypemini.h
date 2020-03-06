#ifndef _GENOTYPEMINI_H_
#define _GENOTYPEMINI_H_

#include <frams/param/param.h>
#include <frams/util/extvalue.h>
#include <frams/genetics/geno.h>
#include <functional>

/** Defines the association between "org:" object (found in genotype files)
	and the GenotypeMini fields. GenotypeMiniLoader uses this definition
	but you can also use it to make MultiParamLoader load genotype
	*/
extern ParamEntry genotypemini_paramtab[];

/** Helper class, mostly useful with MultiParamLoader
	or its specialized version: MiniGenotypeLoader.
	GenotypeMini stores the subset of Genotype fields (the ones normally saved in .gen files)
	*/
class GenotypeMini
{
public:
	SString name, genotype, info, uid;
	double info_timestamp;
	SString info_author, info_email;
	paInt info_author_ispublic, info_email_ispublic, info_origin;
	SString info_how_created, info_performance;
	double energy0, lifespan, velocity, distance, vertvel, vertpos;
	paInt numparts, numjoints, numneurons, numconnections, ordnumber, generation, instances, is_valid;
	ExtValue user1, user2, user3;
	ExtObject data;
	void clear();

	GenotypeMini();
	GenotypeMini(const GenotypeMini &src);
	void initData();

#define GENOTYPEMINI_USE_GENMAN(gm) GenotypeMini::useGenManMutate(std::bind(&GenMan::mutate,&(gm),std::placeholders::_1))
	static void useGenManMutate(std::function<Geno(const Geno&)> gmm);
	/** GenMan object must be provided before using the 'mutate()' operation, like this:
		GENOTYPEMINI_USE_GENMAN(genetics.genman);
		*/

#define STATRICKCLASS GenotypeMini
	PARAMPROCDEF(p_mutate);
#undef STATRICKCLASS
};

#endif
