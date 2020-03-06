// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2017  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

// Copyright (C) 1999,2000  Adam Rotaru-Varga (adam_rotaru@yahoo.com), GNU LGPL
// Copyright (C) since 2001 Maciej Komosinski
// 2018, Grzegorz Latosinski, added support for new API for neuron types and their properties

#include "f4_oper.h"
#include <frams/util/sstring.h>
#include <common/log.h>

#include <stdio.h>
#include <stdlib.h>
#include "common/nonstd_math.h"
#include <string.h>


const char *Geno_f4::all_modifiers = F14_MODIFIERS ","; //comma in f4 is handled the same way (simple node, F4_ADD_SIMP) as modifiers

// codes that can be changed (apart from being added/deleted)
#define MUT_CHAN_CODES "<[#"
#define REP_MAXCOUNT 19

#define FIELDSTRUCT Geno_f4

static ParamEntry GENO4param_tab[] =
{
	{ "Genetics: f4", 1, F4_COUNT + F4_ADD_COUNT + 1, },
	{ "f4_mut_add", 0, 0, "Add node", "f 0 100 50", FIELD(prob[F4_ADD]), "mutation: probability of adding a node", },
	{ "f4_mut_add_div", 0, 0, "- add division", "f 0 100 20", FIELD(probadd[F4_ADD_DIV]), "add node mutation: probability of adding a division", },
	{ "f4_mut_add_conn", 0, 0, "- add connection", "f 0 100 15", FIELD(probadd[F4_ADD_CONN]), "add node mutation: probability of adding a neural connection", },
	{ "f4_mut_add_neupar", 0, 0, "- add neuron property", "f 0 100 5", FIELD(probadd[F4_ADD_NEUPAR]), "add node mutation: probability of adding a neuron property/modifier", },
	{ "f4_mut_add_rep", 0, 0, "- add repetition", "f 0 100 10", FIELD(probadd[F4_ADD_REP]), "add node mutation: probability of adding a repetition", },
	{ "f4_mut_add_simp", 0, 0, "- add simple node", "f 0 100 50", FIELD(probadd[F4_ADD_SIMP]), "add node mutation: probability of adding a random, simple gene", },
	{ "f4_mut_del", 0, 0, "Delete node", "f 0 100 20", FIELD(prob[F4_DEL]), "mutation: probability of deleting a node", },
	{ "f4_mut_mod", 0, 0, "Modify node", "f 0 100 30", FIELD(prob[F4_MOD]), "mutation: probability of changing a node", },
	{ "f4_mut_exmod", 1, 0, "Excluded modifiers", "s 0 30", FIELD(excluded_modifiers), "Modifiers that will not be added nor deleted during mutation\n(all: " F14_MODIFIERS ")", },
	{ 0, },
};

#undef FIELDSTRUCT


Geno_f4::Geno_f4()
{
	supported_format = '4';
	par.setParamTab(GENO4param_tab);
	par.select(this);
	par.setDefault();

	mutation_method_names = new const char*[F4_COUNT + F4_ADD_COUNT - 1];
	int index = 0;
	mutation_method_names[index++] = "added division";
	mutation_method_names[index++] = "added neural connection";
	mutation_method_names[index++] = "added neuron property";
	mutation_method_names[index++] = "added repetition gene";
	mutation_method_names[index++] = "added a simple node";
	mutation_method_names[index++] = "deleted a node";
	mutation_method_names[index++] = "modified a node";
	if (index != F4_COUNT + F4_ADD_COUNT - 1) logMessage("Geno_f4", "Constructor", 3, "Mutation names init error");
}

void Geno_f4::setDefaults()
{
	excluded_modifiers = F14_MODIFIERS_RARE F14_MODIFIERS_VISUAL;
}

int Geno_f4::ValidateRec(f4_node *geno, int retrycount) const
{
	// ! the genotype is geno->child (not geno) !
	// build from it with repair on

	f4_Cells cells(geno->child, 1);
	cells.simulate();  //we should simulate?!

	// errors not fixed:
	if (GENOPER_OPFAIL == cells.geterror())
	{
		if (cells.geterrorpos() >= 0) return 1 + cells.geterrorpos();
		return GENOPER_OPFAIL;
	}
	// errors can be fixed
	if (GENOPER_REPAIR == cells.geterror())
	{
		cells.repairGeno(geno, 1);
		// note: geno might have been fixed
		// check again
		int res2 = GENOPER_OK;
		if (retrycount > 0)
			res2 = ValidateRec(geno, retrycount - 1);

		if (res2 == GENOPER_OK) return GENOPER_REPAIR;
		return res2;
	}
	// no errors:
	return GENOPER_OK;
}


