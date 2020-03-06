// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include <common/virtfile/stdiofile.h>
#include <frams/util/sstringutils.h>
#include <frams/genetics/preconfigured.h>
#include <common/loggers/loggertostdout.h>

/**
 @file
 Sample code: Testing genotype validity

 \include geno_test.cpp
 */

int main(int argc, char*argv[])
{
	LoggerToStdout messages_to_stdout(LoggerBase::Enable); //comment this object out to mute error/warning messages
	PreconfiguredGenetics genetics;

	if (argc <= 1)
	{
		puts("no genotype");
		return 10;
	}
	SString gen(argv[1]);
	if (!strcmp(gen.c_str(), "-"))
	{
		gen = 0;
		StdioFILEDontClose in(stdin);
		loadSString(&in, gen);
	}
	Geno g(gen);
	puts(g.isValid() ? "valid" : "invalid");
	return !g.isValid();
}
