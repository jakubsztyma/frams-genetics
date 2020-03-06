// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

// Copyright (C) 1999,2000  Adam Rotaru-Varga (adam_rotaru@yahoo.com), GNU LGPL
// Copyright (C) since 2001 Maciej Komosinski
// 2018, Grzegorz Latosinski, added support for new API for neuron types and their properties

#include "f4_conv.h"
#include <common/log.h>
#include "../genooperators.h" //for GENOPER_OK constant

#ifdef DMALLOC
#include <dmalloc.h>
#endif


GenoConv_f40::GenoConv_f40()
{
	name = "Developmental encoding";
	in_format = '4';
	out_format = '0';
	mapsupport = 1;
}


SString GenoConv_f40::convert(SString &in, MultiMap *map, bool using_checkpoints)
{
	int res;
	f4_Model *model = new f4_Model();
	res = model->buildFromF4(in, using_checkpoints);
	if (GENOPER_OK != res)
	{
		delete model;
		return SString();  // oops
	}
	if (NULL != map)
		// generate to-f0 conversion map
		model->getCurrentToF0Map(*map);
	SString out = model->getF0Geno().getGenes();
	delete model;
	return out;
}


GenoConv_F41_TestOnly::GenoConv_F41_TestOnly()
{
	name = "Only for testing, approximate f4->f1 converter"; //Do not use in production! (adam)
	in_format = '4';
	out_format = '1';
	mapsupport = 0;
}


SString GenoConv_F41_TestOnly::convert(SString &in, MultiMap *map, bool using_checkpoints)
{
	int res;
	f4_Model *model = new f4_Model();
	res = model->buildFromF4(in, using_checkpoints);
	if (GENOPER_OK != res)
	{
		delete model;
		return SString();  // oops
	}
	SString out;
	model->toF1Geno(out);
	delete model;
	return out;
}


f4_Model::f4_Model() : Model()
{
	cells = NULL;
}

f4_Model::~f4_Model()
{
	if (cells) delete cells;
}

int f4_Model::buildFromF4(SString &geno, bool using_checkpoints)
{
	int i;

	error = GENOPER_OK;
	errorpos = -1;

	// build cells, and simulate
	if (cells) delete cells;
	cells = new f4_Cells(geno, 0);
	if (GENOPER_OK != cells->geterror())
	{
		error = cells->geterror();
		errorpos = cells->geterrorpos();
		//delete cells;
		return error;
	}

	cells->simulate();
	if (GENOPER_OK != cells->geterror())
	{
		error = cells->geterror();
		errorpos = cells->geterrorpos();
		return error;
	}

	// reset recursive traverse flags
	for (i = 0; i < cells->nc; i++)
		cells->C[i]->recProcessedFlag = 0;

	open(using_checkpoints); // begin model build

	// process every cell
	int res;
	for (i = 0; i < cells->nc; i++)
	{
		res = buildModelRec(cells->C[i]);
		if (res)
		{
			logMessage("f4_Model", "buildModelRec", 2, "Error in building Model");
			error = res;
			break;
		}
	}

	res = close();
	if (0 == res) // invalid
		error = -10;

	return error;
}


f4_Cell* f4_Model::getStick(f4_Cell *C)
{
	if (T_STICK4 == C->type) return C;
	if (NULL != C->dadlink)
		return getStick(C->dadlink);
	// we have no more dadlinks, find any stick
	for (int i = 0; i < cells->nc; i++)
		if (cells->C[i]->type == T_STICK4)
			return cells->C[i];
	// none!
	logMessage("f4_Model", "getStick", 2, "Not a single stick");
	return NULL;
}


