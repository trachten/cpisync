/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * Implements a one-way CPISync algorithm in which only the server gets synchronized
 * with the data from the client.  The client data does not change.
 * 
 * In effect, only half a round of communication (client->server) is used.
 * 
 * This is identical to CPISync_HalfRound *except* that it hashes all data and
 * doesn't open or close communicants; a higher-level protocol then has to resolve
 * hashes to actual set elements.
 * 
 * Created on October 28, 2011, 2:11 PM
 */

#ifndef CPISYNC_HALFROUND_HASHED_H
#define	CPISYNC_HALFROUND_HASHED_H
 
#include <string>
#include "CPISync_HalfRound.h"

class CPISync_HalfRound_Hashed : public CPISync_HalfRound {
    public:
        // Duplicate the CPISync constructors, but set the oneWay flag to true
     CPISync_HalfRound_Hashed(long m_bar, long bits, int epsilon, int redundant=0) : CPISync_HalfRound(m_bar, bits, epsilon, redundant)
     {  hashQ = true; keepAlive = true; SyncID = SYNC_TYPE::CPISync_HalfRound_Hashed; }
    
    string getName() override {return CPISync_HalfRound::getName() + "   * hashed\n";}
};

#endif	/* CPISYNC_HALFROUND_HASHED_H */

