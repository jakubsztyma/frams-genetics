// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

// Copyright (C) 1999,2000  Adam Rotaru-Varga (adam_rotaru@yahoo.com), GNU LGPL

#ifndef _F4_GENERAL_H_
#define _F4_GENERAL_H_

#include <frams/util/3d.h>
#include <frams/util/sstring.h>
#include <frams/util/multirange.h>
#include <frams/genetics/geneprops.h>

#ifdef DMALLOC
#include <dmalloc.h>
#endif

/**
 * Performs single rotation angle decrementation on a given value.
 * @param v pointer to the decremented value
 */
void rolling_dec(double *v);

/**
 * Performs single rotation angle incrementation on a given value.
 * @param v pointer to the incremented value
 */
void rolling_inc(double *v);

class f4_node;   // later
class f4_Cell;   // later
class f4_Cells;  // later


/** @name Types of f4_Cell's */
//@{
#define T_UNDIFF4 40 ///<undifferentiated cell
#define T_STICK4  41 ///<differentiated to stick, cannot divide
#define T_NEURON4 42 ///<differentiated to neuron, can divide
//@}

/**
 * Scans f4 genotype string for a stopping character and returns the position of
 * this stopping character or 1 if the end of string was reached. This method is used
 * for closing braces, like ), >, ]. It runs recursively when opening braces
 * like (, <, # are found.
 * @param s string with the f4 genotype
 * @param slen length of a given string
 * @param stopchar character to be found
 * @return 1 if end of string was reached, or position of found character in sequence
 */
int scanrec(const char* s, unsigned int slen, char stopchar);


class f4_CellLink;

/** @name Constraints of f4 genotype structures */
//@{
#define MAXINPUTS 100 ///<maximum number of neuron inputs
#define MAX4CELLS 100 ///<maximum number of f4 organism cells
//@}

/**
 * Abstract cell type - the representation of single component in the  developmental
 * encoding. In the beginning, each f4_Cell is undifferentiated. During the process
 * of development it can divide or differentiate into a stick or a neuron. If it
 * differentiates to a neuron, then it preserves the ability to divide, but divided
 * cells will be the same type as the parent cell. If it is a stick, then it cannot
 * be divided anymore.
 *
 * From f4_Cell array the final Model of a creature is created.
 */
class f4_Cell
{
public:
	/**
	 * Represents the repetition marker. It holds information about the pointer
	 * to the repetition node and the count of repetitions.
	 */
	class repeat_ptr
	{
	public:
		repeat_ptr() : node(NULL), count(-1) { };

		/**
		 * A constructor that takes the pointer to the repetition node and the count of repetitions.
		 * @param a pointer to f4_node for repetition character
		 * @param b the number of repetitions
		 */
		repeat_ptr(f4_node *a, int b) : node(a), count(b) { };

		inline void makeNull() { node = NULL; count = -1; };

		inline bool isNull() const { return ((node == NULL) || (count <= 0)); };

		inline void dec() { count--; };
		f4_node    *node; ///<pointer to the repetition code
		int       count; ///<repetition counter
	};

	/**
	 * Represents the stack of repeat_ptr objects. The objects are
	 * pushed to the stack when '#' repetition symbol appears, and are popped when
	 * the end of the current cell definition, i.e. the '>' character, appears. After the
	 * '>' character, the cell is duplicated as many times as it is defined after the
	 * repetition marker.
	 */
	class repeat_stack
	{
	public:
		repeat_stack() { top = 0; }

		inline void clear() { top = 0; }

		/**
		 * Pushes repeat_ptr object onto the stack. If the stack size is exceeded, then no
		 * information is provided.
		 * @param rn repetition node info
		 */
		inline void push(repeat_ptr rn) { if (top >= stackSize) return; ptr[top] = rn; top++; }

		inline void pop() { if (top > 0) top--; }

		/**
		 * Gets the current top element.
		 * @return pointer to the element on top of the repeat_stack object
		 */
		inline repeat_ptr* first() { return &(ptr[top - (top > 0)]); };
		static const int stackSize = 4;  ///<max 4 nested levels
		repeat_ptr ptr[stackSize]; ///<array holding pointers to repeat_ptr
		short int top;  ///<index of the top of the stack
	};

