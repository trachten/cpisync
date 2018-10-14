//
// Created by Bowen on 10/9/18.
//

#ifndef CPISYNCLIB_PERFORMANCEDATA_H
#define CPISYNCLIB_PERFORMANCEDATA_H

#include "GenSync.h"
#include "kshinglingSync.h"
#include <fstream>

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

    enum class PlotType {
        PLOT2D,
        PLOT3D,
        BOTH
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
    void calCostReport(PlotType plot_type, bool check_outcome=true);

    void genReport(string file_name){
      write2file(file_name);
    };

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
    int stringSize, shingleLen, mbar, editDist, numParts;
    size_t bits;

    string AliceTxt, BobTxt;



    //GenPlot
    map<string,vector<vector<long>>> data3D; // map<label,content(X,Y,Z)>
    map<string,vector<vector<long>>> data2D; // map<label,content(X,Y)>
    string fileName;


    /**
  * Store Values in Genplot class - for 2D plots
  * Find the right label and add the data
  * @param label
  * @param X
  * @param Y
  */
    void plot2D(string label, long X, long Y);
    /**
     * Store Values in Genplot class - for 3D plots
     * Find the right label and add the data
     * @param label
     * @param X
     * @param Y
     * @param Z
     */
    void plot3D(string label, long X, long Y, long Z);

    /**
     * Export Data into a txt file for python code to process
     * Every instance of a class is one data file
     */
    void write2file(string file_name);
};

#endif //CPISYNCLIB_PERFORMANCEDATA_H
