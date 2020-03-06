// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

// Copyright (C) 1999,2000  Adam Rotaru-Varga (adam_rotaru@yahoo.com), GNU LGPL
// 2018, Grzegorz Latosinski, added support for new API for neuron types and their properties

#include "f4_general.h"
#include "../genooperators.h" //for GENOPER_ constants
#include <common/nonstd_stl.h>
#include <common/log.h>
#include <frams/model/model.h> // for min and max attributes
#include <common/nonstd_math.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

void rolling_dec(double *v)
{
	*v -= 0.7853;  // 0.7853981  45 degrees
}

void rolling_inc(double *v)
{
	*v += 0.7853;  // 0.7853981  45 degrees
}

int scanrec(const char* s, unsigned int slen, char stopchar)
{
	unsigned int i = 0;
	//DB( printf("    scan('%s', '%c')\n", s, stopchar); )
	while (1)
	{
		if (i >= slen)  // ran out the string, should never happen with correct string
			return 1;
		if (stopchar == s[i])  // bumped into stopchar
			return i;
		if (i < slen - 1) // s[i] is not the last char
		{
			if (s[i] == '(')
			{
				i += 2 + scanrec(s + i + 1, slen - i - 1, ')');
				continue;
			}
			if (s[i] == '<')
			{
				i += 2 + scanrec(s + i + 1, slen - i - 1, '>');
				continue;
			}
			if (s[i] == '#')
			{
				i += 2 + scanrec(s + i + 1, slen - i - 1, '>');
				continue;
			}
		}
		// s[i] a non-special character
		i++;
	}
	return i;
}


f4_Cell::f4_Cell(int nname,
	f4_Cell *ndad, int nangle, GeneProps newP)
{
	name = nname;
	type = T_UNDIFF4;
	dadlink = ndad;
	org = NULL;
	genot = NULL;
	gcur = NULL;
	active = 1;
	repeat.clear();
	//genoRange.clear(); -- implicit

	anglepos = nangle;
	commacount = 0;
	childcount = 0;
	P = newP;
	rolling = 0;
	xrot = 0;
	zrot = 0;
	//OM = Orient_1;
	ctrl = 0;
	inertia = 0.8;
	force = 0.04;
	sigmo = 2;
	nolink = 0;

	// adjust firstend and OM if there is a stick dad
	if (ndad != NULL)
	{
		// make sure it is a stick (and not a stick f4_Cell!)
		if (T_STICK4 == ndad->type)
		{
			//firstend = ndad->lastend;
			//OM = ndad->OM;
			ndad->childcount++;
		}
		if (T_NEURON4 == ndad->type)
		{
			inertia = ndad->inertia;
			force = ndad->force;
			sigmo = ndad->sigmo;
		}
	}
	// adjust lastend
	//lastend = firstend + ((Orient)OM * (Pt3D(1,0,0) * P.len));
	mz = 1;
}


f4_Cell::f4_Cell(f4_Cells *nO, int nname, f4_node *ngeno, f4_node *ngcur, f4_Cell *ndad, int nangle, GeneProps newP)
{
	name = nname;
	type = T_UNDIFF4;
	dadlink = ndad;
	org = nO;
	genot = ngeno;
	gcur = ngcur;
	active = 1;
	repeat.clear();
	//genoRange.clear(); -- implicit
	// preserve geno range of parent cell
	if (NULL != ndad)
		genoRange.add(ndad->genoRange);

	anglepos = nangle;
	commacount = 0;
	childcount = 0;
	P = newP;
	rolling = 0;
	xrot = 0;
	zrot = 0;
	//OM = Orient_1;
	ctrl = 0;
	inertia = 0.8;
	force = 0.04;
	sigmo = 2;
	nolink = 0;

	// adjust firstend and OM if there is a stick dad
	if (ndad != NULL)
	{
		// make sure it is a stick (and not a stick f4_Cell!)
		if (T_STICK4 == ndad->type)
		{
			//firstend = ndad->lastend;
			//OM = ndad->OM;
			ndad->childcount++;
		}
		if (T_NEURON4 == ndad->type)
		{
			inertia = ndad->inertia;
			force = ndad->force;
			sigmo = ndad->sigmo;
		}
	}
	// adjust lastend
	//lastend = firstend + ((Orient)OM * (Pt3D(1,0,0) * P.len));
	mz = 1;
}


f4_Cell::~f4_Cell()
{
	// remove links
	if (nolink)
	{
		int i;
		for (i = nolink - 1; i >= 0; i--)
			delete links[i];
		nolink = 0;
	}
}


/* return codes:
	>1 error at pos
	0  halt development for a cycle
	-1  development finished OK
	*/
