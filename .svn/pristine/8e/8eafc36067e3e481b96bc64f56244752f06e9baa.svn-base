/*
 *  conv_f8tof1.cpp
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-03-21.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#include "conv_f8tof1.h"
#include "conv_f8_utils.h"
#include <sstream>
#include <string>
#include <algorithm>
#include "lexglobal.h"
#include "conv_f8tof1_scanner.h"
namespace prs {
	#include "conv_f8tof1_grammar.c"
}

#if CONV_DEBUG > 0
	#include <iostream>
#endif

#define CONV_DEBUG 0 //0 - off, 1 - minimal, 2 - full

using namespace std;

Lsystem::Lsystem()
{
        for(int i=0;i<strlen(GenoConv_F8ToF1::simpleprods);i++)
        {
                string s(1,GenoConv_F8ToF1::simpleprods[i]);
                SString ss(s.c_str());
        	this->primitiveProductions.insert(make_pair(s, new PrimitiveProduction(ss)));
        }
}

Lsystem::~Lsystem() {
	for (map<string, Production*>::iterator iter = this->productions.begin();
		 iter != this->productions.end(); iter++) {
		delete iter->second;
	}
	for (map<string, PrimitiveProduction*>::iterator iter = this->primitiveProductions.begin();
		 iter != this->primitiveProductions.end(); iter++) {
		delete iter->second;
	}
	for (map<string, ParamProduction*>::iterator iter = this->paramProductions.begin(); 
		 iter != this->paramProductions.end(); iter++) {
		delete iter->second;
	}
	for (vector<NeuronProduction*>::iterator iter = this->neuronProductions.begin();
		 iter != this->neuronProductions.end(); iter++) {
		delete *iter;
	}
}

PrimitiveProduction* Lsystem::getPrimitiveProduction(SString name) {
	string sname = sstringToString(name);
	if (this->primitiveProductions.find(sname) == this->primitiveProductions.end()) {
		PrimitiveProduction *pp = new PrimitiveProduction(name);
		this->primitiveProductions.insert(make_pair(sname, pp));
	}
	return this->primitiveProductions[sname];
}

ParamProduction* Lsystem::getParamProduction(SString name) {
	string sname = sstringToString(name);
	if (this->paramProductions.find(sname) == this->paramProductions.end()) {
		ParamProduction *pp = new ParamProduction(name);
		this->paramProductions.insert(make_pair(sname, pp));
	}
	return this->paramProductions[sname];
}

SString Lsystem::toString() {
	this->removeEmptySubproductionsAndProductions();
	SString result = "";
	result += SString::valueOf(this->iterations) + "\n";
	for (map<string, double>::iterator it = this->startParams.begin(); 
		 it != this->startParams.end(); it++) {
		result += stringToSString(it->first) + "=" + SString::valueOf(it->second) + "\n";
	}
	result += SString("---") + "\n";
	result += stringToSString(this->firstProductionName) + "\n";
	for (map<string, Production*>::iterator prodIter = this->productions.begin();
		 prodIter != this->productions.end(); prodIter++) {
		Production *p = prodIter->second;
		result += p->name + "(";
		for (int i = 1; i <= p->parameters.size(); i++) {
			result += p->parameters.getParameterName(i);
			if (i < p->parameters.size()) {
				result += ",";
			}
		}
		result += "):";
		for (int subprodIter = 0; subprodIter < p->subproductions.size(); subprodIter++) {
			SubProduction *sp = &(p->subproductions[subprodIter]);
			for (int condIter = 0; condIter < sp->conditions.size(); condIter++) {
				Condition *c = &(sp->conditions[condIter]);
				RelationType r = c->relation;
				SString op = (r == r_greater) ? ">" : (r == r_greaterEqual) ? ">=" : (r == r_less) ? "<" :
				(r == r_lessEqual) ? "<=" : (r == r_equal) ? "==" : (r == r_different) ? "!=" : "";
				result += c->parameter + op + SString::valueOf(c->value);
				if (condIter != sp->conditions.size() - 1) {
					result += ",";
				}
			}
			if (sp->conditions.size() > 0) {
				result += SString("|");
			}
			for (vector<ActionStrP>::iterator actionIter = sp->actions.begin();
				 actionIter != sp->actions.end(); actionIter++) {
				ActionStrP *a = &(*actionIter);
				if (a->action == NULL) {
					continue;
				}
				result += a->action->getF8Representation();
				if (!a->action->ignoreParams) {
					result += SString("(");
					for (int paramIter = 0; paramIter < a->params.size(); paramIter++) {
						result += convertReversePolishNotationToNatural(a->params[paramIter]);
						if (paramIter != a->params.size() - 1) {
							result += ",";
						}
					}
					result += SString(")");					
				}
			}
			if (subprodIter != p->subproductions.size() - 1) {
				result += SString(":");
			}
		}
		result += SString("\n");
	}	
	return result;
}

vector<Action*> Lsystem::getAllActions(bool normal, bool primitives, bool params, bool neurons) {
	vector<Action*> actions;
	if (normal) {
		for (map<string, Production*>::iterator iter = this->productions.begin(); 
			 iter != this->productions.end(); iter++) {
			actions.push_back(iter->second);
		}
	}
	if (primitives) {
		for (map<string, PrimitiveProduction*>::iterator iter = this->primitiveProductions.begin(); 
			 iter != this->primitiveProductions.end(); iter++) {
			actions.push_back(iter->second);
		}
	}
	if (params) {
		for (map<string, ParamProduction*>::iterator iter = this->paramProductions.begin(); 
			 iter != this->paramProductions.end(); iter++) {
			actions.push_back(iter->second);
		}
	}
	if (neurons) {
		for (vector<NeuronProduction*>::iterator iter = this->neuronProductions.begin();
			 iter != this->neuronProductions.end(); iter++) {
			actions.push_back(*iter);
		}
	}
	return actions;
}

void Lsystem::removeEmptySubproductionsAndProductions() {
	/* po wykonaniu moga pozostac puste subprodukcje (usuwanie wywolan do usunietych wczesniej produkcji
	 * ale i tak znikna one przy kolejnym wywolaniu, tak wiec ilosc smieci nie powinna byc razaca
	 */
	
	//delete empty subproductions
	for (map<string, Production*>::iterator prodIter = this->productions.begin();
		 prodIter != this->productions.end(); prodIter++) {
		vector<SubProduction> *subproductions = &(prodIter->second->subproductions);
		for (vector<SubProduction>::iterator i = subproductions->begin(); i != subproductions->end(); /*nothing*/) {
			if ((*i).actions.size() == 0) {
				i = subproductions->erase(i);
			} else {
				i++;
			}
		}
	}
	
	//find empty productions
	vector<SString> emptyProductionNames;	
	for (map<string, Production*>::iterator prodIter = this->productions.begin();
		 prodIter != this->productions.end(); prodIter++) {
		if (prodIter->second->subproductions.size() == 0 && 
			sstringToString(prodIter->second->name) != this->firstProductionName) {
			emptyProductionNames.push_back(prodIter->second->name);
		}
	}
	
	//delete calls to empty productions
	for (map<string, Production*>::iterator prodIter = this->productions.begin();
		 prodIter != this->productions.end(); prodIter++) {
		vector<SubProduction> *subproductions = &(prodIter->second->subproductions);
		for (vector<SubProduction>::iterator subProdIter = subproductions->begin();
			 subProdIter != subproductions->end(); subProdIter++) {
			SubProduction *sp = &(*subProdIter);
			for (vector<ActionStrP>::iterator actionIter = sp->actions.begin(); 
				 actionIter != sp->actions.end(); /*nothing*/) {
				bool deleted = false;
				if ((*actionIter).action != NULL) {
					vector<SString>::iterator result = find(emptyProductionNames.begin(), emptyProductionNames.end(), 
															(*actionIter).action->name);
					if (result != emptyProductionNames.end()) { //emptyProductionNames contains the action name
						actionIter = sp->actions.erase(actionIter);
						deleted = true;
					}
				}
				if (!deleted) {
					actionIter++;
				}
			}
		}
	}
	
	//delete empty productions
	for(int i = 0; i < emptyProductionNames.size(); i++) {
		this->productions.erase(sstringToString(emptyProductionNames[i]));
	}
}

