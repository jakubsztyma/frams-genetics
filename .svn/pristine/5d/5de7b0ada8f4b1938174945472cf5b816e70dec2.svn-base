// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "fL_oper.h"
#include <common/loggers/loggers.h>
#include "../fH/fH_oper.h"
#include <algorithm>

#define FIELDSTRUCT Geno_fL
static ParamEntry GENOfLparam_tab[] =
{
	{"Genetics: fL", 3, FL_OPCOUNT + FL_MUTGROUPSCOUNT + FL_CHG_COUNT + 2 + FL_ADD_COUNT, },
	{"Genetics: fL: Probabilities of mutating axiom and rules", },
	{"Genetics: fL: Probabilities of mutation types", },
	{"fL_maxdefinedwords", 0, 0, "Maximum number of defined words", "d 0 100 10", FIELD(maxdefinedwords), "Maximum number of words that can be defined in L-System", },

	{"fL_axm_mut_prob", 1, 0, "Axiom mutation", "f 0 1 0.2", FIELD(groupprobabilities[FL_AXM_WORD_MUT_PROB]), "Probability of performing mutation operations on axiom", },
	{"fL_rul_mut_prob", 1, 0, "Rule's successor mutation", "f 0 1 0.8", FIELD(groupprobabilities[FL_RUL_WORD_MUT_PROB]), "Probability of performing mutation operations on the successor of random rule", },

	{"fL_mut_addition", 2, 0, "Addition of word to sequence", "f 0 1 0.2", FIELD(operations[FL_ADD_WORD]), "Probability of adding random existing word to the axiom or one of successors", },

	{"fL_mut_add_stick", 2, 0, " - addition of stick", "f 0 1 0.2", FIELD(addtypes[FL_ADD_STICK]), "Probability of adding stick", },
	{"fL_mut_add_neuro", 2, 0, " - addition of neuron", "f 0 1 0.2", FIELD(addtypes[FL_ADD_NEURO]), "Probability of adding neuron", },
	{"fL_mut_add_conn", 2, 0, " - addition of neuron connection", "f 0 1 0.2", FIELD(addtypes[FL_ADD_CONN]), "Probability of adding connection", },
	{"fL_mut_add_rot", 2, 0, " - addition of rotation words", "f 0 1 0.2", FIELD(addtypes[FL_ADD_ROT]), "Probability of adding one of rotation words", },
	{"fL_mut_add_branch", 2, 0, " - addition of branched stick", "f 0 1 0.2", FIELD(addtypes[FL_ADD_BRANCH]), "Probability of adding branch with rotation and stick", },
	{"fL_mut_add_other", 2, 0, " - addition of defined words", "f 0 1 0.4", FIELD(addtypes[FL_ADD_OTHER]), "Probability of adding other word, defined in genotype", },

	{"fL_mut_worddefaddition", 2, 0, "Addition of new word definition", "f 0 1 0.05", FIELD(operations[FL_ADD_WDEF]), "Probability of adding new word definition to the genotype", },
	{"fL_mut_ruleaddition", 2, 0, "Addition of new rule definition", "f 0 1 0.1", FIELD(operations[FL_ADD_RULE]), "Probability of adding new rule definition for existing word", },
	{"fL_mut_rulecond", 2, 0, "Modification of rule condition", "f 0 1 0.1", FIELD(operations[FL_CHG_COND]), "Probability of modifying random rule condition", },

	{"fL_mut_changeword", 2, 0, "Change of random word", "f 0 1 0.3", FIELD(operations[FL_CHG_WORD]), "Probability of changing word name or formula of a random word from axiom or one of successors", },
	{"fL_mut_changeword_formula", 2, 0, " - change of formula", "f 0 1 0.7", FIELD(chgoperations[FL_CHG_WORD_FORMULA]), "Probability of changing formula in word", },
	{"fL_mut_changeword_name", 2, 0, " - change of name", "f 0 1 0.3", FIELD(chgoperations[FL_CHG_WORD_NAME]), "Probability of changing name in word", },

	{"fL_mut_changeiter", 2, 0, "Change of L-System iteration", "f 0 1 0.3", FIELD(operations[FL_CHG_ITER]), "Probability of changing number of iterations of L-Systems", },
	{"fL_mut_changeiter_step", 2, 0, "Step of iteration changing", "f 0 1 1.0", FIELD(iterchangestep), "Minimal step that should be used for changing iterations in L-Systems", },
	{"fL_mut_deletion", 2, 0, "Deletion of random word", "f 0 1 0.2", FIELD(operations[FL_DEL_WORD]), "Probability of deleting random word from axiom or random successor (also deletes rule if there is only one word in successor)", },
	{ 0, },
};
#undef FIELDSTRUCT

