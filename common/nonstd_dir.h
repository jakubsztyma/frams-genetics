// This file is a part of Framsticks SDK.  http://www.framsticks.com/
// Copyright (C) 1999-2015  Maciej Komosinski and Szymon Ulatowski.
// See LICENSE.txt for details.

#ifndef _NONSTD_DIR_H_
#define _NONSTD_DIR_H_

#if defined _WIN32 && !defined SHP && !defined __BORLANDC__
#include "dirent_windows.h"
#else
#ifdef SHP
#include "dirent_bada.h"
#else
#include <dirent.h>
#endif
#endif

#endif

