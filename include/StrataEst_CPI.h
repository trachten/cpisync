//
// Created by Bowen Song on 10/26/18.
//

#ifndef CPISYNCLIB_STRATAEST_CPI_H
#define CPISYNCLIB_STRATAEST_CPI_H

#include "Auxiliary.h"
#include "Communicant.h"
#include "Exceptions.h"
#include "Logger.h"
#include "DataObject.h"
#include "CPISync.h"
#include "InterCPISync.h"
#include "CPISync_HalfRound.h"
#include "CPISync_HalfRound_Hashed.h"
#include "CPISync_ExistingConnection.h"

class StrataEst_CPI {
public:
    StrataEst_CPI(size_t value_size, size_t num_strata = 16, size_t num_Eval_Pts = 40);

    ~StrataEst_CPI();

    // assume all inputs are unique.
    void insert(DataObject * datum);

    /**
     * Estimate the set difference by examine subtracted Strata
     * @return Estimated set difference
     */
    size_t estimate();


private:
size_t bits, numStrata, space, numEvalPts;
vector<CPISync> Strata;


    // inject the element into the strata for a specific IBLT
    void _inject(DataObject* item, int index);
    /**
     * Get the index of the IBF what this Elem is assigned to. The bigger the number less the chance
     * @param Elem
     * @return index of the assignent for the IBF from 0
     */
    int get_Assign_Ind(DataObject* Elem);

};

#endif //CPISYNCLIB_STRATAEST_CPI_H
