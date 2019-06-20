/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

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
    CPISync_ExistingConnection(long m_bar, long bits, int epsilon, int redundant=0,bool hashes = false) : CPISync(m_bar, bits, epsilon, redundant, hashes)
    { hashQ=false; keepAlive = true; SyncID = SYNC_TYPE::CPISync_ExistingConnection; }
    
    string getName() override {return CPISync::getName() + "   * assuming an existing connection\n";}
};

#endif	/* CPISYNC_EXISTINGCONNECTION_H */

