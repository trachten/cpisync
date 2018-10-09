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

    void plot2D(string label, long X, long Y){
        if (data2D.find(label)==data2D.end()) { // if no label of such kind is in there
            vector<long [2]> *tmp;
            tmp->push_back({X,Y});
            data2D.insert(make_pair(label,*tmp));
        } else{
            (data2D[label]).push_back({X,Y});
        }

    };
    void plot3D(string label, long X, long Y, long Z){
        data3D[label].push_back({X,Y,Z});
    };

    void write2file(){
        ofstream myfile;
        myfile.open("./tests/perf/"+fileName+".txt");

        for (auto item : data3D){
            myfile << "Label:"+item.first+"\n";
            string tmpx, tmpy, tmpz;
            for (auto item : item.second){
                tmpx += item[0];
                tmpy += item[1];
                tmpz += item[2];
            }
            myfile << "X:"+tmpx+"\n";
            myfile << "Y:"+tmpy+"\n";
            myfile << "Z:"+tmpz+"\n";
        }

        for (auto item : data2D){
            myfile << "Label:"+item.first+"\n";
            string tmpx, tmpy;
            for (auto item : item.second){
                tmpx += item[0];
                tmpy += item[1];
            }
            myfile << "X:"+tmpx+"\n";
            myfile << "Y:"+tmpy+"\n";
        }

        myfile.close();
    }

private:
    map<string,vector<long [3]>> data3D; // map<label,content>
    map<string,vector<long [2]>> data2D; // map<label,content>
    string fileName;
};

#endif //CPISYNCLIB_GENPLOT_H
