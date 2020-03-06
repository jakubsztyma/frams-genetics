// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/util/extvalue.h>
#include <frams/util/sstring.h>
#include <stack>
#include "fL_matheval.h"
#include <frams/genetics/genooperators.h>

// Used available operators in MathEvaluation

double madd(double left, double right)
{
	return left + right;
}

double msub(double left, double right)
{
	return left - right;
}

double mmul(double left, double right)
{
	return left * right;
}

double mand(double left, double right)
{
	return left && right;
}

double mor(double left, double right)
{
	return left || right;
}

double mgreater(double left, double right)
{
	return left > right;
}

double mless(double left, double right)
{
	return left < right;
}

double mequal(double left, double right)
{
	return left == right;
}

double mnotequal(double left, double right)
{
	return left != right;
}

double meqless(double left, double right)
{
	return left <= right;
}

double meqgreater(double left, double right)
{
	return left >= right;
}

// Methods for converting evaluation tokens to string

std::string MathEvaluation::Number::toString()
{
	return std::string(SString::valueOf(value).c_str());
}

std::string MathEvaluation::Variable::toString()
{
	if (id == -1) return std::string(1, VARIABLEPREFIX) + "t";
	return std::string(1, VARIABLEPREFIX) + std::to_string(id);
}

std::string MathEvaluation::Parenthesis::toString()
{
	return ptype == ParenthesisType::OPEN ? "(" : ")";
}

std::string MathEvaluation::Operator::toString()
{
	return opname;
}

void MathEvaluation::Number::operator=(const Number& src)
{
	value = src.value;
}

double MathEvaluation::Operator::execute(double left, double right)
{
	return operation(left, right);
}

void MathEvaluation::registerOperator(double(*operation)(double left, double right), int precedence, MathEvaluation::Associativity assoc, std::string opsymbol)
{
	operators[opsymbol] = new Operator(operation, precedence, assoc, opsymbol);
	operatorstrings.push_back(opsymbol);
}

void MathEvaluation::registerOperators()
{
	// list of available operators in MathEvaluation
	registerOperator(madd, 2, Associativity::LEFT, "+");
	registerOperator(msub, 2, Associativity::LEFT, "-");
	registerOperator(mmul, 3, Associativity::LEFT, "*");
	registerOperator(mgreater, 1, Associativity::LEFT, ">");
	registerOperator(mless, 1, Associativity::LEFT, "<");
	registerOperator(meqgreater, 1, Associativity::LEFT, ">=");
	registerOperator(meqless, 1, Associativity::LEFT, "<=");
	registerOperator(mequal, 1, Associativity::RIGHT, "=");
	registerOperator(mnotequal, 1, Associativity::RIGHT, "<>");
	registerOperator(mand, 0, Associativity::LEFT, "&");
	registerOperator(mor, 0, Associativity::LEFT, "|");
	arithmeticoperatorscount = 3;
	comparisonoperatorscount = 6;
}

void MathEvaluation::clearPostfix()
{
	if (!postfixlist.empty())
	{
		for (Token *el : postfixlist)
		{
			// only numbers or parenthesis should be removed - operators and
			// variables are removed later during destruction of the
			// MathEvaluation object
			if (el->type == TokenType::NUMBER ||
				el->type == TokenType::PARENTHESIS)
				delete el;
		}
		postfixlist.clear();
	}
}

MathEvaluation::MathEvaluation(int varcount) : varcount(varcount), vars(varcount, NULL)
{
	registerOperators();
	for (int i = 0; i < varcount; i++)
	{
		vars[i] = new Variable(i);
	}
	t = new Variable(-1);
	t->value = 0;
}

MathEvaluation::~MathEvaluation()
{
	clearPostfix();

	std::unordered_map<std::string, Operator *>::iterator it;
	for (it = operators.begin(); it != operators.end(); it++)
	{
		delete it->second;
	}
	operators.clear();

	for (Variable *var : vars)
	{
		delete var;
	}
	vars.clear();
	delete t;
}

