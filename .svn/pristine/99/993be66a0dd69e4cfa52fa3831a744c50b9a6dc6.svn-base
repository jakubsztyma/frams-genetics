/*
 *  geno_f8.cpp
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-06-07.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "geno_f8.h"
#include <cstdlib>
//#include <sys/time.h>
#include "conv_f8tof1.h"
#include "multimap.h"
#include <iostream>
#include <string>
#include "sstringutils.h"
#include "conv_f8_utils.h"

#define GENO_F8_DEBUG 1 //0 - off, 1 - on

#define FIELDSTRUCT Geno_f8

static ParamEntry GENO8param_tab[]=
{
{"Genetics: f8",1,F8_OPERATION_COUNT,},
{"f8_mut_chg_begin_arg", 0, 0, "Change beginning argument", "f 0 100 7", FIELD(operation[F8_CHANGE_BEGINNING_ARG]),"mutation: probability of changing a beginning argument", },
{"f8_mut_chg_arg", 0, 0, "Change argument", "f 0 100 7", FIELD(operation[F8_CHANGE_ARG]),"mutation: probability of changing a production's argument", },
{"f8_mut_del_comm", 0, 0, "Delete command", "f 0 100 8", FIELD(operation[F8_DELETE_COMMAND]),"mutation: probability of deleting a command", },
{"f8_mut_insert_comm", 0, 0, "Insert commands", "f 0 100 8", FIELD(operation[F8_INSERT_COMMANDS]),"mutation: probability of inserting commands", },
{"f8_mut_enc", 0, 0, "Encapsulate commands", "f 0 100 8",FIELD(operation[F8_ENCAPSULATE]),"mutation: probability of encapsulating commands", },
{"f8_mut_chg_cond_sign", 0, 0, "Change condition sign", "f 0 100 7",FIELD(operation[F8_CHANGE_CONDITION_SIGN]),"mutation: probability of changing a condition sign", },
{"f8_mut_add_param", 0, 0, "Add parameter", "f 0 100 8", FIELD(operation[F8_ADD_PARAMETER]),"mutation: probability of adding a parameter to the production", },
{"f8_mut_add_cond", 0, 0, "Add condition", "f 0 100 8", FIELD(operation[F8_ADD_CONDITION]),"mutation: probability of adding a condition to the subproduction", },
{"f8_mut_add_subprod", 0, 0, "Add subproduction", "f 0 100 8", FIELD(operation[F8_ADD_SUBPRODUCTION]),"mutation: probability of adding a subproduction", },
{"f8_mut_chg_iter_number", 0, 0, "Change iteration number", "f 0 100 7", FIELD(operation[F8_CHANGE_ITERATIONS_NUMBER]),"mutation: probability of changing a number of iterations", },
{"f8_mut_del_param", 0, 0, "Delete parameter", "f 0 100 8", FIELD(operation[F8_DELETE_PARAMETER]),"mutation: probability of deleting a parameter", },
{"f8_mut_del_cond", 0, 0, "Delete condition", "f 0 100 8", FIELD(operation[F8_DELETE_CONDITION]),"mutation: probability of deleting a condition", },
{"f8_mut_add_loop", 0, 0, "Add loop", "f 0 100 0", FIELD(operation[F8_ADD_LOOP]),"mutation: probability of adding a loop", },
{"f8_mut_del_loop", 0, 0, "Delete loop", "f 0 100 0", FIELD(operation[F8_DELETE_LOOP]),"mutation: probability of deleting a loop", },
{"f8_mut_del_prod", 0, 0, "Delete production", "f 0 100 8", FIELD(operation[F8_DELETE_PRODUCTION]),"mutation: probability of deleting a production", },
{0,},
};

#undef FIELDSTRUCT

ProductionInfo::ProductionInfo(SString name, int paramCount) {
	this->name = name;
	this->paramCount = paramCount;
}

Geno_f8::Geno_f8()
{
	supported_format = '8';

        for(int i=0;i<strlen(GenoConv_F8ToF1::simpleprods);i++)
	        this->simpleCommandLetters.push_back(GenoConv_F8ToF1::simpleprods[i]);
                
	this->converter = new GenoConv_F8ToF1();

	par.setParamTab(GENO8param_tab);
	par.select(this);
	par.setDefault();

	/*mutation_method_names = new char*[F8_OPERATION_COUNT - 1]; //FIXME
	int index = 0;
	mutation_method_names[index++]="changed beginning argument";
	mutation_method_names[index++]="changed argument";
	mutation_method_names[index++]="deleted command";
	mutation_method_names[index++]="inserted command";
	mutation_method_names[index++]="encapsulated command";
	mutation_method_names[index++]="changed condition sign";
	mutation_method_names[index++]="added parameter";
	mutation_method_names[index++]="added condition";
	mutation_method_names[index++]="added subproduction";
	mutation_method_names[index++]="changed iterations number";
	mutation_method_names[index++]="deleted parameter";
	mutation_method_names[index++]="deleted condition";
	mutation_method_names[index++]="added loop";
	mutation_method_names[index++]="deleted loop";
	*/
	
#ifdef GENO_F8_DEBUG > 0
	for (int i = 0; i < F8_OPERATION_COUNT; i++) {
		this->operation[i] = 1.0 / (double)F8_OPERATION_COUNT;
	}
#endif
}

Geno_f8::~Geno_f8() {
	delete this->converter;
}


char* Geno_f8::getSimplest() {
	return "1\n---\nP0\nP0():X\n";
}

int Geno_f8::checkValidity(const char * geno) {
	SString ssgeno = SString(geno);
	SString f1ssgeno = this->converter->convert(ssgeno, NULL);
	const char* f1genosrc = f1ssgeno;
	Geno f1geno(f1genosrc);
	if (/*this->converter->checkSyntax(geno) &&*/ f1geno.isValid()) {
		return GENOPER_OK;
	} else {
		return 1;
	}
}

int Geno_f8::validate(char *& geno) {
	SString in = geno;
	SString validated = "";
	SString line;
	int pos = 0;
	bool firstLine = true;
	bool beforeDelim = true;
	bool justAfterDelim = false;
	while (in.getNextToken(pos, line, '\n')) {
		if (firstLine) {
			firstLine = false;
			const char* str = geno;
			std::istringstream i(str);
			double x;
			if (i >> x) {
				validated += line + "\n";
			} else {
				validated += "10\n";
			}
		} else if (beforeDelim) {
			if (line[0] == 'P' && line.indexOf('(') != -1 && line.indexOf(')') != -1 && line.indexOf(':') != -1) {
				validated += "---\n";
				validated += line + "\n";
				beforeDelim = false;
			}
			int eqSignPos = line.indexOf('=');
			if (line[0] == 'n' && eqSignPos != -1) {
				validated += line + "\n";
			} else if (line == SString("---")) {
				validated += line + "\n";
				beforeDelim = false;
				justAfterDelim = true;
			}
		} else if (justAfterDelim && line[0] == 'P') {
			const char* digits = "0123456789";
			SString s = line.substr(1);
			bool ok = true;
			for (int i = 0; i < s.len(); i++) {
				if (!strContainsOneOf(s(i), digits)) {
					ok = false;
					break;
				}
			}
			if (ok) {
				validated += line +"\n";
			}
			justAfterDelim = false;
		} else if (line[0] == 'P' && line.indexOf('(') != -1 && line.indexOf(')') != -1 && line.indexOf(':') != -1) {
			validated += line + "\n";
		}
	}
	free(geno); //must take care of the original allocation
	char* validatedTmp = (char*)malloc(strlen(validated) + 1); //allocate for mutated genotype
	strcpy(validatedTmp, validated); //the rest is originalg = mutated;
	geno = validatedTmp;

	return GENOPER_OK;
}