Geno_fL::Geno_fL()
{
	par.setParamTab(GENOfLparam_tab);
	par.select(this);
	par.setDefault();
	supported_format = 'L';
	iterchangestep = 1.0;
	maxdefinedwords = 10;
}

int Geno_fL::checkValidity(const char *geno, const char *genoname)
{
	LoggerToMemory eh(LoggerBase::Enable | LoggerToMemory::StoreAllMessages, LOG_WARN);
	fL_Builder builder(false, false);

	int err = builder.parseGenotype(geno);
	if (err != 0)
	{
		return err;
	}

	if (builder.countSticksInSequence(&builder.genotype) == 0)
	{
		return GENOPER_OPFAIL;
	}
	double neededtime = 0;
	Model *m = builder.developModel(neededtime);
	if (!m)
	{
		return GENOPER_OPFAIL;
	}
	if (!m->isValid())
	{
		delete m;
		return GENOPER_OPFAIL;
	}
	delete m;


	return GENOPER_OK;
}

int Geno_fL::validate(char *&geno, const char *genoname)
{
	LoggerToMemory eh(LoggerBase::Enable | LoggerToMemory::StoreAllMessages, LOG_WARN);
	fL_Builder builder(false, false);

	int err = builder.parseGenotype(geno);
	if (err != 0)
	{
		return err;
	}
	double neededtime = 0;
	Model *m = builder.developModel(neededtime);
	if (!m->isValid())
	{
		delete m;
		return GENOPER_OPFAIL;
	}
	if (neededtime != builder.time)
	{
		builder.time = neededtime;
		free(geno);
		geno = strdup(builder.toString().c_str());
		delete m;
		return GENOPER_OK;
	}
	delete m;
	return GENOPER_OK;
}

bool Geno_fL::addWord(std::list<fL_Word *>* list, fL_Word *definition, std::list<fL_Word *>::iterator it)
{
	fL_Word *newword = new fL_Word();
	*newword = *definition;

	// if word has parameters
	if (newword->npar > 0)
	{
		// create ParamObject that will hold parameter data
		newword->data = ParamObject::makeObject(newword->tab);
		Param par(newword->tab);
		par.select(newword->data);
		par.setDefault();
		for (int i = 0; i < par.getPropCount(); i++)
		{
			newword->parevals.push_back(NULL);
		}
		if (newword->name.startsWith("rot"))
		{
			double rot = rndDouble(2);
			MathEvaluation *eval = new MathEvaluation(0);
			eval->convertString(SString::valueOf(rot).c_str());
			newword->parevals[0] = eval;
		}
		else if (newword->name == "N")
		{
			SString det;
			NeuroClass *cls = getRandomNeuroClass();
			det = cls->getName();
			Geno_fH::mutateNeuronProperties(det);
			par.setStringById(FL_PE_NEURO_DET, det);
		}
		else if (newword->name == "C")
		{
			MathEvaluation *eval = new MathEvaluation(0);
			eval->convertString(SString::valueOf(rndDouble(2) - 1).c_str());
			newword->parevals[0] = eval;
		}
	}

	list->insert(it, newword);
	return true;
}

