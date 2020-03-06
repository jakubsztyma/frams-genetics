// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#include "defgenoconv.h"

#include GEN_CONFIG_FILE

#ifdef USE_GENCONV_f10
#include "f1/f1_conv.h"
#endif
#ifdef USE_GENCONV_f20
#include "f2/f2_conv.h"
#endif
#ifdef USE_GENCONV_f32
#include "f3/f3_conv.h"
#endif
#ifdef USE_GENCONV_f40
#include "f4/f4_conv.h"
#endif
#ifdef USE_GENCONV_f41_TEST
#include "f4/f4_conv.h"
#endif
#ifdef USE_GENCONV_f50
#include "f5/f5_conv.h"
#endif
#ifdef USE_GENCONV_f60
#include "f6/f6_geno.h"
#endif
#ifdef USE_GENCONV_f70
#include "f7/f7_conv.h"
#endif
#ifdef USE_GENCONV_f81     
#include "f8/f8_conv.h"
#endif
#ifdef USE_GENCONV_f90
#include "f9/f9_conv.h"
#endif
#ifdef USE_GENCONV_fF0
#include "fF/fF_conv.h"
#endif
#ifdef USE_GENCONV_fn0
#include "fn/fn_conv.h"
#endif
#ifdef USE_GENCONV_fBH
#include "fB/fB_conv.h"
#endif
#ifdef USE_GENCONV_fH0
#include "fH/fH_conv.h"
#endif
#ifdef USE_GENCONV_fL0
#include "fL/fL_conv.h"
#endif

void DefaultGenoConvManager::addDefaultConverters()
{
#ifdef USE_GENCONV_f10
	addConverter(new GenoConv_f1());
#endif
#ifdef USE_GENCONV_f20
	addConverter(new GenoConv_f20());
#endif
#ifdef USE_GENCONV_f32
	addConverter(new GenoConv_f32());
#endif
#ifdef USE_GENCONV_f40
	addConverter(new GenoConv_f40());
#endif
#ifdef USE_GENCONV_f41_TEST
	addConverter(new GenoConv_f41_TestOnly());
#endif
#ifdef USE_GENCONV_f50
	addConverter(new GenoConv_f50);
#endif
#ifdef USE_GENCONV_f60
	addConverter(new GenoConv_f60);
#endif
#ifdef USE_GENCONV_f70
	addConverter(new GenoConv_f70);
#endif
#ifdef USE_GENCONV_f81
	addConverter(new GenoConv_F8ToF1()); //uncompilable for now... needs lemon, and still borland complains for a hundred of unknown reasons with all kinds of nonsense messages :/
#endif
#ifdef USE_GENCONV_f90
	addConverter(new GenoConv_f90);
#endif
#ifdef USE_GENCONV_fF0
	addConverter(new GenoConv_fF0);
#endif
#ifdef USE_GENCONV_fn0
	addConverter(new GenoConv_fn0);
#endif
#ifdef USE_GENCONV_fBH
	addConverter(new GenoConv_fBH);
#endif
#ifdef USE_GENCONV_fH0
	addConverter(new GenoConv_fH0);
#endif
#ifdef USE_GENCONV_fL0
	addConverter(new GenoConv_fL0);
#endif

	param.updatetab();
}
