/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <iostream>
#include "ConstantsAndTypes.h"
#include "Auxiliary.h"

void Logger::error(string msg) {
    perror(msg.c_str());
}

void Logger::error_and_quit(const string& msg) {
    error(msg);
    exit(GENERAL_ERROR);
}