int MathEvaluation::convertString(std::string expression)
{
	originalexpression = expression;
	clearPostfix(); //clear previous objects
	ExtValue val;

	unsigned int it = 0;

	std::list<Token*> operatorstack; // stores operators for later

	bool canbeunary = true; // used to determine if -/+ operators should be unary
	while (it < expression.size())
	{
		bool parsenumber = false;
		int oplen = 2; // first method checks if 2-character operators match, then 1-character

		// if following letters describe variable
		if (expression[it] == VARIABLEPREFIX ||
			(canbeunary && strchr("+-", expression[it]) &&
			it + 1 < expression.size() &&
			expression[it + 1] == VARIABLEPREFIX))
		{
			// determine if before variable there is an unary operator '-'
			// if yes, the variable will be negated
			double mult = expression[it] == '-' ? -1 : 1;
			it += expression[it] == VARIABLEPREFIX ? 1 : 2;
			// if there is no space after '$' character, return error
			if (it >= expression.size())
			{
				std::string message = "Evaluation error: Invalid variable '";
				message += expression.substr(it) + "'";
				logMessage("MathEvaluation", "convertString", LOG_ERROR, message.c_str());
				while (!operatorstack.empty())
				{
					if (operatorstack.back()->type != TokenType::OPERATOR)
					{
						delete operatorstack.back();
					}
					operatorstack.pop_back();
				}
				return -1;
			}

			//determine id of variable
			const char *ptr = expression[it] == 't' ? expression.c_str() + it + 1 : val.parseNumber(expression.c_str() + it, ExtPType::TInt);
			int index = expression[it] == 't' ? -1 : val.getInt();
			bool invalidid = false;
			// if id is parsed properly
			if (ptr) // if current element is number, then add it to RPN
			{
				// if id is exceeding available amount of variables, then variable
				// is invalid
				if (index >= varcount)
				{
					invalidid = true;
				}
				else
				{
					Variable *v = index == -1 ? t : vars[index];
					// if variable had unary '-', then is multiplied by -1
					// multiplying value stored in variable would change value
					// in every occurrence of variable
					if (mult == -1)
					{
						postfixlist.push_back(new Number(-1));
						postfixlist.push_back(v);
						postfixlist.push_back(operators["*"]);
					}
					// push pointer to a variable in order to make evaluation
					// reusable for other values of variables
					else
					{
						postfixlist.push_back(v);
					}
					int offset = ptr - (expression.c_str() + it);
					it += offset;
					// after variable there cannot be unary operator
					canbeunary = false;
				}
			}
			// if parsing of variable went wrong, then return error message
			if (!ptr || invalidid)
			{
				std::string message = "Evaluation error: Invalid variable '";
				message += expression.substr(it) + "'";
				logMessage("MathEvaluation", "convertString", LOG_ERROR, message.c_str());
				while (!operatorstack.empty())
				{
					if (operatorstack.back()->type != TokenType::OPERATOR)
					{
						delete operatorstack.back();
					}
					operatorstack.pop_back();
				}
				return -1;
			}
		}
		// if current characters describe operators
		else if ((it + 1 < expression.size() && operators.find(expression.substr(it, oplen)) != operators.end())
			|| operators.find(expression.substr(it, --oplen)) != operators.end())
		{
			// if operator is + or - and from context it is known that it is
			// unary version of one of those operators, then move control to
			// number parsing
			if (canbeunary && strchr("+-", expression[it]) &&
				it + 1 < expression.size() && isdigit(expression[it + 1]))
			{
				parsenumber = true;
			}
			else
			{
				// otherwise prepare pointer to a given operator
				Operator *newop = operators[expression.substr(it, oplen)];

				bool finished = operatorstack.empty();

				// before operator will be put onto the operator stack, all operators
				// with higher precedence than the current operator should be popped
				// from the stack and added to postfix list
				while (!finished)
				{

					Token *curr = operatorstack.back();
					// when token in the operator stack is opened parenthesis,
					// then the process of taking operators from stack stops.
					if (curr->type == TokenType::PARENTHESIS)
					{
						Parenthesis *par = (Parenthesis *)curr;
						if (par->ptype == ParenthesisType::OPEN)
						{
							finished = true;
						}
					}
					else
					{
						// if current operator in stack is left-associated and
						// its precedence is equal or greater than precedence of
						// new operator than current operator is taken from stack
						// and added to postfixlist. If current operator is
						// right-associated, then its precedence must be strictly
						// higher than new operator precedence
						Operator *op = (Operator *)curr;
						if (newop->assoc == Associativity::LEFT && newop->precedence <= op->precedence)
						{
							postfixlist.push_back(op);
							operatorstack.pop_back();
						}
						else if (newop->assoc == Associativity::RIGHT && newop->precedence < op->precedence)
						{
							postfixlist.push_back(op);
							operatorstack.pop_back();
						}
						else
						{
							finished = true;
						}
					}
					finished = finished || operatorstack.empty();
				}
				// after operator there cannot be unary operator - wrap it in
				// parenthesis
				canbeunary = false;
				// add new operator to stack
				operatorstack.push_back(newop);
				it += oplen;
			}
		}
		else if (expression[it] == '(' || expression[it] == ')')
		{
			// if current character is open parenthesis, then add it to
			// the operator stack
			if (expression[it] == '(')
			{
				Parenthesis *par = new Parenthesis(ParenthesisType::OPEN);
				// after open parenthesis there can be unary operator
				canbeunary = true;
				operatorstack.push_back(par);
			}
			else
			{
				// if current character is closed parenthesis, then all operators
				// are popped from stack into postfix list until first open
				// parenthesis is found
				bool finpop = false;
				while (!finpop)
				{
					// if there is no more operators in the stack, then parenthesis
					// is mismatched
					if (operatorstack.empty())
					{
						logMessage("MathEvaluation", "convertString", LOG_ERROR, "Evaluation error: Parenthesis mismatch");
						return -1;
					}
					Token *curr = operatorstack.back();
					if (curr->type == TokenType::PARENTHESIS)
					{
						// if corresponding parenthesis is found, then opening
						// parenthesis is deleted and removed from stack - postfix
						// notation does not have parenthesis
						Parenthesis *par = (Parenthesis *)curr;
						if (par->ptype == ParenthesisType::OPEN)
						{
							delete par;
							operatorstack.pop_back();
							finpop = true;
						}
					}
					else
					{
						postfixlist.push_back(curr);
						operatorstack.pop_back();
					}
				}
				// after closed parenthesis unary operators does not exist
				canbeunary = false;
			}
			it++;
		}
		else if (isspace(expression[it])) // all whitespaces are skipped
		{
			it++;
		}
		else // if above conditions are not satisfied, then method assumes that
		{    // characters describe number
			parsenumber = true;
		}
		if (parsenumber)
		{
			// if parsing went successfully, then push number to postfix list
			const char *ptr = val.parseNumber(expression.c_str() + it, ExtPType::TDouble);
			if (ptr) // if current element is number, then add it to RPN
			{
				Number *num = new Number(val.getDouble());
				postfixlist.push_back(num);
				int offset = ptr - (expression.c_str() + it);
				it += offset;
				// There are no unary operators after number
				canbeunary = false;
			}
			else
			{
				// otherwise return error
				std::string message = "Evaluation error: Invalid letter '";
				message += expression[it];
				message += "'";
				logMessage("MathEvaluation", "convertString", LOG_ERROR, message.c_str());
				while (!operatorstack.empty())
				{
					if (operatorstack.back()->type != TokenType::OPERATOR)
					{
						delete operatorstack.back();
					}
					operatorstack.pop_back();
				}
				return -1;
			}
		}
	}

	// Pop remaining operators from stack and add them to the postfix list
	while (!operatorstack.empty())
	{
		Token *curr = operatorstack.back();
		if (curr->type == TokenType::OPERATOR)
		{
			postfixlist.push_back(curr);
			operatorstack.pop_back();
		}
		else
		{
			// if current token in the stack is open parenthesis, then mismatch
			// occured
			logMessage("MathEvaluation", "convertString", LOG_ERROR, "Evaluation error: Parenthesis mismatch");
			while (!operatorstack.empty())
			{
				if (operatorstack.back()->type != TokenType::OPERATOR)
				{
					delete operatorstack.back();
				}
				operatorstack.pop_back();
			}
			return -1;
		}
	}

	return 0;
}

