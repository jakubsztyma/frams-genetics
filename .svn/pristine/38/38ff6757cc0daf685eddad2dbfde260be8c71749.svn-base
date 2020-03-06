%include {
#include <iostream>
#include <sstream>
#include "lemonglobal.h"
#include "conv_f8tof1.h"
#include "conv_f8_utils.h"
#define PARSER_DEBUG 0
}
%token_type {Token}
%default_type {Token}
%type production_tail {ProductionTailToken}
%type prod_piece {ProdPieceToken}

%left COMMA.
%left PLUS MINUS.
%left DIV TIMES.

program ::= counter start_params delim first_prod productions. {
	*syntaxOk = true;	
}

counter ::= double_val(A) NEWLINE. {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "counter ::= double_val NEWLINE." << std::endl;
#endif
		}
		lsys->iterations = (int) A.dblValue; 
	}
}
			
delim ::= DELIMETER NEWLINE.

start_params ::= start_parameter start_params.
start_params ::= .

start_parameter ::= param_name(A) ASSIGN double_val(B) NEWLINE. { 
	if (!syntaxOnly) {
		{	
#if PARSER_DEBUG > 0
			std::cout << "start_parameter ::= param_name ASSIGN double_val NEWLINE." << std::endl;
#endif
		}
		lsys->startParams[sstringToString(*(A.strValue))] = B.dblValue;
#if PARSER_DEBUG > 0
		//cout << "**** " << lsys->startParams["n0"] << endl;
		//cout << "**** " << lsys->startParams["n1"] << endl;
#endif
		delete A.strValue;
	}
}

productions ::= production productions_others. 
productions_others ::= production productions_others. 
productions_others ::= .

first_prod ::= prod_name(A) NEWLINE. { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "first_prod ::= prod_name NEWLINE." << std::endl;
#endif
		}
		lsys->firstProductionName = sstringToString(*(A.strValue));
		delete A.strValue;
	}
}

production ::= production_head(A) SEMICOLON production_tail(B) NEWLINE. { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production ::= production_head SEMICOLON production_tail NEWLINE." << std::endl;
#endif
		}
		Production *p = lsys->productions[sstringToString(*(A.strValue))];
		for (vector<SString>::iterator paramIter = A.vectorStr->begin(); paramIter != A.vectorStr->end(); paramIter++) {
#if PARSER_DEBUG > 0
			std::cout << "1.1 " << *paramIter << std::endl;
			std::cout << *(A.strValue) << " # " << (p == NULL) << " # " << true << std::endl;
#endif
			p->parameters.addParameter(*paramIter);
		}
		p->subproductions = *(B.subproductions);
		delete A.strValue;
		delete A.vectorStr;
		delete B.subproductions;
	}
}

production_head(A) ::= prod_name(B) LPAREN prod_params(C) RPAREN. {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_head ::= prod_name LPAREN prod_params RPAREN." << std::endl;
#endif
		}
#if PARSER_DEBUG > 0
		std::cout << "---------" << *(B.strValue) << std::endl;
#endif
		A.strValue = new SString(*(B.strValue));
		A.vectorStr = new vector<SString>(*(C.vectorStr));
		delete B.strValue;
		delete C.vectorStr;
	}
}
production_head(A) ::= prod_name(B) LPAREN RPAREN. {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_head ::= prod_name LPAREN RPAREN." << std::endl;
#endif
		}
#if PARSER_DEBUG > 0
		std::cout << "---------" << *(B.strValue) << std::endl;
#endif
		A.strValue = new SString(*(B.strValue));
		A.vectorStr = new vector<SString>();
		delete B.strValue;
	}
}

prod_params(A) ::= param_name(B) COMMA prod_params(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_params ::= param_name COMMA prod_params." << std::endl;
#endif
		}
		A.vectorStr = new vector<SString>();
		A.vectorStr->push_back(*(B.strValue));
		for (vector<SString>::iterator iter = C.vectorStr->begin(); iter != C.vectorStr->end(); iter++) {
			A.vectorStr->push_back(*iter);
		}
		delete B.strValue;
		delete C.vectorStr;
	}
}
prod_params(A) ::= param_name(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_params ::= param_name." << std::endl;
#endif
		}
		A.vectorStr = new vector<SString>();
		A.vectorStr->push_back(*(B.strValue));
		delete B.strValue;
	}
}

