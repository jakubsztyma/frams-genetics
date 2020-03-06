#include <frams/util/sstringutils.h>
#include <common/virtfile/stdiofile.h>
#include <frams/genetics/preconfigured.h>
#include <frams/param/paramtree.h>
#include <frams/param/mutparamlist.h>
#include <frams/vm/classes/genoobj.h>
#include <frams/vm/classes/collectionobj.h>
#include <frams/vm/classes/3dobject.h>
#include <frams/neuro/neuroimpl.h>
#include <frams/neuro/neurofactory.h>
#include <frams/model/geometry/modelgeoclass.h>
#include <frams/model/modelobj.h>
#include "genotypeloader.h"
#include "paramtree_print.h"

// This program tests parameter tree construction for all paramtab's that are available in SDK.
// See paramtree_stdin_test.cpp and app_group_names.txt for a more complete set (from Framsticks GUI) of paramtab objects.
// See mutableparam_test.cpp for a demonstration on how to detect (and possibly respond to) changing parameter definitions.
int main()
{
	StdioFILE::setStdio(); //setup VirtFILE::Vstdin/out/err
	PreconfiguredGenetics genetics;

	Param genotypemini_param(genotypemini_paramtab);
	NeuroFactory neurofac;
	neurofac.setStandardImplementation();
	NeuroNetConfig nn_config(&neurofac);
	ModelGeometry modelgeo;

	MutableParamList combined;
	combined += &genetics.genman.par;
	combined += &GenoObj::getStaticParam();
	combined += &ModelObj::getStaticParam();
	combined += &VectorObject::getStaticParam();
	combined += &DictionaryObject::getStaticParam();
	combined += &Pt3D_Ext::getStaticParam();
	combined += &Orient_Ext::getStaticParam();
	combined += &genotypemini_param;
	combined += &nn_config.par;
	combined += &modelgeo.par;

	ParamTree tree(&combined);

	printTree(&tree.root);

	neurofac.freeImplementation(); //just to avoid memory leak
}