ostream& operator<<(ostream& os, const Condition& c) {
	RelationType r = c.relation;
	SString op = (r == r_greater) ? ">" : (r == r_greaterEqual) ? ">=" : (r == r_less) ? "<" :
	(r == r_lessEqual) ? "<=" : (r == r_equal) ? "==" : (r == r_different) ? "!=" : "";
	os <<  c.parameter << " " << op << " " << c.value;
	return os;
}

const double ParameterCollection::getValue(int position) {
	string name = this->parameters[position - 1];
	return this->paramValues[name];
}

const double ParameterCollection::getValue(SString name) {
	return this->paramValues[sstringToString(name)];
}

const SString ParameterCollection::getParameterName(int position) {
	return stringToSString(this->parameters[position - 1]);
}

const int ParameterCollection::getParameterPosition(SString name) {
	for (int i = 0; i < this->parameters.size(); i++) {
		string &s = this->parameters[i];
		if (s == sstringToString(name)) {
			return i + 1;
		}
	}
	return -1;
}

void ParameterCollection::setValue(int position, double value) {
	string name = this->parameters[position - 1];
	this->paramValues[name] = value;
}

void ParameterCollection::setValue(SString name, double value) {
	this->paramValues[sstringToString(name)] = value;
}

void ParameterCollection::addParameter(SString name, int position, double value) {
	//TODO sprawdzenie czy position > 0
	string sname = sstringToString(name);
	if (position == -1) {
		this->parameters.push_back(sname);
		this->paramValues[sname] = value;
	} else {
		this->parameters.reserve(position);
		this->parameters.insert(this->parameters.begin() + (position -1), sname);
		this->paramValues[sname] = value;
	}
	//this->paramValues[name] = value;
}

