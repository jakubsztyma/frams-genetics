// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <algorithm>
#include <stack>
#include "fL_general.h"
#include <frams/util/multirange.h>
#include <iterator>

const char *fL_part_names[FL_PART_PROPS_COUNT] = { "dn", "fr", "ing", "as" };
const char *fL_part_fullnames[FL_PART_PROPS_COUNT] = { "details", "friction", "ingestion", "assimilation" };

const char *fL_joint_names[FL_JOINT_PROPS_COUNT] = { "stif", "rotstif", "stam" };
const char *fL_joint_fullnames[FL_JOINT_PROPS_COUNT] = { "stiffness", "rotation stiffness", "stamina" };

#define FIELDSTRUCT fL_Word
ParamEntry fL_word_paramtab[] =
{
	{ "Word", 1, 2, "w" },
	{ "name", 0, PARAM_CANOMITNAME, "word name", "s", FIELD(name), },
	{ "npar", 0, PARAM_CANOMITNAME, "number of parameters", "d 0 " FL_MAXPARAMS " 0", FIELD(npar), },
	{ 0, 0, 0, }
};
#undef FIELDSTRUCT

#define FIELDSTRUCT fL_Rule
ParamEntry fL_rule_paramtab[] =
{
	{ "Rule", 1, 3, "r" },
	{ "pred", 0, 0, "predecessor", "s", FIELD(predecessor), },
	{ "cond", 0, 0, "parameter condition", "s", FIELD(condition), },
	{ "succ", 0, 0, "successor", "s", FIELD(successor), },
	{ 0, 0, 0, }
};
#undef FIELDSTRUCT

#define FIELDSTRUCT fL_Builder
ParamEntry fL_builder_paramtab[] =
{
		{"LSystemInfo", 1, 4, "i"},
		{"axiom", 0, 0, "starting sequence of L-System", "s", FIELD(axiom),},
		{"time", 0, PARAM_CANOMITNAME, "development time", "f 0.0 " FL_MAXITER " 1.0", FIELD(time),},
		{"numckp", 0, PARAM_CANOMITNAME, "number of checkpoints", "d 1 50 1", FIELD(numckp),},
		{"maxwords", 0, PARAM_CANOMITNAME, "Maximum number of words within genotype sequence", "d -1 9999 -1", FIELD(maxwords),},
		{0,0,0,}
};
#undef FIELDSTRUCT

fL_Builder::~fL_Builder()
{
	// first remove words from builder
	for (fL_Word *word : genotype)
	{
		delete word;
	}
	genotype.clear();
	// remove rules from builder
	for (fL_Rule *rule : rules)
	{
		delete rule;
	}
	rules.clear();

	// remove words definitions with their ParamTabs
	std::unordered_map<std::string, fL_Word *>::iterator it;
	for (it = words.begin(); it != words.end(); it++)
	{
		ParamObject::freeParamTab(it->second->tab);
		delete it->second;
	}
	words.clear();
}

bool fL_Builder::getNextObject(int &pos, const SString &src, SString &token)
{
	// if position exceeds length then return false
	if (pos >= src.len()) return false;
	int opencount = -1;
	int i = pos;
	for (; i < src.len(); i++)
	{
		// token cannot contain branching parenthesis
		if (src[i] == '[' || src[i] == ']')
		{
			// if token started - return parenthesis mismatch
			if (opencount != -1)
			{
				pos = -1;
				return false;
			}
			// otherwise [ and ] are interpreted as tokens and they do not have parenthesis
			token = src.substr(pos, i + 1 - pos);
			pos = i + 1;
			return true;
		}
		// every word, except [ and ], has () parenthesis
		// every open parenthesis increment opencount counter;
		if (src[i] == '(')
		{
			if (opencount == -1) opencount = 1;
			else opencount++;
		}
		// every close parenthesis decrement opencount counter
		else if (src[i] == ')')
		{
			// if there were no open parenthesis, return parenthesis mismatch
			if (opencount == -1)
			{
				pos = -1;
				return false;
			}
			else opencount--;
		}
		// if counter reaches 0, the token extraction is finished
		if (opencount == 0)
		{
			break;
		}
	}
	if (opencount == 0)
	{
		token = src.substr(pos, i + 1 - pos);
		pos = i + 1;
		return true;
	}
	// if there was no closing parenthesis, then return parenthesis mismatch
	pos = -1;
	return false;
}

std::string fL_Builder::trimSpaces(const std::string& data)
{
	size_t first = data.find_first_not_of(' ');
	if (std::string::npos == first)
	{
		return data;
	}
	size_t last = data.find_last_not_of(' ');
	return data.substr(first, (last - first + 1));
}

