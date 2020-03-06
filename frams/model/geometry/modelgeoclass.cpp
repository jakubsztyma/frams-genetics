// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2016  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "modelgeoclass.h"
#include "modelgeometryinfo.h"
#include <frams/vm/classes/collectionobj.h>
#include <frams/vm/classes/3dobject.h>

#define FIELDSTRUCT ModelGeometry
static ParamEntry modelgeo_paramtab[] =
{
	{ "Creature: Geometry", 1, 5, "ModelGeometry",
	"Approximately estimates sizes, volume, and area of a Model based on the geometry of its parts.\n"
	"Example usage:\n"
	"Simulator.print(ModelGeometry.forModel(Model.newFromString(\"//0\\np:sh=1\\n\")).area());\n\n"
	"ModelGeometry.geom_density refers to the global simulator parameter (also available in GUI).\n"
	"To set geom_density for individual ModelGeometry objects:\n"
	"var mg=ModelGeometry.forModel(GenePools[0][0].getModel()); mg.geom_density=2; GenePools[0][0].data->area=mg.area();\n" },
	{ "geom_density", 0, 0, "Density", "f 0.01 100.0 1.0", FIELD(density), "Affects the geometry calculation precision" }, //note: we used 'geom_density' instead of 'density' to make the name more unique - because sim_params merges all configuration fields in a single namespace.
	{ "forModel", 0, PARAM_USERHIDDEN, "", "p oModelGeometry(oModel)", PROCEDURE(p_formodel), "The returned ModelGeometry object can be used to calculate geometric properties (volume, area, sizes) of the associated model. The density is copied from the current global ModelGeometry.geom_density on object creation." },
	{ "volume", 0, PARAM_NOSTATIC | PARAM_USERHIDDEN, "volume", "p f()", PROCEDURE(p_volume), },
	{ "area", 0, PARAM_NOSTATIC | PARAM_USERHIDDEN, "area", "p f()", PROCEDURE(p_area), },
	{ "sizesAndAxes", 0, PARAM_NOSTATIC | PARAM_USERHIDDEN, "sizesAndAxes", "p oVector()", PROCEDURE(p_sizesandaxes), "The returned vector contains XYZ (sizes) and Orient (axes) objects." },
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

ExtObject ModelGeometry::makeDynamicObject(ModelGeometry* mg)
{
	return ExtObject(&mg->par, mg);
}

ModelGeometry::ModelGeometry(ModelObj *mo)
:par(modelgeo_paramtab, this)
{
	cached_for_density = -1; //invalid value, will be updated on first request
	invalidateAllCached();
	model = mo;
	if (model != NULL)
		model->incref();
}

ModelGeometry::~ModelGeometry()
{
	if (model != NULL)
		model->decref();
}

// Mark all 3 results as invalid.
// Validity of these 3 values must be maintained independently,
// as each of them is calculated by an individual call.
void ModelGeometry::invalidateAllCached()
{
	cached_volume = -1;
	cached_area = -1;
	cached_sizes.x = -1;
}

// Invalidates cached results if a new density is requested
// (called in all geometry calculation functions)
void ModelGeometry::onDensityChanged()
{
	if (cached_for_density != density)
	{
		invalidateAllCached();
		cached_for_density = density;
	}
}

void ModelGeometry::p_formodel(ExtValue *args, ExtValue *ret)
{
	Model *m = ModelObj::fromObject(*args);
	if (m != NULL)
	{
	        if (m->getShapeType() == Model::SHAPE_BALL_AND_STICK)
			{
			Model *converted = new Model;
			converted->open();
			converted->buildUsingSolidShapeTypes(*m, Part::SHAPE_CYLINDER, 0.2);
			converted->close();
			m=converted;
			}
		ModelGeometry *mg = new ModelGeometry((ModelObj*)m);
		mg->density = density;
		ret->setObject(ModelGeometry::makeDynamicObject(mg));
	}
	else
		ret->setEmpty();
}

void ModelGeometry::p_volume(ExtValue *args, ExtValue *ret)
{
	onDensityChanged();
	if (cached_volume < 0) //calculate if invalid
		cached_volume = ModelGeometryInfo::volume(*model, density);
	ret->setDouble(cached_volume);
}

void ModelGeometry::p_area(ExtValue *args, ExtValue *ret)
{
	onDensityChanged();
	if (cached_area < 0) //calculate if invalid
		cached_area = ModelGeometryInfo::area(*model, density);
	ret->setDouble(cached_area);
}

void ModelGeometry::p_sizesandaxes(ExtValue *args, ExtValue *ret)
{
	onDensityChanged();
	if (cached_sizes.x < 0) //calculate if invalid
		ModelGeometryInfo::findSizesAndAxes(*model, density, cached_sizes, cached_axes);

	VectorObject* n = new VectorObject;
	n->data += new ExtValue(Pt3D_Ext::makeDynamicObject(cached_sizes));
	n->data += new ExtValue(Orient_Ext::makeDynamicObject(new Orient_Ext(cached_axes)));
	ret->setObject(n->makeObject());
}