std::list<fL_Word *>* Geno_fL::selectRandomSequence(fL_Builder *creature, int &numparams, int &ruleid)
{
	std::list<fL_Word *> *list = NULL;
	int axiomorrules = roulette(groupprobabilities, FL_MUTGROUPSCOUNT);
	bool axiomused = axiomorrules == FL_AXM_WORD_MUT_PROB || creature->rules.size() == 0;
	if (axiomused)
	{
		list = &creature->genotype;
		numparams = 0;
		ruleid = -1;
	}
	else
	{
		int rid = rndUint(creature->rules.size());
		list = &creature->rules[rid]->objsucc;
		numparams = creature->rules[rid]->objpred->npar;
		ruleid = rid;
	}
	return list;
}

fL_Word* Geno_fL::randomWordDefinition(fL_Builder *creature, int method)
{
	if (method == FL_ADD_OTHER && creature->builtincount < (int)creature->words.size())
	{
		return creature->words[creature->wordnames[creature->builtincount + rndUint((int)creature->words.size() - creature->builtincount)]];
	}
	else
	{
		if (method == FL_ADD_OTHER) // we should be able to select stick, neuro or conn
		{
			double alttypes[FL_ADD_COUNT - 2];
			alttypes[FL_ADD_STICK] = addtypes[FL_ADD_STICK];
			alttypes[FL_ADD_NEURO] = addtypes[FL_ADD_NEURO];
			alttypes[FL_ADD_CONN] = addtypes[FL_ADD_CONN];
			alttypes[FL_ADD_ROT] = addtypes[FL_ADD_ROT];
			method = roulette(alttypes, FL_ADD_COUNT - 2);
		}
		switch (method)
		{
		case FL_ADD_STICK:
			return creature->words["S"];
		case FL_ADD_NEURO:
			if (getActiveNeuroClassCount() == 0)
				return creature->words["S"];
			else
				return creature->words["N"];
		case FL_ADD_CONN:
			return creature->words["C"];
		case FL_ADD_ROT:
		{
			int rottype = rndUint(3);
			switch (rottype)
			{
			case 0:
				return creature->words["rotX"];
			case 1:
				return creature->words["rotY"];
			case 2:
				return creature->words["rotZ"];
			}
			break;
		}
		case FL_ADD_BRANCH:
			// return NULL
			break;
		}
	}
	return NULL;
}

void Geno_fL::deleteBranch(std::list<fL_Word *> *list, std::list<fL_Word *>::iterator openbranchposition)
{
	fL_Branch *branch = (fL_Branch *)(*openbranchposition);
	if (branch->btype == fL_Branch::BranchType::OPEN)
	{
		int bcount = 1;
		delete (*openbranchposition);
		openbranchposition = list->erase(openbranchposition);
		for (; openbranchposition != list->end(); openbranchposition++)
		{
			if ((*openbranchposition)->type == fLElementType::BRANCH)
			{
				branch = (fL_Branch *)(*openbranchposition);
				if (branch->btype == fL_Branch::BranchType::OPEN)
				{
					bcount++;
				}
				else
				{
					bcount--;
					if (bcount == 0)
					{
						delete branch;
						list->erase(openbranchposition);
						break;
					}
				}
			}
		}
	}
	else
	{
		openbranchposition++;
		if (openbranchposition != list->end())
		{
			delete (*openbranchposition);
			list->erase(openbranchposition);
		}
	}
}

