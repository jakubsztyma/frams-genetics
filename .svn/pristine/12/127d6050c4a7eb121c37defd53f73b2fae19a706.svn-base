// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "nn_layout_model.h"

NNLayoutState_Model::NNLayoutState_Model(Model* m)
{
	model = m;
	neuron_pos = (m->getNeuroCount() > 0) ? new NeuronPos[m->getNeuroCount()] : NULL;
}

NNLayoutState_Model::~NNLayoutState_Model()
{
	if (neuron_pos) delete[] neuron_pos;
}

int NNLayoutState_Model::GetElements()
{
	return model->getNeuroCount();
}

int *NNLayoutState_Model::GetXYWH(int el)
{
	return &neuron_pos[el].x;
}

void NNLayoutState_Model::SetXYWH(int el, int x, int y, int w, int h)
{
	NeuronPos &np = neuron_pos[el];
	np.x = x; np.y = y; np.w = w; np.h = h;
}

int NNLayoutState_Model::GetInputs(int el)
{
	return model->getNeuro(el)->getInputCount();
}

int NNLayoutState_Model::GetLink(int el, int i)
{
	return model->getNeuro(el)->getInput(i)->refno;
}

int *NNLayoutState_Model::GetLinkXY(int el, int i)
{
	static int XY[2];
	int *xywh = GetXYWH(el);
	XY[0] = 0;
	XY[1] = ((1 + i)*xywh[3]) / (GetInputs(el) + 1);
	return XY;
}
