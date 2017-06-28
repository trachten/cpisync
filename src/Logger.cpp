/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <iostream>
#include "Auxiliary.h"
#include "Logger.h"

void Logger::error(string msg) {
    perror(msg.c_str());
}

void Logger::error_and_quit(string msg) {
    error(msg);
    exit(GENERAL_ERROR);
}