	/**
	 * Creates a new f4_Cell object.
	 * @param nname name of a cell, can be T_UNDIFF4, T_STICK4 or T_NEURON4
	 * @param ndad pointer to the parent of the created cell
	 * @param nangle the amount of commas affecting branch angles
	 * @param newP genotype properties of a given cell
	 */
	f4_Cell(int nname, f4_Cell *ndad, int nangle, GeneProps newP);
	/**
	 * Creates a new f4_Cell object.
	 * @param nO pointer to an organism containing the cell
	 * @param nname name of the cell, can be T_UNDIFF4, T_STICK4 or T_NEURON4
	 * @param ngeno pointer to the root of the genotype tree
	 * @param ngcur pointer to the f4_node representing the current cell in the genotype tree
	 * @param ndad pointer to the parent of the created cell
	 * @param nangle the number of commas affecting branch angles
	 * @param newP genotype properties of a given cell
	 */
	f4_Cell(f4_Cells *nO, int nname, f4_node *ngeno, f4_node *ngcur, f4_Cell *ndad, int nangle, GeneProps newP);

	~f4_Cell();

	/**
	 * Performs a single step of cell development. This method requires a pointer to
	 * the f4_Cells object in org attribute. If the current node in genotype tree
	 * is the branching character '<', the cell divides into two cells, unless the
	 * cell was already differentiated into the stick cell. Otherwise, the current
	 * differentiation or modification is performed on the cell. If current node is
	 * creating a connection between two neuron nodes and the input node is not
	 * yet developed, the simulation of the development of the current cell waits until
	 * the input node is created. The onestep method is deployed for every cell
	 * at least once. If one cell requires another one to develop, onestep
	 * should be deployed again on this cell. This method, unlike genotype tree
	 * creation, checks semantics. This means that this function will fail if:
	 *  - the cell differentiated as a stick will have branching node '<',
	 *  - the undifferentiated cell will have termination node '>' (end of cell development without differentiation),
	 *  - the stack of repetition marker '#' will exceed maximum allowed value of repetition,
	 *  - the stick modifiers, like rotation, will be applied on neuron cell,
	 *  - the differentiated cell will be differentiated again,
	 *  - the neuron class inside cell connection (i.e. N[G:5]) is not a sensor,
	 *  - the connection between neurons cannot be established,
	 *  - the neuron class is not valid.
	 *
	 * @return 0 if development was successful, 1 if there was an error in genotype tree
	 */
	int onestep();

	/**
	 * Adds a link between this neuron cell and a given neuron cell in nfrom. If the nfrom object
	 * is not given, neuron type in nt should be a sensor type.
	 * @param nfrom input neuron cell, or NULL if not given
	 * @param nw weight of connection
	 * @param nt empty string or name of sensor class
	 * @return 0 if link is established, -1 otherwise
	 */
	int   addlink(f4_Cell *nfrom, double nw, string nt);

	/**
	 * Adjusts properties of stick objects.
	 */
	void  adjustRec();

	int        name;               ///<name of cell (number)
	int        type;               ///<type
	f4_Cell *dadlink;              ///<pointer to cell parent
	f4_Cells  *org;	               ///<uplink to organism

	f4_node *genot; 	           ///<genotype tree
	f4_node *gcur;                 ///<current genotype execution pointer
	int active;                    ///<determines whether development is still active
	repeat_stack repeat;           ///<stack holding repetition nodes and counters
	int recProcessedFlag;          ///<used during recursive traverse
	MultiRange genoRange;          ///<remember the genotype codes affecting this cell so far

	GeneProps    P;                ///<properties
	int          anglepos;         ///<number of position within dad's children (,)
	int          childcount;       ///<number of children
	int          commacount;       ///<number of postitions at lastend (>=childcount)
	double       rolling;          ///<rolling angle ('R') (around x)
	double       xrot;			   ///<rotation angle around x
	double       zrot;             ///<horizontal rotation angle due to branching (around z)

	double       mz;               ///<freedom in z
	int          p2_refno;         ///<the number of the last end part object, used in f0
	int          joint_refno;      ///<the number of the joint object, used in f0
	int          neuro_refno;      ///<the number of the neuro object, used in f0

	int          ctrl;             ///<neuron type
	double       inertia;          ///<inertia of neuron
	double       force;            ///<force of neuron
	double       sigmo;            ///<sigmoid of neuron
	f4_CellLink *links[MAXINPUTS]; ///<array of neuron links
	int          nolink;           ///<number of links
	NeuroClass *neuclass;          ///<pointer to neuron class
};

/**
 * Class representing link between neuron cells.
 */
