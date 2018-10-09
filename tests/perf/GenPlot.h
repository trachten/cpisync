//
// Created by Bowen Song on 10/8/18.
//

#ifndef CPISYNCLIB_GENPLOT_H
#define CPISYNCLIB_GENPLOT_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>



using namespace std;

class GenPlot {
public:

    /**
     * Open a file
     * @param file_name file is within the "perf" directory by default
     */
    GenPlot(string file_name){
        fileName = file_name;
    };

    /**
     * close file
     */
    ~GenPlot();

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
    void write2file();

private:
    map<string,vector<vector<long>>> data3D; // map<label,content(X,Y,Z)>
    map<string,vector<vector<long>>> data2D; // map<label,content(X,Y)>
    string fileName;
};

#endif //CPISYNCLIB_GENPLOT_H
