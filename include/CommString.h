/*
 * This code is part of the CPISync project developed at Boston University:
 *      {@link http://nislab.bu.edu}
 * 
 * The code, explanation, references, API, and a demo can be found on this
 * web page.  The main theoretical basis for the approach can be found at
 * {@link http://ipsit.bu.edu/documents/ieee-it3-web.pdf}
 *   Y. Minsky, A. Trachtenberg, and R. Zippel,
 *   "Set Reconciliation with Nearly Optimal Communication Complexity",
 *   IEEE Transactions on Information Theory, 49:9.
 * 
 * Elements of the CPISync project code have been worked on, at various points, by:
 * @author Ari Trachtenberg
 * @author Sachin Agarwal 
 * @author Paul Varghese
 * @author Jiaxi Jin
 * @author Jie Meng
 * @author Alexander Smirnov

 */

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
    CommString(string initial = "", bool base64 = false);
    
    /**
     * Destructor
     */
    ~CommString();

    // Accessors
    /**
     * @return the internal string of the class, potentially including ASCII escape characters.
     */
    string getString();

    string getName() { return "CommString"; }
    
    /**
     * Implemented methods from Communicant
     */
    void commConnect();
    void commListen();
    void commClose();
    void commSend(const char *toSend, const int numBytes);
    string commRecv(long numBytes);

protected:
    stringstream *stream; // the output stream to which to write characters

};

#endif	/* COMMSTRING_H */

