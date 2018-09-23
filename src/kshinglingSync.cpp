//
// Created by Bowen Song on 9/23/18.
//

#include <Exceptions.h>
#include "kshinglingSync.h"


string kshinglingSync::getName(){ return "This is a kshinglingSync:\n*Each Shingle size: " + toStr(k) + "\n*size (bits) of shingle set: " + toStr(myK_Shingle.set_bit_size());}