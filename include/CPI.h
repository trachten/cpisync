//
// Created by Bowen Song on 10/29/18.
//

#ifndef CPISYNCLIB_CPI_H
#define CPISYNCLIB_CPI_H

#include <NTL/RR.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_pXFactoring.h>
#include "Auxiliary.h"

using namespace NTL;

class CPI {
public:
    friend class Communicant;
    CPI(size_t value_size, size_t num_evals);

    ~CPI();
    bool set_reconcile(long otherSetSize, const vec_ZZ_p& otherEvals, vec_ZZ_p &delta_self, vec_ZZ_p &delta_other);

private:
    size_t bits, numEvals, redundant_k;

   ZZ fieldSize;

   ZZ_p makeData(ZZ_p num) const;
};
#endif //CPISYNCLIB_CPI_H
