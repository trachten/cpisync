//
// Created by Zifan Wang on 8/26/19
//

#ifndef CPISYNCLIB_STRATA_H
#define CPISYNCLIB_STRATA_H


#include <CPISync/Syncs/IBLT.h>
#include <CPISync/Data/DataObject.h>


using std::vector;

class Strata{
    public:        
        /**
         * Initialize a strata
         * @param elemSize size of elements in the inner IBLT
         * @param innerLen length of IBLT
         * @param outerLen length of strata
         **/
        Strata(size_t elemSize, size_t innerLen, size_t outerLen);

        /**
         * Initialize a strata with given IBLTs
         * @param datums IBLTs to be put into current Strata
         **/
        Strata(const vector<IBLT> &datums);

        ~Strata();

        /**
         * Minus operation for strata structure. Each cell of strata will perform IBLT subtraction
         * */
        Strata &operator -=(const Strata &other);


        /**
         * Insert a dataobject into strata
         * @param datum the pointer of data object to be inserted
         * */
        void insert(shared_ptr<DataObject> &datum);

        /**
         * Delete a dataobject from strata
         * @param datum the pointer of data object to be deleted
         * */
        void erase(shared_ptr<DataObject> &datum);

        /**
         * decode function and get the estimated number of symm difference
         * @require should perform subtraction with another set's strata before decoding
         * @return estimated symm difference with a probablity no less than the actual difference
         * */
        long decode();

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
        
        /**
         * Get an IBF in current strata
         * @param index target index for the IBF
         * @return IBF
         **/
        IBLT getCell(long index);




    private:
        Strata();

        /**
         * Calculate the index for element to be inserted into strata according to its trailing zeros
         * @param elem the element to be inserted
         * @return the index of IBFs in current strata
         * */
        long _getIndex(const ZZ elem);

        /* size of elements, length of IBFs, length of strata */
        size_t elmSize, inLen, outLen;    

        /* A data structure bookkeeping all IBFs in current strata */
        vector<IBLT> cells;

};














#endif // CPISYNCLIB_STRATA_H