// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "neurodiagram.h"
#include "nn_layout.h"
#include <frams/neuro/neurolibrary.h>
#include <frams/mech/mechworld.h>
#include <frams/util/multirange.h>
#include "canvasutil.h"
#include <frams/neuro/neuroimpl.h>
#include <frams/model/modelobj.h>
#include <frams/simul/simul.h>
#include "common/nonstd_time.h"

#define FIELDSTRUCT NeuroDiagram
ParamEntry neurodiagram_paramtab[] =
{
	{ "NeuroDiagram", 1, 4, "NeuroDiagram", "Can be used as the client object in the Window.", },
	{ "new", 0, PARAM_USERHIDDEN, "create new NeuroDiagram", "p oNeuroDiagram", PROCEDURE(p_new), },
	{ "showCreature", 0, PARAM_USERHIDDEN | PARAM_NOSTATIC, "show dynamic NN", "p(oCreature)", PROCEDURE(p_showcr), },
	{ "showModel", 0, PARAM_USERHIDDEN | PARAM_NOSTATIC, "show static NN", "p(oModel)", PROCEDURE(p_showmod), },
	{ "hide", 0, PARAM_USERHIDDEN | PARAM_NOSTATIC, "hide NN", "p()", PROCEDURE(p_hide), },
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

Param neurodiagram_param(neurodiagram_paramtab, 0);

static struct ColorDefs colordefs;

void NeuroDiagram::p_new(ExtValue*args, ExtValue*ret)
{
	NeuroDiagram *d = new NeuroDiagram(&colordefs);
	d->drawbackground = false;
	ret->setObject(ExtObject(&neurodiagram_param, d));
}

void NeuroDiagram::p_showcr(ExtValue*args, ExtValue*ret)
{
	Creature *cr = 0;
	if (args->type == TObj)
	{
		const ExtObject& o = args->getObject();
		cr = (Creature*)o.getTarget();
	}
	showLive(cr);
}

void NeuroDiagram::p_showmod(ExtValue*args, ExtValue*ret)
{
	Model *mod = ModelObj::fromObject(args[0]);
	show(mod);
}

static void addNeuroDescription(SString &t, Neuro *n)
{
	static Param par;
	SString c(n->getClassName());
	NeuroClass* cl = n->getClass();
	t += c;
	t += " (";
	if (cl)
		t += cl->getLongName();
	else
		t += "Unknown";
	t += ")";
}

NeuroDiagram::NeuroDiagram(ColorDefs *cd)
	:FramDrawToolkit(cd), livewire(false), indestructor(false), showing_not_alive_label(false), o(0),
	warn_if_not_alive(true), selection(*this), drawbackground(true), linetype(true), layouttype(2)
{
	scroll.setMargin(10, 10); // appropriate size should be adjusted
	dontPaintOutside(0);
	add(&scroll);
	pluginactive = false;
	FramDrawToolkit::setBackColor(ColorDefs::neurobackground);
}

NeuroDiagram::~NeuroDiagram()
{
	indestructor = 1;
	hide();
	remove(&scroll);
	updatePlugin();
}

void NeuroDiagram::hide()
{
	showing_not_alive_label = 0;
	if (o) o->delmodel_list.remove(killnode);
	FOREACH(NeuroProbe*, pr, probes)
		delete pr;
	probes.clear();
	selection.clear(0);
	cr = 0;
	livewire = false;
}

class NNLayoutState_Neurodiagram : public NNLayoutState
{
public:
	NeuroDiagram *nd;
	NNLayoutState_Neurodiagram(NeuroDiagram *_nd) :nd(_nd) {}

	int GetElements()
	{
		return nd->scroll.count();
	}

	int *GetXYWH(int el)
	{
		return &nd->scroll.getInfo(el)->pos.x;
	}

	void SetXYWH(int el, int x, int y, int w, int h)
	{
		ScrollInfo *si = nd->scroll.getInfo(el);
		si->pos.set(x, y); si->size.set(w, h);
	}

	int GetInputs(int el)
	{
		return nd->getNS(el)->n->getInputCount();
	}

	int GetLink(int el, int i)
	{
		return nd->getNS(el)->n->getInput(i)->refno;
	}

	int *GetLinkXY(int el, int i)
	{
		static int XY[2];
		int *xywh = GetXYWH(el);
		XY[0] = 0;
		XY[1] = ((1 + i)*xywh[3]) / (GetInputs(el) + 1);
		return XY;
	}
};


void NeuroDiagram::show(Model *o_)
{
	hide();
	o = o_;
	scroll.removeAll();
	if (o)
	{
		Neuro *n;
		int i;
		killnode = o->delmodel_list.add(STATRICKCALLBACK(this, &NeuroDiagram::onKill, 0));

		// create symbol objects
		for (i = 0; n = o->getNeuro(i); i++)
		{
			NeuroSymbol *ns = new NeuroSymbol(*this, n);
			scroll.add(ns, 1); // autodel	
		}
		if (i)
		{
			struct NNLayoutFunction *nnfun = &nn_layout_functions[layouttype];
			NNLayoutState_Neurodiagram nn(this);
			nnfun->doLayout(&nn);
		}
		scroll.invalidate();
		scroll.autoZoom();
	}

	updatePlugin();
	requestPaint();
}

void NeuroDiagram::showLive(Creature *_cr)
{
	showing_not_alive_label = 0;
	if (!_cr) { show(0); return; }
	show(&_cr->getModel());
	cr = _cr;
	livewire = true;
	updatePlugin();
	requestPaint();
}

void NeuroDiagram::paint()
{
	if (drawbackground)
	{
		setColor(ColorDefs::neurobackground);
		clear();
	}

	if (countNeurons() > 0)
	{
		CanvasWindowContainer::paint();
	}
	else
	{
		setColor(ColorDefs::neuroneuron);
		drawAlignedText(size.x / 2, (size.y - textHeight()) / 2, 0, "[No neural network]");
	}

	if (showing_not_alive_label)
	{
		if (time(0) > showing_not_alive_label)
			showing_not_alive_label = 0;
		else
		{
			setColor(0, 0, 0);
			drawAlignedText(not_alive_location.x, not_alive_location.y, 0, "select a creature");
			drawAlignedText(not_alive_location.x, not_alive_location.y + textHeight(), 0, "to probe neurons");
		}
	}
}

void NeuroDiagram::resize(int w, int h)
{
	CanvasWindowContainer::resize(w, h);
	scroll.autoZoom();
}

int NeuroDiagram::countNeurons()
{
	return scroll.count();
}

void NeuroDiagram::addProbe(int i)
{
	if (i >= countNeurons()) return;
	NeuroProbe *probe = new NeuroProbe(getNS(i));
	Pixel s=getSize();
	s.x=s.y=max(probe->getSize().x,min(s.x/3,s.y/3));
	probes += (void*)probe;
	add(probe);
	probe->resizeClient(s);
	updatePlugin();
	requestPaint();
}

void NeuroDiagram::onKill(void*obj, intptr_t dummy)
{
	show(0);
}

///////////////////////////

NeuroSymbol::NeuroSymbol(NeuroDiagram &nd, Neuro * _n)
	:FramDrawToolkit(nd.getColorDefs()), selected(0), n(_n), diagram(nd)
{
	tooltip = "#";
	tooltip += SString::valueOf((int)n->refno);
	tooltip += " - ";
	label = tooltip;
	addNeuroDescription(tooltip, n);
	label += n->getClassName();
	if (n->getClassParams().len())
	{
		tooltip += "\n"; tooltip += n->getClassParams();
	}
}

void NeuroSymbol::paint()
{
	if (selected)
	{
		setColor(ColorDefs::neuroselection);
		fillRect(0, 0, size.x, size.y);
	}
	diagram.setClip();
	diagram.setColor(ColorDefs::neuroneuron);
	drawNeuroSymbol(this, n->getClass(), 0, 0, size.x, size.y);

	if (size.y > 4 * textHeight())
	{
		const char* t = label.c_str();
		setColor(ColorDefs::neurosymbol);
		drawAlignedText(size.x / 2, size.y - textHeight(), 0, t);

		NeuroImpl *ni = NeuroNetImpl::getImpl(n);
		if (ni && (ni->getChannelCount() > 1))
		{
			drawLine(size.x - size.x / 16, size.y / 2 - size.y / 16,
				size.x - size.x / 8, size.y / 2 + size.y / 16);
			char t[20];
			sprintf(t, "%d", ni->getChannelCount());
			moveTo(size.x, size.y / 2 - textHeight());
			drawText(t);
		}
	}

	/*

				NeuroDiagram
				*........................................
				.                                       .
				.                    NeuroSymbol        .
				.      (pos.x,pos.y)-*.........         .
				.                    . |\     . ^  s    .
				.            ..._____._| \    . |  i    .
				.                    . |  \___. |  z    .
				.                  __._|  /   . |  e    .
				.                 |  . | /    . |  .    .
				.                 |  . |/     . |  y    .
				.                 |  .......... v       .
				.                 |  <-------->         .
				.      .......... |    size.x           .
				.      . |\     . |                     .
				.    __._| \    . |                     .
				.      . |  \___._|                     .
				.    __._|  /   .                       .
				.   |  . | /    .                       .
				.   |  . |/     .                       .
				.   |  ..........                       .
				.   |                                   .
				.   |________________...                .
				.........................................

				*/

	// NeuroSymbol is also responsible for drawing connection lines from its inputs to other NeuroSymbols' outputs
	NeuroSymbol *ns2;
	if (!diagram.isLive())
		diagram.setColor(ColorDefs::neurolink);
	for (int iw = 0; iw < n->getInputCount(); iw++)
	{
		ns2 = diagram.getNS(n->getInput(iw)->refno); // the other NeuroSymbol (our input will connect to its output)

		int yw = inputY(iw);
		int xw = yw / 4; // x coordinate of the first corner point, depends on yw to avoid overlapping between inputs
		drawLine(size.x / 4, yw, xw, yw); // first horizontal segment (to the left)
		if (diagram.isLive())
			diagram.setWireColor(ns2->n->state, 0);
		// linetype: 1 (default) - draw straight or U-shape depending on layout
		//           0 (debug option) - only draw straight lines
		if ((diagram.linetype != 1) || (ns2->pos.x + ns2->size.x / 2 < pos.x))
		{ // straight line to the other neuron's output (the signal goes forwards)
			ns2->lineTo(ns2->size.x, ns2->size.y / 2);
		}
		else
		{ // make an U-shaped loop from 3 segments - vert/horiz/vert (the signal goes backwards)
			int y2;
			int down;
			if (ns2 == this) down = (iw >= ((n->getInputCount()) / 2)); else down = (ns2->pos.y > (pos.y + size.y));
			if (down)
			{
				y2 = (pos.y + size.y + (size.y - yw) / 3);
			}
			else
			{
				y2 = pos.y - yw / 3;
				if ((ns2->pos.y<pos.y) && (ns2->pos.y>(pos.y - ns2->size.y))) y2 -= pos.y - ns2->pos.y;
			}
			// note: "diagram" uses global coordinate system, so we add "pos" or "ns2->pos" to get NeuroSymbol's global positions
			diagram.lineTo(pos.x + xw, y2);
			diagram.lineTo(ns2->pos.x + ns2->size.x, y2);
			diagram.lineTo(ns2->pos.x + ns2->size.x, ns2->pos.y + ns2->size.y / 2);
		}

	}
}

void NeuroSymbol::mouse(int x, int y, int t)
{
	if ((t & (LeftButton | ShiftButton)) == (LeftButton | ShiftButton))
	{
		ScrollManager& sc = diagram.scroll;
		sc.setPos2(n->refno, pos.x + x - diagram.symboldragpos.x, pos.y + y - diagram.symboldragpos.y);
		sc.validate();
		requestPaint();
	}
}

int NeuroSymbol::mouseclick(int x, int y, int t)
{
	if ((t & (LeftButton | DblClick)) == (LeftButton | DblClick))
	{
		if (diagram.isLive())
			diagram.addProbe(n->refno);
		else
		{
			if (diagram.warn_if_not_alive)
			{
				diagram.showing_not_alive_label = time(0) + 10;
				diagram.not_alive_location.x = pos.x + x;
				diagram.not_alive_location.y = pos.y + y;
				diagram.requestPaint();
			}
		}
		return LeftButton | DblClick;
	}

	if ((t & (LeftButton | ShiftButton)) == (LeftButton | ShiftButton))
	{
		if (selected)
			diagram.selection.remove(Model::neuroToMap(n->refno));
		else
			diagram.selection.add(Model::neuroToMap(n->refno));
		diagram.symboldragpos.set(x, y);
		return LeftButton | ShiftButton;
	}

	if (t & LeftButton)
	{
		diagram.selection.set(Model::neuroToMap(n->refno));
		return LeftButton;
	}

	return 0;
}

// coordinate y of i-th input
int NeuroSymbol::inputY(int i)
{
	return (1 + i)*size.y / ((n->getInputCount()) + 1);
}

SString NeuroSymbol::hint(int x, int y)
{
	if ((y >= 0) && (y < size.y))
		if (x < size.x / 4)
		{ // inputs?
		if (n->getInputCount() > 0)
		{
			int i = (y*n->getInputCount()) / size.y;
			double w;
			Neuro* target = n->getInput(i, w);
			if (target)
			{
				SString t = "connected to #";
				t += SString::valueOf((int)target->refno);
				t += " - ";
				addNeuroDescription(t, target);
				//		if (w!=1.0)
				{
					t += ", weight=";
					t += SString::valueOf(w);
				}
				return t;
			}
		}
		}
	return CanvasWindow::hint(x, y);
}

/////////////////////////

NeuroProbe::NeuroProbe(NeuroSymbol* ns)
	:DCanvasWindow(DCanvasWindow::Title + DCanvasWindow::Border + DCanvasWindow::Close + DCanvasWindow::Size,
	ns->getLabel().c_str(), &neurochart, &neurochart)
{
	holdismine = 0;
	drawing = 0; whichdrawing = -1;
	clientbordersset = 0;
	adjustingvalue = 0;
	link = ns;
	tooltip = SString("Probe for ") + ns->tooltip;
	setPos(ns->getPos().x, ns->getPos().y);
	neurochart.printMinMax(0);
	neurochart.data.setMinMax(-1, 1);
	chnum = 1; chnum2 = 0; chsel = 0;
	chselwidth = 0;
	chselecting = 0;
	updateChannelCount(NeuroNetImpl::getImpl(link->n));
}

void NeuroProbe::onClose()
{
	link->diagram.probes -= this;
	delete this;
}

NeuroProbe::~NeuroProbe()
{
	if (holdismine)
		link->n->flags &= ~Neuro::HoldState;
}

void NeuroProbe::paint()
{
	static char t[40];
	if (!clientbordersset)
	{
		clientbordersset = 1;
		setClientBorder(0, 1, 16, textHeight() + 2); // don't use textheight outside paint/mouse events
	}
	int hold = link->n->flags & Neuro::HoldState;
	float state = (float)link->n->state;
	NeuroImpl *ni = 0;
	if (chsel != 0)
	{
		ni = NeuroNetImpl::getImpl(link->n);
		if (chsel < 0)
		{
			int dr = -chsel - 1;
			if (whichdrawing != dr)
			{
				drawing = ni->getDrawing(dr);
				whichdrawing = dr;
			}
			if (drawing)
			{
				int *dr = drawing;
				int w = size.x - 2, h = size.y - clienttop - clientbottom;
				int scale = min(w, h);
				int x0 = clienttop + leftborder + ((w > h) ? (w - h) / 2 : 0);
				int y0 = clientleft + topborder + ((h > w) ? (h - w) / 2 : 0);

				while (*dr != NeuroImpl::ENDDRAWING)
				{
					int first = 1;
					unsigned char r, g, b;
					FramDrawToolkit::splitRGB(*(dr++), r, g, b);
					setColor(r, g, b);
					while (*dr != NeuroImpl::ENDDRAWING)
					{
						int x = ((*(dr++))*scale) / (NeuroImpl::MAXDRAWINGXY + 1) + x0;
						int y = ((*(dr++))*scale) / (NeuroImpl::MAXDRAWINGXY + 1) + y0;
						if (first) { moveTo(x, y); first = 0; }
						else lineTo(x, y);
					}
					dr++;
				}
			}
		}
	}
	DCanvasWindow::paintWithClient((chsel < 0) ? 0 : client);
	setColor(0, 0, 0);
	int yline = size.y - 2;
	if (chsel >= 0)
	{
		if (ni) state = (float)ni->getState(chsel);
		yline -= textHeight();
		int y = mapClientY(neurochart.mapData(state));
		int x = size.x - 15 - 1;
		drawLine(1, yline, size.x - 2, yline);
		if (hold)
		{
			sprintf(t, "hold: %1.3g", state);
			fillRect(x, y - 1 - 5, 15, 3 + 5 + 5);
			setColor(255, 0, 0);
			fillRect(x + 2, y - 1, 15 - 2 - 2, 3);
		}
		else
		{
			sprintf(t, "signal: %1.3g", state);
			fillRect(x, y - 1, 15, 3);
		}
		drawAlignedText(size.x - textHeight(), yline, 1, t);
	}

	if ((chnum > 1) || (chnum2 > 0))
	{
		if (chselecting) setColor(255, 255, 255); else setColor(0, 70, 0);
		if (chsel < 0)
			sprintf(t, "%c/%c", 'A' - chsel - 1, 'A' + chnum2 - 1);
		else
			sprintf(t, "%d/%d", chsel, chnum);
		moveTo(0, yline - textHeight());
		chselwidth = textWidth(t);
		drawText(t, -1, getSize().x);
	}
	else
		chselwidth = 0;
}

void NeuroProbe::mouse(int x, int y, int b)
{
	if (chselecting)
	{
		int ch = chsel0 + (x - chselx0) / 10;
		if (selectChannel(ch)) requestPaint();
		b &= ~LeftButton;
	}
	DCanvasWindow::mouse(x, y, b);
	if (adjustingvalue)
	{
		double st = neurochart.unmapData(unmapClientY(y));
		if (st<-1.0) st = -1.0; else if (st>1.0) st = 1.0;
		if (chsel == 0)
			link->n->state = st;
		else if (chsel >= 0)
		{
			NeuroImpl *ni = NeuroNetImpl::getImpl(link->n);
			if (ni) ni->setCurrentState(st, chsel);
		}
		requestPaint();
	}
}

void NeuroProbe::mouseunclick(int x, int y, int b)
{
	adjustingvalue = 0;
	chselecting = 0;
	DCanvasWindow::mouseunclick(x, y, b);
}

bool NeuroProbe::insideChSelector(int x, int y)
{
	if ((x > 0) && (x < chselwidth))
	{
		int sy = size.y;
		if (chsel >= 0) sy -= textHeight();
		return ((y<sy) && (y>(sy - textHeight())));
	}
	return 0;
}

int NeuroProbe::mouseclick(int x, int y, int b)
{
	if ((b & LeftButton) && insideChSelector(x, y))
	{
		chselx0 = x; chsel0 = chsel;
		chselecting = 1;
		requestPaint();
		return LeftButton;
	}
	int ret = DCanvasWindow::mouseclick(x, y, b);
	if (ret)
	{
		link->diagram.selection.set(Model::neuroToMap(link->n->refno));
		return ret;
	}
	if (b & LeftButton)
	{
		if (x > size.x - 16)
		{
			link->n->flags |= Neuro::HoldState;
			holdismine = 1;
			adjustingvalue = 1;
			mouse(x, y, b);
			return LeftButton;
		}
		else if (y > size.y - 16)
		{
			link->n->flags ^= Neuro::HoldState;
			holdismine = ((link->n->flags&Neuro::HoldState) != 0);
			requestPaint();
			return LeftButton;
		}
	}
	return 0;
}

SString NeuroProbe::hint(int x, int y)
{
	if ((chsel >= 0) && (x<size.x - 16) && (y>size.y - 16))
		return SString((link->n->flags&Neuro::HoldState) ? "Click to release" : "Click to hold");
	else if (insideChSelector(x, y))
		return SString::sprintf("channel %d of %d (click and drag to switch channels)", chsel, chnum);
	return DCanvasWindow::hint(x, y);
}

/** @return true == channel changed */
bool NeuroProbe::selectChannel(int ch)
{
	if (ch < -chnum2) ch = -chnum2; else if (ch >= chnum) ch = chnum - 1;
	if (ch == chsel) return false;
	chsel = ch;
	neurochart.data.clear();
	return true;
}

void NeuroProbe::updateChannelCount(NeuroImpl *ni)
{
	if (!ni) return;
	chnum = ni->getChannelCount();
	chnum2 = ni->getDrawingCount();
	if (chsel >= chnum) selectChannel(chnum - 1);
	if (chsel < -chnum2) selectChannel(-chnum2);
}

void NeuroProbe::sampling()
{
	NeuroImpl *ni = NeuroNetImpl::getImpl(link->n);
	updateChannelCount(ni);
	if (!chsel)
		neurochart.data += (float)(link->n->state);
	else
		neurochart.data += (float)(ni->getState(chsel));
	whichdrawing = -1;
}

////

void NeuroDiagram::probeSampling(void*obj, intptr_t dummy)
{
	FOREACH(NeuroProbe*, pr, probes) pr->sampling();
	requestPaint();
}

void NeuroDiagram::updatePlugin()
{
	//int needplugin=(!probes)>0;
	bool needplugin = livewire;
	if (needplugin == pluginactive) return;
	if (needplugin)
	{
		if (!cr) return;
		sim = cr->group->getLibrary().sim;
		pluginnode = sim->l_plugin.add(STATRICKCALLBACK(this, &NeuroDiagram::probeSampling, 0));
	}
	else
		sim->l_plugin.remove(pluginnode);
	pluginactive = needplugin;
}

/////////////

void NeuroDiagramSelection::updateSelection(MultiRange& newsel)
{
	MultiRange added = getAdded(newsel);
	if (!added.isEmpty())
	{
		added.shift(Model::mapToNeuro(0));
		added.intersect(0, diagram.countNeurons() - 1);
		for (int i = 0; i < added.rangeCount(); i++)
		{
			const IRange &r = added.getRange(i);
			for (int j = r.begin; j <= r.end; j++)
				diagram.getNS(j)->selected = 1;
		}
	}
	MultiRange removed = getRemoved(newsel);
	if (!removed.isEmpty())
	{
		removed.shift(Model::mapToNeuro(0));
		removed.intersect(0, diagram.countNeurons() - 1);
		for (int i = 0; i < removed.rangeCount(); i++)
		{
			const IRange &r = removed.getRange(i);
			for (int j = r.begin; j <= r.end; j++)
				diagram.getNS(j)->selected = 0;
		}
	}
	if (!diagram.indestructor) diagram.requestPaint();
}
