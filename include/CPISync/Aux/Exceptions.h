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
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <NTL/ZZ.h>
#include <string>
#include <map>
#include <vector>
#include <iterator>
#include <list>
#include <set>
#include <algorithm>
#include <csignal>
#include <sys/wait.h>
#include <climits>
#include <cstring>
#include <memory>
#include <CPISync/Aux/ConstantsAndTypes.h>
#include <CPISync/Aux/Logger.h>

// namespaces
using std::runtime_error;
using std::string;

/**
 * Denotes that a specific class has not (yet?) been implemented.
 */
class UnimplementedClassException : public logic_error {
public:
    UnimplementedClassException() : logic_error("This class has not yet been implemented.") {}
};

/**
 * Denotes that a specific method has not (yet?) been implemented.
 */
class UnimplementedMethodException:public logic_error {
public:
    UnimplementedMethodException() : logic_error("This method has not yet been implemented.") {}
};

class SyncFailureException: public runtime_error {
public:
    explicit SyncFailureException(const string &specifics) : runtime_error(specifics)  {}

};


#endif	/* EXCEPTIONS_H */