class f4_CellLink
{
public:
	/**
	 * Constructor for f4_CellLink class. Parameter nfrom represents input
	 * neuron cell or NULL if connection has defined sensor type inside, like "[G:5]".
	 * The name of sensor class defined inside neuron connection is stored in the nt
	 * parameter.
	 * @param nfrom pointer to input neuron cell or NULL
	 * @param nw weight of connection
	 * @param nt name of neuron class or empty string
	 */
	f4_CellLink(f4_Cell *nfrom, double nw, string nt);

	f4_Cell *from; ///<pointer to input neuron cell
	string t;      ///<empty if 'from' cell is given, NeuroClass name otherwise
	double w;      ///<weight of connection
};


/**
 * A class representing a collection of cells. It is equivalent to an organism.
 */
class f4_Cells
{
public:

	/**
	 * Constructor taking genotype in a form of a tree.
	 * @param genome genotype tree
	 * @param nrepair 0 if nothing to repair
	 */
	f4_Cells(f4_node *genome, int nrepair);

	/**
	 * Constructor taking genotype in a form of a string.
	 * @param genome genotype string
	 * @param nrepair 0 if nothing to repair
	 */
	f4_Cells(SString &genome, int nrepair);

	/**
	 * Destructor removing cells from memory.
	 */
	~f4_Cells();

	/**
	 * Adds a new cell to organism.
	 * @param newcell cell to be added
	 */
	void addCell(f4_Cell *newcell);

	/**
	 * Creates an approximate genotype in the f1 encoding and stores it in a given parameter.
	 * @param out the string in which the approximate f1 genotype will be stored
	 */
	void toF1Geno(SString &out);

	/**
	 * Performs a single step of organism development. It runs each active cell
	 * in the organism.
	 * @return 0 if all cells are developed, or 1 otherwise
	 */
	int  onestep();

	/**
	 * Performs the full development of organism and returns error code if something
	 * went wrong.
	 * @return 0 if organism developed successfully, error code if something went wrong
	 */
	int  simulate();

	/**
	 * Returns error code of the last simulation.
	 * @return error code
	 */
	int  geterror() { return error; };

	/**
	 * Returns position of an error in genotype.
	 * @return position of an error
	 */
	int  geterrorpos() { return errorpos; };

	/**
	 * Sets error code GENOPER_OPFAIL for a simulation on a given position.
	 * @param nerrpos position of an error
	 */
	void setError(int nerrpos);

	/**
	 * Sets the element of genotype to be repaired by removal.
	 * @param nerrpos position of an error in genotype
	 * @param rem the f4_node to be removed from the  genotype tree in order to repair
	 */
	void setRepairRemove(int nerrpos, f4_node *rem);

	/**
	 * Sets repairing of a genotype by inserting a new node to the current genotype.
	 * @param nerrpos position of an error in genotype
	 * @param parent the parent of a new element
	 * @param insert the element to be inserted
	 * @return 0 if repair can be performed, or -1 otherwise because the repair flag wasn't set in the constructor
	 */
	int  setRepairInsert(int nerrpos, f4_node *parent, f4_node *insert);

	/**
	 * Repairs the genotype according to setRepairRemove or setRepairInsert methods.
	 * @param geno pointer to the genotype tree
	 * @param whichchild 1 if first child, 2 otherwise
	 */
	void repairGeno(f4_node *geno, int whichchild);

	// the cells
	f4_Cell *C[MAX4CELLS];  ///<Array of all cells of an organism
	int       nc;           ///<Number of cells in an organism

private:
	// for error reporting / genotype fixing
	int repair;
	int error;
	int errorpos;
	f4_node *repair_remove;
	f4_node *repair_parent;
	f4_node *repair_insert;
	void toF1GenoRec(int curc, SString &out);
	f4_Cell *tmpcel;		// needed by toF1Geno
	f4_node *f4rootnode;          // used by constructor
};


/**
 * A class to organize a f4 genotype in a tree structure.
 */
class f4_node
{
public:
	string name; ///<one-letter 'name', multiple characters for classes
	f4_node *parent; ///<parent link or NULL
	f4_node *child; ///<child or NULL
	f4_node *child2; ///<second child or NULL
	int pos; ///<original position in the string
	int i1; ///<internal int parameter1
	int l1; ///<internal long parameter1 (now also int, since long is not well specified and it is in our scenarios equivalent to int)
	double f1; ///<internal double parameter1
	string s1; ///<internal string parameter1

	f4_node();

	/**
	 * Multiple-character name constructor.
	 * @param nname string from genotype representing node
	 * @param nparent pointer to parent of the node
	 * @param npos position of node substring in the genotype string
	 */
	f4_node(string nname, f4_node *nparent, int npos);

