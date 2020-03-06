// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _GENO_OPERATORS_H_
#define _GENO_OPERATORS_H_

#include <common/nonstd.h>
#include <frams/model/model.h>

/** @file */

/** \name Return codes for genetic operators */
//@{
#define GENOPER_OK          0 ///<operation successful
#define GENOPER_OPFAIL     -1 ///<operation failed or could not be completed
#define GENOPER_REPAIR     -2 ///<do not use in Geno_fx. GenMan uses it in checkValidity()... but will not. only f4 uses it
#define GENOPER_NOOPER     -3 ///<do not use in Geno_fx. GenMan uses it for "no suitable operator for this genotype format"
//@}

/** \name gene/character predefined styles (for style() method) */
//@{
#define GENSTYLE_NONE       0 ///<no style specified (=normal font)
#define GENSTYLE_INVALID    1 ///<this char cannot be accepted
#define GENSTYLE_BOLD       2 ///<bold
#define GENSTYLE_ITALIC     4 ///<italic
#define GENSTYLE_STRIKEOUT  8 ///<strikeout (not recommended)
//UNDERLINE used to mark errors
//@}

/** \name other useful style/color macros */
//@{
#define GENRGB(r,g,b) ((uint32_t)(((uint8_t)(r)|((uint16_t)((uint8_t)(g))<<8))|(((uint32_t)(uint8_t)(b))<<16)))
#define GENSTYLE_RGBS(r,g,b,s) ((uint32_t)((uint8_t)s)<<24 | GENRGB(r,g,b))
#define GENSTYLE_CS(rgb,s) ((uint32_t)((uint8_t)s)<<24 | rgb)

#define GENGETSTYLE(style) ((style)>>24)
#define GENGETCOLOR(style) ((style)&0x00ffffff)
#define GENGET_R(style) ((style)&0xff)
#define GENGET_G(style) ((style>>8)&0xff)
#define GENGET_B(style) ((style>>16)&0xff)

#define GENCOLOR_TEXT    GENRGB(0,0,0) ///<recommended color to use for text genes
#define GENCOLOR_NUMBER  GENRGB(200,0,0) ///<recommended color to use for number genes
//@}

///Base class for genetic operations on genotypes of some genetic format
/**\author Maciej Komosinski

When designing genetic operations on some representation, inherit your class
(for example GenoOper_fMy) from GenoOperators. Define some methods,
like mutate(), in your class, to allow for evolution.
Ensure they have the same names and arguments as the corresponding
virtual methods in Geno_fx. Set the 'supported_format' variable to the
appropriate genetic representation ID.
Whenever arguments are genotypes, they are without
trailing characters which describe genetic format
(for example, "p:", not "//0\np:").
When allocating/reallocating char* parameters, use malloc, free, realloc, strdup, etc.
Do not use new and delete.

All the methods you might define are:
- checkValidity()
- validate()
- mutate()
- crossOver()
- getSimplest()
- style()

Your code must not cause errors (like invalid memory access, memory
leaks) on any arguments, even 'random' ones. GENOPER_OPFAIL should
be returned when an operator cannot cope with its argument genotype.

To compile your code, you may also need some SDK files.
A simple example is Geno_ftest class (see \ref geno_ftest_example "C++ code" for details).
A more realistic example is Geno_f4 derived from Geno_fx: refer to
the available source on developmental encoding and f4 genotype format.*/

class GenoOperators
{
public:
	Param par;
	char supported_format; ///<genotype format which is supported by this class ('6' for GenoOper_f6, 'F' for GenoOper_fF, etc.). Must be initialized in constructor
	string name; ///<name of this set of genetic operators
	const char **mutation_method_names; ///<array of names for mutation methods. If initialized (by new const char*[]), must have entries for each method index returned by mutate(geno,chg,METHOD).  If initialized, it is automatically freed by this destructor.
	GenoOperators() : par(empty_paramtab) { supported_format = 'x'; name = "Default"; mutation_method_names = NULL; setDefaults(); }

	/**Used to perform initializations of Param parameters that are not handled by the Param itself
	(i.e. string parameters or fields that require some complex logic may be initialized here)*/
	virtual void setDefaults() {}

	/**Checks a genotype for minor mistakes and major errors.
	\param geno genotype to be checked
	\param genoname name of the genotype to be checked
	\retval error_position 1-based (or 1 if no exact error position known)
	\retval GENOPER_OK when the genotype is fully valid, and can be translated by the converter with \b no modifications nor tweaks*/
	virtual int checkValidity(const char *geno, const char *genoname) { return GENOPER_NOOPER; }

	/**Validates a genotype. The purpose of this function is to validate
	obvious/minor errors (range overruns, invalid links, etc.). Do not try
	to introduce entirely new genes in place of an error.
	\param geno input/output: genotype to be validated
	\param genoname name of the genotype to be validated
	\retval GENOPER_OK must be returned in any case ("did my best to validate")*/
	virtual int validate(char *&geno, const char *genoname) { return GENOPER_NOOPER; }

