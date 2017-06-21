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
#include "CPISync.h"


class CPISync_OneLessRound : public CPISync {
public:
   // Duplicate the CPISync constructors, but set the noHash flag to true
    CPISync_OneLessRound(long m_bar, long bits, int epsilon, int redundant=0) : CPISync(m_bar, bits, epsilon, redundant, false)
    { hashQ=false; SyncID = SYNCTYPE_CPISync_OneLessRound; }
    
    string getName() {return CPISync::getName() + "   * no hashes\n";}
};

#endif	/* CPISYNC_NOHASH_H */

