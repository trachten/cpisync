//
// Created by Bowen Song on 10/8/18.
//

#include "GenPlot.h"

void GenPlot::plot2D(string label, long X, long Y){
    if (data2D.find(label)==data2D.end()) { // if no label of such kind is in there
        vector<long> tmp(2);
        tmp[0] = X;
        tmp[1] = Y;
        vector<vector<long>> init;
        init.push_back(tmp);
        data2D.insert(make_pair(label,init));
    } else{
        vector<long> tmp(2);
        tmp[0] = X;
        tmp[1] = Y;
        data2D[label].push_back(tmp);
    }

}

void GenPlot::plot3D(string label, long X, long Y, long Z){
    if (data3D.find(label)==data3D.end()) { // if no label of such kind is in there
        vector<long> tmp(3);
        tmp[0] = X;
        tmp[1] = Y;
        tmp[3] = Z;
        vector<vector<long>> init;
        init.push_back(tmp);
        data3D.insert(make_pair(label,init));
    } else{
        vector<long> tmp(3);
        tmp[0] = X;
        tmp[1] = Y;
        tmp[3] = Z;
        data3D[label].push_back(tmp);
    }
}

void GenPlot::write2file(){
    ofstream myfile;
    myfile.open("./tests/perf/"+fileName+".txt");

    for (auto item : data3D){
        myfile << "Label:"+item.first+"\n";
        string tmpx, tmpy, tmpz;
        for (auto item : item.second){
            tmpx += to_string(item[0])+",";
            tmpy += to_string(item[1])+",";
            tmpz += to_string(item[2])+",";
        }
        myfile << "X:"+tmpx+"\n";
        myfile << "Y:"+tmpy+"\n";
        myfile << "Z:"+tmpz+"\n";
    }

    for (auto item : data2D){
        myfile << "Label:"+item.first+"\n";
        string tmpx, tmpy;
        for (auto item : item.second){
            tmpx += to_string(item[0])+",";
            tmpy += to_string(item[1])+",";
        }
        myfile << "X:"+tmpx+"\n";
        myfile << "Y:"+tmpy+"\n";
    }

    myfile.close();
}