unsigned long Geno_f8::style(const char *g, int pos)
{
   char ch=g[pos];
   unsigned long style=GENSTYLE_CS(0,GENSTYLE_NONE); //default, should be changed below
   if (strchr("^[]X",ch))          style=GENSTYLE_RGBS(0,0,0,GENSTYLE_BOLD); else
   if (strchr(":",ch))             style=GENSTYLE_RGBS(220,0,0,GENSTYLE_BOLD); else
   if (strchr("|",ch))             style=GENSTYLE_RGBS(220,0,0,GENSTYLE_NONE);
   return style;
}


int Geno_f8::getProductionsCount(const SString &in) {
	int pos = 0;
	SString line;
	int counter = 0;
	bool beforeDelim = true;
	while (in.getNextToken(pos, line, '\n')) {
		if (line.startsWith("---")) {
			beforeDelim = false;
			//also skip the start production
			in.getNextToken(pos, line, '\n');
			continue;
		} else if (beforeDelim) {
			continue;
		} else {
			counter++;
		}				
	}
	return counter;
}

vector<ProductionInfo> Geno_f8::getProductionsInfo(const SString &in) {
	vector<ProductionInfo> productions;
	SString line;
	int pos = 0;
	bool beforeFirstProd = true;
	SString firstProdName;
	while (in.getNextToken(pos, line, '\n')) {
		if (line.startsWith("P") && line.indexOf('(', 0) == -1) {
			firstProdName = line;
			beforeFirstProd = false;
			continue;
		}
		if (beforeFirstProd) {
			continue;
		}
		int lParenIndex = line.indexOf('(', 0);
		int rParenIndex = line.indexOf(')', 0);
		if (line[0] == 'P' && lParenIndex != -1 && rParenIndex != -1) {
			ProductionInfo info;
			SString prodName = line.substr(0, lParenIndex);
			info.name = prodName;
			info.paramCount = 0;
			//if (line[lParenIndex + 1] == 'n') {
			SString strParams = line.substr(lParenIndex + 1, rParenIndex - lParenIndex - 1);
			int pos2 = 0;
			SString tok;
			while (strParams.getNextToken(pos2, tok, ',')) {
				info.paramCount++;
				info.paramNames.push_back(tok);
			}
			//}
			info.isFirstProduction = (prodName == firstProdName) ? true : false;
			productions.push_back(info);
		}
	}
	return productions;
}

SString Geno_f8::mutateChangeBeginningArg(SString &in, float& chg) {
	SString mutated, line, before, mid, after, tmp;
	int counter = 0;
	int pos = 0;
	bool afterDelim = false;
	bool firstLine = true;
	while (in.getNextToken(pos, line, '\n')) {
		if (firstLine) {
			firstLine = false;
			before += line + "\n";
			continue;
		}
		if (afterDelim) {
			after += line + "\n";
			continue;
		} 
		if (line.startsWith("---")) {
			afterDelim = true;
			after += line + "\n";					
		} else {
			mid += line + "\n";
			counter++;
		}				
	}
	if (counter == 0) {
		chg = 0.0;
		mutated = in;
	} else {
		int randNbr = randomN(counter);
		pos = 0;
		counter = 0;
		while (mid.getNextToken(pos, line, '\n')) {
			if (counter++ == randNbr) {
				int pos2 = 0;
				SString tok;
				line.getNextToken(pos2, tok, '=');
				tmp += tok + "=";
				line.getNextToken(pos2, tok, '=');
				double arg = parseDouble(tok);
				arg += (double) (randomN(11) - 5); //-5..5
				tmp += SString::valueOf(arg) + "\n";
				chg = (float) (SString::valueOf(arg).len()) / (float) in.len();
			} else {
				tmp += line + "\n";
			}
		}
		mid = tmp;
		mutated += before;
		mutated += mid;
		mutated += after;
	}
	return mutated;
}

SString Geno_f8::mutateChangeArg(SString &in, float& chg) {
	SString mutated;

	Lsystem *newLsystem = this->converter->createLsystem(in);
	if (newLsystem == NULL) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
	
	vector<Production*> interestingProductions;
	
	for (map<string, Production*>::iterator prodIter = newLsystem->productions.begin();
		 prodIter != newLsystem->productions.end(); prodIter++) {
		if (prodIter->second->parameters.size() > 0) {
			interestingProductions.push_back(prodIter->second);
		}
	}
	
	if (interestingProductions.size() > 0) {
		int rnd = randomN(interestingProductions.size());
		
		Production *randomProduction = interestingProductions[rnd];
		SString prodName = randomProduction->name;
		
		int paramIndex = randomN(randomProduction->parameters.size());
		
		for (map<string, Production*>::iterator prodIter = newLsystem->productions.begin();
			 prodIter != newLsystem->productions.end(); prodIter++) {
			vector<SubProduction> *subproductions = &(prodIter->second->subproductions);
			for (vector<SubProduction>::iterator subProdIter = subproductions->begin();
				 subProdIter != subproductions->end(); subProdIter++) {
				SubProduction *sp = &(*subProdIter);
				for (vector<ActionStrP>::iterator actionIter = sp->actions.begin();
					 actionIter != sp->actions.end(); actionIter++) {
					if ((*actionIter).action != NULL && (*actionIter).action->name == prodName && (*actionIter).params.size() > paramIndex) {
						SString param = (*actionIter).params[paramIndex];
						int counter = 0;
						int pos = 0;
						SString tok;
						while (in.getNextToken(pos, tok, ',')) {
							counter++;
						}
						int rnd = randomN(counter);
						pos = 0;
						counter = 0;
						SString newParam = "";
						if (randomN(2) == 0 || prodIter->second->parameters.size() == 0) {
							int rnd2 = randomN(5) + 1;
							newParam += param + SString::valueOf(rnd2) + ";";
						} else {
							SString paramName = prodIter->second->parameters.getParameterName(randomN(prodIter->second->parameters.size())+1);
							newParam += param + paramName + ";";
						}
						newParam += (rnd == 0) ? SString("+;") : SString("-;");
						(*actionIter).params[paramIndex] = newParam;
						goto label;
					}
				}
			}
		}
	label:
		chg = 2.0 / (float) in.len();
	}
	
	mutated = newLsystem->toString();
	
	delete newLsystem;
	
	return mutated;
}

