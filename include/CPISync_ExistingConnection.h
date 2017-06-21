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
 * Implements CPISync under the assumption that a connection to another communicant already exists.
 * This might happen if CPISync is called as a subroutine from another sync protocol.
 * 
 * File:   CPISync_ExistingConnection.h
 *
 * Created on December 30, 2011, 10:51 AM
 */

#ifndef CPISYNC_EXISTINGCONNECTION_H
#define	CPISYNC_EXISTINGCONNECTION_H

#include <string>
#include "CPISync.h"

class CPISync_ExistingConnection : public CPISync {
public:
   // Duplicate the CPISync constructors, but initializes "no hashing"
    CPISync_ExistingConnection(long m_bar, long bits, int epsilon, int redundant=0) : CPISync(m_bar, bits, epsilon, redundant, false)
    { hashQ=false; keepAlive = true; SyncID = SYNCTYPE_CPISync_ExistingConnection; }
    
    string getName() {return CPISync::getName() + "   * assuming an existing connection\n";}
};

#endif	/* CPISYNC_EXISTINGCONNECTION_H */

