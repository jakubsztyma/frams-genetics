// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FL_GENERAL_
#define _FL_GENERAL_

#include <frams/util/sstring.h>
#include <frams/param/mutableparam.h>
#include <frams/param/paramobj.h>
#include <vector>
#include <list>
#include <unordered_map>
#include <frams/model/model.h>
#include "fL_matheval.h"

/// determines to which class fL element belongs
enum fLElementType {
	TERM, ///< word of L-System
	INFO, ///< fL_Builder object
	RULE, ///< rule of L-System
	BRANCH ///< special type of fL word, which represents branch
};

/** @name Constants used in fL methods */
//@{
#define FL_PART_PROPS_COUNT   4 ///<Count of part properties
#define FL_JOINT_PROPS_COUNT  3 ///<Count of joint properties
#define FL_PE_NEURO_DET       "d" ///<Id of details type definition in f0_neuro_paramtab
#define FL_PE_CONN_WEIGHT     "w" ///<Id of weight type definition in f0_neuroconn_paramtab
#define FL_PE_CONN_ATTR       "attr" ///<Id of attractor of neural connection
#define FL_DEFAULT_LENGTH     1.0 ///<Default length of a stick in fL encoding
#define FL_MINIMAL_LENGTH     0.0 ///<Minimal length of a stick in fL encoding
#define FL_MAXIMAL_LENGTH     2.0 ///<Maximal length of a stick in fL encoding
#define FL_MAXITER           "100.0" ///<Maximal iteration available in fL
#define FL_MAXPARAMS          "3" ///<Maximal number of user-parameters
//@}

extern const char *fL_part_names[];
extern const char *fL_joint_names[];
extern const char *fL_joint_fullnames[FL_JOINT_PROPS_COUNT];
extern const char *fL_part_fullnames[FL_PART_PROPS_COUNT];

#define LSYSTEM_PARAM_TYPE "s" ///< standard type of L-System elements

class fL_Builder;

/**
 * Most abstract class of L-System. It requires from inheriting classes to implement
 * methods processDefinition and toString.
 */
class fL_Element
{
public:
	fLElementType type; ///< type of fL_Element

	int begin; ///<beginning of the element definition in genotype
	int end; ///<end of the element definition in genotype


	fL_Element(fLElementType type) : type(type) { begin = end = 0; }
	virtual ~fL_Element() { }

	/**
	 * Performs connecting current element to fL_Builder and parsing of input
	 * with respect of builder.
	 * @param builder fL_Builder object responsible for creating this element
	 * @return 0 if processing went successfully, other values otherwise
	 */
	virtual int processDefinition(fL_Builder *builder) = 0;

	/**
	 * Creates string line for current element. This method is used by Builder
	 * to create string representation of words, rules and informations of L-System.
	 * @return stringified element
	 */
	virtual SString toString() = 0;
};

/**
 * Represents word objects of L-System representation. They are used in axiom,
 * produced sequences and rules.
 */
class fL_Word : public fL_Element
{
public:
	SString name; ///<name of word
	int npar; ///<number of word parameters
	MutableParam mut; ///<MutableParam for word parameters
	ParamEntry *tab; ///<ParamTab used for loading and saving properties of word
	void *data; ///<pointer to properties of word
	bool builtin; ///<determines if word is built-in (true) or not (false).
	PartBase *bodyelementpointer; ///<helper field for built-in words
	std::vector<MathEvaluation *> parevals; ///<stores MathEvaluation objects with parameter functions
	double creationiter; ///<this helper field is used during L-System iterations and determines when word was created
	fL_Word(bool builtin = false, int begin = 0, int end = 0) :
		fL_Element(fLElementType::TERM), mut("Word", "Properties"),
		builtin(builtin)
	{
		name = "";
		npar = 0;
		tab = NULL;
		data = NULL;
		creationiter = 0;
		this->begin = begin;
		this->end = end;
		bodyelementpointer = NULL;
	}

	/**
	 * Destroys data ParamObject and MathEvaluation objects. ParamTab for Word
	 * should be deleted separately, because all words with same name share same
	 * pointer to ParamTab.
	 */
	~fL_Word()
	{
		if (data)
			ParamObject::freeObject(data);
		for (MathEvaluation *ev : parevals)
		{
			if (ev) delete ev;
		}
		parevals.clear();
	}