int Geno_f4::validate(char *& geno, const char *genoname)
{
	// convert geno to tree, then try to validate 20 times
	f4_node root;
	if (f4_processrec(geno, 0, &root) || root.childCount() != 1) return GENOPER_OK; // cannot repair
	if (ValidateRec(&root, 20) == GENOPER_REPAIR) // if repaired, make it back to string
	{
		geno[0] = 0;
		root.child->sprintAdj(geno);
	}
	return GENOPER_OK;
}


int Geno_f4::checkValidity(const char* geno, const char *genoname)
{
	f4_node root;
	int res = f4_processrec(geno, 0, &root);
	if (res) return res;  // errorpos, >0
	if (root.childCount() != 1) return 1; //earlier: GENOPER_OPFAIL
	f4_Cells cells(root.child, 0);
	cells.simulate();
	if (cells.geterror() == GENOPER_OPFAIL || cells.geterror() == GENOPER_REPAIR)
	{
		if (cells.geterrorpos() >= 0) return 1 + cells.geterrorpos();
		else return 1; //earlier: GENOPER_OPFAIL;
	}
	else return GENOPER_OK;
}


int Geno_f4::MutateOne(f4_node *& g, int &method) const
{
	// ! the genotype is g->child (not g) !

	// do the mutation
	// pick a random node
	f4_node *n1 = g->child->randomNode();
	vector<NeuroClass*> neulist;
	//DB( printf("%c\n", n1->name); )
	int neuronid = -1;

	switch (roulette(prob, F4_COUNT))
	{
	case F4_ADD:
	{
		// add a node
		switch (method = roulette(probadd, F4_ADD_COUNT))
		{
		case F4_ADD_DIV:
		{
			// add division ('<')
			f4_node *n3 = n1->parent;
			n3->removeChild(n1);
			f4_node *n2 = new f4_node('<', n3, n3->pos);
			n2->addChild(n1);
			// new cell is stick or neuron
			// "X>" or "N>"
			f4_node *n5 = NULL;
			double pr = rndDouble(1);
			pr -= 0.5;
			if (pr < 0) n5 = new f4_node('X', n2, n2->pos);
			else
			{
				// make neuron
				NeuroClass *rndclass = GenoOperators::getRandomNeuroClass();
				if (rndclass == NULL)
				{
					n5 = new f4_node('X', n2, n2->pos);
				}
				else
				{
					f4_node *n4 = new f4_node(rndclass->getName().c_str(), n2, n2->pos);
					if (rndclass->getPreferredInputs() != 0)
					{
						neuronid = -1;
						for (int i = 0; i < g->count(); i++)
						{
							f4_node *gcur = g->ordNode(i);
							char* temp = (char*)gcur->name.c_str();
							NeuroClass *neuclass = GenoOperators::parseNeuroClass(temp);
							if (neuclass != NULL)
							{
								neulist.push_back(neuclass);
							}
							if (g->ordNode(i) == n3)
							{
								neuronid = neulist.size() - 1;
							}
						}
						if (neuronid == -1)
						{
							return GENOPER_OPFAIL;
						}
						n5 = new f4_node('[', n4, n2->pos);
						linkNodeMakeRandom(n5, neuronid, neulist);
					}
					else {
						n5 = n4;
					}
				}
			}
			new f4_node('>', n5, n5->pos);
			n1->parent = n2;
			// now with 50% chance swap children
			if (rndUint(2) == 0)
			{
				n3 = n2->child;
				n2->child = n2->child2;
				n2->child2 = n3;
			}
		}
			break;
		case F4_ADD_CONN:
		{
			// add link
			f4_node *par = n1->parent;
			char* temp = (char*)par->name.c_str();
			NeuroClass *neuclass = GenoOperators::parseNeuroClass(temp);
			if (neuclass != NULL)
			{
				n1->parent->removeChild(n1);
				f4_node *n2 = new f4_node('[', n1->parent, n1->parent->pos);
				n2->addChild(n1);
				n1->parent = n2;
				neuronid = -1;
				for (int i = 0; i < g->count(); i++)
				{
					f4_node *gcur = g->ordNode(i);
					temp = (char*)gcur->name.c_str();
					NeuroClass *neuclass = GenoOperators::parseNeuroClass(temp);
					if (neuclass != NULL)
					{
						neulist.push_back(neuclass);
					}
					if (gcur == par)
					{
						neuronid = neulist.size() - 1;
					}
				}
				if (neuronid == -1)
				{
					return GENOPER_OPFAIL;
				}
				linkNodeMakeRandom(n2, neuronid, neulist);
			}
		}
			break;
		case F4_ADD_NEUPAR:
		{
			// add neuron modifier
			n1->parent->removeChild(n1);
			f4_node *n2 = new f4_node(':', n1->parent, n1->parent->pos);
			nparNodeMakeRandom(n2);
			n2->addChild(n1);
			n1->parent = n2;
		}
			break;
		case F4_ADD_REP:
		{
			// add repetition ('#')
			// repeated code (left child) is the original, right child is empty, count is 2
			f4_node *n3 = n1->parent;
			n3->removeChild(n1);
			f4_node *n2 = new f4_node('#', n3, n3->pos);
			n2->i1 = 2;
			n2->addChild(n1);
			new f4_node('>', n2, n2->pos);
			n1->parent = n2;
		}
			break;
		case F4_ADD_SIMP:
		{
			// add simple node
			// choose a simple node from ADD_SIMPLE_CODES
			n1->parent->removeChild(n1);
			//f4_node *n2 = new f4_node(ADD_SIMPLE_CODES[rndUint(strlen(ADD_SIMPLE_CODES))], n1->parent, n1->parent->pos);
			int modifierid = GenoOperators::getRandomChar(all_modifiers, excluded_modifiers.c_str());
			f4_node *n2 = new f4_node(all_modifiers[modifierid], n1->parent, n1->parent->pos);
			n2->addChild(n1);
			n1->parent = n2;
		}
			break;
		}
	}
		break;

	case F4_DEL:
	{
		method = F4_ADD_COUNT - 1 + F4_DEL;
		// delete a node
		// must pick a node with parent, and at least one child
		// already picked a node, but repeat may be needed
		for (int i = 0; i < 10; i++)
		{
			if ((NULL != n1->parent) && (g != n1->parent))
				if (NULL != n1->child)
					break;
			// try a new one
			n1 = g->child->randomNode();
		}
		if ((NULL != n1->parent) && (g != n1->parent))
		{
			switch (n1->childCount())
			{
			case 0: break;
			case 1:  // one child
			{
				f4_node *n2 = n1->parent;
				n2->removeChild(n1);
				if (NULL != n1->child)
				{
					n1->child->parent = n2;
					n2->addChild(n1->child);
					n1->child = NULL;
				}
				if (NULL != n1->child2)
				{
					n1->child2->parent = n2;
					n2->addChild(n1->child2);
					n1->child2 = NULL;
				}
				// destroy n1
				n1->parent = NULL;
				delete n1;
			}
				break;

			case 2:  // two children
			{
				// two children
				f4_node *n2 = n1->parent;
				n2->removeChild(n1);
				// n1 has two children. pick one randomly 50-50, destroy other
				if (rndUint(2) == 0)
				{
					n1->child->parent = n2;
					n2->addChild(n1->child);
					n1->child = NULL;
					n1->child2->parent = NULL;
				}
				else
				{
					n1->child2->parent = n2;
					n2->addChild(n1->child2);
					n1->child2 = NULL;
					n1->child->parent = NULL;
				}
				// destroy n1
				n1->parent = NULL;
				delete n1;
			}
				break;
			}
		}
		else return GENOPER_OPFAIL;
	}
		break;
	case F4_MOD:
	{
		method = F4_ADD_COUNT - 1 + F4_MOD;
		// change a node
		// the only nodes that are modifiable are MUT_CHAN_CODES
		// try to get a modifiable node
		// already picked a node, but repeat may be needed
		int i = 0;
		while (1)
		{
			if (strchr(MUT_CHAN_CODES, n1->name[0])) break;
			// try a new one
			n1 = g->child->randomNode();
			i++;
			if (i >= 20) return GENOPER_OPFAIL;
		}
		switch (n1->name[0])
		{
		case '<':
		{
			// swap children
			f4_node *n2 = n1->child; n1->child = n1->child2; n1->child2 = n2;
		}
			break;
		case '[':
		{
			neuronid = -1;
			for (int i = 0; i < g->count(); i++)
			{
				f4_node *gcur = g->ordNode(i);
				char *temp = (char*)gcur->name.c_str();
				NeuroClass *neuclass = GenoOperators::parseNeuroClass(temp);
				if (neuclass != NULL)
				{
					neulist.push_back(neuclass);
				}
				if (gcur == n1)
				{
					neuronid = neulist.size() - 1;
				}
			}
			if (neuronid == -1)
			{
				return GENOPER_OPFAIL;
			}
			linkNodeChangeRandom(n1, neuronid, neulist);
		}
			break;

		case '#':
		{
			repeatNodeChangeRandom(n1);
		}
			break;
		}
	}
		break;

	default: //no mutations allowed?
		return GENOPER_OPFAIL;
	}

	return GENOPER_OK;
}

