//
// Created by eliez on 8/10/2018.
//

#ifndef CPISYNCLIB_IBLTSYNC_HALFROUND_H
#define CPISYNCLIB_IBLTSYNC_HALFROUND_H

#include "IBLT.h"
#include "IBLTSync.h"

class IBLTSync_HalfRound : public IBLTSync {
public:
    IBLTSync_HalfRound(size_t expected, size_t eltSize) : IBLTSync(expected, eltSize) {
        oneWay = true;
    }

    string getName() override {return IBLTSync::getName() + "\n*one-way";}
};

#endif //CPISYNCLIB_IBLTSYNC_HALFROUND_H