SString Geno_f8::mutateDeleteCommand(SString &in, float& chg) {
	SString mutated;
	
	Lsystem *lsystem = this->converter->createLsystem(in);
	if (lsystem == NULL) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
	
	map<string, Production*>::iterator prodIter = lsystem->productions.begin();
	for (int i = 0; i < randomN(lsystem->productions.size()); i++) {
		if (i != 0) {
			prodIter++;
		}
	}
	
	Production *randomProduction = prodIter->second;
	if (randomProduction->subproductions.size() > 0) {
		vector<ActionStrP> *actions = &(randomProduction->subproductions.at(randomN(randomProduction->subproductions.size())).actions);
		int rnd = randomN(actions->size());
		if (actions->size() > 0) {
			Action *a = actions->at(rnd).action;
			if (a != NULL) {
				chg = (float) (a->getF8Representation().len()) / (float) in.len();
				actions->erase(actions->begin() + rnd);
			}
		}
	}
	
	mutated = lsystem->toString();
	
	delete lsystem;
	
	return mutated;
}

SString Geno_f8::mutateInsertCommands(SString &in, float& chg) {
	SString mutated;
	
	//cout << "insertCommands 1" << endl;
	Lsystem *lsystem = this->converter->createLsystem(in);
	if (lsystem == NULL) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
	
	//cout << "insertCommands 2" << endl;
	vector<Action*> actions = lsystem->getAllActions(true, true, false, false);
	//cout << "insertCommands 3" << endl;
	
	int random1 = randomN(lsystem->productions.size());
	
	map<string, Production*>::iterator prodIter = lsystem->productions.begin();
	for (int counter = 0; counter < random1; counter++) {
		prodIter++;
	}
	//cout << "insertCommands 4" << endl;
	
	Production *p = prodIter->second;
	SubProduction *sp;
	if (p->subproductions.size() > 0) {
		sp = &(p->subproductions.at(randomN(p->subproductions.size())));
	} else {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
	
	int commandsToInsert = /*randomN(3) +*/ 1;
	int insertLen = 0;
	for (int i = 0; i < commandsToInsert; i++) {
		ActionStrP a;
		if (actions.size() > 0) {
			a.action = actions.at(randomN(actions.size()));
		} else {
			mutated += in;
			chg = -1.0;
			return mutated;
		}
		insertLen += a.action->getF8Representation().len();
		if (a.action->ignoreParams == false && a.action->name[0] == 'P') {
			Production *p = (Production*) a.action;
			insertLen += 2;
			for (int j = 0; j < p->parameters.size(); j++) {
				int rnd = randomN(p->parameters.size() + 1);
				if (rnd == 0) {
					a.params.push_back(SString("0;"));
					insertLen += 1;
				} else {
					SString s = p->parameters.getParameterName(randomN(p->parameters.size()) + 1);
					a.params.push_back(s + ";");
					insertLen += s.len();
				}
			}
		}
		sp->actions.push_back(a);
	}
	//cout << "insertCommands 5" << endl;
	
	mutated = lsystem->toString();
	//cout << "insertCommands 6" << endl;
	chg = (float) insertLen / (float) in.len();
	
	delete lsystem;	
	//cout << "insertCommands 7" << endl;
			
	return mutated;
}

SString Geno_f8::mutateEncapsulate(SString &in, float& chg) {
	SString mutated;
	
	Lsystem *lsystem = this->converter->createLsystem(in);
	if (lsystem == NULL || lsystem->productions.size() == 0) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
	
	int counter = 0;
	int rnd = randomN(lsystem->productions.size());
	int len = 0;
	
	Production *p = new Production();
	p->subproductions.push_back(SubProduction());
	SubProduction *newSubProd = &(p->subproductions.back());
	
	for (map<string, Production*>::iterator prodIter = lsystem->productions.begin();
		 prodIter != lsystem->productions.end(); prodIter++) {
		if (counter++ == rnd) {
			ParameterCollection *params = &(prodIter->second->parameters);
			//copy parameters to the encapsulated production
			for (int i = 1; i <= params->size(); i++) {
				p->parameters.addParameter(params->getParameterName(i));
			}
			
			SubProduction *subproduction;
			if (prodIter->second->subproductions.size() > 0) {
				subproduction = &(prodIter->second->subproductions[randomN(prodIter->second->subproductions.size())]);
			} else {
				mutated += in;
				chg = -1.0;
				return mutated;
			}
			int firstActionIdx;
			if (subproduction->actions.size() > 0) {
				firstActionIdx = randomN(subproduction->actions.size());
			} else {
				mutated += in;
				chg = -1.0;
				return mutated;
			}
			int i;
			vector<ActionStrP> newActions;
			for (i = 0; i < firstActionIdx; i++) {
				newActions.push_back(subproduction->actions[i]);
			}
			for (i = firstActionIdx; i <= firstActionIdx + randomN(4) + 1
				 && i < subproduction->actions.size(); i++) { //1..4 actions
				newSubProd->actions.push_back(subproduction->actions[i]);
				if (subproduction->actions[i].action != NULL) {
					len += subproduction->actions[i].action->getF8Representation().len();
				}
			}
			ActionStrP a;
			a.action = p;
			a.params = vector<SString>();
			for (int j = 0; j < params->size(); j++) {
				//a.params.push_back(SString("0;"));
				a.params.push_back(params->getParameterName(j + 1));
			}
			newActions.push_back(a);
			while (i < subproduction->actions.size()) {
				if (subproduction->actions[i].action != NULL) {
					newActions.push_back(subproduction->actions[i]);
				}
				i++;
			}
			subproduction->actions = newActions;
			break;
		}		
	}
	
	SString newName;
	for (int i = 0; i < 100000; i++) {
		newName = "P";
		newName += SString::valueOf(i);
		if (lsystem->productions[sstringToString(newName)] == NULL) {
			break;
		}
	}	
	p->name = newName;
	lsystem->productions[sstringToString(newName)] = p;
	
	mutated = lsystem->toString();
	
	delete lsystem;

	chg = (float) len / (float) in.len();
	return mutated;
}

SString Geno_f8::mutateDeleteProduction(SString &in, float& chg) {
	SString mutated;
	
	Lsystem *lsystem = this->converter->createLsystem(in);
	if (lsystem == NULL || lsystem->productions.size() == 0) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
	
	if (lsystem->productions.size() < 2) {
		mutated += in;
		chg = 0.0;
                delete lsystem;
		return mutated;
	}

	vector<SString> productionNamesSS = this->converter->readProductionNames(in);
	vector<string> productionNames;
	for (int ssI = 0; ssI < productionNamesSS.size(); ssI++) {
		productionNames.push_back(sstringToString(productionNamesSS[ssI]));
	}
	vector<string>::iterator prodNameIter = productionNames.begin();
	while (prodNameIter != productionNames.end()) {
		//delete first production from candidate list
		if ((*prodNameIter).compare(lsystem->firstProductionName) == 0) {
			prodNameIter = productionNames.erase(prodNameIter);
		} else {
			prodNameIter++;
		}
	}
	//choose production to delete
	string prodNameToDelete = productionNames[randomN(productionNames.size())];
	productionNames.push_back(lsystem->firstProductionName);
	prodNameIter = productionNames.begin();
	while (prodNameIter != productionNames.end()) {
		//delete prodNameToDelete from candidate list
		if ((*prodNameIter).compare(prodNameToDelete) == 0) {
			prodNameIter = productionNames.erase(prodNameIter);
		} else {
			prodNameIter++;
		}
	}
	//choose production to which we will append the contents of deleted production
	string prodNameToAppendTo = productionNames[randomN(productionNames.size())];

	int len = 0;

	Production* deletedProd = lsystem->productions[prodNameToDelete];
	Production* appendProd = lsystem->productions[prodNameToAppendTo];
	//add missing parameters
	for (int i = 1; i <= deletedProd->parameters.size(); i++) {
		SString paramName = deletedProd->parameters.getParameterName(i);
		if (appendProd->parameters.paramExist(paramName) == false) {
			appendProd->parameters.addParameter(paramName);
		}
	}

	//copy subproductions
	for (vector<SubProduction>::iterator subprodIter = deletedProd->subproductions.begin();
		 subprodIter != deletedProd->subproductions.end(); subprodIter++) {
		SubProduction sp;
		//copy conditions
		for (vector<Condition>::iterator condIter = (*subprodIter).conditions.begin();
			 condIter != (*subprodIter).conditions.end(); condIter++) {
			Condition *cFrom = &(*condIter);
			Condition cTo;
			cTo.parameter = SString(cFrom->parameter);
			cTo.relation = cFrom->relation;
			cTo.value = cFrom->value;
			sp.conditions.push_back(cTo);
		}
		for (vector<ActionStrP>::iterator actionIter = (*subprodIter).actions.begin();
			 actionIter != (*subprodIter).actions.end(); actionIter++) {
			ActionStrP aFrom = *actionIter;
			ActionStrP aTo;
			aTo.action = aFrom.action;
			aTo.params = vector<SString>(aFrom.params);
			sp.actions.push_back(aTo);
		}
		appendProd->subproductions.push_back(sp);
	}
	lsystem->productions.erase(string(prodNameToDelete));

	int paramCount = appendProd->parameters.size();
	for (map<string, Production*>::iterator prodIter = lsystem->productions.begin();
		 prodIter != lsystem->productions.end(); prodIter++) {
		for (vector<SubProduction>::iterator subprodIter = prodIter->second->subproductions.begin();
			 subprodIter != prodIter->second->subproductions.end(); subprodIter++) {
			for (vector<ActionStrP>::iterator actionIter = (*subprodIter).actions.begin();
				 actionIter != (*subprodIter).actions.end(); actionIter++) {
				if ((*actionIter).action != NULL && (*actionIter).action->name != NULL) {
					if ((*actionIter).action->name == stringToSString(prodNameToDelete)) {
						(*actionIter).action = appendProd;
						(*actionIter).params = vector<SString>(paramCount, SString("0;"));
					}
					if ((*actionIter).action->name == stringToSString(prodNameToAppendTo)) {
						(*actionIter).action = appendProd;
						SString paramVal = "0;";
						for (int i = (*actionIter).params.size(); i < paramCount; i++) {
							(*actionIter).params.push_back("0;");
						}
					}

				}
			}
		}
	}
	delete deletedProd;


	mutated = lsystem->toString();

	delete lsystem;

	chg = (float) len / (float) in.len();
	return mutated;
}

SString Geno_f8::mutateChangeConditionSign(SString &in, float& chg) {
	SString mutated;

	Lsystem *newLsystem = this->converter->createLsystem(in);
	if (newLsystem == NULL) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}

	map<int, SubProduction*> interestingSubproductions;
	int counter = 0;

	for (map<string, Production*>::iterator prodIter = newLsystem->productions.begin();
		 prodIter != newLsystem->productions.end(); prodIter++) {
		for (vector<SubProduction>::iterator subprodIter = prodIter->second->subproductions.begin();
			 subprodIter != prodIter->second->subproductions.end(); subprodIter++) {
			SubProduction *sp = &(*subprodIter);
			if (sp->conditions.size() > 0) {
				interestingSubproductions[counter++] =  sp;
			}
		}
	}

	if (interestingSubproductions.size() > 0) {
		int random1 = randomN(interestingSubproductions.size());
		SubProduction *randomSubproduction = interestingSubproductions[random1];
		int random2 = randomN(randomSubproduction->conditions.size()); //all interesting conditions have conditions.size() > 0
		Condition *c = &(randomSubproduction->conditions.at(random2));
		c->relation = this->getDifferentCondition(c->relation);
		chg = 2.0 / (float) in.len();
	}

	mutated = newLsystem->toString();

	delete newLsystem;

	return mutated;
}