production_tail(A) ::= subproduction(B) SEMICOLON production_tail(C). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_tail ::= subproduction SEMICOLON production_tail." << std::endl;
#endif
		}
		SubProduction sp;
		sp.conditions = *(B.vectorConditions);
		vector<ActionStrP> actions;
		actions.reserve(B.vectorActions->size());
		for (int i = 0; i < B.vectorActions->size() && i < B.parameters->size(); i++) {
			ActionStrP ap;
			ap.action = (*(B.vectorActions)).at(i);
			ap.params = (*(B.parameters)).at(i);
			actions.push_back(ap);
		}
		sp.actions = actions;
		A.subproductions = new vector<SubProduction>();
		A.subproductions->push_back(sp);
		for (vector<SubProduction>::iterator iter = C.subproductions->begin(); iter != C.subproductions->end(); iter++) {
			A.subproductions->push_back(*iter);
		}
		delete B.vectorConditions;
		delete B.vectorActions;
		delete B.parameters;
		delete C.subproductions;
	}
}
production_tail(A) ::= subproduction(B). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "production_tail ::= subproduction." << std::endl;
#endif
		}
		SubProduction sp;
		sp.conditions = *(B.vectorConditions);
		vector<ActionStrP> actions;
		actions.reserve(B.vectorActions->size());
		for (int i = 0; i < B.vectorActions->size() && i < B.parameters->size(); i++) {
			ActionStrP ap;
			ap.action = (*(B.vectorActions)).at(i);
			ap.params = (*(B.parameters)).at(i);
			actions.push_back(ap);
		}
		sp.actions = actions;
		A.subproductions = new vector<SubProduction>();
		A.subproductions->push_back(sp);
		delete B.vectorConditions;
		delete B.vectorActions;
		delete B.parameters;
	}
}

subproduction(A) ::= conditions(B) PIPE real_prod(C). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "subproduction ::= conditions PIPE real_prod." << std::endl;
#endif
		}
		A.vectorConditions = new vector<Condition>(*(B.vectorConditions));
		A.vectorActions = new vector<Action*>(*(C.vectorActions));
		A.parameters = new vector<vector<SString> >(*(C.parameters));
		delete B.vectorConditions;
		delete C.vectorActions;
		delete C.parameters;
	}
}
subproduction(A) ::= real_prod(C). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "subproduction ::= real_prod." << std::endl;
#endif
		}
		A.vectorConditions = new vector<Condition>();
		A.vectorActions = new vector<Action*>(*(C.vectorActions));
		A.parameters = new vector<vector<SString> >(*(C.parameters));
		delete C.vectorActions;
		delete C.parameters;
	}
}

conditions(A) ::= conditions(B) COMMA conditions(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "conditions ::= condition COMMA conditions." << std::endl;
#endif
		}
		A.vectorConditions = new vector<Condition>(*(B.vectorConditions));
		for (vector<Condition>::iterator iter = C.vectorConditions->begin(); 
			 iter != C.vectorConditions->end(); iter++) {
			A.vectorConditions->push_back(*iter);
		}	
		delete B.vectorConditions;
		delete C.vectorConditions;
	}
}
conditions(A) ::= condition(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "conditions ::= condition." << std::endl;
#endif
		}
		A.vectorConditions = new vector<Condition>();
		A.vectorConditions->push_back(*(B.cond));
		delete B.cond;
	}
}

condition(A) ::= param_name(B) LESS double_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name LESS double_val." << std::endl;
#endif
		}
		A.cond = new Condition();
		A.cond->relation = r_less;
		A.cond->parameter = *(B.strValue);
		A.cond->value = C.dblValue;
		delete B.strValue;
	}
}
condition(A) ::= param_name(B) LESS_EQUAL double_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name LESS_EQUAL double_val." << std::endl;
#endif
		}
		A.cond = new Condition();
		A.cond->relation = r_lessEqual;
		A.cond->parameter = *(B.strValue);
		A.cond->value = C.dblValue;
		delete B.strValue;
	}
}
condition(A) ::= param_name(B) NOT_EQUAL double_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name NOT_EQUAL double_val." << std::endl;
#endif
		}
		A.cond = new Condition();
		A.cond->relation = r_different;
		A.cond->parameter = *(B.strValue);
		A.cond->value = C.dblValue;
		delete B.strValue;
	}
}
condition(A) ::= param_name(B) EQUAL double_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name EQUAL double_val." << std::endl;
#endif
		}
		A.cond = new Condition();
		A.cond->relation = r_equal;
		A.cond->parameter = *(B.strValue);
		A.cond->value = C.dblValue;
		delete B.strValue;
	}
}
condition(A) ::= param_name(B) GREATER_EQUAL double_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name GREATER_EQUAL double_val." << std::endl;
#endif
		}
		A.cond = new Condition();
		A.cond->relation = r_greaterEqual;
		A.cond->parameter = *(B.strValue);
		A.cond->value = C.dblValue;
		delete B.strValue;
	}
}
condition(A) ::= param_name(B) GREATER double_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "condition ::= param_name GREATER double_val." << std::endl;
#endif
		}
		A.cond = new Condition();
		A.cond->relation = r_greater;
		A.cond->parameter = *(B.strValue);
		A.cond->value = C.dblValue;
		delete B.strValue;
	}
}