const int ParameterCollection::size() {
	return this->parameters.size();
}

void ParameterCollection::removeParameter(int position) {
	string name = this->parameters[position - 1];
	vector<string>::iterator it = this->parameters.begin() + (position - 1);
	this->parameters.erase(it);
	this->paramValues.erase(name);
}

void ParameterCollection::removeParameter(SString name) {
	int idx = getParameterPosition(name);
	this->removeParameter(idx);
}

bool ParameterCollection::paramExist(SString name) {
	string sname = sstringToString(name);
	for (vector<string>::iterator it = this->parameters.begin(); it != this->parameters.end(); it++) {
		if ((*it) == sname) {
			return true;
		}
	}
	return false;
}

PrimitiveProduction::PrimitiveProduction(const SString command) {
	this->f8command = command;
	this->ignoreParams = true;
	if (command == SString("[")) {
		this->f1command = SString("(");
		this->name = SString("_primitive production '[' => '('");
	} else if (command == SString("]")) {
		this->f1command = SString(")");
		this->name = SString("_primitive production ']' => ')'");
	} else if (command == SString("^")) {
		this->f1command = SString(",");
		this->name = SString("_primitive production '^' => ','");
	} else {
		this->f1command = command;
		this->name = SString("_primitive production '") + command + "'";
	}
}

const SString PrimitiveProduction::getF1Genotype(const vector<double> params) {
#if CONV_DEBUG > 1
	cout << "@@@@@ Prymityw: " << this->f1command << endl;
#endif
	return SString(this->f1command);
}

const list<ActionP> PrimitiveProduction::getActionList(const vector<double> param) {
	list<ActionP> l;
	ActionP ap;
	ap.action = this;
	ap.params = param;
	l.push_back(ap);
	return l;
}

const SString PrimitiveProduction::getF8Representation() {
	return this->f8command;
}

ParamProduction::ParamProduction(const SString paramName) {
	this->paramName = paramName;
	this->name = SString("_param production") + paramName;
	this->ignoreParams = true;
}

const SString ParamProduction::getF1Genotype(const vector<double> params) {
	SString s = SString::valueOf(params[0]);
#if CONV_DEBUG > 1
	cout << "@@@@@ Param value: " << this->paramName << ": " << params[0] << endl;
#endif
	return s;
	
}

const list<ActionP> ParamProduction::getActionList(const vector<double> param) {
	list<ActionP> l;
	ActionP ap;
	ap.action = this;
	ap.params = param;
	l.push_back(ap);
	return l;
	
}

const SString ParamProduction::getF8Representation() {
	return this->paramName;
}

NeuronProduction::NeuronProduction(const SString body) {
	this->body = body;
	this->name = SString("_neuron production") + body;
	this->ignoreParams = true;
}

const SString NeuronProduction::getF1Genotype(const vector<double> params) {
	return this->body;	
}

const list<ActionP> NeuronProduction::getActionList(const vector<double> param) {
	list<ActionP> l;
	ActionP ap;
	ap.action = this;
	ap.params = param;
	l.push_back(ap);
	return l;
	
}

