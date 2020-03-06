#include <math.h> //HUGE_VAL
#include "geno_f0Fuzzy.h"
#include "neuroimpl-fuzzy.h"
#include "neuroimpl-fuzzy-f0.h"
#include "rndutil.h"
#include "model.h"

#define FIELDSTRUCT Geno_f0Fuzzy
static ParamEntry GENO7param_tab[]=   //external access to f0Fuzzy genetic parameters
{
{"Genetics: f0-fuzzy",1,8,},
{"f0Fuzzy_maxSets",0,0,"Max. sets","d 0 999",FIELD(maximumSetsNr),"Maximum number of fuzzy sets",},
{"f0Fuzzy_maxRules",0,0,"Max. rules","d 0 999",FIELD(maximumRulesNr),"Maximum number of fuzzy rules",},
{"f0Fuzzy_prob0",0,0,"Add new fuzzy set (with simple rule)","f 0 1",FIELD(probtab[F0Fuzzy_ADDSET]),"Probability of adding new fuzzy set with simple rule, which uses this set (1=proceed on all genes, 0.1=proceed on ten percent)",},
{"f0Fuzzy_prob1",0,0,"Remove fuzzy set","f 0 1",FIELD(probtab[F0Fuzzy_REMSET]),"Probability of removing fuzzy set (1=proceed on all genes, 0.1=proceed on ten percent)",},
{"f0Fuzzy_prob2",0,0,"Add new rule","f 0 1",FIELD(probtab[F0Fuzzy_ADDRULE]),"Probability of adding new rule (1=proceed on all genes, 0.1=proceed on ten percent)",},
{"f0Fuzzy_prob3",0,0,"Remove rule","f 0 1",FIELD(probtab[F0Fuzzy_REMRULE]),"Probability of removing rule (1=proceed on all genes, 0.1=proceed on ten percent)",},
{"f0Fuzzy_prob4",0,0,"Add new input/output to existing rule","f 0 1",FIELD(probtab[F0Fuzzy_MODRULE]),"Probability of adding new input or output to exisitng rule (1=proceed on all genes, 0.1=proceed on ten percent)",},
{"f0Fuzzy_prob5",0,0,"Remove input/output from existing rule","f 0 1",FIELD(probtab[F0Fuzzy_MOD2RULE]),"Probability of removing input or output from exisitng rule (1=proceed on all genes, 0.1=proceed on ten percent)",},
{0,},
};
#undef FIELDSTRUCT

Geno_f0Fuzzy::Geno_f0Fuzzy()
{
  par.setParamTab(GENO7param_tab);
  par.select(this);
  supported_format='0';
  name="Fuzzy control system";
  maximumSetsNr=50;
  maximumRulesNr=100;
  probtab[F0Fuzzy_ADDSET] = 0.01;
  probtab[F0Fuzzy_REMSET] = 0.0112;
  probtab[F0Fuzzy_ADDRULE]= 0.01;
  probtab[F0Fuzzy_REMRULE]= 0.0222; //when new set is created, new rule (using this new set) is also created but when rule is removed, unused set are removed too
  probtab[F0Fuzzy_MODRULE] = 0.005;
  probtab[F0Fuzzy_MOD2RULE] = 0.005;
}

char *Geno_f0Fuzzy::getSimplest()
{
  return "p:\np:1\nj:0,1\nn:d=\"G\",j=0\nn:d=\"|\",j=0\nn:d=\"Fuzzy: ns=1, nr=1, fs=-1;0;0;1;, fr=0;0:0;0/\"\nn:d=\"ChSel:ch=0\"\nc:2, 0\nc:3, 2\nc:1, 3\n";
};

int Geno_f0Fuzzy::markDuplicatedSets(const double fuzzySets1[], const int setsNr1, const double fuzzySets2[], const int setsNr2, int transTable[], const int setsNr3)
{
  if(setsNr1+setsNr2!=setsNr3) return -1;
  int i, j;
  for(i=0;i<setsNr1;i++)
    for(j=0;j<setsNr2;j++)
      if((fuzzySets1[4*i]==fuzzySets2[4*j])&&(fuzzySets1[4*i+1]==fuzzySets2[4*j+1])&&
         (fuzzySets1[4*i+2]==fuzzySets2[4*j+2])&&(fuzzySets1[4*i+3]==fuzzySets2[4*j+3]))
           transTable[setsNr1+j]=i; //mark equal sets
  return 0;
}


int Geno_f0Fuzzy::nearestSet(double set[4], double sets[], int setsNr)
{
  int i, nearest=0;
  double disCur, disNear;

  disCur=disNear=10; //the largest distance i between -1;-1;-1;-1 and 1;1;1;1, and equals 8.
  for(i=0;i<setsNr;i++)
  {
    disCur=fabs(sets[4*i]-set[0])+fabs(sets[4*i+1]-set[1])+
           fabs(sets[4*i+2]-set[2])+fabs(sets[4*i+3]-set[3]);
    if(disCur<disNear){ nearest=i; disNear=disCur; }
  }

  return nearest;
}

int Geno_f0Fuzzy::changeSetsNumeration(SString &rules, int transTable[], int nrItems)
{
  //for each rule (from rules string), change number of fuzzy set into new one
  int pos=0;
  SString oneRule, newRules;
  while(rules.getNextToken(pos, oneRule, '/'))
  {
    SString cond, dec, nrStr;
    int p=0, setNr;
    char temp[10];
    oneRule.getNextToken(p, cond, ':');
    oneRule.getNextToken(p, dec, '/');
    p=0;
    while(cond.getNextToken(p, nrStr, ';')) //first: input number
    {
      newRules += nrStr; newRules += ";";
      cond.getNextToken(p, nrStr, ';'); //then fuzzy set nr
      if((setNr=atoi(nrStr))>=nrItems)
        sprintf(temp, "%i", setNr); //might occur when new fuzzy set and new fuzzy rule added - does not need to be transformated
      else
        sprintf(temp, "%i", transTable[setNr]); //transformate set number
      newRules += temp;
      if(p<cond.len())
        newRules += ";";
    }
    p=0; newRules += ":";
    while(dec.getNextToken(p, nrStr, ';')) //first: output number
    {
      newRules += nrStr; newRules += ";";
      dec.getNextToken(p, nrStr, ';'); //then fuzzy set nr
      if((setNr=atoi(nrStr))>=nrItems)
        sprintf(temp, "%i", setNr); //might occur when new fuzzy set and new fuzzy rule added - does not need to be transformated
      else
        sprintf(temp, "%i", transTable[setNr]); //transformate set number
      newRules += temp;
      if(p<dec.len())
        newRules += ";";
    }
    newRules += "/";
  }

  rules=newRules;
  return 0;
}

int Geno_f0Fuzzy::removeUnusedSets(SString &sets, SString rules, int &setsNr, int transTable[])
{
  //for each rule (from rules string), change number of fuzzy set into new one
  int pos=0, i, nrUsedSets, *transTableOryginal = new int[setsNr];
  bool *setUsed = new bool[setsNr], unusedPresent;
  SString oneRule, newSets, nrStr;
  for(i=0;i<setsNr;i++) { setUsed[i]=false; transTableOryginal[i]=transTable[i];} //copy oryginal values of trans table

  //mark used sets
  while(rules.getNextToken(pos, oneRule, '/'))
  {
    SString cond, dec;
    int p=0, setNr;
    oneRule.getNextToken(p, cond, ':');
    oneRule.getNextToken(p, dec, '/');
    p=0;
    while(cond.getNextToken(p, nrStr, ';')) //first: input number
    {
      cond.getNextToken(p, nrStr, ';'); //then fuzzy set nr
      setNr=atoi(nrStr);
      if((setNr>=setsNr)||(setNr<0)) setNr=randomN(setsNr); //if everything ok, impossible to happen
      setUsed[setNr]=true;
    }
    p=0;
    while(dec.getNextToken(p, nrStr, ';')) //first: output number
    {
      dec.getNextToken(p, nrStr, ';'); //then fuzzy set nr
      setNr=atoi(nrStr);
      if((setNr>=setsNr)||(setNr<0)) setNr=randomN(setsNr); //if everything ok,impossible to happen
      setUsed[setNr]=true;
    }
  }

  for(i=0, nrUsedSets=0, unusedPresent=false;i<setsNr;i++)
    if(setUsed[i]==false)
    {
      unusedPresent=true;
      //change numbers in transTable - some set will be removed, so further renumeration is needed
      for(int j=0;j<setsNr;j++) if(transTableOryginal[j]>i) transTable[j]--;
    }
    else
      nrUsedSets++;

  SAFEDELETEARRAY(transTableOryginal)

  if(!unusedPresent)
  {
    SAFEDELETEARRAY(setUsed)
    return 0; //all sets are necessery
  }

  //remove from new fuzzy sets unused sets
  pos=0;
  newSets="";
  int semicol=0, nrOfSet=0;
  SString tempSet="";
  while(sets.getNextToken(pos, nrStr, ';'))
  {
    tempSet += nrStr; tempSet += ';';
    if(++semicol==4)
    {
      if(setUsed[nrOfSet])
        newSets += tempSet;
      tempSet="";
      semicol=0;
      nrOfSet++;
    }
  }

  setsNr=nrUsedSets;
  sets=newSets;

  SAFEDELETEARRAY(setUsed)

  return 1;
}

