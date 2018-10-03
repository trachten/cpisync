//
// Created by Bowen on 10/3/18.
// This is for adjacency matrix
// Allow negative weights for directed graph
//
//

#ifndef CPISYNCLIB_ADJMTX_H
#define CPISYNCLIB_ADJMTX_H

#include <NTL/ZZ.h>
#include <vector>
#include <utility>
#include <string>
#include "Auxiliary.h"
#include "DataObject.h"
#include <map>

using std::vector;
using std::string;
using std::stringstream;
using std::pair;
using namespace NTL;

class AdjMtx {
public:
    // CONTRUCTOR AND DISTRUCTOR
    AdjMtx();
    ~AdjMtx();

    /**
     * append to vertex list
     * @param shingle <ZZ>
     * @return
     */
    bool addNewVex(ZZ shingle);
//    bool addNewVex(string shingle);

    void create(vector<ZZ> _ver);
//    void create(vector<string> _ver);

    vector<ZZ> getGraphVex(){
        return vertex;
    }

    int getGraphSize(){
        return vertex.size();
    }
    /**
     * check if vex is part of Vertex
     * @param vex
     * @return
     */
    bool contains(ZZ vex){
        return (find(vertex.begin(), vertex.end(), vex) != vertex.end());
    }


    int getWeight(ZZ vfrom, ZZ vto);

    bool addWeigth(ZZ vfrom, ZZ vto, int add_weight=1);

    bool setWeight(ZZ vfrom, ZZ vto, int set_weight);

    void printGraph(vector<ZZ> print_vertex);
    void printGraph();
    /**
     * change the weight of
     * @param vfrom
     * @param vto
     * @param del_weight
     * @return
     */
    bool delWeigth(ZZ vfrom, ZZ vto, int del_weight=1);

private:
    map<pair<ZZ, ZZ >, int > graph;  // if two vex tex is not connected, it would not be in the graph.
    vector<ZZ> vertex;


};


#endif //CPISYNCLIB_ADJMTX_H
