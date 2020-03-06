// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2018  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "paramtree.h"

ParamTree::ParamTreeNode *ParamTree::addNode(ParamTree::ParamTreeNode* parent, const string& name, int group)
{
	std::shared_ptr<ParamTreeNode> new_it(new ParamTreeNode(this, parent, name, group));
	ParamTreeNode *last = parent->first_child.get();
	if (last)
	{
		ParamTreeNode *next;
		while (true)
		{
			next = last->next_sibling.get();
			if (next == NULL) break;
			last = next;
		}
		last->next_sibling = new_it;
	}
	else
		parent->first_child = new_it;
	return new_it.get();
}

ParamTree::ParamTreeNode *ParamTree::findNode(ParamTree::ParamTreeNode *parent, const string& name)
{
	for (ParamTree::ParamTreeNode *it = parent->first_child.get(); it != NULL; it = it->next_sibling.get())
		if (it->name == name) return it;
	return NULL;
}

static int countChars(const char* str, char ch)
{
	int count = 0;
	for (; *str; str++)
		if (*str == ch) count++;
	return count;
}

ParamTree::ParamTree(ParamInterface *_pi)
	:pi(*_pi)
{
	int g;
	int level = 0, hasmorelevels = 1;
	root.tree = this;
	root.name = "[Tree root]";
	for (; hasmorelevels; level++)
	{
		hasmorelevels = 0;
		for (g = 0; g<pi.getGroupCount(); g++)
		{
			const char* grname = pi.grname(g);
			int colons = countChars(grname, ':');
			if (colons>level) hasmorelevels = 1;
			if (colons != level) continue;

			string name = grname; // "abc:def:ghi"
			ParamTreeNode *parentnode = &root;
			int i = 0;
			// search for parentnode for level 'level'
			for (int haslevel = 0; haslevel < level; haslevel++)
			{
				i = name.find(':'); // always succeeds because we counted the colons before
				// "na:me" -> prefix="na",name="me"
				string prefix = name.substr(0, i);
				// search for parent node in listview ('prefix')
				ParamTreeNode *it = findNode(parentnode, prefix);
				name = name.substr(i + 1);
				if (it) parentnode = it;
				else parentnode = addNode(parentnode, prefix, -1);
			}
			addNode(parentnode, name, g);
		}
	}
}
