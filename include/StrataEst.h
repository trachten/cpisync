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
#include "DataObject.h"
#include "IBLT.h"
#include "Auxiliary.h"

using std::vector;
using std::hash;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;


typedef unsigned long int hashVal;

// Preset IBF Parameters
//const size_t idSumByteSize = 4, hashSumByteSize = 4, countByteSize = 4;


// use H_z to hash each set element as we sample

// 16 / 32 number of strata and 40 / 80 cells by default

class StrataEst{
public:

    /**
     * Construct a vector of IBF Hierarchy as Strata
     * @param num_strata default value 16 / 32
     * @param num_cells default value 40 / 80
     * @param value_size default value 8
     */
    StrataEst(size_t value_size, size_t num_strata = 16, size_t num_cells=40);

    ~StrataEst();

    void insert(DataObject * datum);

    /**
     * Estimate the set difference by examine subtracted Strata
     * @return Estimated set difference
     */
    size_t estimate();

    /**
     * Sample from set and pour into Strata Estimator
     * @return Strata Estimator
     */
    vector<IBLT> getStrata();

    /**
     * Subtracts two StrataEst.
     * -= is destructive and assigns the resulting StrataEst to the value
     * @require StrataEst must have the same number of Stratas, entries, and the values must be of the same size
     */
    StrataEst& operator-=(const StrataEst& other);

    /**
     * @return the number of cells in an IBF.
     */
    size_t IBFsize() const;

    /**
     * @return the number of IBFs in the Strata.
     */
    size_t size() const;

    /**
     * @return the size of a value stored in the IBF.
     */
    size_t eltSize() const;

private:

    // assume all inputs are unique.
    void _insert(ZZ item);

    // inject the element into the strata for a specific IBLT
    void _inject(ZZ& item, int index);

    /**
     * Get the index of the IBF what this Elem is assigned to. The bigger the number less the chance
     * @param Elem
     * @return index of the assignent for the IBF from 0
     */
    int get_Assign_Ind(ZZ& Elem);

    size_t numStrata, numCells, bits, space;

    vector<IBLT> Strata;

    vector<ZZ> setElems;

};






#endif //CPISYNCLIB_STRATAEST_H