const SString NeuronProduction::getF8Representation() {
	return this->body;
}

Production::Production() {
	this->ignoreParams = false;
}

const SString Production::getF1Genotype(const vector<double> params) {
	return SString(); //return empty
}

const list<ActionP> Production::getActionList(const vector<double> params) {
	list<ActionP> l;
#if CONV_DEBUG > 1
	cout << "params.size(): " << params.size() << ", this->parameters.size(): " << this->parameters.size() << endl;
#endif
	for (int i = 0; i < params.size() && i < this->parameters.size(); i++) {
		this->parameters.setValue(i + 1, params[i]);
	}
	
	//iterate through subproductions
	for (vector<SubProduction>::iterator subProdIter = this->subproductions.begin();
		 subProdIter != this->subproductions.end(); subProdIter++) {
#if CONV_DEBUG > 1
		cout << "this->subproductions.size(): " << this->subproductions.size() << endl;
#endif
		SubProduction &sp = *subProdIter;
		bool conditionsOK = true;
		//check conditions of subproduction
		for (vector<Condition>::iterator condIter = sp.conditions.begin();
			 condIter != sp.conditions.end(); condIter++) {
			if (conditionsOK == false) {
				break; //because it's no use checking further
			}
			Condition &c = *condIter;
			switch (c.relation) {
				case r_greater:
					if (this->parameters.getValue(c.parameter) <= c.value) {
						conditionsOK = false;
					}
					break;
					case r_greaterEqual:
					if (this->parameters.getValue(c.parameter) < c.value) {
						conditionsOK = false;
					}
					break;
					case r_less:
					if (this->parameters.getValue(c.parameter) >= c.value) {
						conditionsOK = false;
					}
					break;
					case r_lessEqual:
					if (this->parameters.getValue(c.parameter) > c.value) {
						conditionsOK = false;
					}
					break;
					case r_equal:
					if (this->parameters.getValue(c.parameter) != c.value) {
						conditionsOK = false;
					}
					break;
					case r_different:
					if (this->parameters.getValue(c.parameter) == c.value) {
						conditionsOK = false;
					}
					break;
			}	
		}
		if (conditionsOK) {
			//iterate through each action in subproduction
			for (int i = 0; i < sp.actions.size(); i++) {
#if CONV_DEBUG > 1
				cout << "sp.actions.size(): " << sp.actions.size() << endl;
#endif
				Action *action = sp.actions[i].action;
				vector<SString> strParams = sp.actions[i].params;
				vector<double> params;
				//replace parameter names with values
				for (vector<SString>::iterator paramIter = strParams.begin();
					 paramIter != strParams.end(); paramIter++) {
					SString parameter;
					SString element;
					int pos = 0;
					while ((*paramIter).getNextToken(pos, element, ';')) {
						if (element[0] == 'n') {
							double val = this->parameters.getValue(element);
							parameter += SString::valueOf(val) + ";";
						} else {
							parameter += element + ";";
						}
					}
					params.push_back(parseExpression(parameter));
				}
				
				ActionP ap;
				ap.action = action;
				ap.params = params;
				l.push_back(ap);
			}
		}
	}
	
	return l;
}

const SString Production::getF8Representation() {
	return this->name;
}









const char* GenoConv_F8ToF1::simpleprods="X[]^RrLlAaCcFfMmSsIiQqWw"; //Ee skipped


#define FIELDSTRUCT GenoConv_F8ToF1

static ParamEntry GENOCONVf8param_tab[]=
{
{"Genetics: f8: Converter",1,1,},
{"f8conv_maxlen", 0, 0, "Maximal genotype length", "d 10 10000 500", FIELD(maxF1Length),"Maximal length of the resulting f1 genotype, in characters. If the f8 L-system produces longer f1 genotype, it will be considered incorrect.", },
{0,},
};

#undef FIELDSTRUCT

Param GenoConv_F8ToF1::staticpar; 


GenoConv_F8ToF1::GenoConv_F8ToF1()
{
	name = "Generative encoding";
	in_format = '8';
	out_format = '1';
	mapsupport = 0;
	maxF1Length = 500;
        par.setParamTab(GENOCONVf8param_tab);
       	par.select(this);
        par.setDefault();
}