SString Geno_f8::mutateAddParameter(SString &in, float& chg) {
	SString mutated;

	Lsystem *lsystem = this->converter->createLsystem(in);
	if (lsystem == NULL || lsystem->productions.size() == 0) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}

	//cout << "addParameter 1" << endl;
	int rnd = randomN(lsystem->productions.size());
	int chglen = 0;

	SString prodName = "";

	int counter = 0;
	for (map<string, Production*>::iterator prodIter = lsystem->productions.begin();
		 prodIter != lsystem->productions.end(); prodIter++) {
		if (counter++ == rnd) {
			prodName = prodIter->second->name;
			SString newParam;
			for (int i = 0; i < 10000; i++) {
				newParam = "n";
				newParam += SString::valueOf(i);
				if (!prodIter->second->parameters.paramExist(newParam)) {
					prodIter->second->parameters.addParameter(newParam);
					chglen += newParam.len();
					break;
				}
			}

			if (prodName == stringToSString(lsystem->firstProductionName)) {
				lsystem->startParams[sstringToString(newParam)] = 0.0;
			}
			break;
		}
	}
	//cout << "addParameter 2" << endl;
	for (map<string, Production*>::iterator prodIter = lsystem->productions.begin();
		 prodIter != lsystem->productions.end(); prodIter++) {
		for (vector<SubProduction>::iterator subprodIter = prodIter->second->subproductions.begin();
			 subprodIter != prodIter->second->subproductions.end(); subprodIter++) {
			for (vector<ActionStrP>::iterator actionIter = (*subprodIter).actions.begin();
				 actionIter != (*subprodIter).actions.end(); actionIter++) {
				if ((*actionIter).action != NULL && (*actionIter).action->name != NULL) { 
					if ((*actionIter).action->name == prodName) {
						int randParamVal = randomN(prodIter->second->parameters.size() + 1);
						SString paramVal = NULL;
						if (randParamVal == 0) {
							paramVal = "0;";
						} else {
							paramVal = prodIter->second->parameters.getParameterName(randParamVal) + ";";
						}
						//(*actionIter).params.push_back(paramVal);
						chglen += 2;
					}
				}
			}
		}
	}
	//cout << "addParameter 3" << endl;
	
	mutated = lsystem->toString();
	//cout << "addParameter 4" << endl;
	
	delete lsystem;
	
	chg = (float) chglen / (float) in.len();
	return mutated;
}

