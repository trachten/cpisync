/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * Implements a one-way CPISync algorithm in which only the server gets synchronized
 * with the data from the client.  The client data does not change.
 * 
 * In effect, only half a round of communication (client->server) is used.
 *
 * Created on October 28, 2011, 2:11 PM
 */

#ifndef CPISYNC_HALFROUND_H
#define	CPISYNC_HALFROUND_H

#include <string>
#include "CPISync.h"

class CPISync_HalfRound: public CPISync {
    public:
        // Duplicate the CPISync constructors, but set the oneWay flag to true
     CPISync_HalfRound(long m_bar, long bits, int epsilon, int redundant=0) : CPISync(m_bar, bits, epsilon, redundant, false)
     { oneWay=true; SyncID = SYNCTYPE_CPISync_HalfRound; }
    
    string getName() {return CPISync::getName() + "   * one-way\n";}
};

#endif	/* ONEWAYCPISYNC_H */

