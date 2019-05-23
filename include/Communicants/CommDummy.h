/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/*
 * This communicant communicates to and from a queue of characters. The queue is external so it can be used as
 * a shared resource between two CommDummys. This communicant is meant for use in tests/unit/CommunicantTest.cpp;
 * as such, it has not been integrated as a formal communicant with GenSync.
 *
 * File:   CommDummy.h
 * Author: Eliezer Pearl
 *
 * Created on May 24, 2018, 10:13 AM
 */

#ifndef COMMDUMMY_H
#define COMMDUMMY_H

#include <string>
#include <sstream>
#include <cstring>
#include <queue>

#include <Communicants/Communicant.h>

using namespace std;
class CommDummy : public Communicant {
public:
    using Communicant::commSend;  // see https://stackoverflow.com/questions/888235/overriding-a-bases-overloaded-function-in-c

    /** 
    * Constructs a CommDummy that sends to, and receives from, a given queue of 
    * characters.
    * 
    * @param intermediate A pointer to a queue of characters where information 
    * will be exchanged. This is a pointer since intermediate will be modified
    * in memory by CommDummy functions.
    */
    explicit CommDummy(queue<char>* intermediate);
    
    // Destructor
    ~CommDummy() override;
    
    // Inherited Communicant methods
    void commListen() override;
    void commConnect() override;
    void commClose() override;

    void commSend(const char* toSend, int numBytes) override;
    string commRecv(unsigned long numBytes) override ;
    inline string getName() override { return "CommDummy"; }

protected:
    
    // Instance variable that stores a pointer to an intermediate queue of characters.
    queue<char>* intermediate;
};

#endif /* COMMDUMMY_H */

