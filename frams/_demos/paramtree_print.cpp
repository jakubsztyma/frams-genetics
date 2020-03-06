#include <stdio.h>
#include "paramtree_print.h"

//#define NO_BOX_CHARACTERS

#ifdef NO_BOX_CHARACTERS
#define TREEDRAWING_L_MORE  "+-"
#define TREEDRAWING_L_LAST  "'-"
#define TREEDRAWING_I       "| "
#define TREEDRAWING_NOMORE  "  "
#define TREEDRAWING_CHILD   "+"
#define TREEDRAWING_NOCHILD "-"
#else
#define TREEDRAWING_L_MORE  "├─"
#define TREEDRAWING_L_LAST  "└─"
#define TREEDRAWING_I       "│ "
#define TREEDRAWING_NOMORE  "  "
#define TREEDRAWING_CHILD   "┬"
#define TREEDRAWING_NOCHILD "─"
#endif

static void printLines(ParamTree::ParamTreeNode *n, bool first = true)
{
	if (n->parent)
		printLines(n->parent, false);
	printf(n->next_sibling.get() != NULL ?
		(first ? TREEDRAWING_L_MORE : TREEDRAWING_I)
		: (first ? TREEDRAWING_L_LAST : TREEDRAWING_NOMORE));
}

void printTree(ParamTree::ParamTreeNode *n)
{
	while (n != NULL)
	{
		printLines(n);
		printf(n->first_child.get() != NULL ? TREEDRAWING_CHILD : TREEDRAWING_NOCHILD);
		if (n->name[0] != ' ')
			printf(" ");
		printf("%s", n->name.c_str());
		if (n->group_index >= 0)
			printf("\t\t(g=%d,\"%s\")", n->group_index, n->tree->pi.grname(n->group_index));
		else
			printf("\t\t(not a group)");
		printf("\n");
		printTree(n->first_child.get());
		n = n->next_sibling.get();
	}
}