	/**
	 * Checks if there is no other word stored in the builder with the same name,
	 * prepares ParamTab for defined word and adds created word definition to the
	 * builder.
	 * @param builder pointer to the builder that process this word definition
	 * @return 0 if word is added, 1 if word is redefined
	 */
	int processDefinition(fL_Builder *builder);

	/**
	 * Operator copies name string, npar value, pointer to ParamTab and pointers
	 * to MathEvaluation objects for parameters. It sets data of this word to
	 * NULL.
	 * @param src source word
	 */
	void operator=(const fL_Word& src);

	/**
	 * Returns 'w:' line defining word in genotype.
	 * @return string representation of word definition
	 */
	virtual SString toString();

	/**
	 * Returns token version of this word. It is used during converting successor's
	 * list or axiom list into strings. Flag 'keepformulas' should be set to true
	 * if word parameters should be represented as formulas, and set to false if
	 * word parameters should contain the result of evaluation.
	 * @param keepformulas true if parameters should have mathematical formula, false if it should have result of evaluation
	 */
	virtual SString stringify(bool keepformulas = true);

	/**
	 * Saves formulas or evaluations of parameters in ParamObject.
	 * @param keepformulas true if parameters should have mathematical formula, false if it should have result of evaluation
	 * @return 0 if method successfully managed to save data, 1 if an error occured
	 */
	int saveEvals(bool keepformulas);

	/**
	 * Computes distance between two L-System words in genotype. Distance has sense
	 * only when two words have the same name. Otherwise, returned value equals -1.
	 * The distance is computed as Euclidean distance between words arguments.
	 * @param right the second word
	 * @return Euclidean distance between words in genotype or -1 if words have different name
	 */
	double distance(fL_Word *right);
};

/**
 * Represents special branching word in L-System genotype.
 */
class fL_Branch : public fL_Word
{
public:
	/// Determines if branching parenthesis is opened or closed
	enum BranchType {
		OPEN,
		CLOSE
	};
	BranchType btype; ///< branch parenthesis type
	fL_Branch(BranchType branchtype, int begin, int end) : fL_Word(true, begin, end)
	{
		type = fLElementType::BRANCH;
		btype = branchtype;
		name = stringify();
	}
	SString toString() { return ""; };
	SString stringify(bool keepformulas = false) { return btype == BranchType::OPEN ? "[" : "]"; }
};

/**
 * Represents rules in L-System genotype. Every rule has predecessor and successors.
 * The optional argument is a condition of deploying rule. Every rule is ran
 * synchronously for every letter of the current genotype. If condition and
 * predecessor are satisfied, then predecessor is replaced by successors.
 */
class fL_Rule : public fL_Element
{
public:
	SString predecessor; ///<string representation of the predecessor
	SString condition; ///<string representation of the condition
	SString successor; ///<string representation of the successor

	fL_Word *objpred; ///<object representation of the predecessor
	MathEvaluation *condeval; ///<object representation of the condition

	std::list<fL_Word *> objsucc; ///<objec representation of successors

	fL_Rule(int begin, int end)  : fL_Element(fLElementType::RULE)
	{
		predecessor = "";
		condition = "";
		successor = "";
		objpred = NULL;
		condeval = NULL;
		this->begin = begin;
		this->end = end;
	}

	~fL_Rule()
	{
		if (objpred) delete objpred;
		if (condeval) delete condeval;
		for (fL_Word *word : objsucc)
		{
			delete word;
		}
		objsucc.clear();
	}

	/**
	 * Loads rule definition from the genotype, checks if word defined in
	 * predecessor does exist, tokenizes successor and condition.
	 * @param builder pointer to the builder that process this word definition
	 * @return 0 if word is added, 1 if an error occured during parsing
	 */
	int processDefinition(fL_Builder *builder);

	SString toString();

	/**
	 * Runs rule for a given word, if all conditions and starting rules are satisfied.
	 * Method alters given list of tokens by removing matching word pointed by
	 * pointer and iterator of list and inserting successor sequence into list.
	 * Final iterator value points to the first word after processed word.
	 * @param builder builder containing current genotype
	 * @param in word that is currently processed
	 * @param it iterator of genotype list which determines position of current analysis
	 * @param currtime value representing continuous time value for rule
	 * @return 0 if rule processed current word, 1 if rule is no applicable
	 */
	int deploy(fL_Builder *builder, fL_Word *in, std::list<fL_Word *>::iterator &it, double currtime);
};

/**
 * Structure for holding current Turtle state.
 */
