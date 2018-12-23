/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

/* 
 * File:   probCPISync.h
 * Author: Ari Trachtenberg
 * 
 * Implements a data structure for storing mathematical sets of data
 *    in a manner that is consistent with fast synchronization.
 *    More precisely, two remote sets SA and SB (of b-bit vectors)
 *    with m differences between them can be synchronized using roughly
 *    m (b-bit) packets of communication.
 * 
 * This particular implementation is a generalization of CPISync to the case
 * when an upper bound on differences between two sets is not known a priori.
 * The algorithm starts with an initial guess, and then increases it until
 * the correct number of differences are found (subject to a prescribed probability
 * of error).
 * 
 * More information is available at:
 *  * {@link http://ipsit.bu.edu/documents/BUTR2002-01.pdf}
 *   Y. Minsky, A. Trachtenberg,
 *   "Scalable set reconciliation"
 *   Allerton Conference on Communication, Control and Computing,
 *   October 2002.
 *
 * Created on August 19, 2011, 4:50 PM
 */

#ifndef PROBCPISYNC_H
#define	PROBCPISYNC_H

#include "CPISync.h"
class ProbCPISync: public CPISync {
public:
    // Constructors
    /**
     * Specific class constructor.
     *  
     * @param m_bar   Initial estimate on the number of differences between sets.
     *    If more than m_bar differences are evident, subdivides into smaller subproblems.
     * @param bits    The number of bits used to represent a set element.
     * @param epsilon An upper bound on the probability of error of the synchronization,
     *    expressed in its negative log.  In other words, the actually probability of error
     *    is upper bounded by 2^-epsilon.
     * 
     *    Internal parameters are tweaked to guarantee this, subject to an assumption
     *    that an internal hash does not collide, and that there be at least 2 items
     *    in the union of the two sets that are being synchronized.
     */
    ProbCPISync(long m_bar, long bits, int epsilon, bool use_existing = false);

    ~ProbCPISync() override = default;;
private:


};

#endif	/* PROBCPISYNC_H */