SString Geno_f8::mutateAddCondition(SString &in, float& chg) {
	SString mutated, before, mid, after, line;
	vector<ProductionInfo> prodInfo = this->getProductionsInfo(in);
	vector<int> interestingProductions;
	for (int i = 0; i < prodInfo.size(); i++) {
		if (prodInfo[i].paramNames.size() > 0) {
			interestingProductions.push_back(i);
		}
	}
	if (interestingProductions.size() == 0) {
		chg = -1.0;
		mutated = in;
		return mutated;
	}
	int rndProd = interestingProductions[randomN(interestingProductions.size())];
	int pos = 0;
	mutated = "";
	while (in.getNextToken(pos, line, '\n')) {
		if (line.startsWith(prodInfo[rndProd].name) && line.len() > prodInfo[rndProd].name.len()) {
			int subproductionsCount = -1;
			SString tok;
			int pos2 = 0;
			while (line.getNextToken(pos2, tok, ':')) {
				subproductionsCount++;
			}
			if (subproductionsCount == 0) {
				chg = -1.0;
				mutated = in;
				return mutated;
			}
			int rnd = randomN(subproductionsCount);
			int counter = 0;
			int colonIdx = line.indexOf(':', 0);
			pos2 = colonIdx;
			while (counter != rnd) {
				colonIdx = line.indexOf(':', pos2 + 1);
				counter++;
			}
			
			int nextColonIdx = line.indexOf(':', colonIdx + 1);
			if (nextColonIdx == -1) {
				nextColonIdx = 2147483646;
			}
			int pipeIdx = line.indexOf('|', colonIdx + 1);
			mid = "";
			if (pipeIdx < nextColonIdx && pipeIdx != -1) {
				//before += line.substr(colonIdx + 1, pipeIdx - colonIdx - 1);
				before = line.substr(0, pipeIdx);
				mid += ",";
				after = line.substr(pipeIdx);				
			} else {
				before = line.substr(0, colonIdx + 1);
				after = SString("|") + line.substr(colonIdx + 1);
			}
			int paramIdx = (int) randomN(prodInfo[rndProd].paramNames.size());
			mid += prodInfo[rndProd].paramNames[paramIdx];
			mid += (randomN(2) == 0) ? ">" : "<";
			mid += SString::valueOf((int) randomN(21) - 10);
			
			mutated += before + mid + after + "\n";
			chg = (float) mid.len() / (float)in.len();
		} else {
			mutated += line + "\n";
		}
	}
	return mutated;
}

SString Geno_f8::mutateDeleteParameter(SString &in, float& chg) {
	SString mutated;
	
	Lsystem *newLsystem = this->converter->createLsystem(in);
	if (newLsystem == NULL) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
		
	map<int, Production*> interestingProductions;
	int counter = 0;
	
	for (map<string, Production*>::iterator prodIter = newLsystem->productions.begin();
			prodIter != newLsystem->productions.end(); prodIter++) {
		if (prodIter->second->parameters.size() > 0) {
			interestingProductions[counter++] =  prodIter->second;
		}
	}
	
	if (interestingProductions.size() > 0) {
		int rnd = randomN(interestingProductions.size());
		
		Production *randomProduction = interestingProductions[rnd];
		SString prodName = randomProduction->name;
		
		int paramIndex = randomN(randomProduction->parameters.size()) + 1;
		SString paramName = randomProduction->parameters.getParameterName(paramIndex);
		string sparamName = sstringToString(paramName);
		int change = paramName.len();
		
		randomProduction->parameters.removeParameter(paramName);
		
		if (prodName == stringToSString(newLsystem->firstProductionName)) {
			newLsystem->startParams.erase(sstringToString(paramName));
		}
		
		for (vector<SubProduction>::iterator subProdIter = randomProduction->subproductions.begin();
				subProdIter != randomProduction->subproductions.end(); subProdIter++) {
			for (vector<Condition>::iterator condIter = (*subProdIter).conditions.begin(); 
				 condIter != (*subProdIter).conditions.end(); /*nothing*/) {
				if ((*condIter).parameter == paramName) {
					condIter = (*subProdIter).conditions.erase(condIter);
				} else {
					condIter++;
				}
			}
			for (vector<ActionStrP>::iterator actionIter = (*subProdIter).actions.begin();
					actionIter != (*subProdIter).actions.end(); actionIter++) {
				for (vector<SString>::iterator paramIter = (*actionIter).params.begin();
						paramIter != (*actionIter).params.end(); paramIter++) {
					string s = sstringToString(*paramIter);
					if (s.find(sparamName, 0) != string::npos) {
						*paramIter = SString("0;");
					}
				}
			}
		}
				
		for (map<string, Production*>::iterator prodIter = newLsystem->productions.begin();
				prodIter != newLsystem->productions.end(); prodIter++) {
			vector<SubProduction> *subproductions = &(prodIter->second->subproductions);
			for (vector<SubProduction>::iterator subProdIter = subproductions->begin();
			    	subProdIter != subproductions->end(); subProdIter++) {
				SubProduction *sp = &(*subProdIter);
				for (vector<ActionStrP>::iterator actionIter = sp->actions.begin(); 
					 actionIter != sp->actions.end(); actionIter++) {
					if ((*actionIter).action != NULL) {
						if ((*actionIter).action->name == prodName && (*actionIter).params.size() > paramIndex - 1) {
							change += paramName.len(); //more less
							(*actionIter).params.erase((*actionIter).params.begin() + (paramIndex - 1));
						}
					}
				}
			}
		}
		chg = (float) change / (float) in.len();
	}
	
	mutated = newLsystem->toString();
	
	delete newLsystem;
	
	return mutated;
}