int Geno_f0Fuzzy::removeExcessiveInOut(SString &rules, int inputsNr, int outputsNr)
{
  SString newRules, oneRule;
  int pos, i;
  char tempChar50[50];

  pos=0;
  while(rules.getNextToken(pos, oneRule, '/'))
  {
    SString cond, dec, nrStr, newInput, newOutput;
    int p=0;
    bool *usedInOut, first;

    oneRule.getNextToken(p, cond, ':');
    oneRule.getNextToken(p, dec, '/');

    //create input table
    usedInOut = new bool[inputsNr]; for(i=0;i<inputsNr;i++) usedInOut[i]=false;
    //create new rule, which omits double inputs
    p=0; newInput=""; first=true;
    while(cond.getNextToken(p, nrStr, ';')) //first: input number
    {
      int inNr=atoi(nrStr), setNr;
      cond.getNextToken(p, nrStr, ';'); //then fuzzy set nr
      setNr=atoi(nrStr);
      if(!usedInOut[inNr]) //not used yet
      {
        if(!first) newInput += ';';
        first=false;
        sprintf(tempChar50, "%i;%i", inNr, setNr);
        newInput += tempChar50;
        usedInOut[inNr]=true;
      }
    }
    SAFEDELETEARRAY(usedInOut)

    //create output table
    usedInOut = new bool[outputsNr]; for(i=0;i<outputsNr;i++) usedInOut[i]=false;
    //create new rule, which omits double outputs
    p=0; newOutput=""; first=true;
    while(dec.getNextToken(p, nrStr, ';')) //first: output number
    {
      int outNr=atoi(nrStr), setNr;
      dec.getNextToken(p, nrStr, ';'); //then fuzzy set nr
      setNr=atoi(nrStr);
      if(!usedInOut[outNr]) //not used yet
      {
        if(!first) newOutput += ';';
        first=false;
        sprintf(tempChar50, "%i;%i", outNr, setNr);
        newOutput += tempChar50;
        usedInOut[outNr]=true;
      }
    }
    SAFEDELETEARRAY(usedInOut)

    newRules += newInput + ":" + newOutput + "/";
  }

  rules=newRules;

  return 0;
}

int Geno_f0Fuzzy::removeExcessiveRules(SString &rules, int setsNr, int &rulesNr)
{
  int *rulDef, //for each rule, remembers number of inputs and number of outputs used (in rule)
      **rulesBody, //for each rule, remembers: nr input, nr fuzzy set ... nr output, nr fuzzy set ...
      i, j, k, l, nrRulesToRemove, pos, unimpTemp;
  const int original_rulesNr=rulesNr;
  bool *rulesToRemove = new bool[original_rulesNr];
  SString newRules, oneRule;

  for(i=0;i<original_rulesNr;i++) rulesToRemove[i]=false;
  //create space for rules definition: number of inputs and outputs for each rule
  rulDef = new int[2*original_rulesNr];
  if(FuzzyF0String::countInputsOutputs(rules, rulDef, original_rulesNr))
  {
    SAFEDELETEARRAY(rulDef)
    SAFEDELETEARRAY(rulesToRemove)
    return -1; //error while parsing rules string
  }
  //create space for rules
  rulesBody = new int*[original_rulesNr];   //list of rules that will contain rules body
  for (i=0;i<original_rulesNr;i++) rulesBody[i] = new int[2*(rulDef[2*i]+rulDef[2*i+1])];  //each rule can have different number of inputs and outputs
  //get from string rules body into integer variables
  if(FuzzyF0String::convertStrToRules(rules, rulDef, rulesBody, setsNr, original_rulesNr, unimpTemp))
  {
    SAFEDELETEARRAY(rulDef)
    if(rulesBody) for(i=0;i<original_rulesNr;i++) SAFEDELETEARRAY(rulesBody[i])
    SAFEDELETEARRAY(rulesBody)
    SAFEDELETEARRAY(rulesToRemove)
    return -2; //error while parsing rules string
  }

  nrRulesToRemove=0;
  for(i=0;i<original_rulesNr;i++) //for every rule...
    for(j=i+1;j<original_rulesNr;j++) //...compare with further rule's...
    {
      int similarity=0;
      if(rulDef[2*j]!=rulDef[2*i]) continue;// do not check rules with different number of inputs
      for(k=0;k<rulDef[2*i];k++) //..conditional part rule i
        for(l=0;l<rulDef[2*j];l++) //..conditional part rule j
          if((rulesBody[i][k]==rulesBody[j][l])&&(rulesBody[i][k+1]==rulesBody[j][l+1])) //if inputs nr and fuzzy sets nr are the same
            similarity++; //increase similarity number
      //every input and fuzzy set number of rule j equals input and fuzzy set nr of rule i
      if((similarity==rulDef[2*j])&&(similarity==rulDef[2*i]))
      {
        if(!rulesToRemove[j]) nrRulesToRemove++; //because might happen, that rule is mark to remove more than once
        rulesToRemove[j]=true;
      }
    }

  if(nrRulesToRemove>=original_rulesNr) //in fact, it is impossible; but it's safe to protect
  {
    SAFEDELETEARRAY(rulDef)
    if(rulesBody) for(i=0;i<original_rulesNr;i++) SAFEDELETEARRAY(rulesBody[i])
    SAFEDELETEARRAY(rulesBody)
    SAFEDELETEARRAY(rulesToRemove)
    return -3; //error while comparing rules
  }

  if(nrRulesToRemove>0)
  {
    int nrOfRule=0;
    //copy rules, which are not to be removed
    pos=0; newRules="";
    while(rules.getNextToken(pos, oneRule, '/'))
      if(!rulesToRemove[nrOfRule++]) newRules += oneRule + "/";
    rulesNr -= nrRulesToRemove;
    rules = newRules;
  }

  SAFEDELETEARRAY(rulDef)
  if(rulesBody) for(i=0;i<original_rulesNr;i++) SAFEDELETEARRAY(rulesBody[i])
  SAFEDELETEARRAY(rulesBody)
  SAFEDELETEARRAY(rulesToRemove)

  return 0;
};

int Geno_f0Fuzzy::sortSet(double set[4])
{
  int i,j;
  double fTemp;
  for(i=0;i<4;i++)
  {
    if(set[i]>1) set[i]=1;
    if(set[i]<-1) set[i]=-1;
    for(j=0;j<4;j++)
      if((i>j)&&(set[i]<set[j]))
      {fTemp=set[i];set[i]=set[j];set[j]=fTemp;};
  }
  return 0;
}

int Geno_f0Fuzzy::computeSet(double newSet[4])
{ //parameters: most probably size of support and core
  double core = min(fabs(0.15+0.25*RndGen.GaussStd()), 2.0), //core can not be bigger 2
         coreStart = RndGen.Uni(-1,1);

  if(0.5<rnd01)
  {
    newSet[0] = coreStart - min(fabs(0.15*RndGen.GaussStd()), 2.0);
    newSet[1] = coreStart;
    newSet[2] = newSet[1] + core;
    newSet[3] = newSet[2] + min(fabs(0.15*RndGen.GaussStd()), 2.0);
  }
  else
  {
    newSet[2] = coreStart;
    newSet[3] = newSet[2] + min(fabs(0.15*RndGen.GaussStd()), 2.0);
    newSet[1] = newSet[2] - core;
    newSet[0] = newSet[1] - min(fabs(0.15*RndGen.GaussStd()), 2.0);
  }

  sortSet(newSet);

  return 0;
}