int Geno_fL::mutate(char *&geno, float& chg, int &method)
{
	fL_Builder *creature = new fL_Builder(false, false);

	if (creature->parseGenotype(geno) != 0)
	{
		delete creature;
		return GENOPER_OPFAIL;
	}

	int before = creature->countWordsInLSystem();

	method = roulette(operations, FL_OPCOUNT);
	switch (method)
	{
		case FL_CHG_ITER:
		{
			if (rndUint(2) == 0)
			{
				creature->time = creature->time + iterchangestep <= ExtValue::getDouble(FL_MAXITER) ?
						creature->time + iterchangestep : creature->time - iterchangestep;
			}
			else
			{
				creature->time = creature->time - iterchangestep >= 0 ?
						creature->time - iterchangestep : creature->time + iterchangestep;
			}
			break;
		}
		case FL_CHG_COND:
		{
			if (creature->rules.size() > 0)
			{
				int ruleid = rndUint(creature->rules.size());
				if (!creature->rules[ruleid]->condeval)
				{
					creature->rules[ruleid]->condeval = new MathEvaluation(creature->rules[ruleid]->objpred->npar);
				}
				creature->rules[ruleid]->condeval->mutateConditional();
				break;
			}
			// if there are no rules - create one
		}
		[[fallthrough]];
		case FL_ADD_RULE:
		{
			std::unordered_map<std::string, fL_Word *>::iterator pred = creature->words.begin();
			std::vector<fL_Word *> wordswithnorules;
			for (; pred != creature->words.end(); pred++)
			{
				if (!pred->second->builtin)
				{
					bool norules = true;
					for (fL_Rule * r : creature->rules)
					{
						if (pred->second->name == r->objpred->name &&
								pred->second->npar == r->objpred->npar)
						{
							norules = false;
							break;
						}
					}
					if (norules)
					{
						wordswithnorules.push_back(pred->second);
					}
				}
			}
			if (wordswithnorules.size() > 0)
			{
				int predid = rndUint(wordswithnorules.size());
				fL_Rule *newrule = new fL_Rule(0,0);
				fL_Word *pred = new fL_Word();
				*pred = *wordswithnorules[predid];
				newrule->objpred = pred;
				fL_Word *initdef = randomWordDefinition(creature, roulette(addtypes, FL_ADD_COUNT - 1)); // -1 to avoid branching
				addWord(&newrule->objsucc, initdef, newrule->objsucc.begin());
				creature->rules.push_back(newrule);
				break;
			}
			else if (creature->rules.size() > 0)
			{
				int ruleid = rndUint(creature->rules.size());
				fL_Rule *newrule = new fL_Rule(0, 0);
				fL_Word *pred = new fL_Word();
				*pred = *creature->rules[ruleid]->objpred;
				newrule->objpred = pred;
				if (creature->rules[ruleid]->condeval)
				{
					std::string formula = "";
					creature->rules[ruleid]->condeval->RPNToInfix(formula);
					if (formula.find("1.0-(") != 0)
					{
						std::string res = "1.0-(";
						res += formula;
						res += ")";
						newrule->condeval = new MathEvaluation(pred->npar);
						newrule->condeval->convertString(res);
					}
					else
					{
						newrule->condeval = new MathEvaluation(pred->npar);
						newrule->condeval->mutateConditional();
					}
				}
				else
				{
					newrule->condeval = new MathEvaluation(pred->npar);
					newrule->condeval->mutateConditional();
				}
				fL_Word *worddef = randomWordDefinition(creature, roulette(addtypes, FL_ADD_COUNT - 1));
				addWord(&newrule->objsucc, worddef, newrule->objsucc.begin());
				creature->rules.push_back(newrule);
				break;
			}
			// if there are no words, from which rules can be formed, then add one
		}
		[[fallthrough]];
		case FL_ADD_WDEF:
		{
			if (creature->countDefinedWords() <= maxdefinedwords)
			{
				int npar = rndUint(ExtValue::getInt(FL_MAXPARAMS, false));
				for (int i = 0; i < maxdefinedwords; i++)
				{
					std::string name = "w";
					name += std::to_string(i);
					if (creature->words.find(name) == creature->words.end())
					{
						fL_Word *word = new fL_Word(false, 0, 0);
						word->npar = npar;
						word->name = name.c_str();
						word->processDefinition(creature);
						break;
					}
				}
				break;
			}
			//no break at the end of case - if there is too many words, then
			// deletion should be performed
		}
		[[fallthrough]];
		case FL_DEL_WORD:
		{
			int numpars = 0;
			int ruleid = 0;
			std::list<fL_Word *> *list = selectRandomSequence(creature, numpars, ruleid);
			if (ruleid == -1 && creature->countSticksInSequence(list) == 1)
			{
				if (list->size() > 1)
				{
					int rndid = rndUint(list->size() - 1);
					int j = 0;
					std::list<fL_Word *>::iterator it = list->begin();
					if ((*it)->name == "S")
					{
						it++;
					}
					while (it != list->end() && j < rndid && ((*it)->name == "S"))
					{
						if ((*it)->name != "S")
						{
							j++;
						}
						it++;
					}
					if (it != list->end())
					{
						if ((*it)->type == fLElementType::BRANCH)
						{
							deleteBranch(list, it);
						}
						else
						{
							delete (*it);
							list->erase(it);
						}
						break;
					}
					// else add word
				}
				// else add word
			}
			else
			{
				int rndid = rndUint(list->size());
				std::list<fL_Word *>::iterator it = list->begin();
				std::advance(it, rndid);
				if ((*it)->type == fLElementType::BRANCH)
				{
					deleteBranch(list, it);
				}
				else
				{
					delete (*it);
					list->erase(it);
				}
				if (ruleid > -1 && creature->rules[ruleid]->objsucc.size() == 0)
				{
					delete creature->rules[ruleid];
					creature->rules.erase(creature->rules.begin() + ruleid);
				}
				break;
			}
			// if no words available, then add word
		}
		[[fallthrough]];
		case FL_ADD_WORD:
		{
			int numpars = 0;
			int tmp = 0;
			std::list<fL_Word *> *list = selectRandomSequence(creature, numpars, tmp);
			int rndid = rndUint(list->size());
			std::list<fL_Word *>::iterator it = list->begin();
			std::advance(it, rndid);
			int meth = roulette(addtypes, FL_ADD_COUNT);
			if (tmp == -1)
			{ // if sequence is axiom and it does not have non-builtin words
				bool hasdefined = false;
				for (std::list<fL_Word *>::iterator elem = list->begin(); elem != list->end(); elem++)
				{
					if (!(*elem)->builtin)
					{
						hasdefined = true;
						break;
					}
				}
				if (!hasdefined)
				{
					meth = FL_ADD_OTHER;
				}

			}
			if (meth != FL_ADD_BRANCH)
			{
				fL_Word *worddef = randomWordDefinition(creature, meth);
				addWord(list, worddef, it);
			}
			else
			{
				fL_Branch *start = new fL_Branch(fL_Branch::BranchType::OPEN, 0, 0);
				list->insert(it, start);
				int rottype = rndUint(2);
				switch (rottype)
				{
				case 0:
					addWord(list, creature->words["rotY"], it);
				case 1:
					addWord(list, creature->words["rotZ"], it);
				}
				addWord(list, creature->words["S"], it);
				fL_Branch *end = new fL_Branch(fL_Branch::BranchType::CLOSE, 0, 0);
				list->insert(it, end);
			}
			break;
		}
		case FL_CHG_WORD:
		{
			int numpars = 0;
			int tmp = 0;
			std::list<fL_Word *> *list = selectRandomSequence(creature, numpars, tmp);
			int rndid = rndUint(list->size());
			std::list<fL_Word *>::iterator selectedword = list->begin();
			std::advance(selectedword, rndid);
			if ((*selectedword)->type == fLElementType::BRANCH)
			{
				break;
			}
			int chgtype = roulette(chgoperations, FL_CHG_COUNT);
			if (creature->countSticksInSequence(list) == 1 && tmp == -1) // if sequence is axiom
			{
				fL_Word *worddef = randomWordDefinition(creature, roulette(addtypes, FL_ADD_COUNT - 1));

				int numpars = 0;
				std::list<fL_Word *> *list = selectRandomSequence(creature, numpars, tmp);
				int rndid = rndUint(list->size());
				std::list<fL_Word *>::iterator it = list->begin();
				std::advance(it, rndid);

				addWord(list, worddef, it);

				break;
			}
			else if (chgtype == FL_CHG_WORD_NAME)
			{
				if ((*selectedword)->builtin)
				{
					delete (*selectedword);
					selectedword = list->erase(selectedword);
					fL_Word *worddef = randomWordDefinition(creature, roulette(addtypes, FL_ADD_COUNT - 1));
					addWord(list, worddef, selectedword);
				}
				else
				{
					std::vector<fL_Word *> available;
					for (std::unordered_map<std::string, fL_Word *>::iterator wit = creature->words.begin();
							wit != creature->words.end(); wit++)
					{
						if ((*selectedword)->npar == wit->second->npar &&
								(*selectedword)->name != wit->second->name &&
								!wit->second->builtin)
						{
							available.push_back(wit->second);
						}
					}
					if (available.size() > 0)
					{
						int newnameid = rndUint(available.size());
						(*selectedword)->name = available[newnameid]->name;
					}
					else
					{
						delete (*selectedword);
						selectedword = list->erase(selectedword);
						fL_Word *worddef = randomWordDefinition(creature, roulette(addtypes, FL_ADD_COUNT - 1));
						addWord(list, worddef, selectedword);
					}
				}
			}
			else
			{
				if ((*selectedword)->npar > 0)
				{
					int randeval = rndUint((*selectedword)->npar);
					Param par((*selectedword)->tab, (*selectedword)->data);
					if ((*selectedword)->builtin && (*selectedword)->name == "N"
							&& strcmp(par.id(randeval), FL_PE_NEURO_DET) == 0)
					{
						SString res = par.getStringById(FL_PE_NEURO_DET);
						Geno_fH::mutateNeuronProperties(res);
						par.setStringById(FL_PE_NEURO_DET, res);
					}
					else if ((*selectedword)->builtin &&
							(*selectedword)->name == "C" &&
							strcmp(par.id(randeval), FL_PE_CONN_ATTR) == 0)
					{
						SString strattractor = par.getStringById(FL_PE_CONN_ATTR);
						if (strattractor.len() > 0)
						{
							fL_Word *w = NULL;
							creature->createWord(strattractor, w, numpars, 0, 0);
							// mutate attractor parameter
							if (w->npar > 0)
							{
								int rndattr = rndUint(w->npar);
								if (!w->parevals[rndattr])
								{
									w->parevals[rndattr] = new MathEvaluation(numpars);
								}
								w->parevals[rndattr]->mutate(false, false);
							}
							strattractor = w->stringify(true);
							par.setStringById(FL_PE_CONN_ATTR, strattractor);
							delete w;
						}
						else
						{
							if (creature->builtincount < (int)creature->words.size())
							{
								fL_Word *wdef = randomWordDefinition(creature, FL_ADD_OTHER);
								fL_Word *w = new fL_Word();
								*w = *wdef;
								w->data = ParamObject::makeObject(w->tab);
								Param apar(w->tab);
								apar.select(w->data);
								apar.setDefault();
								if (w->npar > 0)
								{
									int rndattr = rndUint(w->npar);
									for (int i = 0; i < w->npar; i++)
									{
										if (i == rndattr)
										{
											MathEvaluation *ev = new MathEvaluation(numpars);
											ev->mutate(false, false);
											w->parevals.push_back(ev);
										}
										else
										{
											w->parevals.push_back(NULL);
										}
									}

								}
								strattractor = w->stringify(false);
								par.setStringById(FL_PE_CONN_ATTR, strattractor);
								delete w;
							}
						}
					}
					else
					{
						if (!(*selectedword)->parevals[randeval])
						{
							(*selectedword)->parevals[randeval] = new MathEvaluation(numpars);
						}
						(*selectedword)->parevals[randeval]->mutate(false, iterchangestep != 1.0);
					}
				}
			}
			break;
		}
	}

	free(geno);
	geno = strdup(creature->toString().c_str());
	chg = (double)abs(before - creature->countWordsInLSystem()) / before;
	delete creature;

	return GENOPER_OK;
}

