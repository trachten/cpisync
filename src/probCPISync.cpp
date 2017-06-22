/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
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