SString Geno_f8::mutateDeleteCondition(SString &in, float& chg) {
	SString mutated;
	
	Lsystem *newLsystem = this->converter->createLsystem(in);
	if (newLsystem == NULL) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
		
	map<int, SubProduction*> interestingSubproductions;
	int counter = 0;
	
	for (map<string, Production*>::iterator prodIter = newLsystem->productions.begin();
		 prodIter != newLsystem->productions.end(); prodIter++) {
		for (vector<SubProduction>::iterator subprodIter = prodIter->second->subproductions.begin();
			 subprodIter != prodIter->second->subproductions.end(); subprodIter++) {
			SubProduction *sp = &(*subprodIter);
			if (sp->conditions.size() > 0) {
				interestingSubproductions[counter++] =  sp;
			}
		}
	}
	
	if (interestingSubproductions.size() > 0) {
		int rnd = randomN(interestingSubproductions.size());
		SubProduction *randomSubproduction = interestingSubproductions[rnd];
		vector<Condition>::iterator condIter = randomSubproduction->conditions.begin();
		condIter += randomN(randomSubproduction->conditions.size() - 1);
		Condition c = *condIter;
		chg = (float) (c.parameter.len() + 1 + SString::valueOf(c.value).len()) / (float) in.len();
		randomSubproduction->conditions.erase(condIter);
	}
	
	mutated = newLsystem->toString();
	
	delete newLsystem;
	
	return mutated;
}

SString Geno_f8::mutateAddLoop(SString &in, float& chg) {
	/*
	 * Podczas wczytania genotypu do obiektu typu Lsystem nastepuje rozwiniecie petli, wiec kolejne mutacje
	 * zniweczyly by zysk ktory mozna uzyskac dzieki petlom. Dlatego nic nie zmieniamy.
	 */
	SString mutated, line;
	int pos = 0;
	while (in.getNextToken(pos, line, '\n')) {
		mutated += line + "\n";
	}
	chg = 0.0;
	return mutated;
}

SString Geno_f8::mutateDeleteLoop(SString &in, float& chg) {
	/*
	 * Podczas wczytania genotypu do obiektu typu Lsystem nastepuje rozwiniecie petli, wiec kolejne mutacje
	 * zniweczyly by zysk ktory mozna uzyskac dzieki petlom. Dlatego nic nie zmieniamy.
	 */
	SString mutated, line;
	int pos = 0;
	while (in.getNextToken(pos, line, '\n')) {
		mutated += line + "\n";
	}
	chg = 0.0;
	return mutated;
}

SString Geno_f8::mutateAddSubproduction(SString &in, float& chg) {
	SString mutated, line;
	vector<ProductionInfo> prodInfo = this->getProductionsInfo(in);
	int rndProd = randomN(prodInfo.size());
	int pos = 0;
	mutated = "";
	while (in.getNextToken(pos, line, '\n')) {
		if (line.startsWith(prodInfo[rndProd].name) && line.indexOf(':', 0) != -1) {
			SString tmp = ":";
			int paramCount = prodInfo[rndProd].paramNames.size();
			if (paramCount > 0) {
				tmp += prodInfo[rndProd].paramNames[(int) randomN(paramCount)];
				tmp += (randomN(2) == 0) ? ">" : "<";
				tmp += SString::valueOf((int) randomN(21) - 10);
				tmp += "|";
			}
			tmp += "X\n";
			chg = (float) tmp.len() / (float) in.len();
			mutated += line + tmp;
		} else {
			mutated += line + "\n";
		}
	}
	return mutated;
}

SString Geno_f8::mutateChangeIterationsNumber(SString &in, float& chg) {
	SString mutated;	
	Lsystem *newLsystem = this->converter->createLsystem(in);
	if (newLsystem == NULL) {
		mutated += in;
		chg = -1.0;
		return mutated;
	}
	
	newLsystem->iterations += randomN(7) - 3; //-3..3
	if (newLsystem->iterations < 1) {
		newLsystem->iterations = 1;
	}
	mutated = newLsystem->toString();	
	delete newLsystem;	
	chg = 1.0 / (float) in.len();
	return mutated;
}

int Geno_f8::mutate(char *&g,float& chg, int &method) {
	SString in = g;

	/*struct timeval tv;
	gettimeofday(&tv, NULL);
	srandom(tv.tv_sec + tv.tv_usec);
	*/
	method = roulette(this->operation, F8_OPERATION_COUNT);
	if (method < 0) {
		return GENOPER_OPFAIL;
	}

#if GENO_F8_DEBUG > 0
	string mutationName = (method == 0) ? "F8_CHANGE_BEGINNING_ARG" : //TODO use mutation_method_names[] here
	(method == 1) ? "F8_CHANGE_ARG" :
	(method == 2) ? "F8_DELETE_COMMAND" :
	(method == 3) ? "F8_INSERT_COMMANDS" :
	(method == 4) ? "F8_ENCAPSULATE" :
	(method == 5) ? "F8_CHANGE_CONDITION_SIGN" :
	(method == 6) ? "F8_REPLACE_COMMAND" :
	(method == 7) ? "F8_ADD_PARAMETER" :
	(method == 8) ? "F8_ADD_CONDITION" :
	(method == 9) ? "F8_ADD_SUBPRODUCTION" :
	(method == 10) ? "F8_CHANGE_ITERATIONS_NUMBER" :
	(method == 11) ? "F8_DELETE_PARAMETER" :
	(method == 12) ? "F8_DELETE_CONDITION" :
	(method == 13) ? "F8_ADD_LOOP" :
	(method == 14) ? "F8_DELETE_LOOP" :
	(method == 15) ? "F8_OPERATION_COUNT" : "*invalid*";
	//cout << "-------------------- " << mutationName << " --------------------" << endl;
#endif

	const char* mutatedTmp;
	SString ssMutatedTmp;

	//cout << "mutate 1" << endl;
	switch (method) {
		case F8_CHANGE_BEGINNING_ARG:
			ssMutatedTmp = this->mutateChangeBeginningArg(in, chg);
			break;
		case F8_CHANGE_ARG:
			ssMutatedTmp = this->mutateChangeArg(in, chg);
			break;
		case F8_DELETE_COMMAND:
			ssMutatedTmp = this->mutateDeleteCommand(in, chg);
			break;
		case F8_INSERT_COMMANDS:
			ssMutatedTmp = this->mutateInsertCommands(in, chg);
			break;
		case F8_ENCAPSULATE:
			ssMutatedTmp = this->mutateEncapsulate(in, chg);
			break;
		case F8_CHANGE_CONDITION_SIGN:
			ssMutatedTmp = this->mutateChangeConditionSign(in, chg);
			break;
		/*case F8_REPLACE_COMMAND:
			ssMutatedTmp = this->mutateReplaceCommand(in, chg);
			break;*/
		case F8_ADD_PARAMETER:
			ssMutatedTmp = this->mutateAddParameter(in, chg);
			break;
		case F8_ADD_CONDITION:
			ssMutatedTmp = this->mutateAddCondition(in, chg);
			break;
		case F8_ADD_SUBPRODUCTION:
			ssMutatedTmp = this->mutateAddSubproduction(in, chg);
			break;
		case F8_CHANGE_ITERATIONS_NUMBER:
			ssMutatedTmp = this->mutateChangeIterationsNumber(in, chg);
			break;
		case F8_DELETE_PARAMETER:
			ssMutatedTmp = this->mutateDeleteParameter(in, chg);
			break;
		case F8_DELETE_CONDITION:
			ssMutatedTmp = this->mutateDeleteCondition(in, chg);
			break;
		case F8_ADD_LOOP:
			ssMutatedTmp = this->mutateAddLoop(in, chg);
			break;
		case F8_DELETE_LOOP:
			ssMutatedTmp = this->mutateDeleteLoop(in, chg);
			break;
		case F8_DELETE_PRODUCTION:
			ssMutatedTmp = this->mutateDeleteProduction(in, chg);
			break;
		default:
			return GENOPER_OPFAIL;
	}

	//cout << "mutate 2" << endl;
	if (ssMutatedTmp.len() < 16) {
		return GENOPER_OPFAIL;
	}
	if (chg == -1.0) {
		chg = 0.0;
		return GENOPER_OPFAIL;
	}
	
	mutatedTmp = ssMutatedTmp;
	
	if (this->checkValidity(mutatedTmp) == GENOPER_OK) { 
		free(g); //must take care of the original allocation
		g = (char*)malloc(strlen(mutatedTmp) + 1); //allocate for mutated genotype
		strcpy(g, mutatedTmp); //the rest is originalg = mutated;
		//cout << "mutate 3" << endl;
		return GENOPER_OK;
	} else {
		return GENOPER_OPFAIL;
	}
}

