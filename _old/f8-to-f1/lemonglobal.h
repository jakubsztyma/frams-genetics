/*
 *  lemonglobal.h
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-04-04.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */
#include "conv_f8tof1.h"
#include "sstring.h"

struct Token {
	double dblValue;
	//int intValue;
	SString *strValue;
	char strArrValue[30];
	vector<SString> *vectorStr;
	vector<Condition> *vectorConditions;
	vector<Action*> *vectorActions;
	vector<vector<SString> > *parameters;
	Condition *cond;
};

struct ProductionTailToken {
	vector<SubProduction> *subproductions;
};

struct ProdPieceToken {
	vector<vector<SString> > *parameters;
	vector<Action*> *actions;
};