real_prod(A) ::= . {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "real_prod ::= ." << std::endl;
#endif
		}
		A.vectorActions = new vector<Action*>();
		A.parameters = new vector<vector<SString> >();
	}
}
real_prod(A) ::= prod_piece(B) real_prod(C). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "real_prod ::= prod_piece real_prod." << std::endl;
			for (vector<Action*>::iterator actIter = B.actions->begin(); actIter != B.actions->end(); actIter++) {
				cout << "\t" << (*actIter)->name << endl;
			}
#endif
		}
		//A = B.clone() && A.append(C)
		A.vectorActions = new vector<Action*>(*(B.actions));
		A.parameters = new vector<vector<SString> >(*(B.parameters));
		if (C.vectorActions != NULL && C.parameters != NULL) {
			for (vector<Action*>::iterator iter = C.vectorActions->begin(); iter != C.vectorActions->end(); iter++) {
				A.vectorActions->push_back(*iter);
			}
			for(vector<vector<SString> >::iterator iter = C.parameters->begin(); iter != C.parameters->end(); iter++) {
				A.parameters->push_back(*iter);
			}
			delete C.vectorActions;
			delete C.parameters;
		} else {
#if PARSER_DEBUG > 0
			cout << "\tNULL~~~~~~~~~~~~" << endl;
#endif
		}
		delete B.actions;
		delete B.parameters;
	}
}
real_prod(A) ::= FORLOOP_BEGIN real_prod(B) FORLOOP_END LPAREN single_val(C) RPAREN real_prod(D). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "real_prod ::= FORLOOP_BEGIN real_prod FORLOOP_END LPAREN single_val RPAREN." << std::endl;
#endif
		}
		A.vectorActions = new vector<Action*>();
		A.parameters = new vector<vector<SString> >();
		int iterations = (int) parseExpression(*(C.strValue));
		for (int i = 0; i < iterations; i++) {
			for (vector<Action*>::iterator iter = B.vectorActions->begin(); iter != B.vectorActions->end(); iter++) {
				A.vectorActions->push_back(*iter);
			}
			for (vector<vector<SString> >::iterator iter = B.parameters->begin(); iter != B.parameters->end(); iter++) {
				A.parameters->push_back(*iter);
			}
		}
		for (vector<Action*>::iterator iter = D.vectorActions->begin(); iter != D.vectorActions->end(); iter++) {
			A.vectorActions->push_back(*iter);
		}
		for (vector<vector<SString> >::iterator iter = D.parameters->begin(); iter != D.parameters->end(); iter++) {
			A.parameters->push_back(*iter);
		}
		delete B.vectorActions;
		delete B.parameters;
		delete C.strValue;
		delete D.vectorActions;
		delete D.parameters;
	}
}

prod_piece(A) ::= prod_name(B) LPAREN multiple_val(C) RPAREN. {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= prod_name LPAREN multiple_val RPAREN." << std::endl;
#endif
		}
		A.actions = new vector<Action*>();
		A.parameters = new vector<vector<SString> >();
		
		Production *p = lsys->productions.find(sstringToString(*(B.strValue)))->second;
		A.actions->push_back(p);
		A.parameters->push_back(*(C.vectorStr));
		delete B.strValue;
		delete C.vectorStr;
	}
}
prod_piece(A) ::= prod_name(B) LPAREN RPAREN. {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= prod_name LPAREN RPAREN." << std::endl;
#endif
		}
		A.actions = new vector<Action*>();
		vector<SString> param;
		A.parameters = new vector<vector<SString> >();
		A.parameters->push_back(param);
		
		Production *p = lsys->productions.find(sstringToString(*(B.strValue)))->second;
		A.actions->push_back(p);
		delete B.strValue;
	}
}
prod_piece(A) ::= command(B). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= command."  << std::endl;
#endif
		}
		A.actions = new vector<Action*>();
		A.parameters = new vector<vector<SString> >();
		PrimitiveProduction *pp = lsys->getPrimitiveProduction(*(B.strValue));
		A.actions->push_back(pp);
		vector<SString> param;
		A.parameters->push_back(param);
		delete B.strValue;
	}
}
prod_piece(A) ::= param_name(B). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= paramName." << std::endl;
#endif
		}
		A.actions = new vector<Action*>();
		A.parameters = new vector<vector<SString> >();
		ParamProduction *pp = lsys->getParamProduction(*(B.strValue));
		A.actions->push_back(pp);
		vector<SString> param;
		param.push_back(*(B.strValue));
		A.parameters->push_back(param);
		delete B.strValue;
	}
}
prod_piece(A) ::= neuron(B). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_piece ::= neuron." << std::endl;
#endif
		}
		A.actions = new vector<Action*>();
		A.parameters = new vector<vector<SString> >();
		NeuronProduction *np = new NeuronProduction(*(B.strValue));
		lsys->neuronProductions.push_back(np);
		A.actions->push_back(np);
		vector<SString> param;
		A.parameters->push_back(param);
		delete B.strValue;		
	}
}

