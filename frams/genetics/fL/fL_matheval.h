// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _FL_MATHEVAL_H_
#define _FL_MATHEVAL_H_

#include <string>
#include <unordered_map>
#include <list>
#include <vector>

#define VARIABLEPREFIX '$'

#define MATH_MUT_DELETION   0
#define MATH_MUT_CHANGEVAL  1
#define MATH_MUT_CHANGEOPER 2
#define MATH_MUT_INSERTION  3
#define MATH_MUT_COUNT      4

#define MAX_MUT_FORMULA_SIZE 7
#define XOVER_MAX_MIGRATED_RULES 3

/** @name Available operators for MathEvaluation */
//@{
double madd(double left, double right); ///<add
double msub(double left, double right); ///<subtract
double mmul(double left, double right); ///<multiply
double mand(double left, double right); ///<and
double mor(double left, double right); ///<or
double mgreater(double left, double right); ///<greater than
double mless(double left, double right); ///<less than
double mequal(double left, double right); ///<equal to
double mnotequal(double left, double right); ///<not equal to
double meqless(double left, double right); ///<less or equal
double meqgreater(double left, double right); ///<greater or equal
//@}

/**
 * Class for performing simple mathematical parsing and evaluations of expressions
 * stored in strings. At first, the class object converts given string into list
 * of objects that can be described as Reverse Polish Notation, or suffix notation.
 * From this notation object can efficiently perform calculations for different
 * values of variables, without re-tokenizing of a string.
 */
class MathEvaluation
{
	/// determines to whih class token belongs
	enum TokenType {
		NUMBER, ///<Number token
		PARENTHESIS, ///<Parenthesis token
		OPERATOR,  ///<Operator token
		VARIABLE ///<Variable token
	};
	/// associativity of operator
	enum Associativity {
		RIGHT,
		LEFT
	};
	/// determines if parenthesis is opening of closing
	enum ParenthesisType {
		OPEN,
		CLOSE
	};

	/**
	 * Simplest token of MathEvaluation Reverse Polish Notation. Every class used
	 * in RPN notation should inherit from this class.
	 */
	class Token
	{
	public:
		TokenType type; ///<type of token
		/**
		 * Method is used during converting list of token objects into infix
		 * notation. Every class should implement this method and provide useful
		 * and valid information for infix notation.
		 * @return stringified token
		 */
		virtual std::string toString() = 0;
		Token(TokenType type) : type(type) {}
		virtual ~Token() {}
	};

	/**
	 * Describes all numbers available in object infix notation.
	 */
	class Number : public Token
	{
	public:
		double value; ///<value of stored number
		Number(double value) : Token(TokenType::NUMBER), value(value)
		{ }
		~Number() {}
		std::string toString();
		/**
		 * Copies value field from given source.
		 * @param src other Number object
		 */
		void operator=(const Number& src);
	};

	/**
	 * Special type of Number object, which holds external variables, represented
	 * as $id, where id is a number.
	 */
	class Variable : public Number
	{
	public:
		int id; ///<id of Variable object. Object is accessed via $id
		Variable(int id) : Number(0), id(id)
		{
			type = TokenType::VARIABLE;
		}
		~Variable() {}
		std::string toString();
	};

	/**
	 * Class describing operator in RPN objects. It holds informations about
	 * precedence, associativity and symbol of the operator. Operator can have 1
	 * or 2 characters, not more or less. Operator requires function that takes
	 * two doubles and returns double value -- the result of this function is the
	 * result of operator execution.
	 */
	class Operator : public Token
	{
		double(*operation)(double left, double right); ///<field holding pointer to operator function
	public:
		int precedence; ///<precedence of the operator
		Associativity assoc; ///<associativity of the operator
		std::string opname; ///<symbol of the operator, can have 1 or 2 characters

		/**
		 * Uses method pointed by 'operation' pointer to return operator result
		 * for a given numbers.
		 * @param left number on the left side of the operator
		 * @param right number on the right side of the operator
		 * @return operator result
		 */
		double execute(double left, double right);

		Operator(double(*operation)(double left, double right), int precedence,
			Associativity assoc, std::string opname) :
			Token(TokenType::OPERATOR),
			operation(operation),
			precedence(precedence),
			assoc(assoc),
			opname(opname)
		{ }
		~Operator() {}
		std::string toString();
	};

