/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * Implements a CPISync algorithm in which set elements are represented
 * in full (as opposed to being hashed to a fixed length).  In this case, set differences
 * determined by CPISync correspond to actual set elements (rather than their hashes), so
 * that no extra communication is needed to determine hash inverses.
 * 
 * In effect, one less round of communication (client<->server) is used.
 * 
 * Created on November 17, 2011, 10:39 PM
 */

#ifndef CPISYNC_ONEROUND_H
#define	CPISYNC_ONEROUND_H

#include <string>
#include <CPISync/Syncs/CPISync.h>


class CPISync_OneLessRound : public CPISync {
public:
   // Duplicate the CPISync constructors, but set the noHash flag to true
    CPISync_OneLessRound(long m_bar, long bits, int epsilon, int redundant=0) : CPISync(m_bar, bits, epsilon, redundant, false)
    {
		hashQ=false;
		SyncID = SYNC_TYPE::CPISync_OneLessRound;
    }
    
    string getName() override {return CPISync::getName() + "\n   *no hashes";}
};

#endif	/* CPISYNC_NOHASH_H */