int f4_Cell::onestep()
{
	if (gcur == NULL)
	{
		active = 0;
		return 0;  // stop
	}
	while (NULL != gcur)
	{
		//DB( printf("  %d (%d) executing '%c' %d\n", name, type, gcur->name, gcur->pos); )
		// currently this is the last one processed
		// the current genotype code is processed
		//genoRange.add(gcur->pos,gcur->pos+gcur->name.length()-1);
		bool neuclasshandler = false; // if set to true, then there is a set of characters that can be assigned to a neuron class type
		// old semantics, one-character
		if (gcur->name.length() == 1)
		{
			genoRange.add(gcur->pos, gcur->pos);
			char name = gcur->name[0];
			switch (name)
			{
			case '<':
			{
				// cell division!
				//DB( printf("  div! %d\n", name); )

				// error: sticks cannot divide
				if (T_STICK4 == type)
				{
					// cannot fix
					org->setError(gcur->pos);
					return 1;  // stop
				}

				// undiff divides
				if (T_UNDIFF4 == type)
				{
					// commacount is set only when daughter turns into X
					// daughter cell
					// adjust new len
					GeneProps newP = P;
					newP.propagateAlong(false);
					f4_Cell *tmp = new f4_Cell(org, org->nc, genot, gcur->child2, this, commacount, newP);
					tmp->repeat = repeat;
					repeat.clear();
					org->addCell(tmp);
				}
				// a neuron divides: create a new, duplicate links
				if (T_NEURON4 == type) {
					// daughter cell
					f4_Cell *tmp = new f4_Cell(org, org->nc, genot, gcur->child2,
						// has the same dadlink
						this->dadlink, commacount, P);
					tmp->repeat = repeat;
					repeat.clear();
					// it is a neuron from start
					tmp->type = T_NEURON4;
					// it has the same type as the parent neuron
					tmp->neuclass = neuclass;
					// duplicate links
					f4_CellLink *ll;
					for (int i = 0; i < nolink; i++)
					{
						ll = links[i];
						tmp->addlink(ll->from, ll->w, ll->t);
					}
					org->addCell(tmp);
				}
				// adjustments for this cell
				gcur = gcur->child;
				// halt development
				return 0;
			}
			case '>':
			{
				// finish
				// see if there is a repet count
				if (repeat.top > 0)
				{ // there is a repeat counter
					if (!repeat.first()->isNull())
					{ // repeat counter is not null
						repeat.first()->dec();
						if (repeat.first()->count > 0)
						{
							// return to repeat
							gcur = repeat.first()->node->child;
						}
						else
						{
							// continue
							gcur = repeat.first()->node->child2;
							repeat.pop();
						}
						break;
					}
					else
					{
						repeat.pop();
					}
				}
				else
				{
					// error: still undiff
					if (T_UNDIFF4 == type)
					{
						// fix it: insert an 'X'
						f4_node *insertnode = new f4_node("X", NULL, gcur->pos);
						if (org->setRepairInsert(gcur->pos, gcur, insertnode)) // not in repair mode, release
							delete insertnode;
						return 1;
					}
					repeat.clear();
					active = 0;  // stop
					// eat up rest
					gcur = NULL;
					return 0;
				}
			}
			[[fallthrough]];
			case '#':
			{
				// repetition marker
				if (repeat.top >= repeat_stack::stackSize)
				{
					// repeat pointer stack is full, cannot remember this one.
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				repeat.push(repeat_ptr(gcur, gcur->i1));
				gcur = gcur->child;
				break;
			}
			case ',':
			{
				commacount++;
				gcur = gcur->child;
				break;
			}
			case 'r':  case 'R':
			{
				// error: if neuron
				if (T_NEURON4 == type)
				{
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				switch (name)
				{
				case 'r':   rolling_dec(&rolling); break;
				case 'R':   rolling_inc(&rolling); break;
				}
				gcur = gcur->child;
				break;
			}
			case 'l':  case 'L':
			case 'c':  case 'C':
			case 'q':  case 'Q':
			case 'a':  case 'A':
			case 'i':  case 'I':
			case 's':  case 'S':
			case 'm':  case 'M':
			case 'f':  case 'F':
			case 'w':  case 'W':
			case 'e':  case 'E':
			case 'd':  case 'D':
			case 'g':  case 'G':
			case 'b':  case 'B':
			case 'h':  case 'H':
			{
				// error: if neuron
				if (T_NEURON4 == type)
				{
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				P.executeModifier(name);
				gcur = gcur->child;
				break;
			}
			case 'X':
			{
				// turn undiff. cell into a stick
				// error: already differentiated
				if (T_UNDIFF4 != type)
				{
					// fix: delete this node
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				type = T_STICK4;
				// fix dad commacount and own anglepos
				if (NULL != dadlink)
				{
					dadlink->commacount++;
					anglepos = dadlink->commacount;
				}
				// change of type halts developments, see comment at 'N'
				gcur = gcur->child;
				return 0;
			}
			case 'N':
			{
				// turn undiff. cell into a neuron
				// error: already differentiated
				if (T_UNDIFF4 != type)
				{
					// fix: delete this node
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				// error: if no previous
				if (NULL == dadlink)
				{
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				string temp1 = "N";
				char *temp = (char*)temp1.c_str();
				neuclass = GenoOperators::parseNeuroClass(temp);
				type = T_NEURON4;
				// change of type also halts development, to give other
				// cells a chance for adjustment.  Namely, it is important
				// to wait for other cells to turn N before adding links
				gcur = gcur->child;
				return 0;
			}
			case '@':
			case '|':
			{
				// neuron rotating / bending
				int j = 1;
				if ('@' == name) j = 1; // rot
				else
					if ('|' == name) j = 2; // bend

				// if undiff, then this is a new muscle. Thanks to f4_processrec @ and | case we can skip repairing
				if (T_UNDIFF4 == type)
				{
					neuclasshandler = true;
					break;
				}

				// error: not a neuron (stick)
				if (T_NEURON4 != type)
				{
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				// error: already has control
				if (ctrl != 0)
				{
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				// make neuron ctrl = 1 or 2
				ctrl = j;
				gcur = gcur->child;
				break;
			}
			case '[':
			{
				// link to neuron
				// error: not a neuron
				if (T_NEURON4 != type)
				{
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				// input (sensor or %d)
				int t = gcur->i1;
				int relfrom = gcur->l1;
				float w = gcur->f1;
				f4_Cell *tneu = NULL;
				if (t < 0) // wrong sensor
				{
					string buf = "wrong sensor in link '";
					buf.append(gcur->s1);
					buf.append("'");
					logMessage("f4_Cell", "onestep", LOG_ERROR, buf.c_str());
					org->setRepairRemove(gcur->pos, gcur);
					return 1;
				}
				else if (t > 0) // sensors
				{
					char *temp = (char*)gcur->s1.c_str();
					NeuroClass *sensortest = GenoOperators::parseNeuroClass(temp);
					if (sensortest == NULL || sensortest->getPreferredInputs() != 0)
					{
						// error: unknown code
						string buf = "wrong sensor in link '";
						buf.append(gcur->s1);
						buf.append("'");
						logMessage("f4_Cell", "onestep", LOG_ERROR, buf.c_str());
						org->setRepairRemove(gcur->pos, gcur);
						return 1;
					}
				}
				else {
					// input from other neuron
					// find neuron at relative i
					// find own index
					int j = 0, k = 0;
					for (int i = 0; i < org->nc; i++)
					{
						if (org->C[i]->type == T_NEURON4) k++;
						if (org->C[i] == this) { j = k - 1; break; }
					}
					// find index of incoming
					j = j + relfrom;
					if (j < 0) goto wait_link;
					if (j >= org->nc) goto wait_link;
					// find that neuron
					k = 0;
					int i;
					for (i = 0; i < org->nc; i++)
					{
						if (org->C[i]->type == T_NEURON4) k++;
						if (j == (k - 1)) break;
					}
					if (i >= org->nc) goto wait_link;
					tneu = org->C[i];
				}
				// add link
				// error: could not add link (too many?)
				if (addlink(tneu, w, gcur->s1))
				{
					// cannot fix
					org->setError(gcur->pos);
					return 1;  // stop
				}
				gcur = gcur->child;
				break;
			}
		wait_link:
			{
				// wait for other neurons to develop
				// if there are others still active
				active = 0;
				int j = 0;
				for (int i = 0; i < org->nc; i++)
				{
					if (org->C[i]->active) j++;
				}
				if (j > 0)
					return 0;  // there is other active, halt, try again
				// no more actives, cannot add link, ignore, but treat not as an error
				gcur = gcur->child;
			}
			break;
			case ':':
			{
				// neuron parameter
				// error: not a neuron
				if (T_NEURON4 != type)
				{
					// fix: delete it
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				int j = gcur->l1;
				switch ((char)gcur->i1)
				{
				case '!':
					if (j)
						force += (1.0 - force) * 0.2;
					else
						force -= force * 0.2;
					break;
				case '=':
					if (j)
						inertia += (1.0 - inertia) * 0.2;
					else
						inertia -= inertia * 0.2;
					break;
				case '/':
					if (j)
						sigmo *= 1.4;
					else
						sigmo /= 1.4;
					break;
				default:
					org->setRepairRemove(gcur->pos, gcur);
					return 1;  // stop
				}
				gcur = gcur->child;
				break;
			}
			case ' ':
			{
				// space has no effect, should not occur
				// fix: delete it
				org->setRepairRemove(gcur->pos, gcur);
				gcur = gcur->child;
				break;
			}
			default:
			{
				// because there are one-character neuron classes, default move control to neuclasshandler
				neuclasshandler = true;
			}
			}
		}
		else
		{
			// if many characters, then it needs to be parsed below
			neuclasshandler = true;
		}

		if (neuclasshandler)
		{
			genoRange.add(gcur->pos, gcur->pos + gcur->name.length() + 2 - 1); // +2 for N:
			if (T_UNDIFF4 != type)
			{
				// fix: delete this node
				org->setRepairRemove(gcur->pos, gcur);
				return 1;  // stop
			}
			// error: if no previous
			if (NULL == dadlink)
			{
				// fix: delete it
				org->setRepairRemove(gcur->pos, gcur);
				return 1;  // stop
			}
			// multiple characters are neuron types. Need to check if exists
			char *temp = (char*)gcur->name.c_str();
			neuclass = GenoOperators::parseNeuroClass(temp);
			if (neuclass == NULL)
			{
				// error: unknown code
				string buf = "unknown code '";
				buf.append(gcur->name);
				buf.append("'");
				logMessage("f4_Cell", "onestep", 2, buf.c_str());
				org->setRepairRemove(gcur->pos, gcur);
				return 1;
			}
			type = T_NEURON4; //they belong to neurons
			gcur = gcur->child;
			return 0; //stop
		}
	}
	active = 0;  // done
	return 0;
}


int f4_Cell::addlink(f4_Cell *nfrom, double nw, string nt)
{
	// if incoming neuron does not produce output, return error
	if (nfrom != NULL && nfrom->neuclass->getPreferredOutput() == 0) return -1;
	if (neuclass->getPreferredInputs() != -1 && nolink >= neuclass->getPreferredInputs()) return -1;
	if (nolink >= MAXINPUTS - 1) return -1; // full!
	links[nolink] = new f4_CellLink(nfrom, nw, nt);
	nolink++;
	return 0;
}


void f4_Cell::adjustRec()
{
	//f4_OrientMat rot;
	int i;

	if (recProcessedFlag)
		// already processed
		return;

	// mark it processed
	recProcessedFlag = 1;

	// make sure its parent is processed first
	if (NULL != dadlink)
		dadlink->adjustRec();

	// count children
	childcount = 0;
	for (i = 0; i < org->nc; i++)
	{
		if (org->C[i]->dadlink == this)
			if (org->C[i]->type == T_STICK4)
				childcount++;
	}

	if (type == T_STICK4)
	{
		if (NULL == dadlink)
		{
			//firstend = Pt3D_0;
			// rotation due to rolling
			xrot = rolling;
			mz = 1;
		}
		else
		{
			//firstend = dadlink->lastend;
			GeneProps Pdad = dadlink->P;
			GeneProps Padj = Pdad;
			Padj.propagateAlong(false);

			//rot = Orient_1;

			// rotation due to rolling
			xrot = rolling +
				// rotation due to twist
				Pdad.twist;
			if (dadlink->commacount <= 1)
			{
				// rotation due to curvedness
				zrot = Padj.curvedness;
			}
			else
			{
				zrot = Padj.curvedness + (anglepos * 1.0 / (dadlink->commacount + 1) - 0.5) * M_PI * 2.0;
			}

			//rot = rot * f4_OrientMat(yOz, xrot);
			//rot = rot * f4_OrientMat(xOy, zrot);
			// rotation relative to parent stick
			//OM = rot * OM;

			// rotation in world coordinates
			//OM =  ((f4_OrientMat)dadlink->OM) * OM;
			mz = dadlink->mz / dadlink->childcount;
		}
		//Pt3D lastoffset = (Orient)OM * (Pt3D(1,0,0)*P.len);
		//lastend = firstend + lastoffset;
	}
}



f4_CellLink::f4_CellLink(f4_Cell *nfrom, double nw, string nt)
{
	from = nfrom;
	w = nw;
	t = nt;
}



f4_Cells::f4_Cells(f4_node *genome, int nrepair)
{
	// create ancestor cell
	repair = nrepair;
	error = 0;
	errorpos = -1;
	repair_remove = NULL;
	repair_parent = NULL;
	repair_insert = NULL;
	tmpcel = NULL;
	f4rootnode = NULL;
	C[0] = new f4_Cell(this, 0, genome, genome, NULL, 0, GeneProps::standard_values);
	nc = 1;
}


f4_Cells::f4_Cells(SString & genome, int nrepair)
{
	int res;
	repair = nrepair;
	error = 0;
	errorpos = -1;
	repair_remove = NULL;
	repair_parent = NULL;
	repair_insert = NULL;
	tmpcel = NULL;
	f4rootnode = NULL;

	// transform geno from string to nodes
	f4rootnode = new f4_node();
	res = f4_processrec(genome.c_str(), (unsigned)0, f4rootnode);
	if ((res < 0) || (1 != f4rootnode->childCount()))
	{
		error = GENOPER_OPFAIL;
		errorpos = -1;
	}

	// create ancestor cell
	C[0] = new f4_Cell(this, 0, f4rootnode->child, f4rootnode->child, NULL, 0, GeneProps::standard_values);
	nc = 1;
}

f4_Cells::~f4_Cells()
{
	// release cells
	int i;
	if (nc)
	{
		for (i = nc - 1; i >= 0; i--)
			delete C[i];
		nc = 0;
	}
	if (f4rootnode)
		delete f4rootnode;
}


int f4_Cells::onestep()
{
	int i, ret, oldnc, ret2;
	oldnc = nc;
	ret = 0;
	for (i = 0; i < oldnc; i++)
	{
		ret2 = C[i]->onestep();
		if (ret2 > 0)
		{
			// error
			C[i]->active = 0;  // stop
			return 0;
		}
		// if still active
		if (C[i]->active)
			ret = 1;
	}
	return ret;
}


int f4_Cells::simulate()
{
	int i;
	error = GENOPER_OK;

	for (i = 0; i < nc; i++)  C[i]->active = 1;

	// execute onestep() in a cycle
	while (onestep());

	if (GENOPER_OK != error) return error;

	// fix neuron attachements
	for (i = 0; i < nc; i++)
	{
		if (C[i]->type == T_NEURON4)
		{
			while (T_NEURON4 == C[i]->dadlink->type)
			{
				C[i]->dadlink = C[i]->dadlink->dadlink;
			}
		}
	}

	// there should be no undiff. cells
	// make undifferentiated cells sticks
	for (i = 0; i < nc; i++)
	{
		if (C[i]->type == T_UNDIFF4)
		{
			C[i]->type = T_STICK4;
			//seterror();
		}
	}

	// recursive adjust
	// reset recursive traverse flags
	for (i = 0; i < nc; i++)
		C[i]->recProcessedFlag = 0;
	// process every cell
	for (i = 0; i < nc; i++)
		C[i]->adjustRec();

	//DB( printf("Cell simulation done, %d cells. \n", nc); )

	return error;
}


void f4_Cells::addCell(f4_Cell *newcell)
{
	if (nc >= MAX4CELLS - 1)
	{
		delete newcell;
		return;
	}
	C[nc] = newcell;
	nc++;
}


void f4_Cells::setError(int nerrpos)
{
	error = GENOPER_OPFAIL;
	errorpos = nerrpos;
}

void f4_Cells::setRepairRemove(int nerrpos, f4_node *rem)
{
	if (!repair)
	{
		// not in repair mode, treat as repairable error
		error = GENOPER_REPAIR;
		errorpos = nerrpos;
	}
	else
	{
		error = GENOPER_REPAIR;
		errorpos = nerrpos;
		repair_remove = rem;
	}
}

int f4_Cells::setRepairInsert(int nerrpos, f4_node *parent, f4_node *insert)
{
	if (!repair)
	{
		// not in repair mode, treat as repairable error
		error = GENOPER_REPAIR;
		errorpos = nerrpos;
		return -1;
	}
	else
	{
		error = GENOPER_REPAIR;
		errorpos = nerrpos;
		repair_parent = parent;
		repair_insert = insert;
		return 0;
	}
}

void f4_Cells::repairGeno(f4_node *geno, int whichchild)
{
	// assemble repaired geno, if the case
	if (!repair) return;
	if ((NULL == repair_remove) && (NULL == repair_insert)) return;
	// traverse genotype tree, remove / insert node
	f4_node *g2;
	if (1 == whichchild) g2 = geno->child;
	else             g2 = geno->child2;
	if (NULL == g2)
		return;
	if (g2 == repair_remove)
	{
		f4_node *oldgeno;
		geno->removeChild(g2);
		if (g2->child)
		{
			// add g2->child as child to geno
			if (1 == whichchild) geno->child = g2->child;
			else             geno->child2 = g2->child;
			g2->child->parent = geno;
		}
		oldgeno = g2;
		oldgeno->child = NULL;
		delete oldgeno;
		if (NULL == geno->child) return;
		// check this new
		repairGeno(geno, whichchild);
		return;
	}
	if (g2 == repair_parent)
	{
		geno->removeChild(g2);
		geno->addChild(repair_insert);
		repair_insert->parent = geno;
		repair_insert->child = g2;
		repair_insert->child2 = NULL;
		g2->parent = repair_insert;
	}
	// recurse
	if (g2->child)  repairGeno(g2, 1);
	if (g2->child2) repairGeno(g2, 2);
}


void f4_Cells::toF1Geno(SString &out)
{
	if (tmpcel) delete tmpcel;
	tmpcel = new f4_Cell(-1, NULL, 0, GeneProps::standard_values);
	out = "";
	toF1GenoRec(0, out);
	delete tmpcel;
}


void f4_Cells::toF1GenoRec(int curc, SString &out)
{
	int i, j, ccount;
	f4_Cell *thisti;
	f4_Cell *thneu;
	char buf[200];

	if (curc >= nc) return;

	if (T_STICK4 != C[curc]->type) return;

	thisti = C[curc];
	if (NULL != thisti->dadlink)
		*tmpcel = *(thisti->dadlink);

	// adjust length, curvedness, etc.
	tmpcel->P.propagateAlong(false);
	while (tmpcel->P.length > thisti->P.length)
	{
		tmpcel->P.executeModifier('l');
		out += "l";
	}
	while (tmpcel->P.length < thisti->P.length)
	{
		tmpcel->P.executeModifier('L');
		out += "L";
	}
	while (tmpcel->P.curvedness > thisti->P.curvedness)
	{
		tmpcel->P.executeModifier('c');
		out += "c";
	}
	while (tmpcel->P.curvedness < thisti->P.curvedness)
	{
		tmpcel->P.executeModifier('C');
		out += "C";
	}
	while (thisti->rolling > 0.0f)
	{
		rolling_dec(&(thisti->rolling));
		out += "R";
	}
	while (thisti->rolling < 0.0f)
	{
		rolling_inc(&(thisti->rolling));
		out += "r";
	}

	// output X for this stick
	out += "X";

	// neurons attached to it
	for (i = 0; i < nc; i++)
	{
		if (C[i]->type == T_NEURON4)
		{
			if (C[i]->dadlink == thisti)
			{
				thneu = C[i];
				out += "[";
				// ctrl
				if (1 == thneu->ctrl) out += "@";
				if (2 == thneu->ctrl) out += "|";
				// links
				for (j = 0; j < thneu->nolink; j++)
				{
					if (j) out += ",";
					if (NULL == thneu->links[j]->from)
					{
						// sensors
						out += thneu->links[j]->t.c_str();
					}
					else
					{
						sprintf(buf, "%d", thneu->links[j]->from->name - thneu->name);
						out += buf;
					}
					out += ":";
					// connection weight
					sprintf(buf, "%g", thneu->links[j]->w);
					out += buf;
				}
				out += "]";
			}
		}
	}

	// sticks connected to it
	if (thisti->commacount >= 2)
		out += "(";

	ccount = 1;
	for (i = 0; i < nc; i++)
	{
		if (C[i]->type == T_STICK4)
		{
			if (C[i]->dadlink == thisti)
			{
				while (ccount < (C[i])->anglepos)
				{
					ccount++;
					out += ",";
				}
				toF1GenoRec(i, out);
			}
		}
	}

	while (ccount < thisti->commacount)
	{
		ccount++;
		out += ",";
	}

	if (thisti->commacount >= 2)
		out += ")";
}



// to organize an f4 genotype in a tree structure

f4_node::f4_node()
{
	name = "?";
	parent = NULL;
	child = NULL;
	child2 = NULL;
	pos = -1;
	l1 = 0;
	i1 = 0;
	f1 = 0.0f;
}

f4_node::f4_node(string nname, f4_node *nparent, int npos)
{
	name = nname;
	parent = nparent;
	child = NULL;
	child2 = NULL;
	pos = npos;
	if (parent) parent->addChild(this);
	l1 = 0;
	i1 = 0;
	f1 = 0.0f;
}

f4_node::f4_node(char nname, f4_node *nparent, int npos)
{
	name = nname;
	parent = nparent;
	child = NULL;
	child2 = NULL;
	pos = npos;
	if (parent) parent->addChild(this);
	l1 = 0;
	i1 = 0;
	f1 = 0.0f;
}

f4_node::~f4_node()
{
	// (destroy() copied here for efficiency)
	// children are destroyed (recursively) through the destructor
	if (NULL != child2)  delete child2;
	if (NULL != child)   delete child;
}

int f4_node::addChild(f4_node *nchi)
{
	if (NULL == child)
	{
		child = nchi;
		return 0;
	}
	if (NULL == child2)
	{
		child2 = nchi;
		return 0;
	}
	return -1;
}

int f4_node::removeChild(f4_node *nchi)
{
	if (nchi == child2)
	{
		child2 = NULL;
		return 0;
	}
	if (nchi == child)
	{
		child = NULL;
		return 0;
	}
	return -1;
}

int f4_node::childCount()
{
	if (NULL != child)
	{
		if (NULL != child2) return 2;
		else return 1;
	}
	else
	{
		if (NULL != child2) return 1;
		else return 0;
	}
}

int f4_node::count()
{
	int c = 1;
	if (NULL != child)  c += child->count();
	if (NULL != child2) c += child2->count();
	return c;
}

f4_node* f4_node::ordNode(int n)
{
	int n1;
	if (0 == n) return this;
	n--;
	if (NULL != child)
	{
		n1 = child->count();
		if (n < n1) return child->ordNode(n);
		n -= n1;
	}
	if (NULL != child2)
	{
		n1 = child2->count();
		if (n < n1) return child2->ordNode(n);
		n -= n1;
	}
	return NULL;
}

f4_node* f4_node::randomNode()
{
	int n = count();
	// pick a random node, between 0 and n-1
	return ordNode(rndUint(n));
}

f4_node* f4_node::randomNodeWithSize(int mn, int mx)
{
	// try random nodes, and accept if size in range
	// limit to maxlim tries
	int i, n, maxlim;
	f4_node *nod = NULL;
	maxlim = count();
	for (i = 0; i < maxlim; i++)
	{
		nod = randomNode();
		n = nod->count();
		if ((n >= mn) && (n <= mx)) return nod;
	}
	// failed, doesn't matter
	return nod;
}

void f4_node::sprint(SString& out)
{
	char buf2[20];
	// special case: repetition code
	if (name == "#")
	{
		out += "#";
		if (i1 != 1)
		{
			sprintf(buf2, "%d", i1);
			out += buf2;
		}
	}
	else {
		// special case: neuron link
		if (name == "[")
		{
			out += "[";
			if (i1 > 0)
			{
				// sensor input
				out += s1.c_str();
			}
			else
			{
				sprintf(buf2, "%d", l1);
				out += buf2;
			}
			sprintf(buf2, ":%g]", f1);
			out += buf2;
		}
		else if (name == ":")
		{
			sprintf(buf2, ":%c%c:", l1 ? '+' : '-', (char)i1);
			out += buf2;
		}
		else if (name == "@" || name == "|")
		{
			if (parent->name == "N")
			{
				out += name.c_str();
			}
			else
			{
				out += "N:";
				out += name.c_str();
			}
		}
		else
		{
			char *temp = (char*)name.c_str();
			NeuroClass *nc = GenoOperators::parseNeuroClass(temp);
			if (nc != NULL)
			{
				out += "N:";
			}
			out += name.c_str();
		}
	}
	if (NULL != child)     child->sprint(out);
	// if two children, make sure last char is a '>'
	if (2 == childCount())
		if (0 == out[0]) out += ">"; else
			if ('>' != out[out.len() - 1]) out += ">";
	if (NULL != child2)    child2->sprint(out);
	// make sure last char is a '>'
	if (0 == out[0]) out += ">"; else
		if ('>' != out[out.len() - 1]) out += ">";
}

void f4_node::sprintAdj(char *& buf)
{
	unsigned int len;
	// build in a SString, with initial size
	SString out(strlen(buf) + 2000);
	out = "";

	sprint(out);

	// very last '>' can be omitted
	len = out.len();
	if (len > 1)
		if ('>' == out[len - 1]) { (out.directWrite())[len - 1] = 0; out.endWrite(); };
	// copy back to string
	// if new is longer, reallocate buf
	if (len + 1 > strlen(buf))
	{
		buf = (char*)realloc(buf, len + 1);
	}
	strcpy(buf, out.c_str());
}

f4_node* f4_node::duplicate()
{
	f4_node *copy;
	copy = new f4_node(*this);
	copy->parent = NULL;  // set later
	copy->child = NULL;
	copy->child2 = NULL;
	if (NULL != child)
	{
		copy->child = child->duplicate();
		copy->child->parent = copy;
	}
	if (NULL != child2)
	{
		copy->child2 = child2->duplicate();
		copy->child2->parent = copy;
	}
	return copy;
}


void f4_node::destroy()
{
	// children are destroyed (recursively) through the destructor
	if (NULL != child2)  delete child2;
	if (NULL != child)   delete child;
}

// scan genotype string and build tree
// return >1 for error (errorpos)
int f4_processrec(const char* genot, unsigned pos0, f4_node *parent)
{
	int i, j, res, t;
	char tc1, tc2, tc3; // tc3 is only to ensure that neuron parameter definition is completed
	int relfrom;
	double w;
	unsigned gpos, oldpos;
	f4_node *node1, *par;
	unsigned beginindex;
	string neutype = "";

	gpos = pos0;
	par = parent;
	if (gpos >= strlen(genot)) return 1;
	while (gpos < strlen(genot))
	{
		neutype = "";
		// first switch across cell dividers and old semantics
		switch (genot[gpos])
		{
		case '<':
		{
			// find out genotype start for child
			j = scanrec(genot + gpos + 1, strlen(genot + gpos + 1), '>');

			node1 = new f4_node("<", par, gpos);
			par = node1;
			res = f4_processrec(genot, gpos + 1, par);
			if (res) return res;
			if (gpos + j + 2 < strlen(genot))
			{
				res = f4_processrec(genot, gpos + j + 2, par);
				if (res) return res;
			}
			else // ran out
			{
				node1 = new f4_node(">", par, strlen(genot) - 1);
				par = node1;
			}
			gpos++;
			return 0;  // OK
		}
		case '>':
		{
			node1 = new f4_node(">", par, gpos);
			par = node1;
			gpos = strlen(genot);
			return 0;  // OK
		}
		case '#':
		{
			// repetition marker, 1 by default
			ExtValue val;
			const char* end = val.parseNumber(genot + gpos + 1, ExtPType::TInt);
			if (end == NULL) i = 1;
			else i = val.getInt();
			// find out genotype start for continuation
			j = scanrec(genot + gpos + 1, strlen(genot + gpos + 1), '>');
			// skip number
			oldpos = gpos;
			gpos += end - (genot + gpos);
			//gpos++;
			//while ((genot[gpos] >= '0') && (genot[gpos] <= '9')) gpos++;node1 = new f4_node("#", par, oldpos);
			node1 = new f4_node("#", par, oldpos);
			node1->i1 = i;
			par = node1;
			res = f4_processrec(genot, gpos, node1);
			if (res) return res;
			if (oldpos + j + 2 < strlen(genot))
			{
				res = f4_processrec(genot, oldpos + j + 2, node1);
				if (res) return res;
			}
			else // ran out
			{
				node1 = new f4_node(">", par, strlen(genot) - 1);
			}
			return 0;  // OK
		}
		case ' ':
		case '\n':
		case '\r':
		case '\t':
		{
			// whitespace: ignore
			gpos++;
			break;
		}
		case 'l':  case 'L':
		case 'c':  case 'C':
		case 'q':  case 'Q':
		case 'r':  case 'R':
		case 'X':  case ',':
		case 'a':  case 'A':
		case 's':  case 'S':
		case 'm':  case 'M':
		case 'i':  case 'I':
		case 'f':  case 'F':
		case 'w':  case 'W':
		case 'e':  case 'E':
		{
			node1 = new f4_node(genot[gpos], par, gpos);
			par = node1;
			gpos++;
			break;
		}
		case '@':  case '|':
		{
			// in order to prevent the presence of "free muscles", we need to ensure that a muscle is written as N@/N| or N:@/N:|
			if (par != NULL && par->name == "N")
			{
				node1 = new f4_node(genot[gpos], par, gpos);
				par = node1;
				gpos++;
			}
			else
			{
				return gpos + 1;
			}
			break;
		}

		case 'N':
		{
			// if there is no colon after N, then there is no class definition
			if (gpos + 1 >= strlen(genot) || genot[gpos + 1] != ':')
			{
				node1 = new f4_node(genot[gpos], par, gpos);
				par = node1;
				gpos++;
				break;
			}
			// if there is a colon determining neuron parameter, then let the switch case colon handle this
			else if (sscanf(genot + gpos + 1, ":%c%c%[:]", &tc1, &tc2, &tc3) == 3)
			{
				node1 = new f4_node(genot[gpos], par, gpos);
				par = node1;
				gpos++;
				break;
			}
			int forgenorange = gpos;
			gpos += 2; //skipping "N:"
			beginindex = gpos;
			char* end = (char*)genot + beginindex;
			GenoOperators::parseNeuroClass(end);
			gpos += end - genot - beginindex;
			neutype = string(genot + beginindex, genot + gpos);
			node1 = new f4_node(neutype, par, forgenorange);
			par = node1;
			break;
		}
		case ':':
		{
			// neuron parameter  +! -! += -= +/ or -/
			if (sscanf(genot + gpos, ":%c%c%[:]", &tc1, &tc2, &tc3) != 3)
				// error: incorrect format
				return gpos + 1 + 1;
			if ('+' == tc1) j = 1;
			else if ('-' == tc1) j = 0;
			else return gpos + 1 + 1;
			switch (tc2)
			{
			case '!':  case '=':  case '/':  break;
			default:
				return gpos + 1 + 1;
			}
			node1 = new f4_node(":", par, gpos);
			node1->l1 = j;
			node1->i1 = (int)tc2;
			par = node1;
			j = scanrec(genot + gpos + 1, strlen(genot + gpos + 1), ':');
			gpos += j + 2;
			break;
		}
		case '[':
		{
			const char *end = parseConnection(genot + gpos, relfrom, w);
			if (end == NULL)
			{
				end = parseConnectionWithNeuron(genot + gpos, neutype, w);
				if (end == NULL) t = -1;
				else t = 1;
			}
			else
			{
				t = 0;
			}
			node1 = new f4_node("[", par, gpos);
			node1->s1 = neutype;
			node1->i1 = t;
			node1->l1 = relfrom;
			node1->f1 = w;
			par = node1;
			j = scanrec(genot + gpos + 1, strlen(genot + gpos + 1), ']');
			gpos += j + 2;
			break;
		}
		default:
		{
			//DB( printf("unknown character '%c' ! \n", genot[gpos]); )
			//add it, build will give the error or repair
			node1 = new f4_node(genot[gpos], par, gpos);
			par = node1;
			gpos++;
			break;
		}
		}
	}

	// should end with a '>'
	if (par)
	{
		if (par->name != ">")
		{
			node1 = new f4_node('>', par, strlen(genot) - 1);
			par = node1;
		}
	}

	return 0;
}

const char* parseConnection(const char *fragm, int& relfrom, double &weight)
{
	const char *parser = fragm;
	if (*parser != '[') return NULL;
	parser++;
	ExtValue val;
	parser = val.parseNumber(parser, ExtPType::TInt);
	if (parser == NULL) return NULL;
	relfrom = val.getInt();
	if (*parser != ':') return NULL;
	parser++;
	parser = val.parseNumber(parser, ExtPType::TDouble);
	if (parser == NULL) return NULL;
	weight = val.getDouble();
	if (*parser != ']') return NULL;
	parser++;
	return parser;
}

const char* parseConnectionWithNeuron(const char *fragm, string &neutype, double &weight)
{
	const char *parser = fragm;
	if (*parser != '[') return NULL;
	parser++;
	char* p = (char*)parser;
	if (GenoOperators::parseNeuroClass(p) == NULL) return NULL;
	neutype = string(parser, (const char *)p);
	parser = p;
	if (*parser != ':') return NULL;
	parser++;
	ExtValue val;
	parser = val.parseNumber(parser, ExtPType::TDouble);
	if (parser == NULL) return NULL;
	weight = val.getDouble();
	if (*parser != ']') return NULL;
	parser++;
	return parser;
}

f4_node* f4_processtree(const char* geno)
{
	f4_node *root;
	int res;
	root = new f4_node();
	res = f4_processrec(geno, 0, root);
	if (res) return NULL;
	//DB( printf("test f4  "); )
	DB(
		if (root->child)
		{
			char* buf = (char*)malloc(300);
			DB(printf("(%d) ", root->child->count());)
				buf[0] = 0;
			root->child->sprintAdj(buf);
			DB(printf("%s\n", buf);)
				free(buf);
		}
	)
		return root->child;
}