vector<SString> GenoConv_F8ToF1::readProductionNames(const SString &in) {
	vector<SString> names;
	SString line;
	int pos = 0;
	bool afterFirstProd = false;
	//ParsingStatus status = firstLine;
	while (in.getNextToken(pos, line, '\n')) {
#if CONV_DEBUG > 1
		std::cout << "### Line: " << line << std::endl;
#endif
		if (line.startsWith("P") && line.indexOf('(', 0) == -1) {
			afterFirstProd = true;
			continue;
		}
		if (!afterFirstProd) {
			continue;
		}
		int lParenIndex = line.indexOf('(', 0);
		if (line.startsWith("P") && lParenIndex != -1) {
			SString prodName = line.substr(0, lParenIndex);
#if CONV_DEBUG > 1
			std::cout << "###Production: " << prodName << std::endl;
#endif
			names.push_back(prodName);
		}
	}
	return names;
}

bool GenoConv_F8ToF1::checkSyntax(const char *geno) {
	return this->parseInput(geno, NULL);
}

SString GenoConv_F8ToF1::convert(SString &in, MultiMap *mmap) {
#if CONV_DEBUG > 0
	cout << "convert() start" << endl;
#endif
	SString dst = "";
	const char* src = in;
	
	if (in.len() < 1 || !this->checkSyntax(src)) {
		return SString();
	}
	
	Lsystem *lsystem = this->createLsystem(in);
	if (lsystem == NULL) {
		return SString();
	}
	if (lsystem->firstProductionName.empty()) {
                delete lsystem;
		return SString();
	}
	
#if CONV_DEBUG > 0
	for (map<string, Production*>::iterator i1 = lsystem->productions.begin(); i1 != lsystem->productions.end(); i1++) {
		Production *p = i1->second;
		cout << "Production: " << p->name << endl;
		for (vector<SubProduction>::iterator i2 = p->subproductions.begin(); i2 != p->subproductions.end(); i2++) {
			SubProduction sp = *i2;
			cout << "\tConditions" << endl;
			for (vector<Condition>::iterator i3 = sp.conditions.begin(); i3 != sp.conditions.end(); i3++) {
				cout << "\t\t" << *i3 << endl;
			}
			cout << "\tAction : params" << endl;
			for (int i = 0; i < sp.actions.size(); i++) {
				Action *a = sp.actions[i].action;
				vector<SString> strParams = sp.actions[i].params;
				cout << "\t\t" << a->name << " : ";
				for (vector<SString>::iterator i4 = strParams.begin(); i4 != strParams.end(); i4++) {
					cout << *i4 << ", ";
				}
				cout << endl;
			}
		}
	}
#endif
#if CONV_DEBUG > 1
	cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
#endif
	
	//cout << "convert() 1" << endl;
	//set parameters for start production
	Production *firstProduction = lsystem->productions[lsystem->firstProductionName];
        if (firstProduction==NULL) {
                delete lsystem;
                return SString(""); //should never happen because of syntax validation
        }

	vector<double> params;
	params.assign(lsystem->startParams.size(), 0.0);
	//cout << "startParams->size: " << lsystem->startParams.size() << endl;
	for (map<string, double>::iterator iter = lsystem->startParams.begin();
		 iter != lsystem->startParams.end(); iter++)
        {
		int position = firstProduction->parameters.getParameterPosition(stringToSString(iter->first));
		//cout << "position of " << iter->first << ": " << position << endl;
		//params.insert(params.begin() + (position - 1), iter->second); //no need because the vector has required length (assign above)
                if (position>params.size()) {
                        delete lsystem;
                        return SString("");
                }
		params[position - 1] = iter->second;
	}
	
	//cout << "convert() 2" << endl;
	
	ActionP ap;
	ap.action = firstProduction;
	ap.params = params;
	
	list<ActionP> actionList;
	actionList.push_back(ap);
	
	//cout << "iterations: " << lsystem->iterations << endl;
	for (int i = 0; i < lsystem->iterations; i++) {
		//cout << "convert() 2.1" << endl;
		list<ActionP> newList;
		for (list<ActionP>::iterator iter = actionList.begin(); iter != actionList.end(); iter++) {
			//cout << "convert() 2.1.1" << endl;
			Action *a = (*iter).action;
			vector<double> p = (*iter).params;
			if (a != NULL) {
				list<ActionP> tmpList = a->getActionList(p);
				newList.insert(newList.end(), tmpList.begin(), tmpList.end());
			}
		}
		actionList = newList;
	}
	
#if CONV_DEBUG > 1
	cout << "&&&&&&&&&&&&&&&&&&&&& ^ &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
	for (list<ActionP>::iterator it = actionList.begin(); it != actionList.end(); it++) {
		cout << (*it).action->name << "(";
		for (vector<double>::iterator it2 = (*it).params.begin(); it2 != (*it).params.end(); it2++) {
			cout << *it2 << ", ";
		}
		cout << ")" << endl;
	}
	cout << "&&&&&&&&&&&&&&&&&&&&& ^ &&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
#endif	
	for (list<ActionP>::iterator iter = actionList.begin(); iter != actionList.end(); iter++) {
		Action *a = (*iter).action;
		vector<double> p = (*iter).params;
		if (a != NULL) {
			dst += a->getF1Genotype(p);
			if (dst.len() > maxF1Length) {
                                delete lsystem;
				return SString(); //genotype becomes too long so we abort conversion
			}
		}
	}
	
	delete lsystem;
	
#if CONV_DEBUG > 0
	cout << "convert() end" << endl;
#endif
	return dst;
}

