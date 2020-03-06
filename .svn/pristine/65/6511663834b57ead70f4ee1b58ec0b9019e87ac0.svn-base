// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2019  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "nonstd_math.h"

RandomGenerator &rndGetInstance()
{
	static RandomGenerator rnd(0);
	return rnd;
}





#ifdef IPHONE
//TODO! -> ? http://stackoverflow.com/questions/12762418/how-to-enable-sigfpe-signal-on-division-by-zero-in-ios-app
void fpExceptInit()
{}

void fpExceptEnable()
{}

void fpExceptDisable()
{}
#endif

#ifdef MACOS
//TODO...?

void fpExceptInit()
{}

void fpExceptEnable()
{}

void fpExceptDisable()
{}
#endif


#if defined LINUX || defined TIZEN || defined __ANDROID__

#include <fenv.h>

void fpExceptInit()
{}

void fpExceptEnable()
{
	feclearexcept(FE_DIVBYZERO);
	feenableexcept(FE_DIVBYZERO);
}

void fpExceptDisable()
{
	fedisableexcept(FE_DIVBYZERO);
}

#endif



#ifdef __BORLANDC__
// there was once a problem like this:
// http://qc.embarcadero.com/wc/qcmain.aspx?d=5128
// http://www.delorie.com/djgpp/doc/libc/libc_112.html
// ? http://www.c-jump.com/CIS77/reference/Intel/CIS77_24319002/pg_0211.htm
// ? http://www.jaist.ac.jp/iscenter-new/mpc/altix/altixdata/opt/intel/vtune/doc/users_guide/mergedProjects/analyzer_ec/mergedProjects/reference_olh/mergedProjects/instructions/instruct32_hh/vc100.htm
// ? http://www.plantation-productions.com/Webster/www.artofasm.com/Linux/HTML/RealArithmetica2.html
// http://blogs.msdn.com/b/oldnewthing/archive/2008/07/03/8682463.aspx
// where each cast of a double into an int would cause an exception. 
// But it was resolved by restarting windows and cleaning all intermediate compilation files :o (restarting windows was the key element! restarting BC++Builder and deleting files would not help)

#include "log.h"

unsigned int fp_control_word_std;
unsigned int fp_control_word_muted;

void fpExceptInit()
{
	//unsigned int was=_clear87();
	//logPrintf("","fpExceptInit",LOG_INFO,"control87 status before clear was %08x", was);
	fp_control_word_std = _control87(0, 0);             //4978 = 1001101110010
	// Make the new fp env same as the old one, except for the changes we're going to make
	fp_control_word_muted = fp_control_word_std | EM_INVALID | EM_DENORMAL | EM_ZERODIVIDE | EM_OVERFLOW | EM_UNDERFLOW | EM_INEXACT;  //4991 = 1001101111111
}

void fpExceptEnable()
{
	unsigned int was = _clear87(); //trzeba czyscic zeby nie bylo exception...
	//logPrintf("","fpExceptEnable ",LOG_INFO,"control87 status before clear was %08x", was);
	_control87(fp_control_word_std, 0xffffffff);
	//logPrintf("","fpExceptEnable ",LOG_INFO,"control87 flags are %08x", _control87(0, 0)); //kontrola co sie ustawilo
}

void fpExceptDisable()
{
	unsigned int was = _clear87(); //trzeba czyscic zeby nie bylo exception...
	//logPrintf("","fpExceptDisable",LOG_INFO,"control87 status before clear was %08x", was);
	_control87(fp_control_word_muted, 0xffffffff);
	//logPrintf("","fpExceptDisable",LOG_INFO,"control87 flags are %08x", _control87(0, 0)); //kontrola co sie ustawilo
}

#endif



#ifdef _MSC_VER
//Moznaby zrobic tak jak pod linuxem czyli wlaczyc exceptiony na poczatku i wylaczac na chwile przy dzieleniu w extvalue.
//To by pozwoli³o na wy³apanie pod visualem z³ych sytuacji kiedy framsy licz¹ na NaN, INF itp.
//http://stackoverflow.com/questions/2769814/how-do-i-use-try-catch-to-catch-floating-point-errors
void fpExceptInit() {}
void fpExceptEnable() {}
void fpExceptDisable() {}
#endif