	/**
	 * Class represents parenthesis token. Parenthesis are considered only during
	 * creation of RPN notation - the notation itself does not contain any
	 * parenthesis, because the order of calculation is determined directly by
	 * operators and numbers order.
	 */
	class Parenthesis : public Token
	{
	public:
		ParenthesisType ptype; ///<determines if parenthesis is open or closed
		Parenthesis(ParenthesisType ptype) :
			Token(TokenType::PARENTHESIS),
			ptype(ptype)
		{ }
		~Parenthesis() { }
		std::string toString();
	};

	/**
	 * Clears postfix objects stored in MathEvaluation object.
	 */
	void clearPostfix();

	/**
	 * Registers operators available in MathEvaluation object. Default operators
	 * are +, -, *, &, |, >, <, >=, <=, =, ~, where ~ is inequality.
	 * Registered operators can have maximally two characters, otherwise they are
	 * ignored.
	 */
	void registerOperators();

	std::string originalexpression;
	std::unordered_map<std::string, Operator *> operators; ///<map containing available operators for MathEvaluation
	std::list<Token *> postfixlist; ///<list with tokens ordered as postfix (RPN) notation of the given string
	int varcount; ///<amount of variables declared for this MathEvaluation object
	std::vector<Variable *> vars; ///<vector of variables, default values are 0
	Variable *t; ///<additional t variable, that can act as time for evaluation
	std::vector<std::string> operatorstrings; ///<list of registered operators
	int arithmeticoperatorscount; ///<count of arithmetic operators
	int comparisonoperatorscount;
public:

	/**
	 * Default constructor. It registers operators for MathEvaluation and prepares
	 * vector of variables.
	 */
	MathEvaluation(int varcount);

	/**
	 * Destructor is responsible for clearing all objects in postfix, all operator
	 * objects and all variables.
	 */
	~MathEvaluation();

	/**
	 * Converts string to std::list of tokens in postfix order that can be later
	 * evaluated. Detailed description of algorithm can be found in source of
	 * method.
	 * @param expression infix mathematical expression
	 * @return 0 if conversion is successful, or -1 if an error occurred
	 */
	int convertString(std::string expression);

	/**
	 * Evaluates expression stored in postfix list. It takes into account changes
	 * made by modifyVariable method. This method should be called if the only
	 * thing changed in expression are variable values - it saves time because
	 * it uses objects created during convertString or evaluate method.
	 * @param res reference to the variable, in which result will be stored
	 * @return 0 if evaluation went successful, or -1 if an error occurred
	 */
	int evaluateRPN(double &res);

	/**
	 * Performs convertString and evaluateRPN methods respectively. It holds
	 * objects created by convertString method.
	 * @param expression infix mathematical expression
	 * @param result reference to the variable, in which result will be stored
	 * @return 0 if evaluation went successful, otherwise it returns the error of previous functions
	 */
	int evaluate(std::string expression, double &result);

	/**
	 * Sets variable with index i with value val.
	 * @param i id of variable, -1 if changed variable should be variable 't' (time)
	 * @param val new value
	 */
	void modifyVariable(int i, double val);

	/**
	 * Registers new operator for MathEvaluation object.
	 * @param operation function that needs to be called during evaluation
	 * @param precedence precedence of operator
	 * @param assoc associativity of the operator
	 * @param opsymbol symbol of the operator
	 */
	void registerOperator(double(*operation)(double left, double right), int precedence, MathEvaluation::Associativity assoc, std::string opsymbol);

	/**
	 * Converts stored RPN list to infix string.
	 * @param result reference to variable that will hold the result
	 * @return 0 if method went successful, or -1 if an error occurred
	 */
	int RPNToInfix(std::string &result);

	/**
	 * Mutates formula.
	 * @param logical if true, method uses logical operators only
	 * @param usetime if true, method may use time variable during mutation
	 * @param 0 if mutation could be performed, -1 if postfixlist is empty
	 */
	int mutate(bool logical, bool usetime);

	/**
	 * Mutates formula so it creates conjuctive of comparisons.
	 */
	void mutateConditional();

	/**
	 * Mutates number by using random value or one of variables.
	 * @param currval current value of number
	 * @param usetime if true, then time variable may be used
	 */
	void mutateValueOrVariable(Number *&currval, bool usetime);

	/**
	 * Returns formula in RPN notation
	 * @param RPN notation string
	 */
	std::string getStringifiedRPN();

	/**
	 * Returns random operator available in object.
	 * @param type 0 for all operators, 1 for arithmetic only, 2 for logical only
	 * @return pointer to random operator
	 */
	Operator* getRandomOperator(int type);
};

#endif // _FL_MATHEVAL_H_