int fL_Builder::createWord(const SString &token, fL_Word *&word, int numparams, int begin, int end)
{
	SString wordn;
	int tokpos = 0;
	// if word name cannot be extracted, then return error
	if (!token.getNextToken(tokpos, wordn, '('))
	{
		return 1;
	}
	std::string wordname = fL_Builder::trimSpaces(wordn.c_str());
	// if word cannot be found in available words, then return error
	if (words.find(wordname) == words.end())
	{
		SString message = "Word '";
		message += wordname.c_str();
		message += "' in sequence does not exist";
		logMessage("fL_Builder", "createWord", LOG_ERROR, message.c_str());
		return 1;
	}

	if (word) delete word;
	// create new word and assign parameters
	word = new fL_Word(false, begin, end);

	*word = *words[wordname];

	SString temp;
	temp = token.substr(tokpos);
	temp = temp.substr(0, temp.len() - 1);

	// if word has parameters
	if (word->npar > 0)
	{
		// create ParamObject that will hold parameter data
		word->data = ParamObject::makeObject(word->tab);
		Param par(word->tab);
		par.select(word->data);
		par.setDefault();
		ParamInterface::LoadOptions opts;

		// load parameters from string
		par.load(ParamInterface::FormatSingleLine, temp, &opts);
		for (int i = 0; i < par.getPropCount(); i++)
		{
			SString t(par.id(i));
			if (word->builtin && (t == SString("d") || t == SString(FL_PE_CONN_ATTR)))
			{
				word->parevals.push_back(NULL);
			}
			else
			{
				// create MathEvaluation object to check if string contained by
				// parameter is valid
				double tmp;
				MathEvaluation *eval = NULL;
				SString seq = par.getString(i);
				// if string is empty, then evaluate this with 0
				// if sequence could not be evaluated, then return error
				if (seq.len() > 0)
				{
					eval = new MathEvaluation(numparams);
					if (eval->evaluate(seq.c_str(), tmp) != 0)
					{
						SString message = "Word in sequence has invalid parameter:  ";
						message += temp;
						logMessage("fL_Builder", "createWord", LOG_ERROR, message.c_str());
						delete eval;
						delete word;
						word = NULL;
						return 1;
					}
				}
				word->parevals.push_back(eval);
			}
		}
	}
	else if (word->npar == 0 && temp.len() > 0)
	{
		SString message = "Too many parameters for word:  ";
		message += token;
		logMessage("fL_Builder", "createWord", LOG_ERROR, message.c_str());
		delete word;
		word = NULL;
		return 1;
	}
	return 0;
}

int fL_Builder::tokenize(const SString &sequence, std::list<fL_Word *> &result, int numparams, int begin, int end)
{
	int pos = 0;
	SString token;
	int branchcount = 0;
	if (result.size() > 0)
	{
		for (fL_Word *word : result)
		{
			delete word;
		}
		result.clear();
	}
	// iterate through available tokens
	while (getNextObject(pos, sequence, token))
	{
		// if token is of open branch type, then add start of branch
		if (token.indexOf("[", 0) != -1)
		{
			fL_Branch *word = new fL_Branch(fL_Branch::BranchType::OPEN, begin, end);
			result.push_back(word);
			branchcount++;
			continue;
		}
		// if token is of closed branch type, then add end of branch
		if (token.indexOf("]", 0) != -1)
		{
			if (branchcount == 0)
			{
				SString message = "Branch parenthesis mismatch at:  ";
				message += sequence;
				logMessage("fL_Builder", "tokenize", LOG_ERROR, message.c_str());
				return 1;
			}
			fL_Branch *word = new fL_Branch(fL_Branch::BranchType::CLOSE, begin, end);
			result.push_back(word);
			branchcount--;
			continue;
		}
		fL_Word *word = NULL;
		if (createWord(token, word, numparams, begin, end) != 0)
		{
			SString message = "Error during parsing words sequence:  ";
			message += sequence;
			logMessage("fL_Builder", "tokenize", LOG_ERROR, message.c_str());
			return 1;
		}
		if (word->name == "C")
		{
			Param par(word->tab, word->data);
			SString attr = par.getStringById(FL_PE_CONN_ATTR);
			if (attr.indexOf("$t", 0) != -1)
			{
				logMessage("fL_Builder", "tokenize", LOG_ERROR, "Attractor definition cannot contain time variable");
				delete word;
				return 1;

			}
			if (attr != "")
			{
				fL_Word *attrword = NULL;
				if (createWord(attr, attrword, numparams, begin, end) != 0)
				{
					SString message = "Error during parsing attractor word:  ";
					message += attr;
					logMessage("fL_Builder", "tokenize", LOG_ERROR, message.c_str());
					delete word;
					if (attrword) delete attrword;
					return 1;
				}
				if (attrword->builtin)
				{
					logMessage("fL_Builder", "tokenize", LOG_ERROR, "Attractor words cannot be built-in");
					delete word;
					delete attrword;
					return 1;
				}
				delete attrword;
			}
		}
		result.push_back(word);
	}

	// check if there were no parenthesis errors in genotype
	if (pos == -1)
	{
		SString message = "Parenthesis mismatch at:  ";
		message += sequence;
		logMessage("fL_Builder", "tokenize", LOG_ERROR, message.c_str());
		return 1;
	}
	if (branchcount != 0)
	{
		SString message = "Branching mismatch at:  ";
		message += sequence;
		logMessage("fL_Builder", "tokenize", LOG_ERROR, message.c_str());
		return 1;
	}
	return 0;
}

void fL_Word::operator=(const fL_Word& src)
{
	if (&src != this)
	{
		name = src.name;
		npar = src.npar;

		//mut = src.mut;
		tab = src.tab;

		parevals = src.parevals;

		builtin = src.builtin;

		data = NULL; // properties cannot be copied
	}
}

