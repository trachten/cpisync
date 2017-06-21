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