int Geno_f8::crossOver(char *&g1,char *&g2,float& chg1,float& chg2) {
	/* wymiana niewielkiej (max. 25%) ilości odpowiadającej sobie informacji
	 * ma to przypominać "celowaną" mutację, w której osobnik dostaje
	 * niewiele wiedzy, która sprawdziła się u innego osobnika
	 */
	SString in1 = g1;
	SString in2 = g2;
	SString mutated;
	//SString dst;
	//const char* src = in;
	//const char* t;
	
	Lsystem *newLsystem = this->converter->createLsystem(in1);
	Lsystem *lsystem2 = this->converter->createLsystem(in2);
	if (newLsystem == NULL || lsystem2 == NULL) {
		chg1 = 0.0;
		chg2 = 0.0;
		return GENOPER_OPFAIL;
	}
	
	//shuffle second parent's productions
	vector<int> indices;
	for (int i = 0; i < lsystem2->productions.size(); i++) {
		indices.push_back(i);
	}
	map<string, Production*> shuffledProductions;
	while (!indices.empty()) {
		int rnd = randomN(indices.size());
		int counter = 0;
		//Production *p;
		for (map<string, Production*>::iterator it = lsystem2->productions.begin();
			 it != lsystem2->productions.end(); it++) {
			if (counter == rnd) {
				//p = it->second;
				shuffledProductions[it->first] = it->second;
				break;
			}
		}
		//delete index number
		indices.erase(indices.begin() + rnd);
	}
	
	int productionCount = randomN(min(newLsystem->productions.size() / 4, lsystem2->productions.size())) + 1;
	
	int counter = 0;
	for (map<string, Production*>::iterator it = shuffledProductions.begin(); 
		 it != shuffledProductions.end() && counter < productionCount; it++) {
		//if selected production's name exist in the first parent
		if (newLsystem->productions.find(it->first) != newLsystem->productions.end()) {
			newLsystem->productions[it->first]->subproductions = it->second->subproductions;
			counter++;
		} else {
			/* This fragment is commented out because we are interested only in matching productions ///////
			//search for first production, which name does not exist in one of the productions
			//selected from second parent
			//this is to avoid information loss when new subproductions will be overwritten
			//just after adding to the child production
			for (map<string, Production*>::iterator it2 = newLsystem->productions.begin();
				 it2 != newLsystem->productions.end(); it2++) {
				if (shuffledProductions.find(it2->first) == shuffledProductions.end()) { //not found
					it2->second->subproductions = it->second->subproductions;
					break;
				}
				//there are no "else" because there is at least twice as many productions
				//in the first parent as selected productions from the second parent
				//so always we will find "free" production name
			}*/
		}		
	}
	
	//check validity of productions indicated in actions
	for (map<string, Production*>::iterator prodIter = newLsystem->productions.begin();
		 prodIter != newLsystem->productions.end(); prodIter++) {
		vector<SString> paramNames;
		for (int i = 1; i <= prodIter->second->parameters.size(); i++) {
			paramNames.push_back(prodIter->second->parameters.getParameterName(i));
		}
		for (vector<SubProduction>::iterator subprodIter = prodIter->second->subproductions.begin();
			 subprodIter != prodIter->second->subproductions.end(); subprodIter++) {
			//conditions
			if (paramNames.size() > 0) {
				for (vector<Condition>::iterator condIter = (*subprodIter).conditions.begin();
					 condIter != (*subprodIter).conditions.end(); condIter++) {
					vector<SString>::iterator searchIter = find(paramNames.begin(), paramNames.end(), 
																(*condIter).parameter);
					if (searchIter == paramNames.end()) { //unknown parameter name!
						//choose random existing parameter
						(*condIter).parameter = paramNames[randomN(paramNames.size())];
					}
				}
			} else { //no params, so delete all conditions
				(*subprodIter).conditions.clear();
			}
			
			//actions
			for (vector<ActionStrP>::iterator i = (*subprodIter).actions.begin(); 
				 i != (*subprodIter).actions.end(); /*nothing*/) {
				bool deleted = false;
				if ((*i).action == NULL) {
					continue;
				}
				SString &actionName = (*i).action->name;
				if (actionName[0] == 'n') {
					if (paramNames.size() > 0) {
						vector<SString>::iterator searchIter = find(paramNames.begin(), paramNames.end(), 
																	actionName);
						if (searchIter != paramNames.end()) { //valid name
							//ensure it is linked to the correct production
							(*i).action = newLsystem->getParamProduction(actionName);
						} else {
							//link to random parameter prodution
							SString name = paramNames[randomN(paramNames.size())];
							(*i).action = newLsystem->getParamProduction(name);
						}
					} else { //no params, so delete this param production
						i = (*subprodIter).actions.erase(i);
						deleted = true;
					}
				} else if (actionName[0] == 'P') {
					if (newLsystem->productions.find(sstringToString(actionName)) != newLsystem->productions.end()) {
						//ensure it is linked to the correct production
						(*i).action = newLsystem->productions[sstringToString(actionName)];
					} else {
						//delete this action
						i = (*subprodIter).actions.erase(i);
						deleted = true;
					}
				}
				if (!deleted) {
					i++;
				}
			}
			if ((*subprodIter).actions.size() == 0) { //we erased all the actions
				ActionStrP a;
				a.action = newLsystem->getPrimitiveProduction(SString("X"));
				(*subprodIter).actions.push_back(a);
			}
		}
	}
	
	mutated = newLsystem->toString();
	
	shuffledProductions.clear();
	
	delete newLsystem;
	delete lsystem2;
	
	free(g1); //must take care of the original allocation
	//free(g2);
	const char* mutatedTmp = mutated;
	char* mutatedTmp2 = (char*)malloc(strlen(mutatedTmp) + 1); //allocate for mutated genotype
    strcpy(mutatedTmp2, mutatedTmp); //the rest is originalg = mutated;
	g1 = mutatedTmp2;
	chg1 = 0.5;
	
	return GENOPER_OK;	
}


