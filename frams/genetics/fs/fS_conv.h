//
// Created by jakub on 22.02.2020.
//


#ifndef FS_FS_CONV_H
#define FS_FS_CONV_H

#include <iostream>
#include "fS_general.h"
#include "frams/util/sstring.h"
#include "frams/util/multimap.h"

using namespace std;

class GenoConv_fS0 : public GenoConverter
{
public:
    GenoConv_fS0() :GenoConverter()
    {
        name = "Solid encoding";

        in_format = 'S';
        out_format = '0';
        mapsupport = 0;
    }
    /// Return empty string if can not convert
    SString convert(SString &i, MultiMap *map, bool using_checkpoints);
    ~GenoConv_fS0() {};
};
#endif //FS_FS_CONV_H