// make a random [ node
void Geno_f4::linkNodeMakeRandom(f4_node *nn, int neuid, vector<NeuroClass*> neulist) const
{
	float prob1;
	NeuroClass *nc = NULL;

	// 35% chance one of *GTS
	prob1 = rndDouble(1);
	prob1 -= 0.35f;
	if (prob1 < 0)
	{
		// '*', 'G', 'T', or 'S', 1/4 chance each
		nc = GenoOperators::getRandomNeuroClassWithOutputAndNoInputs();
	}
	if (nc != NULL)
	{
		nn->i1 = 1;
		nn->s1 = nc->getName().c_str();
		nn->l1 = 0;
	}
	else
	{
		// relative input link
		int id = GenoOperators::getRandomNeuroClassWithOutput(neulist);
		int relid = neuid - id;
		nn->l1 = relid;
		//nn->l1 = (int)(4.0f * (rndDouble(1) - 0.5f));
	}
	// weight
	nn->f1 = GenoOperators::mutateNeuProperty(nn->f1, NULL, -1);
	//nn->f1 = 10.0f * (rndDouble(1) - 0.5f);
}

// change a [ node
void Geno_f4::linkNodeChangeRandom(f4_node *nn, int neuid, std::vector<NeuroClass*> neulist) const      //rewritten by M.K. - should work as before (not tested)
{
	double probs[3] = { 0.1, 0.3, 0.6 };
	NeuroClass *cl;
	// 10% change type
	// 30% change link
	// 60% change weight

	switch (roulette(probs, 3))
	{
	case 0: // change type
		// 80% for link, 20% for random sensor
		if (rndDouble(1) < 0.2f)
		{
			cl = GenoOperators::getRandomNeuroClassWithOutputAndNoInputs();
			if (cl != NULL)
			{
				nn->i1 = 1;
				nn->s1 = cl->name.c_str();
				nn->l1 = 0;
			}
		}
		break;
	case 1: // change link
		if (0 == nn->i1) // relative input link
		{
			int id = GenoOperators::getRandomNeuroClassWithOutput(neulist);
			nn->l1 = neuid - id;
		}
		//nn->l1 += (int)(2.0f * (rndDouble(1) - 0.5f));
		break;
	case 2: // change weight
		nn->f1 = GenoOperators::mutateNeuProperty(nn->f1, NULL, -1);
		//nn->f1 += 1.0f * (rndDouble(1) - 0.5f);
		break;
	}
}

