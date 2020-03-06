// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "modelcheckpoints.h"

ModelCheckpointSupport::ModelCheckpointSupport(ModelSelectionPropagator &msp, ModelViewInterface &_model_view)
	:mod_sel_prop(msp), model_view(_model_view), mod(NULL), mod_checkpoints(NULL)
{
	selection_enabled = false;
}

ModelCheckpointSupport::~ModelCheckpointSupport()
{
	freeModel();
}

void ModelCheckpointSupport::enableSelection(bool e)
{
	if (e == selection_enabled) return;
	selection_enabled = e;
	if (selection_enabled)
	{
		mod_sel_prop.addModelSelection(&model_view.getModelSelection());
		mod_sel_prop.setModel(mod);
	}
	else
	{
		mod_sel_prop.removeModelSelection(&model_view.getModelSelection());
		mod_sel_prop.setModel(NULL);
	}

}

void ModelCheckpointSupport::freeModel()
{
	enableSelection(false);
	SAFEDELETE(mod);
	SAFEDELETE(mod_checkpoints);
}

int ModelCheckpointSupport::showGeno(const Geno* g, bool _use_checkpoints)
{
	use_checkpoints = (g != NULL) && _use_checkpoints;
	freeModel();
	if (g != NULL)
		mod = new Model(*g, true, false);
	toguiCheckpoints(use_checkpoints ? EnabledUnknown : Disabled, 1, 1);
	int ret = model_view.showModel(mod);
	enableSelection(true);
	return ret;
}

void ModelCheckpointSupport::fromguiPhase(float p)
{
	if (mod == NULL) return;
	if (use_checkpoints && (mod_checkpoints == NULL))
	{
		if (p == current_value) return;
		mod_checkpoints = new Model(mod->getGeno(), false, true);
		if (mod_checkpoints->getCheckpointCount() > 1)
		{
			max_value = current_value = mod_checkpoints->getCheckpointCount() - 1;
			current_index = mod_checkpoints->getCheckpointCount();
			toguiCheckpoints(EnabledActive, current_value, current_value);
		}
		else
		{
			toguiCheckpoints(EnabledUnavailable, 1, 1);
		}
	}

	if (!(mod_checkpoints->getCheckpointCount() > 1))
		return;

	current_value = p;
	int i = int(current_value + 0.5);
	if (current_value == max_value)
		i = mod_checkpoints->getCheckpointCount();
	if (i != current_index)
	{
		current_index = i;
		if (current_index < mod_checkpoints->getCheckpointCount())
		{
			model_view.showModel(mod_checkpoints->getCheckpoint(i));
			enableSelection(false);
		}
		else
		{
			model_view.showModel(mod);
			enableSelection(true);
		}
	}
}

int ModelCheckpointSupport::showModel(const Model *m)
{
	freeModel();
	return model_view.showModel(m);
}

Model *ModelCheckpointSupport::getModel()
{
	return mod ? mod : model_view.getModel();
}

ModelSelection& ModelCheckpointSupport::getModelSelection()
{
	return model_view.getModelSelection();
}

ModelCheckpointSupport::CheckpointsMode ModelCheckpointSupport::getCheckpointsMode()
{
	if (use_checkpoints)
	{
		if (mod_checkpoints == NULL)
			return EnabledUnknown;
		if (mod_checkpoints->getCheckpointCount() > 1)
			return EnabledActive;
		else
			return EnabledUnavailable;
	}
	else
		return Disabled;
}

SString ModelCheckpointSupport::checkpointHint()
{
	switch (getCheckpointsMode())
	{
	case EnabledUnknown: return SString("Drag horizontally to show developmental phases");
	case EnabledUnavailable: return SString("Developmental phases not available for this genotype");
	case EnabledActive:
		if (current_index < mod_checkpoints->getCheckpointCount())
			return SString::sprintf("Displaying developmental phase %d of %d", current_index, mod_checkpoints->getCheckpointCount());
		else
			return SString("Displaying the final phase of development");
	}
	return SString();
}

