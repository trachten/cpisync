/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

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
    explicit UnimplementedMethodException(const string &specifics) : runtime_error(specifics) {}
};

class SyncFailureException: public runtime_error {
public:
    explicit SyncFailureException(const string &specifics) : runtime_error(specifics)  {}

};
#endif	/* EXCEPTIONS_H */

