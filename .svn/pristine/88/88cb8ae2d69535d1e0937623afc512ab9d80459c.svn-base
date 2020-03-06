// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NN_LAYOUT_MODEL_H_
#define _NN_LAYOUT_MODEL_H_

#include "nn_layout.h"
#include <frams/model/model.h>

class NNLayoutState_Model : public NNLayoutState
{
public:

	struct NeuronPos { int x, y, w, h; };

	Model *model;
	NeuronPos *neuron_pos;

	NNLayoutState_Model(Model *m);
	~NNLayoutState_Model();

	int GetElements();
	int *GetXYWH(int el);
	void SetXYWH(int el, int x, int y, int w, int h);
	int GetInputs(int el);
	int GetLink(int el, int i);
	int *GetLinkXY(int el, int i);
};

#endif