int Geno_f0Fuzzy::mutate(char *&g, float &chg,int &method)
{
  int const maxTrialNr=10; //maximum number of mutating trial, if for some reason drawed mutation can not be done
  SString fuzzyGeno, finalGenotype, tok, newRulesStr, newSetsStr;
  int newSetsNr, newRulesNr;
  int i, pos, genoUsed, genoAdded, genoRemoved, nrTrials;
  int *transTable;
  char tempChar50[50];
  bool mutated;

  Model model(Geno(g, '0')); //create a model from genotype
  Neuro *fuzzy=findNeuro(&model, Neuro::getClass("Fuzzy"));
  if (!fuzzy) return GENOPER_OPFAIL; //if not present, fail
  const int inputsNr=fuzzy->getInputCount(); //get number of inputs
  const int outputsNr=NI_FuzzyNeuro::countOuts(&model, fuzzy); //count number of outputs

  //get neuron four properties
  SyntParam pi=fuzzy->classProperties();
  model.open();
  const int setsNr=pi.getIntById("ns");
  const int rulesNr=pi.getIntById("nr");
  const SString setsStr=pi.getStringById("fs");
  const SString rulesStr=pi.getStringById("fr");

  if((inputsNr<=0)||(outputsNr<=0)||
     (setsStr.len()==0)||(rulesStr.len()==0)||(setsNr<=0)||(rulesNr<=0)) return GENOPER_OPFAIL; //ERROR

  //find out the real length of genotype
  fuzzyGeno = setsStr+", "+rulesStr;
  genoRemoved=genoAdded=genoUsed=0; //1 geno is i.e. -0.325; but this are 7 characters! so, 1 is used, 6 are unused
  for(i=0;i<fuzzyGeno.len();i++) //count genes in fuzzy genotype
    if((fuzzyGeno[i]==';')||(fuzzyGeno[i]=='/')||(fuzzyGeno[i]==':')) genoUsed++;

  //mutation - drawing
  mutated=false;
  for(nrTrials=0;(nrTrials<maxTrialNr)&&(!mutated);nrTrials++)
  {
    newSetsStr=""; newRulesStr=""; newSetsNr=0; newRulesNr=0;
    switch(method=roulette(probtab, F0Fuzzy_COUNT))
    {
      case F0Fuzzy_ADDSET: //add new fuzzy set
            if(setsNr>=maximumSetsNr) break;
            double newSetD[4];
            computeSet(newSetD); //draw set and rank rising
            if(sprintf(tempChar50, "%.4lf;%.4lf;%.4lf;%.4lf;", newSetD[0], newSetD[1], newSetD[2], newSetD[3]) != EOF) //in fact, always true
            {
              newSetsStr = setsStr + tempChar50;
              //add new rule, using fresh new created fuzzy set (in premise part) - prevents removing of new fuzzy set
              if((sprintf(tempChar50, "%i;%i:%i;%i/", randomN(inputsNr), setsNr, randomN(outputsNr), randomN(setsNr+1)) != EOF)&& //in fact, always true; but if not, nothing wrong happens
                 (rulesNr<maximumRulesNr))
                { newRulesStr = rulesStr + tempChar50; newRulesNr=rulesNr+1; genoAdded+=4; }
              newSetsNr=setsNr+1; //one more fuzzy set
              genoAdded+=4; //four gens added
              mutated=true;
            }
            break;
      case F0Fuzzy_REMSET:{ //remove existing fuzzy set
            if(setsNr<=1) break;
            int setToRemove=randomN(setsNr), foundSetsPartNr=0;
            pos=0;
            //copy sets before set to remove
            while((foundSetsPartNr!=4*setToRemove)&&(setsStr.getNextToken(pos, tok, ';')))
              { newSetsStr += tok + ";"; foundSetsPartNr++; }
            double removedSet[4];
            //omit four numbers - one fuzzy set - and remember parameters of removed set
            for(int ii=0;(ii<4)&&(setsStr.getNextToken(pos, tok, ';'));ii++) removedSet[ii]=atoi(tok);
            //copy sets after set to remove
            while(setsStr.getNextToken(pos, tok, ';'))
              { newSetsStr += tok + ";"; foundSetsPartNr++; }
            newSetsNr=setsNr-1;
            newRulesNr=rulesNr;
            //find the nearest set to removed one
            double *newSets = new double[4*setsNr];
            if(FuzzyF0String::convertStrToSets(newSetsStr, newSets, newSetsNr)!=0)
              { SAFEDELETEARRAY(newSets) break; }
            transTable = new int[setsNr];
            for(int ii=0;ii<setToRemove;ii++) transTable[ii] = ii;
            for(int ii=setToRemove;ii<setsNr;ii++) transTable[ii] = ii-1; //transformate
            transTable[setToRemove]=nearestSet(removedSet, newSets, newSetsNr); //in rules, replace removed set with found nearest
            SAFEDELETEARRAY(newSets)
            //transformate old fuzzy sets number into new fuzzy sets number, according to transTable table
            newRulesStr=rulesStr;
            changeSetsNumeration(newRulesStr, transTable, setsNr);
            SAFEDELETEARRAY(transTable)
            //remove inputs/outputs which are unnecessery
            //remove rules which are unnecessery: conditional part must be unique
            //CAUTION: rules string and number of rules might change
            if((removeExcessiveInOut(newRulesStr, inputsNr, outputsNr)<0)||
               (removeExcessiveRules(newRulesStr, newSetsNr, newRulesNr)<0))
               break;
            genoRemoved+=4;
            mutated=true;
            break;
            }
      case F0Fuzzy_ADDRULE:{ //add new fuzzy rule
            if(rulesNr>=maximumRulesNr) break;
            SString tempSString;
            bool ruleOk=true;
            int nrIns=1+randomN(inputsNr), //draw number of inputs between {1,..,inputsNr}
                nrOuts=1+randomN(outputsNr); //draw number of outputs between {1,..,outputsNr}
            for(i=0;((i<(nrIns+nrOuts))&&(ruleOk));i++)
              if(sprintf(tempChar50, "%i;%i", (i<nrIns)?randomN(inputsNr):randomN(outputsNr), randomN(setsNr)) != EOF) //write: in/out nr ; fuzzy set nr
              {
                tempSString += tempChar50;
                if(i+1==nrIns) //end of conditional part
                  tempSString += ':';
                else if (i+1==nrIns+nrOuts) //end of rule
                  tempSString += '/';
                else
                  tempSString += ';';
              }
              else
                ruleOk = false; //error while transformating, do not add this rule
            if(ruleOk)
            {
              tempSString = tempSString + rulesStr;
              int temp_newRulesNr = rulesNr+1;
              if((removeExcessiveInOut(tempSString, inputsNr, outputsNr)<0)||
                 (removeExcessiveRules(tempSString, setsNr, temp_newRulesNr)<0)||
                 (temp_newRulesNr != rulesNr+1)) //new added rule was removed, so there was no mutation in fact!
                break;
              newSetsStr = setsStr; newSetsNr = setsNr;//do not change sets
              newRulesStr = tempSString; newRulesNr = temp_newRulesNr; //replace with new one
              genoAdded+=2*(nrIns+outputsNr); //gens added: input, set, ... , output, set, ...
              mutated=true;
            }
            break;
            }
      case F0Fuzzy_REMRULE:{ //remove existing fuzzy rule
            if(rulesNr<=1) break;
            int ruleToRemove=randomN(rulesNr), foundRuleNr=0;
            pos=0;
            //copy rules before rule to remove
            while((foundRuleNr!=ruleToRemove)&&(rulesStr.getNextToken(pos, tok, '/')))
              { newRulesStr += tok + "/"; foundRuleNr++; }
            //omit this rule
            rulesStr.getNextToken(pos, tok, '/');
            for(int ii=0;ii<tok.len();ii++) if((tok[ii]==';')||(tok[ii]==':')||(tok[ii]=='/')) genoRemoved++;
            //copy rules after rule to remove
            while(rulesStr.getNextToken(pos, tok, '/'))
              { newRulesStr += tok + "/"; foundRuleNr++; }
            newRulesNr = rulesNr-1;
            newSetsNr = setsNr; newSetsStr = setsStr;
            // after removing of the rule, an unused fuzzy set(s) may occur
            //create space for new transformation table
            int *transTable = new int [newSetsNr];
            for(int ii=0;ii<newSetsNr;ii++) transTable[ii]=ii; //formula 1:1 - no renumeration needed now
            int oldSetNr=newSetsNr; //remember number of sets used in new genotype, but before reduction
            //remove unused sets; if any set was removed, renumeration is needed
            if(removeUnusedSets(newSetsStr, newRulesStr, newSetsNr, transTable))
              changeSetsNumeration(newRulesStr, transTable, oldSetNr); //renumerate new sets
            SAFEDELETEARRAY(transTable);
            mutated=true;
            break;
            }
      case F0Fuzzy_MODRULE:{ //modify fuzzy rule: add new input/output
            int ruleToChange=randomN(rulesNr), foundRuleNr=0;
            SString modRule, cond, dec, tempStr;
            pos=0;
            //copy rules before rule to change
            while((foundRuleNr!=ruleToChange)&&(rulesStr.getNextToken(pos, tok, '/')))
              { newRulesStr += tok + "/"; foundRuleNr++; }
            //change this rule
            rulesStr.getNextToken(pos, modRule, '/');
            //copy rules after rule to remove
            while(rulesStr.getNextToken(pos, tok, '/'))
              { newRulesStr += tok + "/"; foundRuleNr++; }
            pos=0;
            modRule.getNextToken(pos, cond, ':');
            modRule.getNextToken(pos, dec, '/');
            if(0.5<rnd01) //add input
            { if(sprintf(tempChar50, ";%i;%i", randomN(inputsNr), randomN(setsNr)) != EOF) //write: in nr ; fuzzy set nr
                cond += tempChar50;
              else break;
            }
            else //add output
            { if(sprintf(tempChar50, ";%i;%i", randomN(outputsNr), randomN(setsNr)) != EOF) //write: in nr ; fuzzy set nr
                dec += tempChar50;
              else break;
            }
            tempStr = newRulesStr + cond + ":" + dec + "/";
            int temp_rulesNr = rulesNr;
            if((removeExcessiveInOut(tempStr, inputsNr, outputsNr)<0)||
               (removeExcessiveRules(tempStr, setsNr, temp_rulesNr)<0)||
               (temp_rulesNr != rulesNr)) //new added in/out caused duplicated rule, so this mutation is not ok - try other
              break;
            newRulesStr = tempStr;
            newRulesNr = rulesNr; newSetsStr = setsStr; newSetsNr = setsNr;
            genoAdded+=2;
            mutated=true;
            break;
            }
      case F0Fuzzy_MOD2RULE:{ //modify fuzzy rule: remove existing input/output
            int ruleToChange=randomN(rulesNr), foundRuleNr=0;
            SString modRule, cond, dec, tempStr;
            pos=0;
            //copy rules before rule to change
            while((foundRuleNr!=ruleToChange)&&(rulesStr.getNextToken(pos, tok, '/')))
              { newRulesStr += tok + "/"; foundRuleNr++; }
            //change this rule
            rulesStr.getNextToken(pos, modRule, '/');
            //copy rules after rule to remove
            while(rulesStr.getNextToken(pos, tok, '/'))
              { newRulesStr += tok + "/"; foundRuleNr++; }
            pos=0;
            modRule.getNextToken(pos, cond, ':');
            modRule.getNextToken(pos, dec, '/');
            pos=0;
            if(0.5<rnd01) //remove input
            {
              int nrIns=0;
              while(cond.getNextToken(pos, tok, ';'))nrIns++;
              nrIns = 1 + nrIns/2; //odd number of semicolons, so 1 + ...
              if(nrIns<=1) break; //can not remove - only one input!
              int nrInToRem=randomN(nrIns), insFound=0;
              //copy inputs before one to remove
              while((insFound<nrInToRem)&&(cond.getNextToken(pos, tok, ';')))
              {
                if(insFound>0) tempStr += ";"; //add semic. before input number
                tempStr += tok + ";"; //input number
                cond.getNextToken(pos, tok, ';');
                tempStr += tok; //set number
                insFound++;
              }
              if((!cond.getNextToken(pos, tok, ';'))||(!cond.getNextToken(pos, tok, ';'))) //ommit selected input
                break; //error
              //copy inputs after one to remove
              while(cond.getNextToken(pos, tok, ';'))
              {
                tempStr += ";"; tempStr += tok + ";"; //input number
                cond.getNextToken(pos, tok, ';');
                tempStr += tok; //set number
              }
              modRule = newRulesStr + tempStr + ":" + dec + "/";
            }
            else //remove output
            {
              int nrOuts=0;
              while(dec.getNextToken(pos, tok, ';'))nrOuts++;
              nrOuts = 1 + nrOuts/2; //odd number of semicolons, so 1 + ...
              if(nrOuts<=1) break; //can not remove - only one input!
              int nrOutToRem=randomN(nrOuts), outsFound=0;
              //copy outputs before one to remove
              while((outsFound<nrOutToRem)&&(dec.getNextToken(pos, tok, ';')))
              {
                if(outsFound>0) tempStr += ";"; //add semic. before output number
                tempStr += tok + ";"; //output number
                dec.getNextToken(pos, tok, ';');
                tempStr += tok; //set number
                outsFound++;
              }
              if((!dec.getNextToken(pos, tok, ';'))||(!dec.getNextToken(pos, tok, ';'))) //ommit selected output
                break; //error
              //copy outputs after one to remove
              while(dec.getNextToken(pos, tok, ';'))
              {
                tempStr += ";"; tempStr += tok + ";"; //output number
                dec.getNextToken(pos, tok, ';');
                tempStr += tok; //set number
              }
              modRule = newRulesStr + cond + ":" + tempStr + "/";
            }
            tempStr = newRulesStr + modRule;
            int temp_rulesNr = rulesNr;
            if((removeExcessiveInOut(tempStr, inputsNr, outputsNr)<0)||
               (removeExcessiveRules(tempStr, setsNr, temp_rulesNr)<0)||
               (temp_rulesNr != rulesNr)) //removed in/out caused duplicated rule, so this mutation is not ok - try other
              break;
            newRulesStr = tempStr;
            newRulesNr = rulesNr; newSetsStr = setsStr; newSetsNr = setsNr;
            genoRemoved+=2;
            // after removing of an input/output from the rule, an unused fuzzy set may occur
            //create space for new transformation table
            int *transTable = new int [newSetsNr];
            for(int ii=0;ii<newSetsNr;ii++) transTable[ii]=ii; //formula 1:1 - no renumeration needed now
            int oldSetNr=newSetsNr; //remember number of sets used in new genotype, but before reduction
            //remove unused sets; if any set was removed, renumeration is needed
            if(removeUnusedSets(newSetsStr, newRulesStr, newSetsNr, transTable))
              changeSetsNumeration(newRulesStr, transTable, oldSetNr); //renumerate new sets
            SAFEDELETEARRAY(transTable);
            mutated=true;
            break;
            }
    }
  }

  if(!mutated)
    return GENOPER_OPFAIL;

  //update new genotype
  pi.setIntById("ns", newSetsNr);
  pi.setIntById("nr", newRulesNr);
  pi.setStringById("fs",newSetsStr);
  pi.setStringById("fr",newRulesStr);
  pi.update(); //force commit changes
  model.close();
  //get creature whole genotype
  finalGenotype=model.getF0Geno().getGene();

  //replace old genotype with new one
  free(g);
  g=strdup((const char*)finalGenotype);

  chg=(float)(genoAdded+genoRemoved)/(genoAdded+genoRemoved+max(genoUsed-genoRemoved, 0));

  return GENOPER_OK;
}