	/**Mutates a genotype. Mutation should always change something.

	Avoid unnecessary calls in your code. Every genotype argument passed to this
	function is first checked, and validated if checkValidity() reported an error (or
	if there is no checkValidity() implemented). Every resulting genotype is subject
	to the same procedure, unless GENOPER_OPFAIL was returned. Thus you do not have
	to call these functions on input and output genotypes, because they are validated
	if needed.
	\param geno input/output: genotype to be mutated
	\param chg output: initialize with a value (in most cases 0..1) corresponding
	to the amount of genotype mutated. For example, it could be the number of changed
	genes divided by the total number of genes before mutation.
	\param chg method: initialize with the ID (number) of mutation method used.
	\retval GENOPER_OK
	\retval GENOPER_OPFAIL
	\sa
	Mutation example to illustrate the exchange of pointers for \e geno.
	The mutation adds random letter at the beginning or removes last letter from \e geno.
	\code
	{
	int len=strlen(geno);
	if (len==0 || random(2)==0) //add
	{
	method=0;
	char* mutated=(char*)malloc(mutated,len+2); //allocate for mutated genotype
	mutated[0]='A'+random(10); //first char random
	strcpy(mutated+1,geno); //the rest is original
	free(geno); //must take care of the original allocation
	geno=mutated;
	} else
	{
	method=1;
	geno[len-1]=0; //simply shorten the string - remove last char
	}
	chg=1.0/max(len,1); //estimation of mutation strength, divby0-safe
	} \endcode
	*/
	virtual int mutate(char *&geno, float& chg, int &method) { method = -1; chg = -1; return GENOPER_NOOPER; }

	/**Crosses over two genotypes. It is sufficient to return only one child (in \e g1) and set \e chg1 only, then \e g2 must equal "".

	Avoid unnecessary calls in your code. Every genotype argument passed to this
	function is first checked, and validated if checkValidity() reported an error (or
	if there is no checkValidity() implemented). Every resulting genotype is subject
	to the same procedure, unless GENOPER_OPFAIL was returned. Thus you do not have
	to call these functions on input and output genotypes, because they are validated
	if needed.
	\param g1 input/output: parent1 genotype, initialize with child1
	\param g2 input/output: parent2 genotype, initialize with child2 if both children are available
	\param chg1 output: initialize with the fraction of parent1 genes in child1 (parent2 has the rest)
	\param chg2 output: initialize with the fraction of parent2 genes in child2 (parent1 has the rest)
	\retval GENOPER_OK
	\retval GENOPER_OPFAIL
	\sa mutate() for an example*/
	virtual int crossOver(char *&g1, char *&g2, float& chg1, float& chg2) { chg1 = chg2 = -1; return GENOPER_NOOPER; }

	/**\return a pointer to the simplest genotype string*/
	virtual const char* getSimplest() { return NULL; }

	/**You may want to have your genotype colored. This method provides desired character styles for genes.
	\param geno genotype
	\param pos 0-based char offset
	\retval number-encoded visual style (and validity) of the genotype char at \e geno[pos].
	Assume white background.
	\sa GENSTYLE_* macros, like GENSTYLE_BOLD*/
	virtual uint32_t style(const char *geno, int pos) { return GENSTYLE_RGBS(0, 0, 0, GENSTYLE_NONE); }

	///currently not used (similarity of two genotypes)
	virtual float similarity(const char*, const char*) { return GENOPER_NOOPER; }
	virtual ~GenoOperators() { if (mutation_method_names) { delete[]mutation_method_names; mutation_method_names = NULL; } }
	//   virtual char getFormat() {return 255;} //returns supported genotype format, for ex. '1'
	//   virtual int enabled() {return 1;} // should be enabled by default

