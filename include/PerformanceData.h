//
// Created by Bowen on 10/9/18.
//

#ifndef CPISYNCLIB_PERFORMANCEDATA_H
#define CPISYNCLIB_PERFORMANCEDATA_H

#include "GenSync.h"
#include "kshinglingSync.h"
#include "StrataEst.h"
#include "ForkHandle.h"
#include <fstream>
#include <omp.h>
#include <numeric>

#ifdef DEBUG
#define DEBUG_IF(cond) if(true)
#else
#define DEBUG_IF(cond) if(false)
#endif

class PerformanceData {
public:
    PerformanceData(int tes_pts) {
        tesPts = tes_pts;
    };

    ~PerformanceData();

    enum class StringReconProtocol {
        UNDEFINED,
        BEGIN,
        KshinglingSync = BEGIN,
        END
    };

    enum class PlotType {
        PLOT2D,
        PLOT3D,
        BOTH
    };
//
//    void StrKDif3D(pair<int, int> str_sizeRange, pair<int, int> shingle_lenRange, int edit_dist);
//
//    void StrEDDif3D(pair<int, int> str_sizeRange, pair<int, int> edit_distRange, int shingle_len);
//
//    void EDKDif3D(pair<int, int> edit_distRange, pair<int, int> shingle_lenRange, int str_size);
//
//
//    void KDif2D(pair<int, int> shingle_lenRange, int str_size, int edit_dist, int confidence);
//
//    void EDDif2D(pair<int, int> edit_distRange, int str_size, int shingle_len, int confidence);
//
//    void StrDif2D(pair<int, int> str_sizeRange, int shingle_len, int edit_dist, int confidence);

    /**
     * Perpare a shing recon method for random string and random edits testing
     * @param string_size size of the string
     * @param shingle_len shingle length
     * @param edit_distance randome edits
     */
    //void prepareStringRecon(int string_size, int shingle_len, int edit_distance, string Alice_txt = "",
    //                        string Bob_txt = "");

    /**
     * Prepare a set recon protocol include CPIsync, InteractiveCPIsync, and IBLTsync
     * @param base_set_proto one of teh tree protocol
     */
    //void prepareSetComm(StringReconProtocol string_recon_proto, GenSync::SyncProtocol base_set_proto);

    /**
     * Generate a report for Time and Comm cost
     * @param check_outcome check if reconciliation is successful
     * @return cost report
     */
    //forkHandleReport calCostReport(PlotType plot_type, bool check_outcome = true);


//
//    void kshingle2D(list<GenSync::SyncProtocol> setReconProto, pair<int, int> edit_distRange,
//                    int shingle_len, int str_size, int target_confidence);
//
//    void kshingleStr2D(list<GenSync::SyncProtocol> setReconProto, pair<int, int> strSizeRange, int shingleLen,
//                       int editDist, int target_confidence);
//
    void kshingle3D(list<GenSync::SyncProtocol> setReconProto, pair<int, int> edit_distRange,
                    pair<int, int> str_sizeRange, int shingle_len);
    void kshingleCode3D(pair<int, int> edit_distRange,    pair<int, int> str_sizeRange);

    void kshingleBook3D(pair<int, int> edit_distRange,    pair<int, int> str_sizeRange);

    void strataEst3D(pair<size_t,size_t> set_sizeRange, int confidence);

private:
//    vector<ZZ> Alice_shingleSet;// final form will all be in ZZ for each shingle
//    vector<ZZ> Bob_shingleSet;// final form will all be in ZZ for each shingle
//    // assume configurations are the same, else put them in the set

    //set protocols
    GenSync::SyncProtocol baseSetProto;
    StringReconProtocol stringReconProto;

    string stringReconProtoName;
    string setReconProtoName;
    // parameters
    int stringSize, shingleLen, mbar, editDist, numParts, tesPts;
    size_t bits;

    string AliceTxt, BobTxt;



    /**
     * explore relation of edit distance and set difference in kshingling
     * @param shingle_len
     * @param str_size
     * @param edit_dist
     */
    int setdiff(int shingle_len, int str_size, int edit_dist);


};


class PlotRegister { // Export Data into a txt file for external code to graph
public:
    PlotRegister(string _title, vector<string> _labels);// init - open a file with a title and labels

    ~PlotRegister();

    void add(vector<string> datum); // add to data
    void update(); // bulk insert to what is in the data, clear data, close file after.
private:

    void init();

    string title; // title of the graph, used as file name
    vector<string> labels; //label including units
    vector<vector<string>> data; // Number of
};

#endif //CPISYNCLIB_PERFORMANCEDATA_H