int Geno_f0Fuzzy::checkOrValidate(char *&geno, bool repair)
//checks or validates genotype (depends on repair param)
{ //all genotype received, but only fuzzy part is to be validated!!!
  //"Fuzzy: [declarations], [sets], [rules]"
  SString tok, repairedSets, repairedRules, finalGenotype;
  char tempChar50[50];
  int pos=0, i, j, setsFound, rulesFound;
  double set[4];

  Model model(Geno(geno,'0'));
  if (!(model.isValid())) { if(repair) return GENOPER_OK; else return 1; } //ERROR
  Neuro *fuzzy=findNeuro(&model, Neuro::getClass("Fuzzy"));
  if (!fuzzy) { if(repair) return GENOPER_OK; else return 1; } //ERROR
  const int inputsDeclared=fuzzy->getInputCount();
  const int outputsDeclared=NI_FuzzyNeuro::countOuts(&model,fuzzy);
  if((inputsDeclared<=0)||(outputsDeclared<=0))
    { if(repair) return GENOPER_OK; else return 1; } //ERROR

  //get neuron four properties
  SyntParam pi=fuzzy->classProperties();
  model.open();
  int setsDeclared=pi.getIntById("ns");
  int rulesDeclared=pi.getIntById("nr");
  const SString fuzzySets=pi.getStringById("fs");
  const SString fuzzyRules=pi.getStringById("fr");

  //unrepairable errors: there must be some string!
  if((fuzzySets.len()==0)||(fuzzyRules.len()==0))
    if(repair) return GENOPER_OK; //ERROR
    else return 2; //ERROR

  if((!repair)&&((setsDeclared<=0)||(rulesDeclared<=0)))
    return 2; //ERROR

  //repair if corrupted
  if(setsDeclared<=0)setsDeclared=1;
  if(rulesDeclared<=0)rulesDeclared=1;

  //now fuzzy sets are defined; each number is separated with semicolon.
  //acceptables signs are: digits, commas, colons, semicolons.
  i=j=0; setsFound=0;
  while (j<fuzzySets.len())
  {
    SString acceptSigns; //string with acceptable signs
    fuzzySets.getNextToken(j,tok,';');
    if(tok.len()==0) { if(!repair) return j; else continue; }//might happen, when two semicolons stand side by side
    //copy only acceptable signs
    for(int ii=0;ii<tok.len();ii++)
      if(strchr("0123456789.-;", tok[ii])) acceptSigns += tok[ii];
      else if(!repair) return j;
    set[i]=atof(acceptSigns);
    if(set[i]<-1) { if(!repair) return j; else set[i] = -1; }//out of range
    if(set[i]>1) { if(!repair) return j; else set[i] = 1; }//out of range
    if(set[i]==HUGE_VAL) { if(!repair) return j; else set[i] = (i>0?set[i-1]:-1); }//sets must not be decreasing

    if(++i==4)
    {
      //check if sets are rank rising
      if((set[0]>set[1])||(set[1]>set[2])||(set[2]>set[3]))
        { if(!repair) return j; else sortSet(set); }
      if(sprintf(tempChar50, "%.4lf;%.4lf;%.4lf;%.4lf;", set[0], set[1], set[2], set[3]) != EOF)
        {i=0; setsFound++; repairedSets += tempChar50;} //copy correct value
    }

    if((setsFound==setsDeclared)&&(repair))
      break; //found enough sets - if there were some sets left, omit them
  }
  // valitadion: if there were more sets than declares, nothing happens, because pointer is now after comma (end of all sets)
  // checking: error
  if((setsFound!=setsDeclared)&&(!repair))
    return j; //ERROR - wrong number of fuzzy sets in definition or abnormal end of the string

  //now fuzzy rules are defined; each number is separated with semicolon, conditional and decisional part is separated with colon, rules are separated with slash
  //acceptables signs are: digits, semicolons, colons, slashes.
  i=0; rulesFound=0; pos=0;
  while (pos<fuzzyRules.len()) //to the end of string - process loop for each rule
  {
    SString ruleSStr, ruleCondisSStr, ruleDecisSStr, nrSStr, acceptSigns;
    int posRule=0, colons, i, posTemp, inputsUsedNr, outputsUsedNr;
    int *inoutUsed, posOld;
    bool firstInput, firstOutput;

    posOld=pos; acceptSigns = "";
    fuzzyRules.getNextToken(pos,ruleSStr,'/');
    for(int ii=0;ii<pos-posOld;ii++)
      if(strchr("0123456789;:/", ruleSStr[ii]))
        acceptSigns += ruleSStr[ii];
      else if(!repair) return j+pos; //ERROR - unacceptable sign
    //cut conditional part
    acceptSigns.getNextToken(posRule,ruleCondisSStr,':');
    //cut decisional part
    acceptSigns.getNextToken(posRule,ruleDecisSStr,'/');

    //check, how many inputs is used:
    colons=posRule=0;
    while(++posRule<ruleCondisSStr.len())
      if(ruleCondisSStr[posRule]==';') colons++;
    if(!(colons%2)) //nr of colons must be odd. this rule is corrupted
      {if(!repair) return j+pos; else continue; }//check another rule

    inputsUsedNr=(colons+1)/2; //count nr of inputs
    if((inputsUsedNr>inputsDeclared)&&(!repair))
      return j+pos; //ERROR: too many inputs
    //when repair: if inputsUsedNr > declared, do nothing, because later in loop it is checked whether
    //there is no duplicate input number (the only possible way of situation where inputsUsedNr > declared)

    //check how many outputs is used:
    colons=posRule=0;
    while(++posRule<ruleDecisSStr.len())
      if(ruleDecisSStr[posRule]==';') colons++;
    if(!(colons%2)) //nr of colons must be odd. this rule is corrupted - omit one
      {if(!repair) return j+pos; else continue; }//check another rule

    outputsUsedNr=(colons+1)/2; //count nr of outputs
    if((outputsUsedNr>outputsDeclared)&&(!repair))
      return j+pos; //ERROR: too many outputs
    //when repair: if outputsUsedNr > declared, do nothing, because later in loop it is checked, wether
    //there is no duplicate output number (the only possible way of situation where outputsUsedNr > declared)
    inoutUsed = new int[max(inputsDeclared,outputsDeclared)];
    for(i=0;i<max(inputsDeclared,outputsDeclared);i++) inoutUsed[i]=0; //0=unused, -1 = used by input, +1 used by output
    firstInput=true; firstOutput=true;
    //check, whether inputs/outputs or fuzzy set nr is not out of range
    //and check, whether there is no duplicate input/output number
    for(i=0,posTemp=0;i<inputsUsedNr+outputsUsedNr;i++)
    {
      int tempVal;
      if(i==inputsUsedNr) posTemp=0; //because in decision string - parse string from begin
      if(i<inputsUsedNr) //concern inputs
      {
        //check input
        acceptSigns = ""; posOld=posTemp;
        ruleCondisSStr.getNextToken(posTemp,nrSStr,';');
        for(int ii=0;ii<nrSStr.len();ii++)
          if(strchr("0123456789", nrSStr[ii]))
            acceptSigns += nrSStr[ii];
          else if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} //ERROR - out of range
        tempVal=atoi(acceptSigns);
        if(tempVal<0)
          {if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else tempVal=0;}
        if(tempVal>=inputsDeclared)
          {if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else tempVal=inputsDeclared-1; }
        if(inoutUsed[tempVal]==-1) //input is used
        {
          ruleCondisSStr.getNextToken(posTemp,nrSStr,';'); //omit input and fuzzy set
          if(i+1 == inputsUsedNr) repairedRules += ":"; //the end of conditional part - must be here, because of continue
          if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else continue;//next for loop
        }
        else
          inoutUsed[tempVal]=-1; //used by input
        if(!firstInput) repairedRules += ";"; //before input there is semicolon, except of begin of the rule
        firstInput=false;
        //tempVal now has a correct value
        sprintf(tempChar50, "%i;", tempVal);
        //correct input number:
        repairedRules += tempChar50;
        //check fuzzy set nr
        acceptSigns = ""; posOld=posTemp;
        ruleCondisSStr.getNextToken(posTemp,nrSStr,';');
        for(int ii=0;ii<nrSStr.len();ii++)
          if(strchr("0123456789", nrSStr[ii]))
            acceptSigns += nrSStr[ii];
          else if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;}
      }
      else //concern output
      {
        //check output
        acceptSigns = ""; posOld=posTemp;
        ruleDecisSStr.getNextToken(posTemp,nrSStr,';');
        for(int ii=0;ii<nrSStr.len();ii++)
          if(strchr("0123456789", nrSStr[ii]))
            acceptSigns += nrSStr[ii];
          else if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;}
        tempVal=atoi(acceptSigns);
        if(tempVal<0)
          {if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else tempVal=0;}
        if(tempVal>=outputsDeclared)
          {if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else tempVal=outputsDeclared-1; }
        if(inoutUsed[tempVal]==1) //output is used
        {
          ruleDecisSStr.getNextToken(posTemp,nrSStr,';'); //omit output and fuzzy set
          if (i+1 == inputsUsedNr+outputsUsedNr) repairedRules += "/"; //the end of decisional part - must be here, because of continue
          if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else continue; //next for loop
        }
        else
          inoutUsed[tempVal]=1; //used by output
        if(!firstOutput) repairedRules += ";"; //before output there is semicolon, except of begin of the decisional part of the rule
        firstOutput=false;
        //tempVal now has a correct value
        sprintf(tempChar50, "%i;", tempVal);
        //correct output number:
        repairedRules += tempChar50;
        //check fuzzy set nr
        acceptSigns = ""; posOld=posTemp;
        ruleDecisSStr.getNextToken(posTemp,nrSStr,';');
        for(int ii=0;ii<nrSStr.len();ii++)
          if(strchr("0123456789", nrSStr[ii]))
            acceptSigns += nrSStr[ii];
          else if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;}
      }
      tempVal=atoi(acceptSigns);
      if(tempVal<0)
        {if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else tempVal=0;}
      if(tempVal>=setsFound)
        {if(!repair) {SAFEDELETEARRAY(inoutUsed) return j+pos;} else tempVal=setsFound-1; }
      //tempVal now has a correct value
      sprintf(tempChar50, "%i", tempVal);
      //correct set number:
      repairedRules += tempChar50;
      if(i+1 == inputsUsedNr) repairedRules += ":"; //the end of conditional part
      else if (i+1 == inputsUsedNr+outputsUsedNr) repairedRules += "/"; //the end of decisional part
    }
    SAFEDELETEARRAY(inoutUsed)
    rulesFound++;
    if ((rulesFound>=rulesDeclared)&&(repair))
      break; //if there was more rules - omit them by not parsing any more
    if ((rulesFound>rulesDeclared)&&(!repair))
      return j+pos; // ERROR - too many rules - stop parsing
  }

  if ((rulesFound!=rulesDeclared)&&(!repair))
    return j+pos; // ERROR - wrong number of rules (too few)

  if(rulesFound<1) //could happen, when there is one rule and it is unrepairable
  {
    if(sprintf(tempChar50, "%i;%i:%i;%i/", randomN(inputsDeclared), randomN(setsFound), randomN(outputsDeclared), randomN(setsFound)) != EOF)
      repairedRules += tempChar50; //add simple random rule
    else //on error - add zeros
      repairedRules += "0;0:0;0/"; //add the simplies rule, but almost impossible to happen
    rulesFound=1;
  }

  //when changing sets numerations, rules might change and equals
  if((repair)&&(removeExcessiveRules(repairedRules, setsFound, rulesFound)<0))
    return GENOPER_OPFAIL; //ERROR

  //update new genotype
  pi.setIntById("ns", setsFound);
  pi.setIntById("nr", rulesFound);
  pi.setStringById("fs",repairedSets);
  pi.setStringById("fr",repairedRules);
  pi.update(); //force commit changes
  model.close();
  //get creature whole genotype
  finalGenotype=model.getF0Geno().getGene();

  //replace old genotype with new one
  free(geno);
  geno=strdup((const char*)finalGenotype);

  return GENOPER_OK;
};