int fL_Word::processDefinition(fL_Builder *builder)
{
	// if word already exist, then return error
	if (this->name.len() == 0)
	{
		logMessage("fL_Word", "processDefinition", LOG_ERROR, "Axiom name is empty");
		return 1;
	}
	if (builder->words.find(this->name.c_str()) != builder->words.end())
	{
		std::string message = "Word redefinition:  ";
		message += this->name.c_str();
		logMessage("fL_Word", "processDefinition", LOG_ERROR, message.c_str());
		return 1;
	}

	// create ParamTab for word
	for (int i = 0; i < npar; i++)
	{
		std::string n = "n";
		n += std::to_string(i);
		mut.addProperty(NULL, n.c_str(), LSYSTEM_PARAM_TYPE, n.c_str(), "", PARAM_CANOMITNAME, 0, -1);
	}

	tab = ParamObject::makeParamTab((ParamInterface *)&mut, 0, 0, mut.firstMutableIndex());

	builder->words[this->name.c_str()] = this;
	builder->wordnames.push_back(this->name.c_str());
	return 0;
}

int fL_Rule::processDefinition(fL_Builder *builder)
{
	// if there is no word among words that matches predecessor, then return error
	if (builder->words.find(predecessor.c_str()) == builder->words.end())
	{
		logMessage("fL_Rule", "processDefinition", LOG_ERROR, "Word in Rule condition does not exist");
		return 1;
	}

	objpred = new fL_Word();
	*objpred = *builder->words[predecessor.c_str()];

	if (objpred->builtin)
	{
		logMessage("fL_Rule", "processDefinition", LOG_ERROR, "Builtin words cannot be predecessors");
		return 1;
	}

	// parse condition
	if (condition != "")
	{
		if (objpred->builtin && (objpred->name == "N" || objpred->name == "C"))
		{
			logMessage("fL_Rule", "processDefinition", LOG_ERROR, "Rules with neuron/connection word predecessors cannot contain conditions");
			return 1;
		}
		std::string cond = condition.c_str();
		condeval = new MathEvaluation(objpred->npar);
		double tmp;
		if (condeval->evaluate(condition.c_str(), tmp) != 0)
		{
			SString message = "Parametric condition of rule invalid:  ";
			message += condition;
			logMessage("fL_Rule", "processDefinition", LOG_ERROR, message.c_str());
			return 1;
		}
	}

	// parse successor
	if (successor == "")
	{
		logMessage("fL_Rule", "processDefinition", LOG_ERROR, "Successor cannot be empty");
		return 1;
	}

	if (builder->tokenize(successor, objsucc, objpred->npar, begin, end) != 0)
	{
		logMessage("fL_Rule", "processDefinition", LOG_ERROR, "Unable to process successor sequence");
		return 1;
	}

	builder->rules.push_back(this);
	return 0;
}

int fL_Builder::processDefinition(fL_Builder *builder)
{
	// tokenize axiom
	if (tokenize(axiom, genotype, 0, begin, end) != 0)
	{
		logMessage("fL_Builder", "processDefinition", LOG_ERROR, "Unable to process axiom sequence");
		return 1;
	}
	else if (genotype.size() == 0)
	{
		logMessage("fL_Builder", "processDefinition", LOG_ERROR, "Axiom sequence is empty");
		return 1;
	}
	return 0;
}

int fL_Builder::processLine(fLElementType type, const SString &line, fL_Element *&obj, int linenumber, int begin, int end)
{
	ParamEntry *tab;
	// choose proper ParamTab and construct proper object
	switch (type)
	{
		case fLElementType::TERM:
		{
			tab = fL_word_paramtab;
			obj = new fL_Word();
			break;
		}
		case fLElementType::INFO:
		{
			tab = fL_builder_paramtab;
			obj = this;
			break;
		}
		case fLElementType::RULE:
		{
			tab = fL_rule_paramtab;
			obj = new fL_Rule(begin, end);
			break;
		}
		default:
			tab = NULL;
			obj = NULL;
			break;
	}
	Param par(tab);
	par.select(obj);
	par.setDefault();
	ParamInterface::LoadOptions opts;

	par.load(ParamInterface::FormatSingleLine, line, &opts);

	if (opts.parse_failed)
	{
		std::string message = "Error in parsing parameters at line:  " + std::to_string(linenumber);
		logMessage("fL_Builder", "processLine", LOG_ERROR, message.c_str());
		if (obj != this) delete obj;
		return begin + 1;
	}

	return 0;
}

