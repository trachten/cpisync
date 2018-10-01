//
// Created by Bowen Song on 9/23/18.
// Use one or all set recon methods
//

#ifndef CPISYNCLIB_KSHINGLINGSYNC_H
#define CPISYNCLIB_KSHINGLINGSYNC_H

#include "GenSync.h"
#include "SyncMethod.h"
#include "Auxiliary.h"
#include "kshingling.h"
#include "ForkHandle.h"

class kshinglingSync{
public:
    // Constructors and destructors
    /**
     * Constructor for K-shingling Sync
     * @param edit_distance_bar Edit distance upper bound (used for single round sync)
     * @param bits_symbol Bits per symbol (default is ascii symbols which are 8 bits)
     * @param k shingle size; Using CPI sync, m_bar equals edit_distance_bar*k since each edit_distance_bar gets repeated at most k times
     */
    kshinglingSync(GenSync::SyncProtocol sync_protocol,GenSync::SyncComm sync_comm,
                   size_t symbol_size , int m_bar, int num_Parts, int num_ExpElem);

    ~kshinglingSync();

    GenSync SyncHost(string str,K_Shingle& host_content);
    forkHandleReport SyncNreport(GenSync& server, GenSync client);

    multiset<pair<string,int>> getShingles(K_Shingle& host_content){
        multiset<pair<string,int>> res;
        for(auto item : host_content.getShingleSet())
            res.insert(item);
        return res;
    };

    string getString(GenSync host,K_Shingle& host_content);
//    /**
//     * @return print a string that is reconstructed from object's current set of shingles
//     */
//    string printString();

    // Get the name of the sync method
    string getName();


private:
    GenSync::SyncProtocol setSyncProtocol;
    GenSync::SyncComm setSyncComm;
    long bits;
    long mbar;
    int cycleNum;
    int numParts;
    int numExpElem;
    //K_Shingle host_content;
};

#endif //CPISYNCLIB_KSHINGLINGSYNC_H
