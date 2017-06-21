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
 * File:   UID.h
 * A class for generating Uniqued IDed objects.
 * Each object has a unique ID (unique over a run of the program)
 * and an (optional) user-specified hash.
 *
 * Created on September 4, 2011, 6:42 PM
 */

#ifndef UID_H
#define	UID_H
#include "NTL/mat_ZZ_p.h"

using namespace NTL;

class UID {
public:

    /** Constructor
     */
    UID() {
        myID = ID_count++;
    }

    /**
     * @return the unique ID of this object.
     */
    int getObjectID() {
        return myID;
    }

private:
    int myID; /** the ID of this object */
    static int ID_count; /** Maintains a count of the number of UIDs created in the program thus far. */
};

#endif	/* UID_H */