// make a random : node
void Geno_f4::nparNodeMakeRandom(f4_node *nn) const
{
	int sign = (int)rndDouble(2);
	int param = (int)rndDouble(3);
	if (param > 2) param = 2;
	nn->l1 = sign;
	nn->i1 = "!=/"[param];
}

// change a repeat # node
void Geno_f4::repeatNodeChangeRandom(f4_node *nn) const
{
	int count;
	float prob1;

	// change count
	count = nn->i1;
	prob1 = rndDouble(1);
	if (prob1 < 0.5f) count++;
	else count--;
	if (count < 1) count = 1;
	if (count > REP_MAXCOUNT) count = REP_MAXCOUNT;
	nn->i1 = count;
}


int Geno_f4::MutateOneValid(f4_node *& g, int &method) const
// mutate one, until a valid genotype is obtained
{
	// ! the genotype is g->child (not g) !
	int i, res;
	f4_node *gcopy = NULL;
	// try this max 20 times:
	//   copy, mutate, then validate

	for (i = 0; i < 20; i++)
	{
		gcopy = g->duplicate();

		res = MutateOne(gcopy, method);

		if (GENOPER_OK != res)
		{
			// mutation failed, try again
			delete gcopy;
			continue;  // for
		}
		// try to validate it
		res = ValidateRec(gcopy, 10);
		// accept if it is OK, or was repaired
		if (GENOPER_OK == res)
			//(GENOPER_REPAIR == res)
		{
			// destroy the original one
			g->destroy();
			// make it the new one
			*g = *gcopy;
			gcopy->child = NULL;
			gcopy->child2 = NULL;
			delete gcopy;
			res = GENOPER_OK;
			goto retm1v;
		}
		delete gcopy;
	}
	// attempts failed
	res = GENOPER_OPFAIL;
retm1v:
	return res;
}


