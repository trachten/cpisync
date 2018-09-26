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
    kshinglingSync(size_t shingle_len,GenSync::SyncProtocol base_sync_protocol, long edit_distance, long symbol_size=8);

    ~kshinglingSync();

    GenSync SyncHost(string str);
    forkHandleReport SyncServer(GenSync & server, GenSync & client);

    GenSync usingCPISync();

    string getString(K_Shingle host_content, int cycle_num){
        return host_content.reconstructStringBacktracking(cycle_num).first;
    };

    vector<pair<string,int>> getShingles(K_Shingle host_content){
        return host_content.getShingleSet();
    };

    string getHostStr(GenSync host);
//    /**
//     * @return print a string that is reconstructed from object's current set of shingles
//     */
//    string printString();

    // Get the name of the sync method
    string getName();

protected:
    bool oneWay;

private:
    GenSync::SyncProtocol setSyncProtocol;
    long bits;
    long mbar;
    size_t k;  //shingle size
    int cycleNum;
};

#endif //CPISYNCLIB_KSHINGLINGSYNC_H