	/**
	 * Single-character name constructor.
	 * @param nname character from genotype representing node
	 * @param nparent pointer to parent of the node
	 * @param npos position of node character in the genotype string
	 */
	f4_node(char nname, f4_node *nparent, int npos);

	~f4_node();

	/**
	 * Adds the child to the node.
	 * @param nchi the child to be added to the node
	 * @return 0 if the child could be added, -1 otherwise
	 */
	int addChild(f4_node *nchi);

	/**
	 * Removes the child from the node.
	 * @param nchi the child to be removed from the node
	 * @return 0 if child could be removed, -1 otherwise
	 */
	int removeChild(f4_node *nchi);

	/**
	 * Returns the number of children.
	 * @return 0, 1 or 2
	 */
	int childCount();

	/**
	 * Returns the number of nodes coming from this node in a recursive way.
	 * @return the number of nodes from this node
	 */
	int count();

	/**
	 * Returns the nth subnode (0-)
	 * @param n index of the child to be found
	 * @return pointer to the nth subnode or NULL if not found
	 */
	f4_node* ordNode(int n);

	/**
	 * Returns a random subnode.
	 * @return random subnode
	 */
	f4_node* randomNode();

	/**
	 * Returns a random subnode with a given size.
	 * @param min minimum size
	 * @param max maximum size
	 * @return a random subnode with a given size or NULL
	 */
	f4_node* randomNodeWithSize(int min, int max);

	/**
	 * Prints recursively the tree from a given node.
	 * @param buf variable to store printing result
	 */
	void      sprintAdj(char *&buf);

	/**
	 * Recursively copies the genotype tree from this node.
	 * @return pointer to a tree copy
	 */
	f4_node* duplicate();

	/**
	 * Recursively releases memory from all node children.
	 */
	void      destroy();
private:
	void     sprint(SString &out);	// print recursively
};

/**
 * The main function for converting a string of f4 encoding to a tree structure. Prepares
 * f4_node root of tree and runs f4_processrec function for it.
 * @param geno the string representing an f4 genotype
 * @return a pointer to the f4_node object representing the f4 tree root
 */
f4_node* f4_processtree(const char *geno);

/**
 * Scans a genotype string starting from a given position. This recursive method creates
 * a tree of f4_node objects. This method extracts each potentially functional element
 * of a genotype string to a separate f4_nodes. When the branching character '<' occurs,
 * f4_processrec is deployed for the latest f4_node element. This method does not
 * analyse the genotype semantically, it only checks if the syntax is proper. The only
 * semantic aspect is neuron class name extraction, where the GenoOperators
 * class is used to parse the potential neuron class name.
 * @param genot the string holding all the genotype
 * @param pos0 the current position of processing in string
 * @param parent current parent of the analysed branch of the genotype
 * @return 0 if processing was successful, otherwise returns the position of an error in the genotype
 */
int f4_processrec(const char *genot, unsigned pos0, f4_node *parent);

/**
 * Parses notation of the neuron connection - takes the beginning of the connection
 * definition, extracts the relative position of input neurons and the weight of the connection.
 * After successful parsing, returns the pointer to the first character after the connection
 * definition, or NULL if the connection definition was not valid due to the lack of [, :, ]
 * characters or an invalid value of relfrom or weight.
 * @param fragm the beginning of connection definition, should be the '[' character
 * @param relfrom the reference to an int variable in which the relative position of the input neuron will be stored
 * @param weight the reference to a double variable in which the weight of the connection will be stored
 * @return the pointer to the first character in string after connection definition
 */
const char *parseConnection(const char *fragm, int &relfrom, double &weight);

/**
 * Parses the notation of the neuron connection with neuron definition - takes
 * the beginning of the connection definition, extracts the name of neuron class
 * that will be the input for the current neuron and the weight of the connection.
 * After successful parsing, returns a pointer to the first character after the connection
 * definition, or NULL if the connection definition was not valid due to the lack of [, :, ]
 * characters, an invalid value of the weight or an invalid neuron class name.
 * @param fragm the beginning of the connection definition, should be the '[' character
 * @param neutype the reference to a string representing the input neuron class name. The name of the class is validated with GenoOperators::parseNeuroClass()
 * @param weight the reference to a double variable in which the weight of the connection will be stored
 * @return the pointer to the first character in string after the connection definition
 */
const char *parseConnectionWithNeuron(const char *fragm, string &neutype, double &weight);

#endif