	/** \name Some helpful methods for you */
	//@{
	static int roulette(const double *probtab, const int count); 	///<returns random index according to probabilities in the \e probtab table or -1 if all probs are zero. \e count is the number of elements in \e probtab.
	static bool getMinMaxDef(ParamInterface *p, int propindex, double &mn, double &mx, double &def); ///<perhaps a more useful (higher-level) way to obtain min/max/def info for integer and double properties. Returns true if min/max/def was really available (otherwise it is just invented).
	static int selectRandomProperty(Neuro* n); ///<selects random property (either 0-based extraproperty of Neuro or 100-based property of its NeuroClass). -1 if Neuro has no properties.
	static double mutateNeuProperty(double current, Neuro *n, int propindex); ///<returns value \e current mutated for the property \e propindex of NeuroClass \e nc or for extraproperty (\e propindex - 100) of Neuro. Neuro is used as read-only. Give \e propindex == -1 to mutate connection weight (\e nc is then ignored).
	static bool mutatePropertyNaive(ParamInterface &p, int propindex); ///<creep-mutate selected property. Returns true when success. mutateProperty() should be used instead of this function.
	static bool mutateProperty(ParamInterface &p, int propindex); ///<like mutatePropertyNaive(), but uses special probability distributions for some neuron properties.
	static bool getMutatedProperty(ParamInterface &p, int i, double oldval, double &newval); ///<like mutateProperty(), but just returns \e newval, does not get nor set it using \e p.
	static double mutateCreepNoLimit(char type, double current, double stddev, bool limit_precision_3digits); ///<returns \e current value creep-mutated with Gaussian distribution and \e stddev standard deviation. Precision limited to 3 digits after comma when \e limit_precision_3digits is true. \e type must be either 'd' (integer) or 'f' (float/double).
	static double mutateCreep(char type, double current, double mn, double mx, double stddev, bool limit_precision_3digits); ///<just as mutateCreepNoLimit(), but forces mutated value into the [mn,mx] range using the 'reflect' approach.
	static double mutateCreep(char type, double current, double mn, double mx, bool limit_precision_3digits); ///<just as mutateCreepNoLimit(), but forces mutated value into the [\e mn,\e mx] range using the 'reflect' approach and assumes standard deviation to be a fraction of the mx-mn interval width.
	static void setIntFromDoubleWithProbabilisticDithering(ParamInterface &p, int index, double value); ///<sets a double value in an integer field; when a value is non-integer, applies random "dithering" so that both lower and higher integer value have some chance to be set.
	static void linearMix(vector<double> &p1, vector<double> &p2, double proportion); ///<mixes two real-valued vectors; inherited proportion should be within [0,1]; 1.0 does not change values (all inherited), 0.5 causes both vectors to become their average, 0.0 swaps values (none inherited).
	static void linearMix(ParamInterface &p1, int i1, ParamInterface &p2, int i2, double proportion); ///<mixes i1'th and i2'th properties of p1 and p2; inherited proportion should be within [0,1]; 1.0 does not change values (all inherited), 0.5 causes both properties to become their average, 0.0 swaps values (none inherited). For integer properties applies random "dithering" when necessary.
	static int getActiveNeuroClassCount(); ///<returns active class count
	static NeuroClass* getRandomNeuroClass(); ///<returns random neuroclass or NULL when no active classes.
	static NeuroClass* getRandomNeuroClassWithOutput(); ///<returns random neuroclass with output or NULL when no active classes.
	static NeuroClass* getRandomNeuroClassWithInput(); ///<returns random neuroclass with input or NULL when no active classes.
	static NeuroClass* getRandomNeuroClassWithOutputAndNoInputs(); ///<returns random sensor or NULL when no active classes.
	static int getRandomNeuroClassWithOutput(const vector<NeuroClass*>& NClist); ///<returns index of random NeuroClass from the NClist or -1 (no neurons on the list that provide output) \e NClist list of available neuron classes
	static int getRandomNeuroClassWithInput(const vector<NeuroClass*>& NClist); ///<returns index of random NeuroClass from the NClist or -1 (no neurons on the list that want input(s)) \e NClist list of available neuron classes
	static int getRandomChar(const char *choices, const char *excluded); ///<returns index of a random character from 'choices' excluding 'excluded', or -1 when everything is excluded or 'choices' is empty.
	static NeuroClass* parseNeuroClass(char *&s); ///<returns longest matching neuroclass or NULL if the string does not begin with a valid neuroclass name. Advances \e s pointer.
	static Neuro* findNeuro(const Model *m, const NeuroClass *nc); ///<returns pointer to first Neuro of class \e nc, or NULL if there is no such Neuro.
	static int neuroClassProp(char *&s, NeuroClass *nc, bool also_v1_N_props = false); ///<returns 0-based property number for \e neuroclass, 100-based extraproperty number for Neuro, or -1 if the string does not begin with a valid property name. Advance \e s pointer if success.
	static bool isWS(const char c); ///<is \e c a whitespace char?
	static void skipWS(char *&s); ///<advances pointer \e s skipping whitespaces.
	static bool areAlike(char*, char*); ///<compares two text strings skipping whitespaces. Returns 1 when equal, 0 when different.
	static char* strchrn0(const char *str, char ch); ///<like strchr, but does not find zero char in \e str.
	static bool canStartNeuroClassName(const char firstchar); ///<determines if \e firstchar may start NeuroClass name. If not, it may start NeuroClass' (or Neuro's) property name.
	//@}
};


//
// custom distributions for mutations of various parameters
//
/*
static double distrib_weight[]=
{
5,                 // distribution -999 _-^_^-_ +999
-999, 999,         // each weight value may be useful, especially...
-5, -0.3,        // ...little non-zero values
-3, -0.6,
0.6, 3,
0.3, 5,
};
*/

#endif
