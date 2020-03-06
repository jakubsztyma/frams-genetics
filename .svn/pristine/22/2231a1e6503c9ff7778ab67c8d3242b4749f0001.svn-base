/// this is a module included into neuroimpl-fuzzy
/// it converts string parameters - fuzzy sets and fuzzy rules - into appropriate variables

#include "neuroimpl-fuzzy-f0.h"
// nonstd z powodu max()
#include "nonstd.h"

//this part concerns fuzzy sets transformation

/** This function converts given string into numbers representing fuzzy sets */
int FuzzyF0String::convertStrToSets(const SString& str, double numbers[], int nrOfSets)
{
  int pos=0;
  SString t;
  int have=0;
  int maxnumbers=4*nrOfSets; //number of semicolons should be equal 4*nrOfSets

  while (str.getNextToken(pos,t,';'))
    if (have>=maxnumbers)
      break;
    else
      numbers[have++]=atof(t);

  //check if number of read numbers (separated with semicolon) is equal to declared
  if (have != 4*nrOfSets)
    return -1; //number of sets found is lower than declared!

  //check corectness of sets - must not be decreasing
  for(int i=0;i<nrOfSets;i++)
    if((numbers[4*i]>numbers[4*i+1])||(numbers[4*i+1]>numbers[4*i+2])||(numbers[4*i+2]>numbers[4*i+3]))
      return -2; //error

  return 0;
}

//this part concerns fuzzy rules transformation

/** This function counts number of inputs and outputs given in string consisting fuzzy rules.
It counts number of separators:
; between inputs/outputs and fuzzy sets number
: between conditional and decision part of a rule
/ end of rule
*/
int FuzzyF0String::countInputsOutputs(const char* str, int ruldef[], int rulesNr)
{ //ruledef will remember counted number of inputs and outputs for every rule
  const char* t;
  int separators=0, inouts=0;

  for(t=str;*t;t++)
  {
    while(isdigit(*t))
      t++; //only count, does not care about numbers now
    if (!*t)
      break; //end of the string - get out of 'for' loop
    if ( (*t==';')||(*t==':')||(*t=='/') ) //found sth different than digit - it must be a separator
    {
      separators++; //one of separators
      if (*t!=';') // end of [conditional part of] rule
      {
	if (inouts >= 2*rulesNr) //more rules declared in string than declared in rulesNr
	  return -2;
        ruldef[inouts]=(separators+1)/2; //cause fuzzy sets - for 1 in/out there are 2 semicolons
	separators=0;  //begin counting number of in/out from zero
	inouts++; //next part of rule / or next rule
      }
    }
    else // illegal character
      return -1;
  }

  //check, if nr of found rules is equal to declared
  if (inouts == 2*rulesNr) //each rule has a conditional part (inputs) and decisional part (outputs)
    return 0;
  else
    return -5; // ShowMessage("Inconsistent number of rules!");
}

/** This function converts given string into variable 'rules' that remembers all inputs/outputs and fuzzy set.
The procedure is conduct according to ruledef, calculated earlier.*/
int FuzzyF0String::convertStrToRules(const SString& str, const int ruledef[], int **rules, int setsNr, int rulesNr, int &maxOutputNr)
{
  int pos=0, j, k, len=str.len();
  int dNr=0, sNr=0;
  int inNr, outNr; //number of inputs/outputs and corresponding fuzzy sets
  SString t;
  bool conditional=true; //which part of rule: conditional or decisional

  maxOutputNr=0; //sets maximum output nr found in rules string

  //check corectness of the string: number semicolon ... separated with colon or slash
  while(pos<len)
  {
    while((pos<len)&&(isdigit(str.charAt(pos)))) pos++;
    if(!(pos<len))
      break; //end of the string
    if(str.charAt(pos)!=';')
      if((str.charAt(pos)==':')&&(conditional))
        {sNr++; conditional=false;}
      else if((str.charAt(pos)=='/')&&(!conditional))
        {dNr++; conditional=true;}
      else
        return -4; //error - illegal character
    pos++;
  }
  if( (dNr!=sNr) || (dNr!=rulesNr) )
    return -5; //error - wrong number of rules

  pos=0;

  for(j=0;j<rulesNr;j++)
  { //sum of inputs and outputs
    inNr = 2*ruledef[2*j];
    outNr = 2*ruledef[2*j+1];
    for(k=0;k<inNr+outNr;k++)
    {
      t = ""; //clear previous value
      //cuts next integer values
      while ( (pos<len)&&(isdigit(str.charAt(pos))) )
        t += str.charAt(pos++);
      pos++;
      rules[j][k]=atol(t); //convert cut out string into number
      //fuzzy sets - odd index table - are counted from 0,
      //so if 5 fuzzy sets declared, values acceptable are 0,1,2,3,4
      if ( ((k%2)!=0) && (rules[j][k] >= setsNr) )
      	return -1;
      if((k>=inNr)&&((k%2)==0))
        maxOutputNr=max(maxOutputNr,rules[j][k]);
    }
  }
  return 0;
};

