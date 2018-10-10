//
// Created by Bowen on 10/9/18.
//

#ifndef CPISYNCLIB_PERFORMANCEDATA_H
#define CPISYNCLIB_PERFORMANCEDATA_H

#include "GenSync.h"
#include "kshinglingSync.h"
#include "GenPlot.h"

class PerformanceData {
public:
    PerformanceData();
    ~PerformanceData();

    enum class StringReconProtocol {
        UNDEFINED,
        BEGIN,
        KshinglingSync=BEGIN,
        END
    };

    /**
     * Perpare a shing recon method for random string and random edits testing
     * @param string_size size of the string
     * @param shingle_len shingle length
     * @param edit_distance randome edits
     */
    void prepareStringRecon(int string_size, int shingle_len, int edit_distance);

    /**
     * Prepare a set recon protocol include CPIsync, InteractiveCPIsync, and IBLTsync
     * @param base_set_proto one of teh tree protocol
     */
    void prepareSetComm(StringReconProtocol string_recon_proto, GenSync::SyncProtocol base_set_proto, string Alice_txt = "", string Bob_txt="");

    /**
     * Generate a report for Time and Comm cost
     * @param check_outcome check if reconciliation is successful
     * @return cost report
     */
    void calCostReport(bool check_outcome=true);

    void genReport(){
      plot.write2file();
    };


private:
//    vector<ZZ> Alice_shingleSet;// final form will all be in ZZ for each shingle
//    vector<ZZ> Bob_shingleSet;// final form will all be in ZZ for each shingle
//    // assume configurations are the same, else put them in the set


    GenPlot plot;

    //set protocols
    GenSync::SyncProtocol baseSetProto;
    StringReconProtocol stringReconProto;

    string stringReconProtoName;
    string setReconProtoName;
    // parameters
    int stringSize, shingleLen, mbar, editDist;
    size_t bits;
};

#endif //CPISYNCLIB_PERFORMANCEDATA_H
