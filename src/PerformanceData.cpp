////
//// Created by Bowen on 10/9/18.
////

#include "PerformanceData.h"






// Graph and Plot functions

PlotRegister::PlotRegister(string _title, vector<string> _labels) : title(_title), labels(_labels) {
    init();
}

PlotRegister::~PlotRegister() {}

void PlotRegister::init() {
    ofstream myfile;
    //TODO: do soemthing about the directories, this hard coding is not a long term solution
    myfile.open(title + ".txt");

    myfile<< accumulate(std::next(labels.begin()), labels.end(),
                    labels[0], // start with first element
                    [](string a, string b) {
                        return a + "|" + b;
                    }) + "\n";


//    myfile<<accumulate(labels.begin(), labels.end(), string("|")) + "\n";
    myfile.close();
}

void PlotRegister::update() {
    ofstream myfile(title+".txt",ios::app);
    if (!myfile.good()) init();
    for (auto datum : data)
        myfile<< accumulate(next(datum.begin()), datum.end(),
                            datum[0], // start with first element
                            [](string a, string b) {
                                return a + " " + b;
                            }) + "\n";
    data.clear();
    myfile.close();
}

void PlotRegister::add(vector<string> datum) {
    data.push_back(datum);
}

