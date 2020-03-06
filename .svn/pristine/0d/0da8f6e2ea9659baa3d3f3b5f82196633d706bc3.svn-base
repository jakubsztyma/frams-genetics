// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <frams/param/multiparamload.h>
#include <common/virtfile/stdiofile.h>
#include <common/loggers/loggertostdout.h>

/**
 @file
 Sample code: Loading Framsticks "objects" (structures).

 After loading, objects are printed ("saved") to standard output.
 Additional information and messages are printed to standard error.
 You can redirect one or both streams if needed.

 A sample input file for this program is "loader_test_param.in",
 so you can run this program from the "cpp" directory as
 ./loader_test_param  frams/_demos/loader_test_param.in

 \include loader_test_param.cpp
 */

struct Data
{
public:
	SString text, longtext, deftext;
	paInt i1, i2, i3, i4;
	double f1, f2, f3, f4;
	ExtValue x1, x2;
	int notchanged, notloaded, notsaved;
};

#define FIELDSTRUCT Data
ParamEntry data_paramtab[] =
{
	{ "Data", 1, 16, "data", },
	{ "text", 0, 0, "Text", "s 0 10", FIELD(text), }, // 10 = length limit
	{ "longtext", 0, 0, "Long text", "s 1", FIELD(longtext), }, // 1 = multiline, 0 = unlimited
	{ "deftext", 0, 0, "Text with default value", "s 0 0 some text", FIELD(deftext), },
	{ "i1", 0, 0, "Integer 1", "d", FIELD(i1), },          // unrestricted integer
	{ "i2", 0, 0, "Integer 2", "d -5 5 1", FIELD(i2), }, // [-5..5] integer, default=1
	{ "i3", 0, 0, "Integer 3", "d -1 3 0 ~Minus~Zero~One~Two~Three", FIELD(i3), }, // [0..3] + text labels (Param::getText())
	{ "i4", 0, 0, "Integer 4", "d 0 10 15", FIELD(i4), }, // invalid default
	{ "f1", 0, 0, "Float 1", "d", FIELD(f1), },          // unrestricted float
	{ "f2", 0, 0, "Float 2", "f -100 100 -100", FIELD(f2), }, // [-100..100] float, default=-100
	{ "f3", 0, 0, "Float 3", "f -10 10", FIELD(f3), }, // [-10..10] float
	{ "f4", 0, 0, "Float 4", "f 1 -1 44", FIELD(f4), }, // unrestricted float (because min>max), default=44
	{ "x1", 0, 0, "Untyped 1", "x", FIELD(x1), }, // any type (class ExtValue)
	{ "x2", 0, 0, "Untyped 2", "x", FIELD(x2), }, // any type (class ExtValue)
	{ "notchanged", 0, PARAM_READONLY, "Read only field", "d", FIELD(notchanged), }, // neither load() nor setDefault() can change this field
	{ "notloaded", 0, PARAM_DONTLOAD, "Non-loadable field", "d", FIELD(notloaded), }, // load() does not change this field
	{ "notsaved", 0, PARAM_DONTSAVE, "Non-saveable field", "d", FIELD(notsaved), }, // saveSingle/MultiLine() skips this field
	{ 0, 0, 0, },
};
#undef FIELDSTRUCT

int main(int argc, char*argv[])
{
	if (argc < 2)
	{
		fprintf(stderr, "Arguments: filename\n");
		return 1;
	}

	StdioFILEDontClose virt_stderr(stderr);
	StdioFILEDontClose virt_stdout(stdout);
	LoggerToStdout messages_to_stderr(LoggerBase::Enable, &virt_stderr);
	StdioFileSystem_autoselect stdiofilesys;
	MultiParamLoader loader(argv[1]);

	Data data;
	Param param(data_paramtab, &data);

	data.notchanged = 100;
	data.notloaded = 200;

	loader.addObject(&param);
	loader.breakOn(MultiParamLoader::OnError + MultiParamLoader::BeforeObject + MultiParamLoader::AfterObject + MultiParamLoader::OnComment + MultiParamLoader::BeforeUnknown);

	while (int status = loader.go())
	{
		switch (status)
		{
		case MultiParamLoader::OnComment:
			fprintf(stderr, "comment: '%s'\n", loader.getComment().c_str());
			break;

		case MultiParamLoader::BeforeUnknown:
			// At this point we could change our mind and load the unknown object using MultiParamLoader::loadObjectNow() functions.
			// It is "unknown", so we would have to provide its ParamInterface.
			// In fact, this method is used not just for truly unknown objects but also for
			// dynamic objects that cannot be added using MultiParamLoader.addObject().
			fprintf(stderr, "unknown object found: '%s' (will be skipped)\n", loader.getObjectName().c_str());
			break;

		case MultiParamLoader::AfterObject:
			fprintf(stderr, "loaded:\n");
			for (int i = 0; i < param.getPropCount(); i++)
				fprintf(stderr, "%s=%s\n", param.id(i), param.getText(i).c_str());
			fprintf(stderr, "type of 'x1' is: %s\n", data.x1.typeDescription().c_str());
			fprintf(stderr, "type of 'x2' is: %s\n", data.x2.typeDescription().c_str());
			fprintf(stderr, "-----\n\n");
			param.saveMultiLine(&virt_stdout);
			break;

		case MultiParamLoader::BeforeObject:
			fprintf(stderr, "----- object found, loading...\n");
			data.notchanged++;
			param.setDefault(); //reset (initialize) struct contents
			break;

		case MultiParamLoader::OnError:
			fprintf(stderr, "Error: %s", loader.getError().c_str());
		}
	}
	return 0;
}