void fL_Builder::addModelWords()
{
	// stick S
	fL_Word *stick = new fL_Word(true);
	stick->name = "S";
	stick->npar = 8;
	for (int i = 0; i < FL_PART_PROPS_COUNT; i++)
	{
		stick->mut.addProperty(NULL, fL_part_names[i], "s", fL_part_fullnames[i], fL_part_fullnames[i], PARAM_CANOMITNAME, 0, -1);
	}

	for (int i = 0; i < FL_JOINT_PROPS_COUNT; i++)
	{
		stick->mut.addProperty(NULL, fL_joint_names[i], "s", fL_joint_fullnames[i], fL_joint_fullnames[i], PARAM_CANOMITNAME, 0, -1);
	}

	stick->mut.addProperty(NULL, "l", "s", "length", "length", PARAM_CANOMITNAME, 0, -1);
	stick->tab = ParamObject::makeParamTab((ParamInterface *)&stick->mut, 0, 0, stick->mut.firstMutableIndex());
	words["S"] = stick;
	wordnames.push_back("S");

	// neuron N
	fL_Word *neuron = new fL_Word(true);
	neuron->name = "N";
	neuron->npar = 1;
	neuron->mut.addProperty(NULL, "d", "s", "details", "details", 0, 0, -1);
	neuron->tab = ParamObject::makeParamTab((ParamInterface *)&neuron->mut, 0, 0, neuron->mut.firstMutableIndex());
	words["N"] = neuron;
	wordnames.push_back("N");

	// connection C
	fL_Word *connection = new fL_Word(true);
	connection->name = "C";
	connection->npar = 2;
	connection->mut.addProperty(NULL, FL_PE_CONN_WEIGHT, "s", "weight", "weight", PARAM_CANOMITNAME, 0, -1);
	connection->mut.addProperty(NULL, FL_PE_CONN_ATTR, "s", "attractor", "connection attractor", PARAM_CANOMITNAME, 0, -1);
	connection->tab = ParamObject::makeParamTab((ParamInterface *)&connection->mut, 0, 0, connection->mut.firstMutableIndex());
	words["C"] = connection;
	wordnames.push_back("C");

	// rotation objects
	fL_Word *rotx = new fL_Word(true);
	rotx->name = "rotX";
	rotx->npar = 1;
	rotx->processDefinition(this);

	fL_Word *roty = new fL_Word(true);
	roty->name = "rotY";
	roty->npar = 1;
	roty->processDefinition(this);

	fL_Word *rotz = new fL_Word(true);
	rotz->name = "rotZ";
	rotz->npar = 1;
	rotz->processDefinition(this);

	//fL_Branch *branch = new fL_Branch(fL_Branch::BranchType::OPEN, 0, 0);
	//branch->processDefinition(this);

	builtincount = words.size();
}

int fL_Builder::parseGenotype(const SString &genotype)
{
	int pos = 0;
	int lastpos = 0;
	SString line;
	int linenumber = 0;

	fLElementType type = fLElementType::TERM;

	// add default words first to prevent redefinitions
	addModelWords();

	while (genotype.getNextToken(pos, line, '\n'))
	{
		if (line.len() > 0)
		{
			// words can be defined in the beginning of genotype
			if (line.startsWith("w:") && type != fLElementType::TERM)
			{
				logMessage("fL_Builder", "parseGenotype", LOG_ERROR, "All words should be defined in the beginning of genotype");
				return lastpos + 1;
			}
			else if (line.startsWith("i:"))
			{
				// after all words are defined, next definition should be information
				if (type == fLElementType::TERM)
				{
					type = fLElementType::INFO;
				}
				else
				{
					logMessage("fL_Builder", "parseGenotype", LOG_ERROR, "Axioms and iteration number should be defined after word definitions");
					return lastpos + 1;
				}
			}
			else if (line.startsWith("r:"))
			{
				// after information definition, the last thing is rule definitions
				if (type == fLElementType::TERM)
				{
					logMessage("fL_Builder", "parseGenotype", LOG_ERROR, "Axiom is not defined - define it after words definition");
					return lastpos + 1;
				}
				else if (type == fLElementType::INFO)
				{
					type = fLElementType::RULE;
				}
			}
			// create object
			fL_Element *obj = NULL;
			int res = processLine(type, line.substr(2), obj, linenumber, lastpos, pos - 1);
			if (res != 0)
			{
				if (obj && obj != this) delete obj;
				return res;
			}
			if (obj == this)
			{
				begin = lastpos;
				end = pos - 1;
			}
			res = obj->processDefinition(this);
			if (res != 0)
			{
				if (obj && obj != this) delete obj;
				return res;
			}
		}
		lastpos = pos;
	}
	if (type == fLElementType::TERM)
	{
		logMessage("fL_Builder", "parseGenotype", LOG_ERROR, "Info line was not declared");
		return 1;
	}
	return 0;
}

int fL_Word::saveEvals(bool keepformulas)
{
	if (npar > 0)
	{
		Param par(tab);
		par.select(data);
		for (int i = 0; i < npar; i++)
		{
			SString t(par.id(i));
			if (parevals[i] != NULL)
			{
				double val;
				if (parevals[i]->evaluateRPN(val) != 0)
				{
					logMessage("fL_Word", "saveEvals", LOG_ERROR, "Could not stringify mathematical expression in Word");
					return 1;
				}
				if (val == 0)
				{
					par.setString(i, "");
				}
				else
				{
					if (keepformulas)
					{
						std::string res;
						if (parevals[i]->RPNToInfix(res) != 0)
						{
							logMessage("fL_Word", "saveEvals", LOG_ERROR, "Could not stringify mathematical expression in Word");
							return 1;
						}
						par.setString(i, res.c_str());
					}
					else
					{
						SString r = SString::valueOf(val);
						par.setString(i, r);
					}
				}
			}
		}
	}
	return 0;
}

// Methods for converting L-System objects to string

