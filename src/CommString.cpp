/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   CommString.cpp 
 * Created on November 5, 2011, 8:49 PM
 */

#include "Auxiliary.h"
#include "CommString.h"

CommString::CommString(string initial, bool base64) {
    if (base64)
        stream = new stringstream(base64_decode(initial), stringstream::out | stringstream::in);
    else
        stream = new stringstream(initial, stringstream::out | stringstream::in);
}

CommString::~CommString() {
    delete stream;
}

string CommString::getString() {
    return stream->str();
}

void CommString::commConnect() {
} // nothing needed to connect to a string

void CommString::commListen() {
    return;
}// nothing needed to listen from a string

void CommString::commClose() {
    stream->flush();
}

void CommString::commSend(const char *toSend, const int numBytes) {
    // save the next bytes to the string stream
    stream->write(toSend, numBytes);
    addXmitBytes(numBytes); // update the byte transfer counter
}

string CommString::commRecv(long numBytes) {
    // returns the next few bytes from the string stream

    char *tmpBuf = new char[numBytes]; // buffer into which received bytes are placed
    stream->read(tmpBuf, numBytes);

    addRecvBytes(numBytes); // update the received byte counter

    return string(tmpBuf, numBytes);
}
