// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _PARAMTREE_H_
#define _PARAMTREE_H_

#include <frams/param/param.h>
#include <vector>
#include <memory>

// building a tree of Param groups based on the convention used in the Framsticks GUI application:
// - group names containing colon characters ":" are hierarchical paths (like a filesystem),
// - groups referencing common subpaths connect to their common parent nodes,
// - group ordering is preserved,
// - duplicate group names are not allowed.
//
// input: ParamInterface object, only using getGroupCount() and grname()
// output: tree structure in ParamTree.root, traverse using first_child/next_sibling
//
// example input: "a" "b" "c" "a:b:c" (connects to existing "a") "b:c" (connects to existing "b") "b:d" (connects to existing "b")
// example output:
//
//   +-+ a         (node name="a", group name="a")
//   | '-+ b       (dummy node, there is no "a:b" group)
//   |   '-- c     (node name="c", group name="a:b:c")
//   +-+ b
//   | +-- c
//   | '-- d
//   +-- c
//
// sample code: see frams/_demos/paramtree_stdin_test.cpp

class ParamTree
{
public:
	class ParamTreeNode;
	typedef std::shared_ptr<ParamTreeNode> NodePtr;
	class ParamTreeNode
	{
	public:
		ParamTree *tree;
		ParamTreeNode *parent;
		string name; //path component name
		int group_index; //original group index or -1 for dummy nodes
		NodePtr first_child;
		NodePtr next_sibling;

		ParamTreeNode(ParamTree *_tree = NULL, ParamTreeNode *_parent = NULL, const string &_name = "", int _group_index = -1)
			:tree(_tree), parent(_parent), name(_name), group_index(_group_index) {}
	};
	ParamTreeNode root;
	ParamInterface &pi;

	ParamTreeNode *addNode(ParamTreeNode* parent, const string &name, int group);
	ParamTreeNode *findNode(ParamTreeNode *parent, const string& name);

	ParamTree(ParamInterface *_pi);
};

#endif
