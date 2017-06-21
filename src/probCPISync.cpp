/*
 * This code is part of the CPISync project developed at Boston University:
 *      {@link http://nislabcpi.bu.edu/cpisync}
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
 * @version 2.01, 11/20/2013
 */

/* 
 * File:   probCPISync.cpp
 * 
 * Created on August 19, 2011, 4:50 PM
 */
#include <NTL/RR.h>

// project libraries
#include "Auxiliary.h"
#include "Communicant.h"
#include "DataObject.h"
#include "SyncMethod.h"
#include "CPISync.h"
#include "Logger.h"
#include "Exceptions.h"
#include "probCPISync.h"

ProbCPISync::ProbCPISync(long m_bar, long bits, int epsilon) :
CPISync(m_bar, bits, epsilon + ceil(log(bits) / log(2))) // adding lg(b) gives an equivalent probability of error for CPISync
{

  // tweak parameters of CPISync for probabilistic implementation
   probEps = epsilon; // this was the designed probability of error
   probCPI = true; // switches some code in the CPISync superclass
   currDiff = 1;
}
