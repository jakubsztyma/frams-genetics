#include <frams/util/sstringutils.h>
#include <common/virtfile/stdiofile.h>
#include <frams/param/paramtree.h>
#include "paramtree_print.h"

class EmptyParamWithGroupsForTesting : public Param
{
	std::vector<ParamEntry> entries;
	std::vector<std::shared_ptr<std::string>> strings; //could be a simple vector of strings, but then char* pointers can change when adding new strings and so ParamEntry structures would need updating. Therefore using "vector of string pointers" instead of "vector of strings".
public:
	EmptyParamWithGroupsForTesting()
	{
		ParamEntry zero_ending = { 0, 0, 0, 0 };
		entries.push_back(zero_ending);
		setParamTab(getParamTab());
	}
	void addGroup(const char* name)
	{
		std::shared_ptr<string> str(new std::string(name));
		strings.push_back(str);
		ParamEntry tmp = { str.get()->c_str(), 0, 0, 0 };
		entries.insert(entries.begin() + (entries.size() - 1), tmp);
		entries[0].group = entries.size() - 1;
		setParamTab(getParamTab());
	}
	ParamEntry *getParamTab()
	{
		return &entries[0];
	}
};

// This program tests parameter tree construction for paramtab names read from stdin.
// app_group_names.txt can be used as a sample input, because it contains a large set of paramtab objects from Framsticks GUI.
// See paramtree_paramlist_test.cpp for a demonstration of parameter tree construction for all paramtab's that are available in SDK.
// See mutableparam_test.cpp for a demonstration on how to detect (and possibly respond to) changing parameter definitions.
int main()
{
	SString group_names;
	StdioFILE::setStdio(); //setup VirtFILE::Vstdin/out/err
	puts("(loading group names from stdin)");
	loadSString(VirtFILE::Vstdin, group_names);
	int pos = 0;
	SString line;
	EmptyParamWithGroupsForTesting param;
	while (group_names.getNextToken(pos, line, '\n'))
	{
		if ((line.len() > 0) && (line[line.len() - 1] == '\r')) //support for reading \r\n files...
			line = line.substr(0, line.len() - 1);
		if (line.len() > 0 && line[0] != '#') //skip empty lines and #commment lines
			param.addGroup(line.c_str());
	}
	printf("%d groups\n\n", param.getGroupCount());

	ParamTree tree(&param);

	printTree(&tree.root);
}
