#ifndef _MODELOBJ_H_
#define _MODELOBJ_H_

#include "model.h"

extern ParamEntry modelobj_paramtab[];

class ModelObj : public Model
{
public:
#define STATRICKCLASS ModelObj

	PARAMGETDEF(geno);
	PARAMPROCDEF(p_newfromstring);
	PARAMPROCDEF(p_newfromgeno);
	PARAMPROCDEF(p_newwithcheckpoints);

#define GETDELEGATE(name,type,value) PARAMGETDEF(name) {arg1->set ## type (value);}
	GETDELEGATE(numparts, Int, getPartCount())
	GETDELEGATE(numjoints, Int, getJointCount())
	GETDELEGATE(numneurons, Int, getNeuroCount())
	GETDELEGATE(numconnections, Int, getConnectionCount())
	GETDELEGATE(numcheckpoints, Int, getCheckpointCount())
#undef GETDELEGATE

	PARAMPROCDEF(p_getpart);
	PARAMPROCDEF(p_getjoint);
	PARAMPROCDEF(p_getneuro);
	PARAMGETDEF(bboxsize);
	PARAMPROCDEF(p_getcheckpoint);
	PARAMGETDEF(shape_type);
	PARAMGETDEF(solid_model);

#undef STATRICKCLASS

	static ExtObject makeStaticObject(Model* g);
	static ExtObject makeDynamicObject(Model* g);
	static Model* fromObject(const ExtValue& v, bool warn = true);
	static ParamInterface* getInterface();
	static Param& getStaticParam();
	static Param& getDynamicParam();
};

#endif