fL_Word* Geno_fL::getAppropriateWord(fL_Builder *from, fL_Builder *to, fL_Word *fromword, std::unordered_map<std::string, std::string> &map)
{
	if (fromword->name == "[" || fromword->name == "]") // if words are branching words
	{
		fL_Branch *newword = new fL_Branch(fromword->name == "[" ? fL_Branch::BranchType::OPEN : fL_Branch::BranchType::CLOSE, 0, 0);
		return newword;
	}
	if (fromword->builtin)
	{
		fL_Word *newword = new fL_Word();
		(*newword) = (*to->words[fromword->name.c_str()]);
		return newword;
	}
	if (map.find(fromword->name.c_str()) != map.end()) // if word is already mapped
	{
		fL_Word *newword = new fL_Word();
		(*newword) = (*to->words[map[fromword->name.c_str()]]);
		return newword;
	}
	else if (to->words.find(fromword->name.c_str()) != to->words.end() &&
			to->words[fromword->name.c_str()]->npar == fromword->npar) // if there is already same word with same number of parameters
	{
		fL_Word *newword = new fL_Word();
		map[fromword->name.c_str()] = fromword->name.c_str();
		(*newword) = (*to->words[map[fromword->name.c_str()]]);
		return newword;
	}
	for (std::unordered_map<std::string, fL_Word *>::iterator it = to->words.begin();
			it != to->words.end(); it++)
	{ // find word with same number of parameters
		if (fromword->npar == it->second->npar && map.find(fromword->name.c_str()) == map.end() && !it->second->builtin)
		{ // if there is a word with same number of parameters
			map[fromword->name.c_str()] = it->second->name.c_str();
			fL_Word *newword = new fL_Word();
			(*newword) = (*it->second);
			return newword;
		}
	}
	fL_Word *newworddef = new fL_Word();
	(*newworddef) = (*fromword);
	newworddef->parevals.clear();
	if (to->words.find(newworddef->name.c_str()) != to->words.end())
	{
		int i = 0;
		while (true)
		{
			std::string name = "w";
			name += std::to_string(i);
			if (to->words.find(name) == to->words.end())
			{
				newworddef->name = name.c_str();
				break;
			}
			i++;
		}
	}
	newworddef->processDefinition(to);
	map[fromword->name.c_str()] = newworddef->name.c_str();
	fL_Word *newword = new fL_Word();
	(*newword) = (*to->words[map[fromword->name.c_str()]]);
	return newword;
}