int Geno_f4::mutate(char *& g, float & chg, int &method)
{
	f4_node *root = new f4_node;
	if (f4_processrec(g, 0, root) || root->childCount() != 1)
	{
		delete root;
		return GENOPER_OPFAIL;
	} // could not convert or bad: fail
	// mutate one node, set chg as this percent
	chg = 1.0 / float(root->child->count());
	if (MutateOneValid(root, method) != GENOPER_OK)
	{
		delete root;
		return GENOPER_OPFAIL;
	}
	// OK, convert back to string
	g[0] = 0;
	root->child->sprintAdj(g);
	delete root;
	return GENOPER_OK;
}


/*
int Geno_f4::MutateMany(char *& g, float & chg)
// check if original is valid, then
// make a number of mutations
{
int res, n, i;
int totNodes = 0;
int maxToMut = 0;

// convert to tree
f4_node *root;
root = new f4_node();
res = f4_processrec(g, 0, root);
if (res) {
// could not convert, fail
goto retm;
}
if (1 != root->childCount()) {
res = GENOPER_OPFAIL;
goto retm;
}

// check if original is valid
res = ValidateRec( root, 20 );
// might have been repaired!
if (GENOPER_REPAIR==res) {
res = GENOPER_OK;
}
if (GENOPER_OK != res) {
goto retm;
}

// decide number of nodes to mutate
// decide maximum number of nodes to mutate: 0.25*nodes, min 2
totNodes = root->child->count();
maxToMut = (int)( 0.25f * totNodes);
if (maxToMut<2) maxToMut=2;
if (maxToMut>totNodes) maxToMut=totNodes;

// decide number of nodes to mutate
n = (int)( 0.5f + rndDouble(1) * maxToMut );
if (n<1) n=1;
if (n>totNodes) n=totNodes;
// set chg as this percent
chg = ((float)n) / ((float)totNodes);
for (i=0; i<n; i++)
{
res = MutateOneValid(root);
if (GENOPER_OK != res)
{
res = GENOPER_OPFAIL;
goto retm;
}
}
// OK, convert back to string
g[0]=0;
root->child->sprintAdj(g);
retm:
delete root;
return res;
}
*/


int Geno_f4::CrossOverOne(f4_node *g1, f4_node *g2, float chg) const
{
	// ! the genotypes are g1->child and g2->child (not g1 g2) !
	// single offspring in g1
	int smin, smax;
	float size;
	f4_node *n1, *n2, *n1p, *n2p;

	// determine desired size
	size = (1 - chg) * (float)g1->count();
	smin = (int)(size*0.9f - 1);
	smax = (int)(size*1.1f + 1);
	// get a random node with desired size
	n1 = g1->child->randomNodeWithSize(smin, smax);

	// determine desired size
	size = (1 - chg) * (float)g2->count();
	smin = (int)(size*0.9f - 1);
	smax = (int)(size*1.1f + 1);
	// get a random node with desired size
	n2 = g2->child->randomNodeWithSize(smin, smax);

	// exchange the two nodes:
	n1p = n1->parent;
	n2p = n2->parent;
	n1p->removeChild(n1);
	n1p->addChild(n2);
	n2p->removeChild(n2);
	n2p->addChild(n1);
	n1->parent = n2p;
	n2->parent = n1p;

	return GENOPER_OK;
}