SString fL_Word::toString()
{
	Param par(fL_word_paramtab);
	fL_Word *obj = new fL_Word();
	par.select(this);
	SString res;
	par.saveSingleLine(res, obj, true, false);
	res = SString("w:") + res;
	delete obj;
	return res;
}

SString fL_Word::stringify(bool keepformulas)
{
	SString res = name;
	SString params = "";
	if (npar > 0)
	{
		saveEvals(keepformulas);
		Param par(tab);
		void *obj = ParamObject::makeObject(tab);
		par.select(obj);
		par.setDefault();
		par.select(data);
		par.saveSingleLine(params, obj, false, false);
		ParamObject::freeObject(obj);
	}
	res += "(";
	res += params + ")";
	return res;
}

SString fL_Rule::toString()
{
	predecessor = objpred->name;
	std::string tmp;
	if (condeval)
	{
		condeval->RPNToInfix(tmp);
		condition = tmp.c_str();
	}
	else
	{
		condition = "";
	}
	successor = "";
	std::list<fL_Word *>::iterator i;
	for (i = objsucc.begin(); i != objsucc.end(); i++)
	{
		successor += (*i)->stringify();
	}
	Param par(fL_rule_paramtab);
	fL_Rule *obj = new fL_Rule(0, 0);
	par.select(this);
	SString res;
	par.saveSingleLine(res, obj, true, false);
	res = SString("r:") + res;
	delete obj;
	return res;
}

SString fL_Builder::getStringifiedProducts()
{
	axiom = "";
	std::list<fL_Word *>::iterator i;
	for (i = genotype.begin(); i != genotype.end(); i++)
	{
		axiom += (*i)->stringify(false);
	}
	return axiom;
}

SString fL_Builder::toString()
{
	SString res;
	for (std::unordered_map<std::string, fL_Word *>::iterator it = words.begin(); it != words.end(); it++)
	{
		if (!it->second->builtin)
		{
			res += it->second->toString();
		}
	}
	getStringifiedProducts();
	removeRedundantRules();
	Param par(fL_builder_paramtab);
	fL_Builder *obj = new fL_Builder();
	par.select(this);
	SString tmp;
	par.saveSingleLine(tmp, obj, true, false);
	res += SString("i:") + tmp;
	delete obj;
	for (fL_Rule * rule : rules)
	{
		res += rule->toString();
	}
	return res;
}

int fL_Rule::deploy(fL_Builder *builder, fL_Word *in, std::list<fL_Word *>::iterator &it, double currtime)
{
	// if predecessor and given word differ, then rule is not applicable
	if (in->name != objpred->name || in->npar != objpred->npar)
	{
		return 1;
	}
	// store predecessor values in separate array
	double *inwordvalues = new double[in->npar];
	for (int i = 0; i < in->npar; i++)
	{
		if (in->parevals[i] != NULL)
		{
			in->parevals[i]->modifyVariable(-1, currtime == in->creationiter + 1.0 ? 1.0 : currtime - floor(currtime));
			in->parevals[i]->evaluateRPN(inwordvalues[i]);
		}
		else
		{
			inwordvalues[i] = 0;
		}
	}
	// if condition exists
	if (condeval)
	{
		// check if condition is satisfied. If not, rule is not applicable
		for (int i = 0; i < in->npar; i++)
		{
			condeval->modifyVariable(i, inwordvalues[i]);
		}
		double condvalue;
		condeval->evaluateRPN(condvalue);
		if (condvalue == 0)
		{
			delete[] inwordvalues;
			return 1;
		}
	}

	// remove predecessor word from genotype and replace it with successor
	it = builder->genotype.erase(it);
	for (std::list<fL_Word *>::iterator word = objsucc.begin(); word != objsucc.end(); word++)
	{
		// create new word and copy properties from word definition
		fL_Word *nword = new fL_Word(false, begin, end);
		*nword = **word;
		// store information about when word has been created
		nword->creationiter = currtime;
		nword->parevals.clear();
		if (nword->npar > 0)
		{
			nword->data = ParamObject::makeObject(nword->tab);
		}
		// calculate word parameters and store MathEvaluation objects for further
		// time manipulations.
		Param par((*word)->tab, (*word)->data);
		Param npar(nword->tab, nword->data);
		for (int q = 0; q < nword->npar; q++)
		{
			if ((*word)->parevals[q] == NULL)
			{
				if ((*word)->builtin && (strcmp(npar.id(q), "d") == 0))
				{
					SString t = par.getString(q);
					npar.setString(q, t);
					nword->parevals.push_back(NULL);
				}
				if ((*word)->builtin && (strcmp(npar.id(q), FL_PE_CONN_ATTR) == 0))
				{
					SString t = par.getString(q);
					if (t.len() > 0)
					{
						fL_Word *attrword = NULL;
						builder->createWord(t, attrword, in->npar, begin, end);
						for (int j = 0; j < attrword->npar; j++)
						{
							if (attrword->parevals[j])
							{
								for (int i = 0; i < in->npar; i++)
								{
									attrword->parevals[j]->modifyVariable(i, inwordvalues[i]);
								}
							}
						}
						SString res = attrword->stringify(false);
						npar.setString(q, res);
						nword->parevals.push_back(NULL);
						delete attrword;
					}
				}
				else
				{
					//MathEvaluation *ev = new MathEvaluation(0);
					//ev->convertString("0");
					//nword->parevals.push_back(ev);
					nword->parevals.push_back(NULL);
				}
			}
			else
			{
				std::string tmp;
				(*word)->parevals[q]->RPNToInfix(tmp);
				MathEvaluation *ev = new MathEvaluation(in->npar);
				for (int i = 0; i < in->npar; i++)
				{
					ev->modifyVariable(i, inwordvalues[i]);
				}
				ev->modifyVariable(-1, currtime == (*word)->creationiter + 1.0 ? 1.0 : currtime - floor(currtime));
				ev->convertString(tmp);
				nword->parevals.push_back(ev);
			}
		}
		builder->genotype.insert(it, nword);
	}
	delete[] inwordvalues;
	delete in;
	return 0;
}

