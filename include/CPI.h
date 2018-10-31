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
#include "DataObject.h"

using namespace NTL;

class CPI {
public:
    friend class Communicant;

    CPI(size_t value_size, size_t num_evals);

    ~CPI();

    bool set_reconcile(long otherSetSize, const vec_ZZ_p &otherEvals,
                       vec_ZZ_p &selfMinusTheirs, vec_ZZ_p &TheirsMinusSelf);

    bool insert(ZZ Elem);

    //Check the following Parameters
    size_t size() const { return numEvals; };

    size_t eltSize() const { return bits; };

    size_t getProtbEps() const { return probEps; };

private:
    DataObject *invHash(const ZZ_p num) const;

    ZZ_p hash(const DataObject *datum) const;

    ZZ_p hash2(const long num) const;

    bool addElem(DataObject *datum);

    bool delElem(DataObject *datum);

    bool ratFuncInterp(const vec_ZZ_p &evals, long mA, long mB, vec_ZZ_p &P_vec, vec_ZZ_p &Q_vec);

    bool find_roots(vec_ZZ_p &P_vec, vec_ZZ_p &Q_vec, vec_ZZ_p &numerator, vec_ZZ_p &denominator);

    ZZ_p makeData(ZZ_p num) const;

    long bits, numEvals, redundant_k, probEps;

    ZZ fieldSize, DATA_MAX;

    vec_ZZ_p sampleLoc, CPI_evals;

    map<ZZ, DataObject *> CPI_hash;

    bool hashQ = false;
};
#endif //CPISYNCLIB_CPI_H