struct fL_State
{
	Pt3D direction; ///<Direction of turtle
	Part *currpart; ///<Latest created part
	Neuro *currneuron; ///<Latest created neuron
	fL_State() : direction(1,0,0), currpart(NULL), currneuron(NULL)
	{ }
};

/**
 * Main class of L-System processing. It holds all information required to perform
 * steps of development of a creature. It allows to create Timed D0L-Systems with
 * following limitations:
 *
 *  - fractions of time affect only parameters of currently produced words,
 *  - integer steps deploy rules in order to develop new elements of body,
 *  - if parameter should use time fraction, then $t variable must be used,
 *  - $t variable always contain values from 0 to 1 that represent current fraction
 *    of step.
 */
class fL_Builder : public fL_Element
{
public:
	SString axiom; ///<starting sequence of genotype
	double time; ///<time of creature development
	int numckp; ///<number of checkpoints per growth step - checkpoints are always created after rules deployment in integer steps, numckp determines how many checkpoints should be created within single step (to watch changes of parameters)
	int maxwords; ///<maximum number of words that can
	bool using_checkpoints; ///<true if checkpoints should be created, or false
	bool using_mapping; ///<true if mappings should be created, or false

	int builtincount; ///<number of built-in words

	std::list<fL_Word *> genotype; ///<list of current words of genotype
	std::unordered_map<std::string, fL_Word *> words; ///<map from string to words existing in L-System
	std::vector<std::string> wordnames;
	std::vector<fL_Rule *> rules; ///<rules available in system

	fL_Builder(bool using_mapping = false, bool using_checkpoints = false) : fL_Element(fLElementType::INFO),
			using_checkpoints(using_checkpoints), using_mapping(using_mapping)
	{
		axiom = "";
		time = 1.0;
		numckp = 1;
		builtincount = 0;
		maxwords = -1;
	}
	~fL_Builder();

	/**
	 * Helper function for trimming leading and trailing spaces from sequence.
	 * @param data input string
	 * @return string with trimmed spaces
	 */
	static std::string trimSpaces(const std::string& data);

	/**
	 * Processes single line of input sequence and load line properties with use
	 * of proper ParamTab.
	 * @param type which type of line is this
	 * @param line line to be processed
	 * @param obj variable storing pointer to created L-System element
	 * @param linenumber for error messages
	 * @param begin index of first character of line
	 * @param end index of last character of line
	 * @return 0 if processing ended successfully, 1-based position of error otherwise
	 */
	int processLine(fLElementType type, const SString &line, fL_Element *&obj, int linenumber, int begin, int end);

	/**
	 * Parses input genotype with words, information and rules. First of all,
	 * model-specific words defined in addModelWords method are added to words map.
	 * Every genotype should start with words definitions. They cannot collide
	 * with model words and with each other. Secondly, information line, containing
	 * axiom and number of iteration must be processed. In the end, all rules
	 * have to be processed by builder. After determining line type and creating
	 * fL_Element specialized object, the builder runs object's processDefinition
	 * method to generate all important objects and register element in builder
	 * structures.
	 * @param genotype input genotype
	 * @return 0 if processing finished successfully, 1-based position of error if it occured
	 */
	int parseGenotype(const SString &genotype);

	/**
	 * Prepares structures and tokenizes axiom.
	 * @param builder this method should receive pointer to itself
	 * @return 0 if processing went successfully, 1 otherwise
	 */
	int processDefinition(fL_Builder *builder);

	/**
	 * Adds words used for Framsticks body and brain development.
	 */
	void addModelWords();

	/**
	 * Helper method that tokenizes string sequence by closing parenthesis
	 * with respect to parenthesis nesting.
	 * @param pos index of character, from which next token needs to be extracted. Method replaces this parameter with next starting position, or -1 if there was an error during parsing, like parenthesis mismatch
	 * @param src original sequence
	 * @param token reference to variable holding new token
	 * @return true if new token is found, false if there is no more tokens or when there was parenthesis mismatch (if pos == -1, then it is parenthesis mismatch)
	 */
	bool getNextObject(int &pos, const SString &src, SString &token);

	/**
	 * Creates fL_Word object for a given token.
	 * @param worddef string representing single word object in sequence
	 * @param word a reference to a pointer of created word by this method
	 * @param numparams number of parameters for a given sequence (usually number of rule predecessor's arguments)
	 * @param begin the begin of word definition in genotype
	 * @param end the end of word definition in genotype
	 * @return 0 if conversion went successfully, 1 when there is a problem with parsing
	 */
	int createWord(const SString &worddef, fL_Word *&word, int numparams, int begin, int end);

