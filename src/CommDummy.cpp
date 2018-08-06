/* This code is part of the CPISync project developed at Boston University.
 * Please see the README for use and references.
 *
 * File:   CommDummy.cpp
 * Author: kaets
 * 
 * Created on May 24, 2018, 10:13 AM
 */

#include "CommDummy.h"
#include <queue>
#include <string>

CommDummy::CommDummy(queue<char> *intermediate) {
    this->intermediate = intermediate;
}

CommDummy::~CommDummy() {
}

// No action needed to listen to the intermediate.
void CommDummy::commListen() {
    return;
}

// No action needed to connect to the intermediate.
void CommDummy::commConnect() {
    return;
}

// No action needed to close connection with intermediate.
void CommDummy::commClose() {
    return;
}

void CommDummy::commSend(const char* toSend, const int numBytes) {
    // If numBytes is zero, then toSend's length must be calculated.
    const int calcLen = 0;
    const size_t bytes = numBytes == calcLen ? strlen(toSend) : numBytes;
    for (int i = 0; i < numBytes; i++)
        intermediate->emplace(toSend[i]);

    // Update transmitted-bytes-counter.
    addXmitBytes(bytes);
}

string CommDummy::commRecv(unsigned long numBytes) {
    // Create a stringstream to store the first numBytes characters.
    stringstream recv;

    // Get the first numBytes characters.
    for (int i = 0; i < numBytes; i++) {
        recv << intermediate->front();
        intermediate->pop();
    }

    // Update received-bytes-counter.
    addRecvBytes(numBytes);
    return recv.str();
}
