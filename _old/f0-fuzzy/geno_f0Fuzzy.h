#ifndef _GENO_f0Fuzzy_H_
#define _GENO_f0Fuzzy_H_

/// Table of probabilities - names used in project facilitate reading

#define F0Fuzzy_ADDSET   0
#define F0Fuzzy_REMSET   1
#define F0Fuzzy_ADDRULE  2
#define F0Fuzzy_REMRULE  3
#define F0Fuzzy_MODRULE  4
#define F0Fuzzy_MOD2RULE 5
#define F0Fuzzy_COUNT    6

#include "sstring.h"
#include "geno_fx.h"

class Geno_f0Fuzzy : public Geno_fx
{
private:
  /** Function finds number of nearest fuzzy (in list of sets) to given fuzzy set
    \param set fuzzy set to be matched
    \param sets fuzzy sets list
    \param setsNr number of fuzzy sets in list
    @return number of the nearest fuzzy set in sets list
  **/
  int nearestSet(double set[4], double sets[], int setsNr);
  /** Function changes numeration of fuzzy sets numbers in fuzzy rules string, according to transformation table.
    \param rules fuzzy rules string
    \param transTable table of fuzzy sets transformation - [old_number]=new_number
    \param nrItems number of old fuzzy sets
    @return success or failure
  **/
  int changeSetsNumeration(SString &rules, int transTable[], int nrItems);
  /** Function checks which fuzzy sets are used in rules and removed unused from fuzzy sets string, filling
    appropriate values into transformation table.
    \param sets fuzzy sets string
    \param rules fuzzy rules string
    \param setsNr number of fuzzy sets (input value: before removing, output value: after removing)
    \param transTable table of fuzzy sets transformation - [old_number]=new_number
    @return success or failure
  **/
  int removeUnusedSets(SString &sets, SString rules, int &setsNr, int transTable[]);
  /** Function checks every rule whether input or output number does not duplicace in one rule. If so, removes
    excessive input or output.
    \param rules fuzzy rules string
    \param inputsNr number of system inputs
    \param outputsNr number of system outputs
    @return success or failure
  **/
  int removeExcessiveInOut(SString &rules, int inputsNr, int outputsNr);
  /** Function checks rule-based system whether there there is no duplicate rule's premise part. If so, removes
    excessive one.
    \param rules fuzzy rules string
    \param inputsNr number of system inputs
    \param outputsNr number of system outputs
    @return success or failure
  **/
  int removeExcessiveRules(SString &rules, int setsNr, int &rulesNr);
  /** Function sorts given fuzzy sets in not decreasing rank.
    \param set fuzzy set
    @return success or failure
  **/
  int sortSet(double set[4]);
  /** Function calculates randomly new fuzzy set, using random gauss routines from rndutil.h.
    \param newSet values (4 numbers) of new fuzzy set
    @return success or failure
  **/
  int computeSet(double newSet[4]);
  /** Function conducts checkValidity od validate procedures, depending on repair parameter.
    \param geno given input genotype (if repair=true and needs to be repaired and it was possible to repair,
    also output genotype)
    \param repair flag whether repair genotype (if needed) or not
    @return success or failure
  **/
  int checkOrValidate(char *&geno, bool repair);
  /**
    Function compares fuzzy sets from two creatures and mark duplicated fuzzy sets.
    \param fuzzySets1 fuzzy sets from creature #1
    \param setsNr1 number of fuzzy sets from creature #1
    \param fuzzySets2 fuzzy sets from creature #2
    \param setsNr2 number of fuzzy sets from creature #2
    \param transTable table which remembers numbers of duplicated sets
    \param setsNr3 number of fuzzy sets from creatures #1 + #2
    @return success or failure
  **/
  int markDuplicatedSets(const double fuzzySets1[], const int setsNr1, const double fuzzySets2[], const int setsNr2, int transTable[], const int setsNr3);
public:

  double probtab[F0Fuzzy_COUNT]; //table of probabilities - see #define above
  int maximumSetsNr;    //total number of sets can not be greater than maximum
  int maximumRulesNr;   //total number of rules can not be greater than maximum

  Geno_f0Fuzzy();
  char *getSimplest();
  int mutate(char *&g, float &chg,int &method);
  int crossOver(char *&g1, char *&g2, float &chg1, float &chg2);
  int checkValidity(const char *geno);
  int validate(char *&);
};

#endif