int fL_Builder::iterate(double currtime)
{
	// deploy proper rules for all words in current genotype
	std::list<fL_Word *>::iterator word = genotype.begin();
	while (word != genotype.end())
	{
		bool deployed = false;
		for (fL_Rule * rule : rules)
		{
			if (rule->deploy(this, (*word), word, currtime) == 0)
			{
				deployed = true;
				break;
			}
		}
		if (!deployed) word++;
	}
	return 0;
}

int fL_Builder::alterTimedProperties(double currtime)
{
	// alter parameters of all words, if they are time-dependent
	std::list<fL_Word *>::iterator word = genotype.begin();
	while (word != genotype.end())
	{
		if (currtime - (*word)->creationiter <= 1.0)
		{
			for (MathEvaluation *ev : (*word)->parevals)
			{
				if (ev) ev->modifyVariable(-1, currtime == (*word)->creationiter + 1.0 ? 1.0 : currtime - floor(currtime));
			}
		}
		word++;
	}
	return 0;
}

int fL_Builder::alterPartProperties(Part *part, fL_Word *stickword, double &alterationcount)
{
	Param par(stickword->tab, stickword->data);
	Param ppar = part->properties();
	for (int i = 0; i < FL_PART_PROPS_COUNT; i++)
	{
		double mn, mx, df;
		ppar.getMinMaxDouble(ppar.findId(fL_part_names[i]), mn, mx, df);
		double currval;
		if (!stickword->parevals[i])
		{
			currval = df;
		}
		else
		{
			stickword->parevals[i]->evaluateRPN(currval);
			currval = sigmoidTransform(currval, mn, mx);
		}
		double partprop = (ppar.getDoubleById(fL_part_names[i]) * alterationcount +
				currval) / (alterationcount + 1.0);
		ppar.setDoubleById(fL_part_names[i], partprop);
	}
	return 0;
}

double fL_Word::distance(fL_Word *right)
{
	if (name != right->name || npar != right->npar)
	{
		return -1;
	}
	double distance = 0;
	for (int i = 0; i < npar; i++)
	{
		double l = 0;
		double r = 0;
		if (parevals[i]) parevals[i]->evaluateRPN(l);
		if (right->parevals[i]) right->parevals[i]->evaluateRPN(r);
		distance += (l - r) * (l - r);
	}
	return sqrt(distance);
}

Neuro *fL_Builder::findInputNeuron(std::pair<std::list<fL_Word *>::iterator, Neuro *> currneu, fL_Word *attractor)
{
	if (!attractor)
	{
		std::list<fL_Word *>::reverse_iterator riter(currneu.first);
		std::list<fL_Word *>::iterator iter(currneu.first);
		iter++;
		while (riter != genotype.rend() || iter != genotype.end())
		{
			if (iter != genotype.end())
			{
				if ((*iter)->name == "N" && (*iter)->bodyelementpointer != currneu.second)
				{
					return (Neuro *)(*iter)->bodyelementpointer;
				}
				iter++;
			}
			if (riter != genotype.rend())
			{
				if ((*riter)->name == "N" && (*riter)->bodyelementpointer != currneu.second)
				{
					return (Neuro *)(*riter)->bodyelementpointer;
				}
				riter++;
			}
		}
		return NULL;
	}
	else
	{
		double mindistance = -1;
		std::list<fL_Word *>::iterator minit = genotype.end();
		for (std::list<fL_Word *>::iterator it = genotype.begin(); it != genotype.end(); it++)
		{
			double currdist = attractor->distance((*it));
			if (currdist != -1 && (currdist < mindistance || mindistance == -1))
			{
				mindistance = currdist;
				minit = it;
			}
		}
		if (minit != genotype.end())
		{
			for (; minit != genotype.end(); minit++)
			{
				if ((*minit)->name == "N" && (*minit)->bodyelementpointer)
				{
					Neuro *n = (Neuro *)(*minit)->bodyelementpointer;
					if (n->getClass()->getPreferredOutput() != 0)
					{
						return n;
					}
				}
			}
		}
	}
	return NULL;
}

double fL_Builder::sigmoidTransform(double input, double mn, double mx)
{
	return mn + (mx - mn) * (1.0 / (1.0 + exp(-input)));
}

