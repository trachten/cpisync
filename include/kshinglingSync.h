//
// Created by Bowen Song on 9/23/18.
// Use one or all set recon methods
//

#ifndef CPISYNCLIB_KSHINGLINGSYNC_H
#define CPISYNCLIB_KSHINGLINGSYNC_H

#include "CPISync.h"
#include "kshingling.h"

class kshinglingSync : public SyncMethod {
public:
    // Constructors and destructors
    /**
     * Constructor for K-shingling Sync
     * @param edit_distance_bar Edit distance upper bound (used for single round sync)
     * @param bits_symbol Bits per symbol (ascii symbols are 8 bits)
     * @param k shingle size; Using CPI sync, m_bar equals edit_distance_bar*k since each edit_distance_bar gets repeated at most k times
     */
    kshinglingSync(int edit_distance_bar, size_t k, long bits_symbol);
    ~kshinglingSync();

    /**
     * Add a string in to a host
     * @param datum string
     * @return add success, which should always be the case unless it contains stop words
     */
    bool addElem(DataObject* datum) override;

    /**
     * @return print a string that is reconstructed from object's current set of shingles
     */
    string printString();

    // Get the name of the sync method
    string getName() override;
private:
    K_Shingle myK_Shingle;
    int k;  //shingle size
};

#endif //CPISYNCLIB_KSHINGLINGSYNC_H
