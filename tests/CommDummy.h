/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
/* 
 * File:   CommDummy.h
 * Author: kaets
 *
 * Created on May 24, 2018, 10:13 AM
 */

#ifndef COMMDUMMY_H
#define COMMDUMMY_H

#include <string>
#include <sstream>
#include <cstring>
#include <queue>

#include <Communicant.h>

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
    CommDummy(queue<char>* intermediate);
    
    // Destructor
    ~CommDummy();
    
    // Inherited Communicant methods
    void commListen() override;
    void commConnect() override;
    void commClose() override;

    void commSend(const char* toSend, const int numBytes) override;
    string commRecv(long numBytes) override ;
    inline string getName() override { return "dummy"; }

protected:
    
    // Instance variable that stores a pointer to intermediate.
    queue<char>* intermediate;
};

#endif /* COMMDUMMY_H */

