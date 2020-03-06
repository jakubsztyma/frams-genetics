// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _MODELCHECKPOINTS_H_
#define _MODELCHECKPOINTS_H_

#include <frams/userinput/modelviewiface.h>
#include <frams/userinput/modelselprop.h>

/**
Helper class that manages interactions between checkpoint selector, model display and model selection.
1. Showing geno without checkpoints -> disable checkpoint selector, enable model selection, pass to model display
2. Showing geno with checkpoints -> enable checkpoint selector
2a. Checkpoint selected -> disable model selection, display selected checkpoint
2b. Final stage selected -> enable model selection, display final stage
*/
class ModelCheckpointSupport : public ModelViewInterface
{
	ModelSelectionPropagator &mod_sel_prop;
	ModelViewInterface &model_view;
	Model *mod, *mod_checkpoints;
	float current_value, max_value;
	int current_index;
	bool selection_enabled;
	int current_checkpoint;
	bool use_checkpoints;
	void freeModel();
	void enableSelection(bool e);

public:
	enum CheckpointsMode {
		Disabled /** not displaying checkpoints because use_checkpoints=false in showGeno  */,
		EnabledUnknown /** checkpoints enabled, displaying main model, checkpoint availability not known, slider not clicked yet */,
		EnabledActive /** checkpoints enabled, displaying checkpoints or main model depending on the slider  */,
		EnabledUnavailable /** checkpoints enabled but only the main model is available */
	};
	ModelCheckpointSupport(ModelSelectionPropagator &msp, ModelViewInterface &_model_view);
	virtual ~ModelCheckpointSupport();

	void fromguiPhase(float p);
	virtual void toguiCheckpoints(CheckpointsMode mode, float maxvalue, float value) {}
	SString checkpointHint();

	int showGeno(const Geno*, bool use_checkpoints);
	int showModel(const Model *m);
	Model *getModel();
	ModelSelection& getModelSelection();
	CheckpointsMode getCheckpointsMode();
};

#endif
