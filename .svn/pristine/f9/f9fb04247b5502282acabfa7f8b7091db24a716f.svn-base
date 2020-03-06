// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "canvasutil.h"

static int std_unknown_symbol[] = { 12, 1, 4, 25, 25, 75, 25, 75, 75, 25, 75, 25, 25 };
static int std_neuron_symbol[] = { 12, 1, 4, 75, 50, 25, 0, 25, 99, 75, 50, 100, 50 };
static int std_inputonly_symbol[] = { 14, 1, 5, 25, 40, 35, 40, 45, 50, 35, 60, 25, 60, 25, 40 };
static int std_outputonly_symbol[] = { 18, 1, 7, 75, 50, 75, 60, 55, 60, 65, 50, 55, 40, 75, 40, 75, 50, 100, 50 };

int* drawNeuroSymbol(CanvasDrawing* c, NeuroClass* cl, int x, int y, int w, int h)
{
	int *data = std_unknown_symbol;
	if (cl)
	{
		data = cl->getSymbolGlyph();
		if (!data)
		{
			if (cl->getPreferredInputs() == 0)
				data = std_outputonly_symbol;
			else if (cl->getPreferredOutput() == 0)
				data = std_inputonly_symbol;
			else data = std_neuron_symbol;
		}
	}
	drawVector(c, data, x, y, w, h);
	return data;
}

void drawVector(CanvasDrawing* c, int data[], int x, int y, int w, int h)
{
	if (!data) return;
	Pixel p;
	data++;
	for (int NL = *(data++); NL > 0; NL--)
	{
		int NS = *(data++);
		p.x = x + (*(data++)*w) / 100;
		p.y = y + (*(data++)*h) / 100;
		c->moveTo(p);
		for (; NS > 0; NS--)
		{
			p.x = x + (*(data++)*w) / 100;
			p.y = y + (*(data++)*h) / 100;
			c->lineTo(p);
		}
	}
}
