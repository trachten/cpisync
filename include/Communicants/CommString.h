/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   CommString.h
 *
 * An input/output connection to a string.  "sent" bytes are appended to the string and
 * "received" bytes are read from the string at the current string location.
 * 
 * The current value of the string can be accessed from the CommString using getString().
 * Created on November 5, 2011, 8:49 PM
 */

#ifndef COMMSTRING_H
#define	COMMSTRING_H
#include <sstream>
#include <string>
#include "Communicant.h"

class CommString : public Communicant {
public:

    /**
     * Constructs a CommString based on an arbitrary string
     * @param initial The string with which to seed the CommString
     * @param base64 If set to true, then the string is interpreted as a base64
     *  (ASCII armored) text.
     */
    explicit CommString(const string& initial = "", bool base64 = false);
    

     // Destructor
    ~CommString() override;

    // Accessors

    /**
     * @return the internal string of the class, potentially including ASCII escape characters.
     */
    string getString();

    string getName() override { return "CommString"; }
    
    /**
     * Implemented methods from Communicant
     */
    void commConnect() override;
    void commListen() override;
    void commClose() override;
    void commSend(const char *toSend, int numBytes) override;
    string commRecv(unsigned long numBytes) override;

protected:
    stringstream *stream; // the output stream to which to write characters

};

#endif	/* COMMSTRING_H */

