//
// Created by Bowen on 10/9/18.
//

#ifndef CPISYNCLIB_PERFORMANCEDATA_H
#define CPISYNCLIB_PERFORMANCEDATA_H

#include "GenSync.h"
#include "kshinglingSync.h"
#include "StrataEst.h"
#include <fstream>

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

    void StrKDif3D(pair<int, int> str_sizeRange, pair<int, int> shingle_lenRange, int edit_dist);

    void StrEDDif3D(pair<int, int> str_sizeRange, pair<int, int> edit_distRange, int shingle_len);

    void EDKDif3D(pair<int, int> edit_distRange, pair<int, int> shingle_lenRange, int str_size);


    void KDif2D(pair<int, int> shingle_lenRange, int str_size, int edit_dist, int confidence);

    void EDDif2D(pair<int, int> edit_distRange, int str_size, int shingle_len, int confidence);

    void StrDif2D(pair<int, int> str_sizeRange, int shingle_len, int edit_dist, int confidence);

    /**
     * Perpare a shing recon method for random string and random edits testing
     * @param string_size size of the string
     * @param shingle_len shingle length
     * @param edit_distance randome edits
     */
    void prepareStringRecon(int string_size, int shingle_len, int edit_distance, string Alice_txt = "",
                            string Bob_txt = "");

    /**
     * Prepare a set recon protocol include CPIsync, InteractiveCPIsync, and IBLTsync
     * @param base_set_proto one of teh tree protocol
     */
    void prepareSetComm(StringReconProtocol string_recon_proto, GenSync::SyncProtocol base_set_proto);

    /**
     * Generate a report for Time and Comm cost
     * @param check_outcome check if reconciliation is successful
     * @return cost report
     */
    forkHandleReport calCostReport(PlotType plot_type, bool check_outcome = true);

    void genReport(string file_name) {
        write2file(file_name);
    };

    void kshingle2D(list<GenSync::SyncProtocol> setReconProto, pair<int, int> edit_distRange,
                    int shingle_len, int str_size, int target_confidence);

    void kshingleStr2D(list<GenSync::SyncProtocol> setReconProto, pair<int, int> strSizeRange, int shingleLen,
                       int editDist, int target_confidence);

    void kshingle3D(list<GenSync::SyncProtocol> setReconProto, pair<int, int> edit_distRange,
                    pair<int, int> str_sizeRange, int shingle_len);

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


    //GenPlot
    map<string, vector<vector<double>>> data3D, data2D, data4D; // map<label,content(X,Y,Z)> // map<label,content(X,Y)>

    /**
    * Store Values in Genplot class - for 2D plots
    * Find the right label and add the data
    * @param label
    * @param X
    * @param Y
    */
    void plot2D(string label, double X, double Y);

    /**
     * Store Values in Genplot class - for 3D plots
     * Find the right label and add the data
     * @param label
     * @param X
     * @param Y
     * @param Z
     */
    void plot3D(string label, double X, double Y, double Z);

    void plot4D(string label, double X, double Y, double Z, double A);

    /**
     * Export Data into a txt file for python code to process
     * Every instance of a class is one data file
     */
    void write2file(string file_name);

    /**
     * explore relation of edit distance and set difference in kshingling
     * @param shingle_len
     * @param str_size
     * @param edit_dist
     */
    int setdiff(int shingle_len, int str_size, int edit_dist);


};

#endif //CPISYNCLIB_PERFORMANCEDATA_H