int MathEvaluation::evaluateRPN(double &res)
{
	// stack holds number used during operator execution
	std::stack<Number *> numberstack;
	for (std::list<Token *>::iterator it = postfixlist.begin(); it != postfixlist.end(); it++)
	{
		Token *tok = (*it);
		// if token is number or variable - add it to number stack
		if (tok->type == TokenType::NUMBER || tok->type == TokenType::VARIABLE)
		{
			Number *num = new Number(((Number *)tok)->value);
			numberstack.push(num);
		}
		else
		{
			// the token is an operator
			Operator *op = (Operator *)tok;
			// if there isn't at least 2 elements in number stack, then return error
			if (numberstack.size() < 2)
			{
				logMessage("MathEvaluation", "convertString", LOG_ERROR, "Evaluation error: The math expression is not complete");
				while (!numberstack.empty())
				{
					if (numberstack.top()->type == TokenType::NUMBER) delete numberstack.top();
					numberstack.pop();
				}
				return -1;
			}
			// otherwise, pop two top elements from number stack and use them
			// with current operator
			double right = numberstack.top()->value;
			if (numberstack.top()->type == TokenType::NUMBER) delete numberstack.top();
			numberstack.pop();
			double left = numberstack.top()->value;
			if (numberstack.top()->type == TokenType::NUMBER) delete numberstack.top();
			numberstack.pop();
			double result = op->execute(left, right);
			Number *newnum = new Number(result);
			// Push operation result to number stack
			numberstack.push(newnum);
		}
	}

	// in the end of processing, only 1 element should be available in number stack.
	// Otherwise expression was not complete and error will be returned
	if (numberstack.size() != 1)
	{
		logMessage("MathEvaluation", "convertString", LOG_ERROR, "Evaluation error: The math expression is not complete");
		while (!numberstack.empty())
		{
			if (numberstack.top()->type == TokenType::NUMBER) delete numberstack.top();
			numberstack.pop();
		}
		return -1;
	}

	res = numberstack.top()->value;
	if (numberstack.top()->type == TokenType::NUMBER) delete numberstack.top();
	numberstack.pop();
	return 0;
}