//Lsystem* GenoConv_F8ToF1::createLsystem(const SString &in) {
Lsystem* GenoConv_F8ToF1::createLsystem(SString in) {
#if CONV_DEBUG > 0
	cout << "createLsystem() start" << endl;
#endif
	Lsystem *lsys = new Lsystem();

	//read production names and create objects for them
	vector<SString> names = this->readProductionNames(in);
	for (vector<SString>::iterator nameIter = names.begin(); nameIter != names.end(); nameIter++) {
		Production *production = new Production();
		production->name = *nameIter;
		//lsystem->productions.insert(make_pair(*nameIter, production));
		lsys->productions[sstringToString(*nameIter)] = production;
	}
	
#if CONV_DEBUG > 1
	cout << "lsystemprodsize " << lsys->productions.size() << endl;
	for (map<string, Production*>::iterator iii = lsys->productions.begin(); iii != lsys->productions.end(); iii++) {
		cout << "PPP '" << iii->first << "' adr " << (long) &(iii->second) << endl;
	}
#endif
	
	const char* src = in;
	bool result = this->parseInput(src, lsys);
	if (!result) {
		delete lsys;
		return NULL;
	}
	
#if CONV_DEBUG > 1
	
	cout << "@@@@@ Parsed" << endl;
	
	cout << "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&" << endl;
#endif
	//final check
	for (map<string, Production*>::iterator prodIter = lsys->productions.begin();
		 prodIter != lsys->productions.end(); prodIter++) {
		for (vector<SubProduction>::iterator subProdIter = prodIter->second->subproductions.begin();
			 subProdIter != prodIter->second->subproductions.end(); subProdIter++) {
			SubProduction subProduction = *subProdIter;
			for (vector<ActionStrP>::iterator i = subProduction.actions.begin();
				 i != subProduction.actions.end(); /*nothing*/) {
				if ((*i).action == NULL) {
					i = subProduction.actions.erase(i);
				} else {
					i++;
				}
			}
		}
	}
#if CONV_DEBUG > 0
	cout << "createLsystem() end" << endl;
#endif
	return lsys;
}

bool GenoConv_F8ToF1::parseInput(const char* src, Lsystem* lsys) {
	//initialize parser
	int yv;
	istringstream input;
	input.str(string(src));
	ostringstream output;
	yyFlexLexer scanner(&input, &output);
	bool syntaxOk = false;
	void* pParser = prs::ParseAlloc(malloc, lsys, (lsys == NULL), &syntaxOk);
	struct prs::Token t0;
	//t0.strValue = "";
	memset(t0.strArrValue, 0, 30);
	extern YYSTYPE yylval;
	
	//parse input
	// on EOF yylex will return 0
	while((yv = scanner.yylex()) != 0) {
#if CONV_DEBUG > 1
		cout << " yylex() " << yv << " yylval.strVal " << yylval.strVal << endl;
#endif
		memset(t0.strArrValue, 0, 30);
		sprintf(t0.strArrValue, yylval.strVal);
		prs::Parse (pParser, yv, t0);
	}
	prs::Parse(pParser, 0, t0);
	prs::ParseFree(pParser, free);
	
	return syntaxOk;	
}
#undef CONV_DEBUG