int Geno_f4::crossOver(char *&g1, char *&g2, float &chg1, float &chg2)
{
	f4_node root1, root2, *copy1, *copy2;

	// convert genotype strings into tree structures
	if (f4_processrec(g1, 0, &root1) || (root1.childCount() != 1)) return GENOPER_OPFAIL;
	if (f4_processrec(g2, 0, &root2) || (root2.childCount() != 1)) return GENOPER_OPFAIL;

	// decide amounts of crossover, 0.25-0.75
	// adam: seems 0.1-0.9 -- MacKo
	chg1 = 0.1 + rndDouble(0.8);
	chg2 = 0.1 + rndDouble(0.8);

	copy1 = root1.duplicate();
	if (CrossOverOne(copy1, &root2, chg1) != GENOPER_OK) { delete copy1; copy1 = NULL; }
	copy2 = root2.duplicate();
	if (CrossOverOne(copy2, &root1, chg2) != GENOPER_OK) { delete copy2; copy2 = NULL; }

	g1[0] = 0;
	g2[0] = 0;
	if (copy1) { copy1->child->sprintAdj(g1); delete copy1; }
	if (copy2) { copy2->child->sprintAdj(g2); delete copy2; }
	if (g1[0] || g2[0]) return GENOPER_OK; else return GENOPER_OPFAIL;
}

uint32_t Geno_f4::style(const char *g, int pos)
{
	char ch = g[pos];

	// style categories
#define STYL4CAT_MODIFIC F14_MODIFIERS ","
#define STYL4CAT_NEUMOD "[]:+-/!="
#define STYL4CAT_NEUSPECIAL "|@*"
#define STYL4CAT_DIGIT "0123456789."
#define STYL4CAT_REST "XN<># "

	if (!isalpha(ch) && !strchr(STYL4CAT_MODIFIC STYL4CAT_NEUMOD STYL4CAT_NEUSPECIAL STYL4CAT_DIGIT STYL4CAT_REST "\t", ch))
	{
		return GENSTYLE_CS(0, GENSTYLE_INVALID);
	}
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_STRIKEOUT); //default, should be changed below
	if (strchr("X ", ch))                    style = GENSTYLE_CS(0, GENSTYLE_NONE);
	else if (strchr("N", ch))                style = GENSTYLE_RGBS(0, 200, 0, GENSTYLE_NONE);
	else if (strchr("<", ch))                style = GENSTYLE_RGBS(0, 0, 200, GENSTYLE_BOLD);
	else if (strchr(">", ch))                style = GENSTYLE_RGBS(0, 0, 100, GENSTYLE_NONE);
	else if (strchr(STYL4CAT_DIGIT, ch))     style = GENSTYLE_RGBS(100, 100, 100, GENSTYLE_NONE);
	else if (strchr(STYL4CAT_MODIFIC, ch))   style = GENSTYLE_RGBS(100, 100, 100, GENSTYLE_NONE);
	else if (strchr(STYL4CAT_NEUMOD, ch))    style = GENSTYLE_RGBS(0, 150, 0, GENSTYLE_NONE);
	if (isalpha(ch) || strchr(STYL4CAT_NEUSPECIAL, ch))
	{
		while (pos > 0)
		{
			pos--;
			if (!(isalpha(g[pos]) || strchr(STYL4CAT_NEUSPECIAL, ch)))
			{
				if (isupper(g[pos + 1]) && (g[pos] == ':' || g[pos] == '[')) // name of neuron class
					style = GENSTYLE_RGBS(150, 0, 150, GENSTYLE_ITALIC);
				else // property
					style = GENSTYLE_RGBS(100, 100, 100, GENSTYLE_NONE);
			}
		}
	}
	return style;
}
