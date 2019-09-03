/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
#include <NTL/RR.h>

// project libraries
#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Syncs/CPISync.h>
#include <CPISync/Syncs/ProbCPISync.h>

ProbCPISync::ProbCPISync(long m_bar, long bits, int epsilon,bool hashes) :
CPISync(m_bar, bits, (int) epsilon + ceil(log(bits) / log(2)),0,hashes) // adding lg(b) gives an equivalent probability of error for CPISync
{

  // tweak parameters of CPISync for probabilistic implementation
   probEps = epsilon; // this was the designed probability of error
   probCPI = true; // switches some code in the CPISync superclass
   currDiff = 1;

}
