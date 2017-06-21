/*
 * This code is part of the CPISync project developed at Boston University:
 * 
 * The code, explanation, references, API, and a demo can be found on this
 * web page.  The main theoretical basis for the approach can be found at
 * {@link http://ipsit.bu.edu/documents/ieee-it3-web.pdf}
 *   Y. Minsky, A. Trachtenberg, and R. Zippel,
 *   "Set Reconciliation with Nearly Optimal Communication Complexity",
 *   IEEE Transactions on Information Theory, 49:9.
 * 
 * @author Ari Trachtenberg
 * @author Sachin Agarwal 
 * @author Paul Varghese
 * @author Jiaxi Jin
 * @author Jie Meng
 * @author Alexander Smirnov

 */

#include <iostream>
#include "Auxiliary.h"
#include "Logger.h"

bool Logger::enabled = false; // logging is disabled by default

void Logger::error(string msg) {
    perror(msg.c_str());
}

void Logger::error_and_quit(string msg) {
    error(msg);
    exit(GENERAL_ERROR);
}