bool Geno_f8::checkProdNameExist(vector<ProductionInfo> info, SString name) const {
	for (vector<ProductionInfo>::iterator it = info.begin(); it != info.end(); it++) {
		if ((*it).name == name) {
			return true;
		}
	}
	return false;
}

bool Geno_f8::checkParamNameExist(vector<SString> names, SString name) const {
	for (vector<SString>::iterator it = names.begin(); it != names.end(); it++ ) {
		if ((*it) == name) {
			return true;
		}
	}
	return false;
}

SString Geno_f8::getOppositeCondition(const SString &c) const {
	if (c.equals("=="))
		return "!=";
	if (c.equals("!="))
		return "==";
	if (c.equals(">"))
		return "<=";
	if (c.equals("<"))
		return ">=";
	if (c.equals(">="))
		return "<";
	if (c.equals("<="))
		return ">";
	return "<"; //default
}

RelationType Geno_f8::getDifferentCondition(RelationType type) {
	RelationType types[6];
	types[0] = r_greater;
	types[1] = r_greaterEqual;
	types[2] = r_equal;
	types[3] = r_different;
	types[4] = r_lessEqual;
	types[5] = r_less;
	
	for (int i = 0; i < 6; i++) {
		if (types[i] == type) {
			types[i] = types[5];
			//types[5] = type;
			break;
		}
	}
	
	int randomType = randomN(5);
	return types[randomType];
}

SString Geno_f8::removeProductionCalls(const SString production) const {
	SString line = trimSString(production);
	SString result = "";
	bool skip = false;
	for (int i = 0; i < line.len(); i++) {
		if (skip) {
			if (line[i] == ')') {
				skip = false;
			} else {
				//do nothing
			}
		} else {
			if (line[i] == 'P') {
				skip = true;
			} else {
				result += line[i];
			}
		}
	}
	return result;
}

SString Geno_f8::addParameterToCalls(const SString line, SString &prodName) {
	SString newStr = "";
	int colonIdx = line.indexOf(':', 0);
	if (colonIdx != -1 && line.indexOf(prodName, colonIdx) != -1) {
		int pos2 = colonIdx;
		int beginIdx = 0;
		int prodIdx;
		while ((prodIdx = line.indexOf(prodName, pos2)) != -1) {
			int indexRBrace = line.indexOf(')', prodIdx);
			newStr += line.substr(beginIdx, indexRBrace);
			if (newStr[newStr.len() - 1] != '(') {
				newStr += SString(",");
			}
			newStr += "0.0";
			beginIdx = indexRBrace;
			pos2 = indexRBrace;
		}
		newStr += line.substr(pos2);
	} else {
		newStr = line;
	}
	return newStr;
}

SString Geno_f8::deleteParameterFromCalls(const SString line, SString &prodName, int paramIdx) {
	SString newStr = "";
	SString tmp, before, mid, midTmp;
	mid = "";
	int colonIdx = line.indexOf(':', 0);
	if (colonIdx != -1 && line.indexOf(prodName, colonIdx) != -1) {
		int pos2 = colonIdx;
		int beginIdx = 0;
		int prodIdx;
		while ((prodIdx = line.indexOf(prodName, pos2)) != -1) {
			int indexRBrace = line.indexOf(')', prodIdx);
			before = line.substr(beginIdx, prodIdx + prodName.len() + 1);
			midTmp = line.substr(prodIdx + prodName.len() + 1, indexRBrace - (prodIdx + prodName.len() + 1));
			pos2 = 0;
			SString tok;
			int i = 0;
			while (midTmp.getNextToken(pos2, tok, ',')) {
				if (i++ == paramIdx) {
					//do nothing
				} else {
					mid += tok + ",";
				}
			}
			if (mid[mid.len() - 1] == ',') {
				mid = mid.substr(0, mid.len() - 1);
			}
			
			newStr += before + mid + ")";
			beginIdx = indexRBrace;
			pos2 = indexRBrace;
		}
		newStr += line.substr(pos2 + 1);
	} else {
		newStr = line;
	}
	return newStr;
}

SString Geno_f8::testMutate(SString &in, int method) {
	/*struct timeval tv;
	gettimeofday(&tv, NULL);
	srandom(tv.tv_sec + tv.tv_usec);
	*/
	SString mutatedTmp;
	float chg = 0.0;
	switch (method) {
		case F8_CHANGE_BEGINNING_ARG:
			mutatedTmp = this->mutateChangeBeginningArg(in, chg);
			break;
		case F8_CHANGE_ARG:
			mutatedTmp = this->mutateChangeArg(in, chg);
			break;
		case F8_DELETE_COMMAND:
			mutatedTmp = this->mutateDeleteCommand(in, chg);
			break;
		case F8_INSERT_COMMANDS:
			mutatedTmp = this->mutateInsertCommands(in, chg);
			break;
		case F8_ENCAPSULATE:
			mutatedTmp = this->mutateEncapsulate(in, chg);
			break;
		case F8_CHANGE_CONDITION_SIGN:
			mutatedTmp = this->mutateChangeConditionSign(in, chg);
			break;
		/*case F8_REPLACE_COMMAND:
			mutatedTmp = this->mutateReplaceCommand(in, chg);
			break;*/
		case F8_ADD_PARAMETER:
			mutatedTmp = this->mutateAddParameter(in, chg);
			break;
		case F8_ADD_CONDITION:
			mutatedTmp = this->mutateAddCondition(in, chg);
			break;
		case F8_ADD_SUBPRODUCTION:
			mutatedTmp = this->mutateAddSubproduction(in, chg);
			break;
		case F8_CHANGE_ITERATIONS_NUMBER:
			mutatedTmp = this->mutateChangeIterationsNumber(in, chg);
			break;
		case F8_DELETE_PARAMETER:
			mutatedTmp = this->mutateDeleteParameter(in, chg);
			break;
		case F8_DELETE_CONDITION:
			mutatedTmp = this->mutateDeleteCondition(in, chg);
			break;
		case F8_ADD_LOOP:
			mutatedTmp = this->mutateAddLoop(in, chg);
			break;
		case F8_DELETE_LOOP:
			mutatedTmp = this->mutateDeleteLoop(in, chg);
			break;
		default:
			mutatedTmp = "";
	}
	return mutatedTmp;
	
}