void MathEvaluation::modifyVariable(int i, double val)
{
	if (i == -1)
	{
		t->value = val;
	}
	else
	{
		if (!vars[i])
		{
			vars[i] = new Variable(i);
		}
		vars[i]->value = val;
	}
}

int MathEvaluation::evaluate(std::string expression, double &result)
{
	int res = convertString(expression);
	if (res != 0) return res;
	return evaluateRPN(result);
}

int MathEvaluation::RPNToInfix(std::string &result)
{
	if (postfixlist.size() == 0)
	{
		result = "";
		return 0;
	}
	// stack holds stringified chunk and its precedence
	std::stack<std::pair<std::string, int>> chunks;
	// foreach token in postfix list
	for (Token *tok : postfixlist)
	{
		// if token is not an operator, push stringified variable or number
		// on top of stack
		if (tok->type != TokenType::OPERATOR)
		{
			// number or variable has no precedence
			chunks.push({ tok->toString(), -1 });
		}
		else // if token is an operator
		{
			Operator *op = (Operator *)tok;
			int prec = op->precedence;
			// if there are not at least two stringified chunks in stack, return error
			if (chunks.size() < 2)
			{
				logMessage("MathEvaluation", "RPNToInfix", LOG_ERROR,
					"Could not convert RPN notation to infix notation");
				return -1;
			}
			// pop first chunk
			std::pair<std::string, int> right = chunks.top();
			chunks.pop();
			if (right.second != -1 && right.second < op->precedence)
			{
				// if chunk on the right of operator has lower precedence than
				// this operator, then wrap it with parenthesis
				right.first = std::string(1, '(') + right.first + std::string(1, ')');
			}
			else if (right.second >= op->precedence)
			{
				prec = right.second;
			}
			std::pair<std::string, int> left = chunks.top();
			chunks.pop();
			if (left.second != -1 && left.second < op->precedence)
			{
				// if chunk on the left of operator has lower precedence than
				// this operator, then wrap it with parenthesis
				left.first = std::string(1, '(') + left.first + std::string(1, ')');
			}
			std::string res = left.first + op->toString() + right.first;
			// add summed chunk
			chunks.push({ res, prec });
		}
	}
	// if there is more than one chunk then return error
	if (chunks.size() != 1)
	{
		logMessage("MathEvaluation", "RPNToInfix", LOG_ERROR,
			"Could not convert RPN notation to infix notation - formula is not complete");
		return -1;
	}
	result = chunks.top().first;
	chunks.pop();
	return 0;
}

void MathEvaluation::mutateValueOrVariable(MathEvaluation::Number *&currval, bool usetime)
{
	if (rndUint(2) == 0 && varcount > 0) // use variable
	{
		if (currval && currval->type == TokenType::NUMBER)
		{
			delete currval;
		}
		int var = rndUint(varcount + (usetime ? 1 : 0));
		if (varcount == var) // time is used
		{
			currval = t;
		}
		else
		{
			currval = vars[var];
		}
	}
	else
	{
		if (!currval || currval->type == TokenType::VARIABLE)
		{
			currval = new Number(rndDouble(1));
		}
		else
		{
			currval->value = rndDouble(1);
		}
	}
}

MathEvaluation::Operator* MathEvaluation::getRandomOperator(int type)
{ // 0 for all, 1 for arithmetic only, 2 for logical only
	int randop = type == 2 ? arithmeticoperatorscount : 0;
	int count = arithmeticoperatorscount;
	if (type == 0)
	{
		count = operatorstrings.size();
	}
	else if (type == 2)
	{
		count = operatorstrings.size() - arithmeticoperatorscount;
	}
	randop += rndUint(count);
	return operators[operatorstrings[randop]];
}