int fL_Builder::buildModelFromSequence(Model *model)
{
	fL_State currstate;
	std::unordered_map<Part *, double> counters;
	std::stack<fL_State> statestack;
	std::vector<std::pair<std::list<fL_Word *>::iterator, Neuro *>> connsbuffer;
	Part *firstpart = NULL;

	for (std::list<fL_Word *>::iterator w = genotype.begin(); w != genotype.end(); w++)
	{
		fL_Word *word = (*w);
		if (word->builtin)
		{
			if (word->name == "S")
			{
				if (!currstate.currpart)
				{
					if (!firstpart)
					{
						firstpart = new Part();
						firstpart->p = Pt3D_0;
						counters[firstpart] = 0;
						model->addPart(firstpart);
						if (using_mapping) firstpart->addMapping(IRange(word->begin, word->end));
					}
					currstate.currpart = firstpart;
				}
				if (alterPartProperties(currstate.currpart, word, counters[currstate.currpart]) != 0)
				{
					return 1;
				}
				counters[currstate.currpart] += 1;
				Part *newpart = new Part();
				counters[newpart] = 0;
				if (alterPartProperties(newpart, word, counters[newpart]) != 0)
				{
					delete newpart;
					return 1;
				}
				Param par(word->tab, word->data);
				double length;
				if (!word->parevals[FL_PART_PROPS_COUNT + FL_JOINT_PROPS_COUNT])
				{
					length = FL_DEFAULT_LENGTH; // default length value
				}
				else
				{
					double parsedval = 0.0;
					if (word->parevals[FL_PART_PROPS_COUNT + FL_JOINT_PROPS_COUNT]->evaluateRPN(parsedval) != 0)
					{
						delete newpart;
						logMessage("fL_Builder", "developModel", LOG_ERROR,
								"Error parsing word parameter");
						return 1;
					}
					length = sigmoidTransform(parsedval, FL_MINIMAL_LENGTH, FL_MAXIMAL_LENGTH);
				}
				newpart->p = currstate.currpart->p + currstate.direction * length;
				counters[newpart] += 1;
				model->addPart(newpart);
				if (using_mapping) newpart->addMapping(IRange(word->begin, word->end));
				Joint *newjoint = new Joint();
				newjoint->attachToParts(currstate.currpart, newpart);

				Param jpar = newjoint->properties();
				for (int i = 0; i < FL_JOINT_PROPS_COUNT; i++)
				{
					double mn, mx, df;
					jpar.getMinMaxDouble(jpar.findId(fL_joint_names[i]), mn, mx, df);
					double jointprop;
					if (!word->parevals[FL_PART_PROPS_COUNT + i])
					{
						jointprop = df; // assign default value
					}
					else
					{
						if (word->parevals[FL_PART_PROPS_COUNT + i]->evaluateRPN(jointprop) != 0)
						{
							logMessage("fL_Builder", "developModel", LOG_ERROR,
									"Error parsing word parameter");
							delete newjoint;
							return 1;
						}
						jointprop = sigmoidTransform(jointprop, mn, mx);
					}
					jpar.setDoubleById(fL_joint_names[i], jointprop);
				}
				model->addJoint(newjoint);
				if (using_mapping) newjoint->addMapping(IRange(word->begin, word->end));
				currstate.currpart = newpart;
			}
			else if (word->name == "N")
			{
				Param npar(word->tab, word->data);
				Neuro *neu = new Neuro();
				SString details = npar.getStringById("d");
				if (details == "")
				{
					details = "N";
				}
				neu->setDetails(details);
				if (!neu->getClass())
				{
					logMessage("fL_Builder", "developModel", LOG_ERROR, "Error parsing neuron class");
					delete neu;
					return 1;
				}
				model->addNeuro(neu);
				if (using_mapping) neu->addMapping(IRange(word->begin, word->end));
				if (neu->getClass()->getPreferredInputs() != 0)
				{
					currstate.currneuron = neu;
				}
				word->bodyelementpointer = neu;
			}
			else if (word->name == "C")
			{
				connsbuffer.push_back({w, currstate.currneuron});
			}
			else if (word->name.startsWith("rot"))
			{
				Orient rotmatrix = Orient_1;
				double rot;
				if (!word->parevals[0])
				{
					rot = 0;
				}
				else if (word->parevals[0]->evaluateRPN(rot) != 0)
				{
					logMessage("fL_Builder", "developModel", LOG_ERROR, "Error parsing rotation word");
					return 1;
				}

				rot = sigmoidTransform(rot, -M_PI, M_PI);

				if (word->name == "rotX")
				{
					rotmatrix.rotate(Pt3D(rot,0,0));
				}
				else if (word->name == "rotY")
				{
					rotmatrix.rotate(Pt3D(0,rot,0));
				}
				else if (word->name == "rotZ")
				{
					rotmatrix.rotate(Pt3D(0,0,rot));
				}
				currstate.direction = rotmatrix.transform(currstate.direction);
				currstate.direction.normalize();
			}
			else if (word->name == "[")
			{
				statestack.push(currstate);
			}
			else if (word->name == "]")
			{
				currstate = statestack.top();
				statestack.pop();
			}
		}
	}

	// connections need
	// std::pair<std::list<fL_Word *>::iterator, Neuro *> conndata : connsbuffer
	for (unsigned int i = 0; i < connsbuffer.size(); i++)
	{
		if (connsbuffer[i].second == NULL ||
				(connsbuffer[i].second->getClass()->getPreferredInputs() != -1 &&
				connsbuffer[i].second->getInputCount() >=
				connsbuffer[i].second->getClass()->getPreferredInputs()))
		{
			// since connections are separated entities from neurons, it may happen
			// that there will be no neuron to connect to
			// logMessage("fL_Builder", "developModel", LOG_DEBUG, "Connection could not be established");
		}
		else
		{
			Param par((*connsbuffer[i].first)->tab, (*connsbuffer[i].first)->data);
			SString attr = par.getStringById(FL_PE_CONN_ATTR);
			fL_Word *attractor = NULL;
			if (attr.len() > 0)
			{
				createWord(attr, attractor, 0, (*connsbuffer[i].first)->begin, (*connsbuffer[i].first)->end);
			}
			Neuro *neu = findInputNeuron(connsbuffer[i], attractor);
			double weight = 0.0;
			if ((*connsbuffer[i].first)->parevals[0])
			{
				if ((*connsbuffer[i].first)->parevals[0]->evaluateRPN(weight) != 0)
				{
					logMessage("fL_Builder", "developModel", LOG_ERROR,
							"Error parsing word parameter");
					delete attractor;
					return 1;
				}
			}
			if (neu)
			{
				connsbuffer[i].second->addInput(neu, weight);
				if (using_mapping) neu->addMapping(
						IRange((*connsbuffer[i].first)->begin,
								(*connsbuffer[i].first)->end));
			}
			else
			{
				connsbuffer[i].second->addInput(connsbuffer[i].second, weight);
				if (using_mapping) neu->addMapping(
						IRange((*connsbuffer[i].first)->begin,
								(*connsbuffer[i].first)->end));
			}
			delete attractor;
		}
	}
	return 0;
}