int Geno_f0Fuzzy::checkValidity(const char *geno)
{
  char *genoCopy=strdup(geno);
  int res=checkOrValidate(genoCopy, false); //false = on errors do not repair but return error position
  free(genoCopy);
  return res;
};

int Geno_f0Fuzzy::validate(char *&geno)
{
  checkOrValidate(geno, true); //true = on errors try to repair but might be unrepairable
  return GENOPER_OK;
};

int Geno_f0Fuzzy::crossOver(char *&g1, char *&g2, float &chg1,float &chg2)
{ //both bodies should be the same; the only difference should be in fuzzy neuron

  SString finalGenotype, tok1, tok2;
  SString fuzzyGeno1, fuzzyGeno2, newSet, sets3, rules3;
  int setsNr3, rulesNr3;
  int i, j, k, unimpTemp;
  int *transTable, *parentsIn, *parentsOut, *parentRules, *rulDef1, *rulDef2, *rulDef3, **rulesBody1, **rulesBody2;
  int genoUsed1, genoUsed2, copied1, copied2, rulesTaken1, rulesTaken2, rulesUsed3, selectedRule1, selectedRule2;
  int inputsUsed, outputsUsed;
  char tempChar50[50];
  double *fuzzySets1, *fuzzySets2, *fuzzySets3;

  Model model1(Geno(g1,'0'));
  Model model2(Geno(g2,'0'));
  Neuro *fuzzy1=findNeuro(&model1, Neuro::getClass("Fuzzy"));
  Neuro *fuzzy2=findNeuro(&model2, Neuro::getClass("Fuzzy"));
  if (!fuzzy1 || !fuzzy2) return GENOPER_OPFAIL; //ERROR
  const int inputsNr1=fuzzy1->getInputCount();
  const int inputsNr2=fuzzy2->getInputCount();
  const int outputsNr1=NI_FuzzyNeuro::countOuts(&model1,fuzzy1);
  const int outputsNr2=NI_FuzzyNeuro::countOuts(&model2,fuzzy2);
  if((inputsNr1<=0)||(outputsNr1<=0)||(inputsNr2<=0)||(outputsNr2<=0)||
     (inputsNr1!=inputsNr2)||(outputsNr1!=outputsNr2)) return GENOPER_OPFAIL; //ERROR

  //get neurons four properties
  SyntParam pi1=fuzzy1->classProperties();
  SyntParam pi2=fuzzy2->classProperties();
  model1.open();
  model2.open();
  const int setsNr1=pi1.getIntById("ns");
  const int setsNr2=pi2.getIntById("ns");
  const int rulesNr1=pi1.getIntById("nr");
  const int rulesNr2=pi2.getIntById("nr");
  const SString sets1=pi1.getStringById("fs");
  const SString sets2=pi2.getStringById("fs");
  const SString rules1=pi1.getStringById("fr");
  const SString rules2=pi2.getStringById("fr");

  if((sets1.len()==0)||(rules1.len()==0)||(sets2.len()==0)||(rules2.len()==0)||
     (setsNr1<=0)||(rulesNr1<=0)||(setsNr2<=0)||(rulesNr2<=0))
    return GENOPER_OPFAIL; //ERROR

  //find out the real length of genotype - count number of semicolons
  genoUsed1 = 0; //1 geno is i.e. -0.325; but this are 7 characters! so, 1 is used, 6 are unused
  fuzzyGeno1 = sets1+", "+rules1;
  for(i=0;i<fuzzyGeno1.len();i++)
    if((fuzzyGeno1[i]==';')||(fuzzyGeno1[i]=='/')||(fuzzyGeno1[i]==':')) genoUsed1++;
  genoUsed2 = 0; //1 geno is i.e. -0.325; but this are 7 characters! so, 1 is used, 6 are unused
  fuzzyGeno2 = sets2+", "+rules2;
  for(i=0;i<fuzzyGeno2.len();i++)
    if((fuzzyGeno2[i]==';')||(fuzzyGeno2[i]=='/')||(fuzzyGeno2[i]==':')) genoUsed2++;


  // -------------------------------------
  // ------- cross over sets part --------
  // -------------------------------------

  copied1=copied2=0; //how many gens (in whole rules and whole sets) were copied (without changeing!) from parent 1 / 2 ?

  fuzzySets1 = new double[4*setsNr1]; fuzzySets2 = new double[4*setsNr2];
  FuzzyF0String::convertStrToSets(sets1, fuzzySets1, setsNr1);
  FuzzyF0String::convertStrToSets(sets2, fuzzySets2, setsNr2);

  //in fact, there is no crossing over of sets, but copy all parents' sets
  //with marking duplicated sets
  setsNr3 = setsNr1+setsNr2;
  transTable = new int[setsNr3];
  for(i=0;i<setsNr1+setsNr2;i++) transTable[i]=-2; //all sets are taken to descendant
  if(markDuplicatedSets(fuzzySets1, setsNr1, fuzzySets2, setsNr2, transTable, setsNr3)!=0)
    {
      SAFEDELETEARRAY(fuzzySets1) SAFEDELETEARRAY(fuzzySets2) SAFEDELETEARRAY(transTable)
      return GENOPER_OPFAIL; //ERROR
    }

  fuzzySets3 = new double[4*setsNr3];
  j=0; sets3 = "";
  //copy sets from parent 1 and 2
  for(i=0;(i<setsNr1+setsNr2)&&(j<setsNr3);i++)
    if(transTable[i]==-2) //this set is marked to be taken
    {
      if(i<setsNr1)
      {
        fuzzySets3[4*j]=fuzzySets1[4*i]; fuzzySets3[4*j+1]=fuzzySets1[4*i+1];
        fuzzySets3[4*j+2]=fuzzySets1[4*i+2]; fuzzySets3[4*j+3]=fuzzySets1[4*i+3];
      }
      else
      {
        fuzzySets3[4*j]=fuzzySets2[4*(i-setsNr1)]; fuzzySets3[4*j+1]=fuzzySets2[4*(i-setsNr1)+1];
        fuzzySets3[4*j+2]=fuzzySets2[4*(i-setsNr1)+2]; fuzzySets3[4*j+3]=fuzzySets2[4*(i-setsNr1)+3];
      };
      if(sprintf(tempChar50, "%.4lf;%.4lf;%.4lf;%.4lf;", fuzzySets3[4*j], fuzzySets3[4*j+1], fuzzySets3[4*j+2], fuzzySets3[4*j+3]) != EOF)
      {
        transTable[i] = j++; //remember, from which set it was copy: [4]=2 means that fuzzy set [4] from parent 1 was copy into descendant fuzzy set [2]. when i>setsNr1, substract setsNr1 from value to obtain number of fuzzy set from parent 2
        sets3 += tempChar50;
      }
      else
      {
        SAFEDELETEARRAY(fuzzySets1)
        SAFEDELETEARRAY(fuzzySets2)
        SAFEDELETEARRAY(fuzzySets3)
        SAFEDELETEARRAY(transTable)
        return GENOPER_OPFAIL; //ERROR
      }
    }

  // -------------------------------------
  // ------- cross over rules part -------
  // -------------------------------------

  rules3="";
  //draw number of fuzzy rules for the descendant
  rulesNr3 = min(rulesNr1,rulesNr2) + randomN(abs(rulesNr1-rulesNr2)+1);

  rulDef1 = new int[2*rulesNr1]; //for each rule, remembers number of inputs and number of outputs used (in rule)
  rulDef2 = new int[2*rulesNr2];
  rulDef3 = new int[2*rulesNr3]; //it will be filled later, with random values

  FuzzyF0String::countInputsOutputs(rules1, rulDef1, rulesNr1);
  FuzzyF0String::countInputsOutputs(rules2, rulDef2, rulesNr2);
  for(i=0;i<2*rulesNr3;i++) rulDef3[i]=-1; //unknown number of inputs/outputs

  //create space for rules from parent 1 and 2
  rulesBody1 = new int*[rulesNr1];   //list of rules that will contain rules body
  for (i=0;i<rulesNr1;i++) rulesBody1[i] = new int[2*(rulDef1[2*i]+rulDef1[2*i+1])];  //each rule can have different number of inputs and outputs
  rulesBody2 = new int*[rulesNr2];   //list of rules that will contain rules body
  for (i=0;i<rulesNr2;i++) rulesBody2[i] = new int[2*(rulDef2[2*i]+rulDef2[2*i+1])];  //each rule can have different number of inputs and outputs

  FuzzyF0String::convertStrToRules(rules1, rulDef1, rulesBody1, setsNr1, rulesNr1, unimpTemp);
  FuzzyF0String::convertStrToRules(rules2, rulDef2, rulesBody2, setsNr2, rulesNr2, unimpTemp);

  //draw rules from both parents
  parentRules = new int[rulesNr1+rulesNr2];
  for(i=0;i<rulesNr1+rulesNr2;i++) parentRules[i]=-1; //any rule is taken yet

  rulesTaken1=rulesTaken2=rulesUsed3=0;
  while(rulesUsed3<rulesNr3)
  {
   //draw rule from parent 1
   selectedRule1=-1;
   if(rulesTaken1<rulesNr1) //if there are some rules left in parent 1
   {
    //select random rule from parent 1
    k=randomN(rulesNr1);
    if(parentRules[k]==-1) selectedRule1=k;
    else //if rule is taken, search the nearest free
      for(j=0;j<rulesNr1;j++)
        if(parentRules[(k+j)%rulesNr1]==-1) { selectedRule1=(k+j)%rulesNr1; j=rulesNr1; }
        else if(parentRules[abs((k-j)%rulesNr1)]==-1) { selectedRule1=abs((k-j)%rulesNr1); j=rulesNr1; }
   }
   if (selectedRule1==-1) //all rules are taken from parent 1
     break;

   //draw rule from parent 2
   selectedRule2=-1;
   if(rulesTaken2<rulesNr2) //if there are some rules left in parent 2
   {
    //select random rule from parent 2
    k=randomN(rulesNr2);
    if(parentRules[rulesNr1+k]==-1) { parentRules[rulesNr1+k]=-2; selectedRule2=k; } //mark as 'taken'
    else //if rule is taken, search the nearest free
      for(j=0;j<rulesNr2;j++)
        if(parentRules[rulesNr1+(k+j)%rulesNr2]==-1) {selectedRule2=(k+j)%rulesNr2; j=rulesNr2; }
        else if(parentRules[rulesNr1+abs((k-j)%rulesNr2)]==-1) { selectedRule2=abs((k-j)%rulesNr2); j=rulesNr2; }
   }

   if (selectedRule2==-1) //all rules are taken from parent 1
     break;

   parentRules[selectedRule1]=-2;  //mark as 'taken'
   parentRules[rulesNr1+selectedRule2]=-2;

   //draw number of inputs and outputs in new rule, (created by crossing over of 2 parents)
   rulDef3[2*rulesUsed3] = min(rulDef1[2*selectedRule1], rulDef2[2*selectedRule2]) + randomN(abs(rulDef1[2*selectedRule1]-rulDef2[2*selectedRule2])+1); //random number of inputs
   rulDef3[2*rulesUsed3+1] = min(rulDef1[2*selectedRule1+1], rulDef2[2*selectedRule2+1]) + randomN(abs(rulDef1[2*selectedRule1+1]-rulDef2[2*selectedRule2+1])+1); //random number of outputs

   int nrInputsP1 = rulDef1[2*selectedRule1], nrInputsP2 = rulDef2[2*selectedRule2],       //some variables' names
       nrOutputsP1 = rulDef1[2*selectedRule1+1], nrOutputsP2 = rulDef2[2*selectedRule2+1], //become now more understandable
       nrInputsP3 = rulDef3[2*rulesUsed3], nrOutputsP3 = rulDef3[2*rulesUsed3+1];

   //prepare drawing inputs from parent 1 and 2
   int sumParentsInputs = nrInputsP1+nrInputsP2;
   parentsIn = new int[sumParentsInputs]; //size: parent's 1 and 2 number of inputs

   for(i=0;i<sumParentsInputs;i++) parentsIn[i]=-1; //not taken yet
   //search for duplicated output and mark one of them outputs as 'taken'
   for(i=0;i<nrInputsP1;i++)
     for(j=0;j<nrInputsP2;j++)
       if(rulesBody1[selectedRule1][2*i]==rulesBody2[selectedRule2][2*j]) //if inputs numbers are the same,
       {
         bool takenP1=rnd01<0.5; //draw, which input should be mark as taken
         if (takenP1) parentsIn[i]=-2;
         else parentsIn[j+nrInputsP1]=-2;
       }

   //now drawig is well prepared
   inputsUsed=0;
   while(inputsUsed<nrInputsP3) //loop as long, as inputs taken (from p1&p2) < length of descendant's input part
   {
     //draw input (which was not used yet) from both parents
     do k=randomN(sumParentsInputs);
     while (parentsIn[k]!=-1); //draw as long, as input is not taken yet
     if(k<nrInputsP1) //concerns input from parent 1
       sprintf(tempChar50, "%i;%i", rulesBody1[selectedRule1][2*k], transTable[rulesBody1[selectedRule1][2*k+1]]); //get input and transformated fuzzy set nr
     else  //concerns input from parent 2
       sprintf(tempChar50, "%i;%i", rulesBody2[selectedRule2][2*(k-nrInputsP1)], transTable[setsNr1+rulesBody2[selectedRule2][2*(k-nrInputsP1)+1]]); //get input and transformated fuzzy set nr. setsNr1+ because transTable is sum of setsNr1+setsNr2
     rules3 += tempChar50;
     if (inputsUsed+1<nrInputsP3) rules3 += ";";
     inputsUsed++; parentsIn[k]=-2; //mark as taken
   }
   rules3 += ":";

   SAFEDELETEARRAY(parentsIn)

   //prepare drawing outputs from parent 1 and 2
   int sumParentsOutputs = nrOutputsP1+nrOutputsP2;
   parentsOut = new int[sumParentsOutputs]; //size: parent's 1 and 2 number of inputs

   for(i=0;i<sumParentsOutputs;i++) parentsOut[i]=-1; //not taken yet
   //search duplicated output and mark one of them outputs as 'taken'
   for(i=0;i<nrOutputsP1;i++)
     for(j=0;j<nrOutputsP2;j++)
       if(rulesBody1[selectedRule1][2*(i+nrInputsP1)]==rulesBody2[selectedRule2][2*(j+nrInputsP2)]) //if outputs numbers are the same,
       {
         bool takenP1=rnd01<0.5; //draw, which input should be mark as taken
         if (takenP1) parentsOut[i]=-2;
         else parentsOut[j+nrOutputsP1]=-2;
       }

   //now drawig is well prepared
   outputsUsed=0;
   while(outputsUsed<nrOutputsP3) //loop as long, as outputs taken (from p1&p2) < length of descendant's output part
   {
     //draw output (which was not used yet) from both parents
     do k=randomN(sumParentsOutputs);
     while (parentsOut[k]!=-1); //draw as long, as output is not taken yet
     if(k<nrOutputsP1) //concerns output from parent 1
       sprintf(tempChar50, "%i;%i", rulesBody1[selectedRule1][2*(k+nrInputsP1)], transTable[rulesBody1[selectedRule1][2*(k+nrInputsP1)+1]]); //get output and transformated fuzzy set nr
     else  //concerns output from parent 2
       sprintf(tempChar50, "%i;%i", rulesBody2[selectedRule2][2*(k-nrOutputsP1+nrInputsP2)], transTable[setsNr1+rulesBody2[selectedRule2][2*(k-nrOutputsP1+nrInputsP2)+1]]); //get output and transformated fuzzy set nr
     rules3 += tempChar50;
     if (outputsUsed+1<nrOutputsP3) rules3 += ";";
     outputsUsed++; parentsOut[k]=-2; //mark as taken
   }
   rules3 += "/";

   SAFEDELETEARRAY(parentsOut)

   rulesUsed3++; rulesTaken1++; rulesTaken2++;
  }

  //if there is no more rules pairs, and descendant needs more rules, copy rules from 'bigger' parent
  if(rulesTaken1<rulesNr1)
  {
    for(j=0;(j<rulesNr1)&&(rulesUsed3<rulesNr3);j++)
      if(parentRules[j]==-1) //search for not taken rule
      {
        parentRules[j]=-2; //taken
        rulDef3[2*rulesUsed3] = rulDef1[2*j]; rulDef3[2*rulesUsed3+1] = rulDef1[2*j+1]; //copy whole rule - do not change number of inputs and outputs
        for(k=0;k<rulDef3[2*rulesUsed3]+rulDef3[2*rulesUsed3+1];k++) //copy conditional and decisional part
        {
          sprintf(tempChar50, "%i;%i", rulesBody1[j][2*k], transTable[rulesBody1[j][2*k+1]]);
          rules3 += tempChar50;
          if(k+1==rulDef3[2*rulesUsed3]) //if end of inputs, put colon
            rules3 += ":";
          else if(k+1==rulDef3[2*rulesUsed3]+rulDef3[2*rulesUsed3+1]) //if end of outputs, put slash
            rules3 += "/";
          else
            rules3 += ";";
        }
        rulesUsed3++; rulesTaken1++;
        copied1+=2*k; //all inputs, fuzzy sets, ..., all outputs, fuzzy sets
      }
  }
  if(rulesTaken2<rulesNr2)
    for(j=0;(j<rulesNr2)&&(rulesUsed3<rulesNr3);j++)
      if(parentRules[rulesNr1+j]==-1) //search for not taken rule
      {
        selectedRule2=j; parentRules[rulesNr1+j]=-2; //taken
        rulDef3[2*rulesUsed3] = rulDef2[2*j]; rulDef3[2*rulesUsed3+1] = rulDef2[2*j+1]; //copy whole rule - do not change number of inputs and outputs
        for(k=0;k<rulDef3[2*rulesUsed3]+rulDef3[2*rulesUsed3+1];k++) //copy conditional and decisional part
        {
          sprintf(tempChar50, "%i;%i", rulesBody2[j][2*k], transTable[setsNr1+rulesBody2[j][2*k+1]]);
          rules3 += tempChar50;
          if(k+1==rulDef3[2*rulesUsed3]) //if end of inputs, put colon
            rules3 += ":";
          else if(k+1==rulDef3[2*rulesUsed3]+rulDef3[2*rulesUsed3+1]) //if end of outputs, put slash
            rules3 += "/";
          else
            rules3 += ";";
        }
        rulesUsed3++; rulesTaken2++;
        copied2+=2*k; //all outputs, fuzzy sets, ..., all outputs, fuzzy sets 
      }

  //no need to normalize before removing unused sets, because correct (new) number of fuzzy set already written in rules string

  //delete 'old' transTable
  SAFEDELETEARRAY(transTable)

  //create space for new transformation table
  transTable=new int [setsNr3];
  for(int ii=0;ii<setsNr3;ii++) transTable[ii]=ii; //formula 1:1 - no renumeration needed now

  int oldSetNr=setsNr3; //remember number of sets used in new genotype, but before reduction
  //remove unused sets; if any set was removed, renumeration is needed
  if(removeUnusedSets(sets3, rules3, setsNr3, transTable))
    changeSetsNumeration(rules3, transTable, oldSetNr); //renumerate new sets
  //after renumeration, excessive rule or input/output may occur
  if((removeExcessiveInOut(rules3, inputsNr1, outputsNr1)<0)||
     (removeExcessiveRules(rules3, setsNr3, rulesNr3)<0))
  { //should not occur
    SAFEDELETEARRAY(rulDef1) SAFEDELETEARRAY(rulDef2) SAFEDELETEARRAY(rulDef3)
    SAFEDELETEARRAY(transTable) SAFEDELETEARRAY(parentRules) SAFEDELETEARRAY(parentsIn) SAFEDELETEARRAY(parentsOut)
    SAFEDELETEARRAY(fuzzySets1) SAFEDELETEARRAY(fuzzySets2) SAFEDELETEARRAY(fuzzySets3)
    if(rulesBody1) for(i=0;i<rulesNr1;i++) SAFEDELETEARRAY(rulesBody1[i])
    if(rulesBody2) for(i=0;i<rulesNr2;i++) SAFEDELETEARRAY(rulesBody2[i])
    SAFEDELETEARRAY(rulesBody1) SAFEDELETEARRAY(rulesBody2)
    return GENOPER_OPFAIL; //did not successed
  }
//---------------------  end of crossing over  --------------------------------

  if( (sets3.len()==0)||(rules3.len()==0) ) //shold not occur, but for safety:
    {sets3="-1;0;0;1;"; rules3="0;0:0;0/"; setsNr3=1; rulesNr3=1; }

  //update new genotype
  pi1.setIntById("ns", setsNr3);
  pi1.setIntById("nr", rulesNr3);
  pi1.setStringById("fs",sets3);
  pi1.setStringById("fr",rules3);
  pi1.update(); //force commit changes
  model1.close();
  //get creature whole genotype
  finalGenotype=model1.getF0Geno().getGene();

  //replace old genotype with new one
  free(g1);
  g1=strdup((const char*)finalGenotype);

  chg1=1-(float)copied1/genoUsed1;
  chg2=1-(float)copied2/genoUsed2;

  SAFEDELETEARRAY(rulDef1)
  SAFEDELETEARRAY(rulDef2)
  SAFEDELETEARRAY(rulDef3)
  SAFEDELETEARRAY(transTable)
  SAFEDELETEARRAY(parentRules)
  SAFEDELETEARRAY(parentsIn)
  SAFEDELETEARRAY(parentsOut)
  SAFEDELETEARRAY(fuzzySets1)
  SAFEDELETEARRAY(fuzzySets2)
  SAFEDELETEARRAY(fuzzySets3)
  if(rulesBody1) for(i=0;i<rulesNr1;i++) SAFEDELETEARRAY(rulesBody1[i])
  if(rulesBody2) for(i=0;i<rulesNr2;i++) SAFEDELETEARRAY(rulesBody2[i])
  SAFEDELETEARRAY(rulesBody1)
  SAFEDELETEARRAY(rulesBody2)

  return GENOPER_OK;
};