void Geno_fL::migrateRandomRules(fL_Builder *from, fL_Builder *to, int numselrules)
{
	std::unordered_map<std::string, std::string> map;
	if (from->rules.size() > 0)
	{
		for (int i = 0; i < numselrules; i++)
		{
			int rulid = rndUint(from->rules.size());
			fL_Rule *rul = from->rules[rulid];
			fL_Rule *newrule = new fL_Rule(0, 0);
			newrule->objpred = getAppropriateWord(from, to, rul->objpred, map);
			for (fL_Word *w : rul->objsucc)
			{
				fL_Word *el = getAppropriateWord(from, to, w, map);
				if (el->type == fLElementType::BRANCH)
				{
					newrule->objsucc.push_back(el);
					continue;
				}
				Param origpar(w->tab);
				origpar.select(w->data);
				el->data = ParamObject::makeObject(el->tab);
				Param par(el->tab);
				par.select(el->data);
				par.setDefault();
				for (int i = 0; i < el->npar; i++)
				{
					std::string form;
					if (w->builtin && w->name == "N"
							&& strcmp(par.id(i), FL_PE_NEURO_DET) == 0)
					{
						SString res = origpar.getStringById(FL_PE_NEURO_DET);
						par.setStringById(FL_PE_NEURO_DET, res);
						el->parevals.push_back(NULL);
					}
					else if (w->builtin && w->name == "C"
							&& strcmp(par.id(i), FL_PE_CONN_ATTR) == 0)
					{
						SString strattractor = origpar.getStringById(FL_PE_CONN_ATTR);
						if (strattractor.len() > 0)
						{
							fL_Word *tmp = NULL;
							from->createWord(strattractor, tmp, newrule->objpred->npar, 0, 0);
							fL_Word *newsuccword = getAppropriateWord(from, to, tmp, map);
							newsuccword->data = ParamObject::makeObject(el->tab);
							newsuccword->parevals = tmp->parevals;
							tmp->parevals.clear();
							strattractor = newsuccword->stringify(true);
							par.setStringById(FL_PE_CONN_ATTR, strattractor);
							delete newsuccword;
							delete tmp;
						}
						par.setStringById(FL_PE_CONN_ATTR, strattractor);
						el->parevals.push_back(NULL);
					}
					else if (w->parevals[i])
					{
						MathEvaluation *eval = new MathEvaluation(newrule->objpred->npar);
						w->parevals[i]->RPNToInfix(form);
						eval->convertString(form);
						el->parevals.push_back(eval);
					}
					else
					{
						el->parevals.push_back(NULL);
					}
				}
				newrule->objsucc.push_back(el);
			}
			to->rules.push_back(newrule);
		}
	}
}

