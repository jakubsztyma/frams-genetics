// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NN_LAYOUT_H_
#define _NN_LAYOUT_H_

/// abstract neural network layout state
class NNLayoutState
{
public:

	// layout function calls these to inquire about neuron connections:
	virtual int GetElements() = 0;		///< @return number of elements (neurons)
	virtual int GetInputs(int el) = 0;	///< @return number of inputs in element 'el' (referred to as 'N' in subsequent descriptions)
	virtual int GetLink(int el, int i) = 0;	///< @return index of element connected with i-th input of el-th element, or -1 if there is no connection

	// could be useful to refine element placement, but not really used by any layout function:
	virtual int *GetLinkXY(int el, int i) = 0;	///< @return relative coordinates of i-th input (or output when i==number of inputs) in el-th element (ret[0],[1]=x,y). unlike other methods, this information depends on the specific neuron drawing code, that's why NNLayoutState is abstract and can be implemented in context of a network drawing component.

	// layout function calls this to place neurons:
	virtual void SetXYWH(int el, int x, int y, int w, int h) = 0;	// set element position and size

	// layout function user can use this to retrieve the resulting element layout:
	virtual int *GetXYWH(int el) = 0;		///< @return current element (el=0..N-1) position and size ret[0],[1],[2],[3]=x,y,w,h

	virtual ~NNLayoutState() {}
};

struct NNLayoutFunction
{
	const char *name;
	void(*doLayout)(NNLayoutState*);
};

extern struct NNLayoutFunction nn_layout_functions[];

#endif
