//
// Created by Bowen on 10/3/18.
//

#include "AdjMtx.h"

void AdjMtx::create(vector<ZZ> _ver) {
    vertex = _ver;
}

void AdjMtx::create(vector<string> _ver) {
    for (string vex : _ver) {
        vertex.push_back(to_ZZ(vex.data()));
    }
}

bool AdjMtx::addNewVex(ZZ shingle) {
    for (auto item :vertex) {
        if (shingle == item) {
            return false;
        }
    }
    vertex.push_back(shingle);
    return true;
}

bool AdjMtx::addNewVex(string shingle) {
    return addNewVex(to_ZZ(shingle.data()));
}

bool AdjMtx::addWeigth(ZZ vfrom, ZZ vto, int add_weight){
    if (contains(vto) and contains(vfrom)) {
        pair<ZZ, ZZ> vexpair = make_pair(vfrom, vto);
        auto temp = graph.find(vexpair);
        if (temp != graph.end()) {
            temp->second += add_weight;
            return true;
        }else{
            graph.insert(make_pair(vexpair,add_weight));
        }
    }
    return false;
}

bool AdjMtx::setWeight(ZZ vfrom, ZZ vto, int set_weight){
    if (contains(vto) and contains(vfrom)) {
        pair<ZZ, ZZ> vexpair = make_pair(vfrom, vto);
        auto temp = graph.find(vexpair);
        if (temp != graph.end()) {
            temp->second = set_weight;
            return true;
        } else{
            graph.insert(make_pair(vexpair,set_weight));
        }
    }
    return false;
}

bool AdjMtx::delWeigth(ZZ vfrom, ZZ vto, int del_weight){
    if (contains(vto) and contains(vfrom)) {
        pair<ZZ, ZZ> vexpair = make_pair(vfrom, vto);
        auto temp = graph.find(vexpair);
        if (temp != graph.end()) {
            temp->second -= del_weight;
            return true;
        } else{
            graph.insert(make_pair(vexpair,-del_weight));
        }
    }
    return false;
}

bool AdjMtx::_exist(pair<ZZ,ZZ> vexpair){
    auto temp = graph.find(vexpair);
    if (temp != graph.end()){
        return true;
    }
    return false;
}

void AdjMtx::printGraph(vector<ZZ> print_vertex){
    cout << "   ";
    for (auto vex: print_vertex) {
        cout << toStr(vex);
    }
    cout << "\n";
    for (auto vexi: print_vertex){
        cout << toStr(vexi);
        for (auto vexj: print_vertex){
            cout << getWeight(vexi,vexj);
        }
        cout << "\n";
    }
}

void AdjMtx::printGraph(){
    printGraph(vertex);
}

int AdjMtx::getWeight(ZZ vfrom, ZZ vto) {
    pair<ZZ,ZZ> vexpair = make_pair(vfrom,vto);
    auto temp = graph.find(vexpair);
    if (temp != graph.end()){
        return temp->second;
    }
    return 0;
}