multiple_val(A) ::= single_val(B). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "multiple_val ::= single_val." << std::endl;
#endif
		}
		A.vectorStr = new vector<SString>();
		A.vectorStr->push_back(SString(*(B.strValue)));
		delete B.strValue;
	}
}
multiple_val(A) ::= multiple_val(B) COMMA multiple_val(C). {
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "multiple_val ::= multiple_val COMMA multiple_val." << std::endl;
#endif
		}
		A.vectorStr = new vector<SString>();
		for (vector<SString>::iterator iter = B.vectorStr->begin(); iter != B.vectorStr->end(); iter++) {
			A.vectorStr->push_back(*iter);
		}
		for (vector<SString>::iterator iter = C.vectorStr->begin(); iter != C.vectorStr->end(); iter++) {
			A.vectorStr->push_back(*iter);
		}
		delete B.vectorStr;
		delete C.vectorStr;
	}
}

single_val(A) ::= double_val(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= double_val." << std::endl;
#endif
		}
		A.strValue = new SString(SString::valueOf(B.dblValue) + ";");
		delete B.strValue;
	}
}
single_val(A) ::= param_name(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= param_name." << std::endl;
#endif
		}
		A.strValue = new SString(*(B.strValue) + ";");
		delete B.strValue;
	}
}
single_val(A) ::= single_val(B) PLUS single_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val PLUS single_val." << std::endl;
#endif
		}
		A.strValue = new SString(*(B.strValue) + *(C.strValue) + "+;");
		delete B.strValue;
		delete C.strValue;
	}
}
single_val(A) ::= single_val(B) MINUS single_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val MINUS single_val." << std::endl;
#endif
		}
		A.strValue = new SString(*(B.strValue) + *(C.strValue) + "-;");
		delete B.strValue;
		delete C.strValue;
	}
}
single_val(A) ::= single_val(B) TIMES single_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val TIMES single_val." << std::endl;
#endif
		}
		A.strValue = new SString(*(B.strValue) + *(C.strValue) + "*;");
		delete B.strValue;
		delete C.strValue;
	}
}
single_val(A) ::= single_val(B) DIV single_val(C). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "single_val ::= single_val DIV single_val." << std::endl;
#endif
		}
		A.strValue = new SString(*(B.strValue) + *(C.strValue) + "/;");
		delete B.strValue;
		delete C.strValue;
	}
}

double_val(A) ::= DOUBLE_VAL(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "double_val ::= DOUBLE_VAL. -> " << (string(B.strArrValue)).c_str() << std::endl;
#endif
		}
		A.dblValue = atof((string(B.strArrValue)).c_str()); 
	}
}
param_name(A) ::= PARAM_NAME(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "param_name ::= PARAM_NAME. -> " << (string(B.strArrValue)).c_str() << std::endl;
#endif
		}
		A.strValue = new SString(string(B.strArrValue).c_str());
	}
}
prod_name(A) ::= PROD_NAME(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "prod_name ::= PROD_NAME." << std::endl;
#endif
		}
		A.strValue = new SString(string(B.strArrValue).c_str());
	}
}
command(A) ::= COMMAND(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "command ::= COMMAND." << std::endl;
#endif
		}
		A.strValue = new SString(string(B.strArrValue).c_str()); 
	}
}
neuron(A) ::= NEURON(B). { 
	if (!syntaxOnly) {
		{
#if PARSER_DEBUG > 0
			std::cout << "neuron ::= NEURON." << std::endl;
#endif
		}
		A.strValue = new SString(string(B.strArrValue).c_str()); 
	}
}
