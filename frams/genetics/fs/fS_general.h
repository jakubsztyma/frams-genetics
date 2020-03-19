//
// Created by jakub on 21.02.2020.
//

#ifndef CPP_FS_CONV_H
#define CPP_FS_CONV_H

#define FS_OPCOUNT 10

#include <iostream>
#include <vector>
#include <map>
#include <regex>
#include <set>
#include <iterator>
#include <math.h>
#include <cctype>
#include "common/Convert.h"
#include "frams/genetics/genooperators.h"
#include "frams/util/3d.h"
#include "frams/util/sstring.h"
#include "frams/model/model.h"

using namespace std;

class Mode {
public:
    bool cycle, modifier, param; /// Possible modes

    Mode(SString modeStr);
};

class State {
public:
    Pt3D location;  /// Location of the node
    Pt3D v;         /// The normalised vector in which current branch develops
    double fr = 1.0;      /// Friction multiplier
    double sx = 1.0, sy = 1.0, sz = 1.0;      /// Size multipliers

    State(State *_state); /// Derive the state from parent

    State(Pt3D _location, Pt3D _v); /// Create the state from parameters

    /**
     * Add the vector of specified length to location
     * @param length the length of the vector
     */
    void addVector(double length);

    /**
     * Rotate the vector by specified values
     * @param rx rotation by x axis
     * @param ry rotation by y axis
     * @param rz rotation by z axis
     */
    void rotate(double rx, double ry, double rz);
};

class Node {
    friend class fS_Genotype;

    friend class fS_Operators;

private:
    bool isStart;   /// Is a starting node of whole genotype
    char partType; /// The type of the part (E, P, C)
    Part *part;     /// A part object built from node. Used in building the Model
    unsigned int childSize = 0; /// The number of direct children

    map<string, double> params; /// The map of all the node params
    vector<Node *> children;    /// Vector of all direct children
    vector<char> modifiers;     /// Vector of all modifiers
    set<char> joints;           /// Set of all joints

    double getSx();
    double getSy();
    double getSz();
    /**
     * Get the position of part type in genotype
     *
     * @return the position of part type
     */
    int getPartPosition(SString restOfGenotype);

    /**
     * Extract the value of parameter or return default if parameter not exists
     * @return the param value
     */
    double getParam(string key);

    /**
     * Extract modifiers from the rest of genotype
     * @return the remainder of the genotype
     */
    SString extractModifiers(SString restOfGenotype);

    /**
     * Extract part type from the rest of genotype
     * @return the remainder of the genotype
     */
    SString extractPartType(SString restOfGenotype);

    /**
     * Extract params from the rest of genotype
     * @return the remainder of the genotype
     */
    SString extractParams(SString restOfGenotype);

    /**
     * Extract child branches from the rest of genotype
     * @return vector of child branches
     */
    vector <SString> getBranches(SString restOfGenotype);

    /**
     * Get phenotypic state that derives from ancestors.
     * Used when building model
     * @param _state state of the parent
     */
    void getState(State *_state, double psx, double psy, double psz);

    /**
     * Build children internal representations from fS genotype
     * @param restOfGenotype part of genotype that describes the subtree
     */
    void getChildren(SString restOfGenotype);

    /**
     * Create part object from internal representation
     */
    void createPart();

    /**
     * Add joints between current node and the specified child
     * Used in building model
     * @param mode pointer to build model
     * @param child pointer to the child
     */
    void addJointsToModel(Model *model, Node *child);

    /**
     * Get all the nodes from the subtree that starts in this node
     * @param reference to vector which contains nodes
     */
    void getAllNodes(vector<Node *> &allNodes);

    /**
     * Build model from the subtree that starts in this node
     * @param pointer to model
     */
    Part *buildModel(Model *model);

public:
    State *state = nullptr; /// The phenotypic state, that inherits from ancestors
    Mode *mode; /// The mode in which the representation works

    Node(const SString &genotype, Mode *_mode, bool _isStart);

    ~Node();

    /**
     * Get fS representation of the subtree that starts from this noce
     * @param result the reference to an object which is used to contain fS genotype
     */
    void getGeno(SString &result);
};

class fS_Genotype {
    friend class Node;

    friend class fS_Operators;

private:
    Node *startNode;    /// The start (root) node. All other nodes are it's descendants

    /**
     * Get all existing nodes
     * @return vector of all nodes
     */
    vector<Node *> getAllNodes();

    /**
     * Draws a node, which has a index greater that specified
     * @param fromIndex minimal index of the node
     * @return pointer to drawn node
     */
    Node *chooseNode(int fromIndex);

    /**
     * Draws an integer value from given range
     * @param to maximal value
     * @param from minimal value
     * @return Drawn value
     */
    int randomFromRange(int to, int from);

    /**
     * Draws a value from defined distribution
     * @return Drawn value
     */
    void randomFromDistribution();

    /**
     * Find a node that is nearest (euclidean distance to specified node) and is not a child of specified node
     * @return Nearest node
     */
    Node *getNearestNode(vector<Node *> allNodes, Node *node);

public:
    /**
     * Counts all the nodes in genotyp
     * @return node count
     */
    int getNodeCount();

    /**
     * Build internal representation from fS format
     * @param genotype in fS format
     */
    fS_Genotype(const SString &genotype);

    ~fS_Genotype();

    /**
     * Builds Model object from internal representation
     * @param pointer to model that will contain a built model
     */
    void buildModel(Model *model);

    /**
     * @return genotype in fS format
     */
    SString getGeno();

    /**
     * Performs add joint mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool addJoint();

    /**
     * Performs remove mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool removeJoint();

    /**
     * Performs add part mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool addPart();

    /**
     * Performs change part type mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool changePartType();

    /**
     * Performs remove part type mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool removePart();

    /**
     * Performs add param mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool addParam();

    /**
     * Performs remove param mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool removeParam();

    /**
     * Performs change param mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool changeParam();

    /**
     * Performs add modifier mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool addModifier();

    /**
     * Performs remove modifier mutation on genotype
     * @return true if mutation succeeded, false otherwise
     */
    bool removeModifier();

    /**
     * Performs mutation on genotype.
     * @return true if mutation succeeded, false otherwise
     */
    void mutate();
};


#endif //CPP_FS_CONV_H
