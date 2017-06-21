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
 * File:   Exceptions.h
 * Implements exceptions specifically used in this program.
 *
 * Created on July 21, 2011, 10:24 PM
 */

#ifndef EXCEPTIONS_H
#define	EXCEPTIONS_H

#include <stdexcept>
        
/**
 * Denotes that a specific method has not (yet?) been implemented.
 * @param specifics A string to supply with the exception.
 */
class UnimplementedMethodException:public runtime_error {
public:
    UnimplementedMethodException(string specifics) : runtime_error(specifics) {}
};

class SyncFailureException: public runtime_error {
public:
    SyncFailureException(string specifics) : runtime_error(specifics)  {}

};
#endif	/* EXCEPTIONS_H */

