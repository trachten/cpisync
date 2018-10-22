//
// Created by Bowen Song on 10/19/18.
//
// Strata Estimator


#ifndef CPISYNCLIB_STRATAEST_H
#define CPISYNCLIB_STRATAEST_H

/**
 * Strata Estimator:
 * Estimate Set Difference by sampling
 *
 * Eppstein, David, et al. "What's the difference?: efficient set reconciliation without prior context."
 * ACM SIGCOMM Computer Communication Review. Vol. 41. No. 4. ACM, 2011.
 */

#include <vector>
#include <utility>
#include <string>
#include <NTL/ZZ.h>
#include <sstream>
#include "IBLT.h"
#include "Auxiliary.h"

using std::vector;
using std::hash;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;


typedef unsigned long int hashVal;

typedef vector<IBLT> IBF;

// Preset IBF Parameters
const int idSumByteSize = 4, hashSumByteSize = 4, countByteSize = 4;


// use H_z to hash each set element as we sample

// 16 / 32 number of strata and 80 cells by default

class StrataEst{
public:

    friend class Communicant;

    StrataEst(int num_strata, int num_cells, int value_size);

    ~StrataEst();

    void insert(ZZ item);

    void estimate(IBF myIBF);

private:
    int numStrata, numCells, bits;



};






#endif //CPISYNCLIB_STRATAEST_H