int Geno_fL::crossOver(char *&g1, char *&g2, float& chg1, float& chg2)
{
	fL_Builder *creature1 = new fL_Builder(false, false);
	fL_Builder *creature1template = new fL_Builder(false, false);
	fL_Builder *creature2 = new fL_Builder(false, false);
	fL_Builder *creature2template = new fL_Builder(false, false);

	int count1 = creature1->countWordsInLSystem();
	int count2 = creature2->countWordsInLSystem();

	if (creature1->parseGenotype(g1) != 0 || creature2->parseGenotype(g2) != 0)
	{
		delete creature1;
		delete creature2;
		delete creature1template;
		delete creature2template;
		return GENOPER_OPFAIL;
	}

	creature1template->parseGenotype(g1);
	creature2template->parseGenotype(g2);

	int numselrules = 1 + rndUint(XOVER_MAX_MIGRATED_RULES);
	numselrules = numselrules < (int)creature1->rules.size() ? numselrules : (int)creature1->rules.size();

	migrateRandomRules(creature1template, creature2, numselrules);

	numselrules = 1 + rndUint(XOVER_MAX_MIGRATED_RULES);
	numselrules = numselrules < (int)creature1->rules.size() ? numselrules : (int)creature1->rules.size();

	migrateRandomRules(creature2template, creature1, numselrules);

	free(g1);
	free(g2);

	g1 = strdup(creature1->toString().c_str());
	g2 = strdup(creature2->toString().c_str());

	chg1 = (double)count1 / creature1->countWordsInLSystem();
	chg1 = (double)count2 / creature2->countWordsInLSystem();

	delete creature1;
	delete creature2;
	delete creature1template;
	delete creature2template;

	return GENOPER_OK;
}

uint32_t Geno_fL::style(const char *geno, int pos)
{
	char ch = geno[pos];
	uint32_t style = GENSTYLE_CS(0, GENSTYLE_STRIKEOUT);
	if (pos == 0 || geno[pos - 1] == '\n' || ch == ':') // single-character line definition
	{
		style = GENSTYLE_CS(GENCOLOR_TEXT, GENSTYLE_BOLD);
	}
	else if (strchr("()", ch) != NULL)
	{
		style = GENSTYLE_RGBS(50, 50, 50, GENSTYLE_BOLD);
	}
	else if (isalpha(ch)) // properties name
	{
		style = GENSTYLE_RGBS(0, 200, 0, GENSTYLE_BOLD);
	}
	else if (isdigit(ch) || strchr(",.=", ch)) // properties values
	{
		style = GENSTYLE_CS(GENCOLOR_TEXT, GENSTYLE_NONE);
	}
	else if (ch == '\"')
	{
		style = GENSTYLE_RGBS(200, 0, 0, GENSTYLE_BOLD);
	}

	return style;
}
