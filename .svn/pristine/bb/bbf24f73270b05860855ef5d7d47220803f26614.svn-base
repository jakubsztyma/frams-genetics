/*
 *  geno_f8.h
 *  L-systemToF1
 *
 *  Created by Maciej Wajcht on 08-06-07.
 *  Copyright 2008 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _GENO_F8_H_
#define _GENO_F8_H_

#include "param.h"
#include "sstring.h"
#include "geno_fx.h"
#include <vector>
#include "conv_f8tof1.h"

using namespace std;

//indices in array with probabilities of mutation operators
#define F8_CHANGE_BEGINNING_ARG 0
#define F8_CHANGE_ARG 1
#define F8_DELETE_COMMAND 2
#define F8_INSERT_COMMANDS 3
#define F8_ENCAPSULATE 4
#define F8_CHANGE_CONDITION_SIGN 5
//#define F8_REPLACE_COMMAND 6
#define F8_ADD_PARAMETER 6
#define F8_ADD_CONDITION 7
#define F8_ADD_SUBPRODUCTION 8
#define F8_CHANGE_ITERATIONS_NUMBER 9
#define F8_DELETE_PARAMETER 10
#define F8_DELETE_CONDITION 11
#define F8_ADD_LOOP 12
#define F8_DELETE_LOOP 13
#define F8_DELETE_PRODUCTION 14
#define F8_OPERATION_COUNT 15

class ProductionInfo {
public:
	ProductionInfo() {};
	ProductionInfo(SString name, int paramCount);
	SString name;
	int paramCount;
	vector<SString> paramNames;
	bool isFirstProduction;
};

class Geno_f8 : public Geno_fx {
public:
	Geno_f8();
	~Geno_f8();

   	//probabilities of mutation operators
	double operation[F8_OPERATION_COUNT];

	int checkValidity(const char * geno);
	int validate(char * &geno);
	int mutate(char *&g,float& chg, int &method);
	int crossOver(char *&g1,char *&g2,float& chg1,float& chg2);
	unsigned long style(const char *g, int pos);
	char* getSimplest();

	SString testMutate(SString &in, int method);
protected:
	GenoConv_F8ToF1 *converter;
	
	vector<char> simpleCommandLetters;
	
	int getProductionsCount(const SString &in);
	vector<ProductionInfo> getProductionsInfo(const SString &in);
	bool checkProdNameExist(vector<ProductionInfo> info, SString name) const;
	bool checkParamNameExist(vector<SString> names, SString name) const;
	SString getOppositeCondition(const SString &c) const;
	SString removeProductionCalls(const SString production) const;
	SString mutateChangeBeginningArg(SString &in, float& chg);
	SString mutateChangeArg(SString &in, float& chg);
	SString mutateDeleteCommand(SString &in, float& chg);
	SString mutateInsertCommands(SString &in, float& chg);
	//SString mutateReplaceCommand(SString &in, float& chg);
	//SString mutateInsertReplaceCommand(SString &in, float& chg, bool replace);
	SString mutateEncapsulate(SString &in, float& chg);
	SString mutateDeleteProduction(SString &in, float& chg);
	SString mutateChangeConditionSign(SString &in, float& chg);
	SString mutateAddParameter(SString &in, float& chg);
	SString mutateAddCondition(SString &in, float& chg);
	SString mutateDeleteParameter(SString &in, float& chg);
	SString mutateDeleteCondition(SString &in, float& chg);
	SString mutateAddLoop(SString &in, float& chg);
	SString mutateDeleteLoop(SString &in, float& chg);
	SString mutateAddSubproduction(SString &in, float& chg);
	SString mutateChangeIterationsNumber(SString &in, float& chg);
	SString addParameterToCalls(const SString line, SString &prodName);
	SString deleteParameterFromCalls(const SString line, SString &prodName, int paramIdx);
private:
	RelationType getDifferentCondition(RelationType type);
};

#endif