	/**
	 * Helper function that converts input sequence into list of L-System words.
	 * @param sequence input sequence of stringified word objects
	 * @param result reference to list holding generated words
	 * @param numparams number of parameters that can be used for words stored in result list
	 * @return 0 if tokenizing finished successfully, 1 otherwise
	 */
	int tokenize(const SString &sequence, std::list<fL_Word *> &result, int numparams, int begin, int end);

	/**
	 * Uses rules to process current genotype and make next iteration step for
	 * L-System development.
	 * @param currtime current time of genotype processing
	 * @return always 0
	 */
	int iterate(double currtime);

	/**
	 * Alters only parameters that depend on time. Should be used if only fraction
	 * of iteration has changed, not integer part.
	 * @param currtime current time of processing
	 * @return always 0
	 */
	int alterTimedProperties(double currtime);

	/**
	 * Developes L-System from given genotype and builds Framsticks Model from it.
	 * When using_checkpoints is enabled, method generates checkpoint for each
	 * step defined in timestamp.
	 * @param neededtime reference to a time value after stopping development (usually it will be equal to time specified in the time field, unless the number of allowed words will be exceeded earlier)
	 * @return final model from a fL genotype
	 */
	Model* developModel(double &neededtime);

	/**
	 * Creates new checkpoint for a given model based on current state of genotype.
	 * @param model reference to model
	 * @return 0 if developing went successfully, 1 otherwise
	 */
	int buildModelFromSequence(Model *model);

	/**
	 * Returns stringified product of L-System development.
	 * @return stringified words of developed L-System
	 */
	SString getStringifiedProducts();

	/**
	 * Returns genotype stored by objects in fL_Builder.
	 * @return genotype converted from L-System objects
	 */
	SString toString();

	/**
	 * Alters part properties according to informations stored in stickword.
	 * Method takes current values of part's properties computed from previous
	 * calls of the alterPartProperties and computes mean according to upcoming
	 * values of properties.
	 * @param part analysed part
	 * @param stickword the L-System word that affects current part
	 * @param alterationcount the number of times of part modifications - used to compute mean of every word properties
	 * @return always 0
	 */
	int alterPartProperties(Part *part, fL_Word *stickword, double &alterationcount);

	/**
	 * Finds input neuron that is nearest to attractor or connection definition.
	 * When attractor object is given, then the word with name matching attractor and
	 * with nearest values of parameters is chosen as the point, from which input
	 * neuron is looked for. The searching goes both sides.
	 * @param currneu object storing informations about connection word iterator and current neuron
	 * @param attractor pointer to an attractor definition presented in connection word
	 * @return pointer to the input neuron, or NULL if no neuron could be found
	 */
	Neuro* findInputNeuron(std::pair<std::list<fL_Word *>::iterator, Neuro *> currneu, fL_Word *attractor);

	/**
	 * Removes joints, parts and neurons with its connections from current model, without
	 * removing checkpoint data.
	 * @param m model to clear
	 */
	void clearModelElements(Model *m);

	/**
	 * Converts parameters defined in built-in words into desired
	 * range with use of sigmoid function, which ensures staying
	 * within min and max value.
	 * @param input the value from evaluation of parameter
	 * @param min minimal value of property
	 * @param max maximal value of property
	 * @return value of body element property
	 */
	double sigmoidTransform(double input, double min, double max);

	/**
	 * Counts words defined in the genotype.
	 * @return number of defined words
	 */
	int countDefinedWords();

	/**
	 * Counts number of sticks in a given sequence
	 * @return number of sticks in sequence
	 */
	int countSticksInSequence(std::list<fL_Word *> *sequence);

	/**
	 * Counts all definitions of words, all words in axiom and rule successors in a genotype.
	 * Used for computing change between original genotype and mutation.
	 * @return number of words in definitions, axiom and rule successors
	 */
	int countWordsInLSystem();

	/**
	 * Sorts rules and removes rules that will not be used. The "sorting" is
	 * done in such way that all rules with conditions are first, and rules
	 * without conditions go to the end. If there are more than one rule
	 * for the same word with no condition or same condition, than the
	 * second one is removed.
	 */
	void removeRedundantRules();
};

#endif // _FL_GENERAL_
