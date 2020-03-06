//
// Created by jakub on 22.02.2020.
//


#ifndef FS_FS_CONV_H
#define FS_FS_CONV_H

#include <iostream>
#include "fS_general.h"
#include "frams/util/sstring.h"

using namespace std;

class GenoConv_fS0// : public GenoConverter
{
public:
    GenoConv_fS0() {}// :GenoConverter()
    SString convert(SString &i);//(SString &i, MultiMap *map, bool using_checkpoints);
    ~GenoConv_fS0() {};
};
#endif //FS_FS_CONV_H