void fL_Builder::clearModelElements(Model *m)
{
	for (int i = 0; i < m->getJointCount(); i++)
	{
		m->removeJoint(i, 0);
	}
	for (int i = 0; i < m->getNeuroCount(); i++)
	{
		m->removeNeuro(i, true);
	}
	for (int i = 0; i < m->getNeuroCount(); i++)
	{
		m->removePart(i, 0, 0);
	}
	m->clearMap();
}

Model* fL_Builder::developModel(double &neededtime)
{
	double curriter = 0;
	double timestamp = 1.0 / numckp;
	double t = 0;
	Model *m = new Model();
	m->open(using_checkpoints);
	bool wordsexceeded = false;
	for (; t <= time; t+= timestamp)
	{
		alterTimedProperties(t); // always alter timed properties in the beginning
		// if iteration exceeds integer value, then deploy rules
		if (floor(t) > curriter)
		{
			iterate(t);
			curriter+=1.0;
		}
		if (using_checkpoints)
		{
			clearModelElements(m);
			if (buildModelFromSequence(m) != 0)
			{
				delete m;
				return NULL;
			}
			m->checkpoint();
		}
		if (maxwords != -1 && ((int)genotype.size()) > maxwords)
		{
			wordsexceeded = true;
			break;
		}
	}

	if (wordsexceeded)
	{
		neededtime = t;
	}
	else
	{
		neededtime = time;
	}

	// if exact time of development was not reached due to floating point errors,
	// then alter timed properties
	if (time < t)
	{
		alterTimedProperties(time);
	}
	clearModelElements(m);
	if (buildModelFromSequence(m) != 0)
	{
		delete m;
		return NULL;
	}
	if (using_checkpoints)
	{
		m->checkpoint();
	}
	m->close();
	return m;
}

int fL_Builder::countSticksInSequence(std::list<fL_Word *> *sequence)
{
	int count = 0;
	for (std::list<fL_Word *>::iterator it = sequence->begin(); it != sequence->end(); it++)
	{
		if ((*it)->builtin && (*it)->name == "S")
		{
			count++;
		}
	}
	return count;
}

int fL_Builder::countDefinedWords()
{
	return words.size() - builtincount;
}

int fL_Builder::countWordsInLSystem()
{
	int count = genotype.size();
	for (fL_Rule *rul: rules)
	{
		count += rul->objsucc.size();
	}
	count += words.size();
	return count;
}

void fL_Builder::removeRedundantRules()
{
	for (std::vector<fL_Rule *>::iterator it = rules.begin();
			it != rules.end(); it++)
	{
		std::vector<fL_Rule *>::iterator it2 = it;
		it2++;
		while (it2 != rules.end())
		{
			bool todelete = false;
			if ((*it)->objpred->name == (*it2)->objpred->name)
			{
				if ((*it)->condeval == NULL && (*it2)->condeval == NULL)
				{
					todelete = true;
				}
				else if ((*it)->condeval == NULL && (*it2)->condeval != NULL)
				{
					std::iter_swap(it, it2);
				}
				else if ((*it)->condeval != NULL && (*it2)->condeval != NULL)
				{
					if ((*it)->condeval->getStringifiedRPN() ==
							(*it2)->condeval->getStringifiedRPN())
					{
						todelete = true;
					}
				}
			}
			if (todelete)
			{
				delete (*it2);
				it2 = rules.erase(it2);
			}
			else
			{
				it2++;
			}
		}
	}
}
