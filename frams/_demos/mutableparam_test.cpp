#include <frams/util/sstringutils.h>
#include <common/virtfile/stdiofile.h>
#include <frams/param/mutableparam.h>
#include <frams/param/mutparamlist.h>
#include <frams/param/paramtrans.h>

static void printParam(ParamInterface &pi)
{
	printf("groups:\n");
	for (int g = 0; g < pi.getGroupCount(); g++)
		printf("- %s\n", pi.grname(g));
	printf("properties:\n");
	for (int i = 0; i < pi.getPropCount(); i++)
		if (!(pi.flags(i) & PARAM_USERHIDDEN))
			printf("- %s (i=%d)\n", pi.id(i), i);
}

int main()
{
	StdioFILE::setStdio(); //setup VirtFILE::Vstdin/out/err

	//creating 3 objects
	MutableParam p1("Object1", "Group 1"), p2("Object2", "Group 2"), p3("Object3", "Group 3");

	p1.addGroup("Added Group");
	p1.addGroup("Added Group 2");

	p1.addProperty(NULL, "f1", "d 0 5", "Field 1", "Description 1", 0/*flags*/, 0/*group*/, -1);
	p2.addProperty(NULL, "f2", "s", "Field 2", "Description 2", 0/*flags*/, 1/*group*/, -1);
	p3.addProperty(NULL, "f3", "d 0 1", "Field 3", "Description 3", 0/*flags*/, 1/*group*/, -1);

	MutableParamList combined;
	combined += &p1;
	combined += &p2;
	combined += &p3;

	ParamTransaction paramtrans(combined);

	printf("\n=== Combined ===\n");
	printParam(combined);

	printf("\nNow adding new property in p1...\n");
	p1.addProperty(NULL, "f4", "x", "Field 4", "Description 4", 0/*flags*/, 0/*group*/, -1);

	printf("\n=== After adding f4 ===\n");
	printParam(combined);

	printf("\n=== Transaction view maintains the original index association ===\n");
	//...so it can be safely iterated by index while param properties are being added or removed
	//without knowing or detecing what was changed.
	//Use case: simulator parameters in GUI - setting the "expdef" property
	//affects the properties of the selected and set experiment definition.
	printParam(paramtrans);

	printf("\nProperties changed: %s   Groups changed: %s\n\n", paramtrans.propChanged() ? "YES" : "NO", paramtrans.groupChanged() ? "YES" : "NO");

	printf("Now removing group in p1...\n");
	p1.removeGroup(1);

	printf("\nProperties changed: %s   Groups changed: %s\n\n", paramtrans.propChanged() ? "YES" : "NO", paramtrans.groupChanged() ? "YES" : "NO");

	//in GUI, paramtrans.groupChanged() can be used to trigger rebuilding the tree of groups
	//(see also: Framsticks GUI and sources of paramtree_* examples).

	return 0;
}
