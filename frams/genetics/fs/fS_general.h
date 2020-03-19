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
#include "common/Convert.h"
#include "frams/genetics/genooperators.h"
#include "frams/util/3d.h"
#include "frams/util/sstring.h"
#include "frams/model/model.h"

using namespace std;

class Mode {
public:
    bool cycle, modifier, param;

    Mode(SString modeStr);
};

class State {
public:
    Pt3D location, v;
    double fr;

    State(State *_state);

    State(Pt3D _location, Pt3D _v);

    void addVector(double length);

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

    int getPartPosition(SString restOfGenotype);

    double getParam(string key, double defaultValue);

    SString extractModifiers(SString restOfGenotype);

    SString extractPartType(SString restOfGenotype);

    SString extractParams(SString restOfGenotype);

    vector <SString> getBranches(SString restOfGenotype);

    void getState(State *_state, double psx, double psy, double psz);

    void getChildren(SString restOfGenotype);

    void createPart();

    void addJointsToModel(Model *model, Node *child, Part *part, Part *childPart);

    void getAllNodes(vector<Node *> &allNodes);

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
