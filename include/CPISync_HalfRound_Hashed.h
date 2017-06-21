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
     {  hashQ = true; keepAlive = true; SyncID = SYNCTYPE_CPISync_HalfRound_Hashed; }
    
    string getName() {return CPISync_HalfRound::getName() + "   * hashed\n";}
};

#endif	/* CPISYNC_HALFROUND_HASHED_H */