/// updated by Macko to follow new SDK standards (no more neuroitems)
int f4_Model::buildModelRec(f4_Cell *C)
{
	int partidx;
	int j, res;
	MultiRange range;

	if (C->recProcessedFlag)
		// already processed
		return 0;

	// mark it processed
	C->recProcessedFlag = 1;

	// make sure parent is a stick
	if (NULL != C->dadlink)
		if (C->dadlink->type != T_STICK4)
		{
		C->dadlink = getStick(C->dadlink);
		}

	// make sure its parent is processed first
	if (NULL != C->dadlink)
	{
		res = buildModelRec(C->dadlink);
		if (res) return res;
	}

	char tmpLine[100];

	range = C->genoRange;
	if (C->type == T_STICK4)
	{
		int jj_p1_refno;  // save for later
		// first end is connected to dad, or new
		if (C->dadlink == NULL)
		{
			// new part object for firstend
			// coordinates are left to be computed by Model
			sprintf(tmpLine, "fr=%g,ing=%g,as=%g",
				/*1.0/C->P.mass,*/ C->P.friction, C->P.ingestion, C->P.assimilation
				//C->firstend.x, C->firstend.y, C->firstend.z
				);
			partidx = addFromString(PartType, tmpLine, &range);
			if (partidx < 0) return -1;
			this->checkpoint();
			jj_p1_refno = partidx;
		}
		else {
			// adjust mass/vol of first endpoint
			jj_p1_refno = C->dadlink->p2_refno;
			Part *p1 = getPart(jj_p1_refno);
			p1->mass += 1.0;
			//      p1->volume += 1.0/C->P.mass;
		}
		// new part object for lastend
		sprintf(tmpLine, "fr=%g,ing=%g,as=%g",
			//C->lastend.x, C->lastend.y, C->lastend.z
			/*"vol=" 1.0/C->P.mass,*/ C->P.friction, C->P.ingestion, C->P.assimilation
			);
		partidx = addFromString(PartType, tmpLine, &range);
		if (partidx < 0) return -2;
		C->p2_refno = partidx;

		// new joint object
		// check that the part references are valid
		int jj_p2_refno = C->p2_refno;
		if ((jj_p1_refno < 0) || (jj_p1_refno >= getPartCount())) return -11;
		if ((jj_p2_refno < 0) || (jj_p2_refno >= getPartCount())) return -12;
		sprintf(tmpLine, "p1=%d,p2=%d,dx=%g,dy=0,dz=0,rx=%g,ry=0,rz=%g"\
			",stam=%g",
			jj_p1_refno, jj_p2_refno,
			// relative position -- always (len, 0, 0), along the stick
			// this is optional!
			C->P.length,
			// relative rotation
			C->xrot, C->zrot,
			//C->P.ruch,   // rotstif
			C->P.stamina
			);
		partidx = addFromString(JointType, tmpLine, &range);
		if (partidx < 0) return -13;
		this->checkpoint();
		C->joint_refno = partidx;
	}

	if (C->type == T_NEURON4) ///<this case was updated by MacKo
	{
		const char* nclass = C->neuclass->name.c_str();
		int partno, jointno;
		if (C->neuclass->getPreferredLocation() == 0)
		{
			if (strcmp(nclass, "N") == 0)
			{
				partno = C->dadlink->p2_refno;
				if ((partno < 0) || (partno >= getPartCount())) return -21;
				else sprintf(tmpLine, "p=%d,d=\"N:in=%g,fo=%g,si=%g\"", partno, C->inertia, C->force, C->sigmo);
			}
			else
			{
				sprintf(tmpLine, "d=\"%s\"", nclass);
			}
			partidx = addFromString(NeuronType, tmpLine, &range);
			if (partidx < 0) return -22;
			this->checkpoint();
			C->neuro_refno = partidx;
		}
		else if (C->neuclass->getPreferredLocation() == 1) // attached to Part or have no required attachment - also part
		{
			partno = C->dadlink->p2_refno;
			if ((partno < 0) || (partno >= getPartCount())) return -21;
			if (strcmp(nclass, "N") == 0)
			{
				sprintf(tmpLine, "p=%d,d=\"N:in=%g,fo=%g,si=%g\"", partno, C->inertia, C->force, C->sigmo);
			}
			else
			{
				sprintf(tmpLine, "p=%d,d=\"%s\"", partno, nclass);
			}
			partidx = addFromString(NeuronType, tmpLine, &range);
			if (partidx < 0) return -22;
			this->checkpoint();
			C->neuro_refno = partidx;
		}
		else // attached to Joint
		{
			jointno = C->dadlink->joint_refno;
			sprintf(tmpLine, "n:j=%d,d=\"%s\"", jointno, nclass);
			partidx = addFromString(NeuronType, tmpLine, &range);
			if (partidx < 0) return -32;
			this->checkpoint();
		}
		C->neuro_refno = partidx;
		int n_refno = C->neuro_refno;

		if ((strcmp(nclass,"N") == 0) && C->ctrl)
		{
			if (1 == C->ctrl)
				sprintf(tmpLine, "j=%d,d=\"@:p=%g\"", C->dadlink->joint_refno, C->P.muscle_power);
			else
				sprintf(tmpLine, "j=%d,d=\"|:p=%g,r=%g\"", C->dadlink->joint_refno, C->P.muscle_power, C->mz);
			partidx = addFromString(NeuronType, tmpLine, &range);
			if (partidx < 0) return -32;
			sprintf(tmpLine, "%d,%d", partidx, n_refno);
			if (addFromString(NeuronConnectionType, tmpLine, &range) < 0) return -33;
			this->checkpoint();
		}

		for (j = 0; j < C->nolink; j++)
		{
			if (NULL != C->links[j]->from)
				buildModelRec(C->links[j]->from);

			tmpLine[0] = 0;
			if (C->links[j]->from == NULL)
			{
				const char* nclass = C->links[j]->t.c_str();
				char* temp = (char*)C->links[j]->t.c_str();
				NeuroClass *sensortest = GenoOperators::parseNeuroClass(temp);
				//backward compatibility for G*TS
				if (C->links[j]->t == "*" || C->links[j]->t == "S" || C->links[j]->t == "T")
				{
					partno = C->dadlink->p2_refno;
					sprintf(tmpLine, "p=%d,d=\"%s\"", partno, nclass);
				}
				else if (C->links[j]->t == "G")
				{
					jointno = C->dadlink->joint_refno;
					sprintf(tmpLine, "j=%d,d=\"%s\"", jointno, nclass);
				}				
				else if (sensortest->getPreferredLocation() == 0)
				{
					sprintf(tmpLine, "d=\"%s\"",nclass);
				}
				else if (sensortest->getPreferredLocation() == 1)
				{
					partno = C->dadlink->p2_refno;
					sprintf(tmpLine, "p=%d,d=\"%s\"", partno, nclass);
				}
				else
				{
					jointno = C->dadlink->joint_refno;
					sprintf(tmpLine, "j=%d,d=\"%s\"", jointno, nclass);
				}

			}
			int from = -1;
			if (tmpLine[0]) //input from receptor
			{
				from = addFromString(NeuronType, tmpLine, &range);
				if (from < 0) return -34;
				this->checkpoint();
			} /*could be 'else'...*/

			if (NULL != C->links[j]->from) // input from another neuron
				from = C->links[j]->from->neuro_refno;
			if (from >= 0)
			{
				sprintf(tmpLine, "%d,%d,%g", n_refno, from, C->links[j]->w);
				if (addFromString(NeuronConnectionType, tmpLine, &range) < 0) return -35;
				this->checkpoint();
			}
		}
	}
	return 0;
}


void f4_Model::toF1Geno(SString &out)
{
	cells->toF1Geno(out);
}