void MathEvaluation::mutateConditional()
{
	if (varcount > 0)
	{
		int currsize = postfixlist.size();
		int varid = rndUint(varcount);
		postfixlist.push_back(vars[varid]);
		if (rndUint(2) == 0 && varcount > 1)
		{
			int varid2 = rndUint(varcount - 1);
			if (varid2 >= varid) varid2++;
			postfixlist.push_back(vars[varid2]);
		}
		else
		{
			Number *num = new Number(rndDouble(1));
			postfixlist.push_back(num);
		}
		int opid = arithmeticoperatorscount + rndUint(comparisonoperatorscount);
		postfixlist.push_back(operators[operatorstrings[opid]]);
		if (currsize > 0)
		{
			postfixlist.push_back(operators["&"]);
		}
	}
}

int MathEvaluation::mutate(bool logical, bool usetime)
{
	if (postfixlist.size() == 0)
	{
		Number *val = new Number(rndDouble(1));
		postfixlist.push_back(val);
		return -1;
	}
	int method = rndUint(postfixlist.size() < MAX_MUT_FORMULA_SIZE ? MATH_MUT_COUNT : MATH_MUT_COUNT - 1);
	switch (method)
	{
	case MATH_MUT_INSERTION:
	{
		std::list<Token *> insertpair;
		Number *val = NULL;
		mutateValueOrVariable(val, usetime);
		insertpair.push_back(val);
		std::list<Token *>::iterator it = postfixlist.begin();
		// insertion can be applied from 1st occurrence
		int insertlocation = 1 + rndUint(postfixlist.size() - 1);
		std::advance(it, insertlocation);
		Operator *rndop;
		if (insertlocation == (int)postfixlist.size() - 1)
		{
			rndop = getRandomOperator(logical ? 2 : 1);
		}
		else
		{
			rndop = getRandomOperator(logical ? 0 : 1);

		}
		insertpair.push_back(rndop);
		postfixlist.insert(it, insertpair.begin(), insertpair.end());
		break;
	}
	case MATH_MUT_CHANGEVAL:
	{
		std::vector<std::list<Token *>::iterator> numbersineval;
		int id = 0;
		for (std::list<Token *>::iterator it = postfixlist.begin(); it != postfixlist.end(); it++)
		{
			if ((*it)->type == TokenType::NUMBER || (*it)->type == TokenType::VARIABLE)
			{
				numbersineval.push_back(it);
			}
			id++;
		}
		int randid = rndUint(numbersineval.size());
		Number *numptr = (Number *)(*numbersineval[randid]);
		mutateValueOrVariable(numptr, usetime);
		(*numbersineval[randid]) = numptr;
		break;
	}
	case MATH_MUT_CHANGEOPER:
	{
		std::vector<std::list<Token *>::iterator> ops;
		for (std::list<Token *>::iterator it = postfixlist.begin(); it != postfixlist.end(); it++)
		{
			if ((*it)->type == TokenType::OPERATOR)
			{
				ops.push_back(it);
			}
		}
		if (ops.size() > 0)
		{
			int randid = rndUint(ops.size());
			Operator *rndop;
			if (randid == (int)ops.size() - 1)
			{
				rndop = getRandomOperator(logical ? 2 : 1);
			}
			else
			{
				rndop = getRandomOperator(logical ? 0 : 1);
			}
			(*ops[randid]) = rndop;
		}
		break;
	}
	case MATH_MUT_DELETION:
	{
		std::list<Token *>::iterator it = postfixlist.begin();
		std::vector<std::list<Token *>::iterator> firstofpairs;
		while (it != postfixlist.end())
		{
			if ((*it)->type == TokenType::NUMBER || (*it)->type == TokenType::VARIABLE)
			{
				std::list<Token *>::iterator next = it;
				next++;
				if (next != postfixlist.end() && (*next)->type == TokenType::OPERATOR)
				{
					firstofpairs.push_back(it);
				}
			}
			it++;
		}
		if (firstofpairs.size() > 0)
		{
			int rndid = rndUint(firstofpairs.size());
			if ((*firstofpairs[rndid])->type == TokenType::NUMBER)
			{
				delete (*firstofpairs[rndid]);
			}
			firstofpairs[rndid] = postfixlist.erase(firstofpairs[rndid]);
			postfixlist.erase(firstofpairs[rndid]);
		}
		break;
	}
	}
	return method;
}

std::string MathEvaluation::getStringifiedRPN()
{
	std::string res = "";
	for (Token *el: postfixlist)
	{
		res += el->toString();
		res += " ";
	}
	return res;
}
