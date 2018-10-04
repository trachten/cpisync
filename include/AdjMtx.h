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

    bool addNewVex(string shingle);
    /**
     * Add a list of vertex at once
     * @param _ver list of shingles
     */
    void create(vector<ZZ> _ver);
    void create(vector<string> _ver);

    /**
     * list of shingles
     * @return list of vertices
     */
    vector<ZZ> getGraphVex(){
        return vertex;
    }

    /**
     * number pf verticies in the graph
     * @return
     */
    int getGraphSize(){
        return vertex.size();
    }

    void sortVex();

    /**
     * check if vex is one of the Vertices
     * @param vex
     * @return
     */
    bool contains(ZZ vex){
        return (find(vertex.begin(), vertex.end(), vex) != vertex.end());
    }


    int getWeight(ZZ vfrom, ZZ vto);

    ZZ StrtotheZZ(const string& str);
    string ZZtotheStr(const ZZ& zz);

    /**
     * print the graph of concerned vertex
     * @param print_vertex ONLY applicable to string input
     */
    void printGraph(vector<ZZ> print_vertex);
    void printGraph();
    /**
     * change the weight of graph, allow negative
     * @param vfrom
     * @param vto
     * @param del_weight
     * @return
     */
    bool addWeigth(ZZ vfrom, ZZ vto, int add_weight=1);
    bool setWeight(ZZ vfrom, ZZ vto, int set_weight);
    bool delWeigth(ZZ vfrom, ZZ vto, int del_weight=1);

private:
    map<pair<ZZ, ZZ >, int > graph;  // if two vex tex is not connected, it would not be in the graph.
    vector<ZZ> vertex;


};


#endif //CPISYNCLIB_ADJMTX_H
