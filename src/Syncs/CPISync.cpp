/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

// standard libraries
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <map>
#include <iterator>
#include <NTL/RR.h>
#include <NTL/ZZ_p.h>
#include <NTL/ZZ_pX.h>
#include <NTL/ZZ_pXFactoring.h>

// project libraries
#include "Aux/Auxiliary.h"
#include "Aux/SyncMethod.h"
#include "Syncs/CPISync.h"
#include "Aux/Exceptions.h"

// namespaces

using namespace NTL;

// helper procedures
void CPISync::initData(int num) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::initData");
    // set the lengths
    sampleLoc.SetLength(num);
    CPI_evals.SetLength(num);

    // populate the arrays
    for (int ii = 0; ii < num; ii++) {
        sampleLoc[ii] = to_ZZ_p(DATA_MAX) + ii + 1; // i.e. from the region outside where valid data might lie
        CPI_evals[ii] = 1;
    }

    probCPI = oneWay = keepAlive = false; // assume not OneWay or Probabilistic synchronization unless otherwise stated, and manage our own communicant connections
    SyncID = SYNC_TYPE::CPISync;
}

CPISync::CPISync(long m_bar, long bits, int epsilon, int redundant, bool hashes /* = false */) :
maxDiff(m_bar), probEps(epsilon), hashQ(hashes) {
Logger::gLog(Logger::METHOD,"Entering CPISync::CPISync");

    // set default parameters
    if (hashQ) {
    /* if hashes are being used, we have to account for the probability of a collision by
     * by splitting the error probability between hash collisions and sync failures.
     * The former is controlled by lengthening the effective bit-representation of strings.
     */

    // Use big floats(RR) to prevent underflow which results in a "negative exponent in _ntl_gexp" error
    // bitNum = (long) 2 * bits + log(-1.0/log(1.0-pow(2.0,-epsilon-1.0)))/log(2) - 1;
    bitNum = conv<long>(ceil(RR_TWO * (RR) bits + log(-RR_ONE/log(RR_ONE-pow(RR_TWO,(RR) -epsilon-RR_ONE)))/log(RR_TWO) - RR_ONE));

    /*
     *  The analysis here is based on the birthday paradox.
     *  The probability of a collision for (at most) 2^bits elements chosen from
     *  a space of 2^x is bounded by roughly 1-exp(-2^(2*bits)/(2^(x+1)).  Setting
     *  this upper bounded by 2^-(epsilon+1) yields the above value
     */
      
      epsilon = epsilon + 1; // half the prob. error for the sync failure probability below
      Logger::gLog(Logger::METHOD_DETAILS," ... upping bitNum to "+toStr(bitNum));
    }
    else
      bitNum = bits;

    currDiff = maxDiff;

    if (redundant == 0) // i.e. use the probability of error to calculate redundancy
        redundant_k = to_long(CeilToZZ(to_RR(epsilon) / bitNum));
    else { // compute the probability of error
        redundant_k = redundant;
        probEps = redundant * bitNum;
    }

    if (redundant_k <= 0) //k at least 1
        redundant_k = 1;

    DATA_MAX = power(ZZ_TWO, bitNum);
    fieldSize = NextPrime(DATA_MAX + maxDiff + redundant_k);
    ZZ_p::init(fieldSize);

    initData(maxDiff + redundant_k); // initialize sample locations and metadata
}

CPISync::~CPISync() {
    sampleLoc.kill();
    CPI_hash.clear();
    CPI_evals.kill();
}

string CPISync::getName() {
    Logger::gLog(Logger::METHOD,"Entering CPISync::getName");
    string methodName;
    if (!probCPI) {
        methodName = "Basic CPI Sync";
    } else {
        methodName = "Probabilistic CPI Sync";
    }

    ostringstream result;
    result << methodName + "\n   * base field size = " << fieldSize << "\n   * mbar = " << maxDiff;
    result << "\n   * b = " << bitNum << "\n   * epsilon = " << probEps << endl;
    return result.str();
}

bool CPISync::ratFuncInterp(const vec_ZZ_p& evals, long mA, long mB, vec_ZZ_p& P_vec, vec_ZZ_p& Q_vec) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::ratFuncInterp");
    /* The coding attempts to follow the notation in
     ** Y. Minsky, A. Trachtenberg, and R. Zippel,
     **   Set Reconciliation with Nearly Optimal Communication Complexity,
     **   IEEE Trans. Inf. Theory 49:9, pp. 2213-2218 (see page 7, for example).
     */

    // local variables
    long ii, jj;
    long mbar = evals.length(), mAbar, mBbar, rank;
    long delta = mA - mB;

    vec_ZZ_p coefficient_vec;

    // 0. Compute bounds on one-sided set differences
    mAbar = (mbar + delta) / 2; /** (Upper bound on the degree of the numerator polynomial)+1:
                               * ((mbar + delta) % 2 == 0) ? (mbar + delta - 1) / 2 : (mbar + delta) / 2; */
    mBbar = (mbar - delta) / 2; /** (Upper bound on the degree of the denominator polynomial)+1:
                               * ((mbar - delta) % 2 == 0) ? (mbar - delta - 1) / 2 : (mbar - delta) / 2; */

    // ... sanity checks
    if ((mAbar < 0) || (mBbar < 0)) {
        Logger::gLog(Logger::METHOD, "0. function interpolation failed, more sample points needed.\n");
        return false;
    }

    // 1. Construct and solve a linear equations that produces the interpolation
    // van_matrix is, in terms of the article referenced above:
    //    k_i ^{d1-1} ... 1 | - f_i k_i^{d2-1} ... -f_i || f_i k_i^d2 - k_i^d1
    //    A || B, where Ax = B yields x = p_{d1-1} ... p_0 | q_{d2-1} ... q0
    //    The solution x is stored in coefficient_vec below.

    mat_ZZ_p van_matrix; // a concatenation of Vandermonde matrices
    van_matrix.SetDims(mbar, mAbar + mBbar + 1);
    for (ii = 0; ii < mbar; ii++) {
        // might be possible to streamline for large mAbar/mBbar
        for (jj = 0; jj < mAbar; jj++)
            van_matrix[ii][jj] = power(sampleLoc[ii], mAbar - jj - 1);
        for (jj = 0; jj < mBbar; jj++)
            van_matrix[ii][jj + mAbar] = -evals[ii] * power(sampleLoc[ii], mBbar - jj - 1);
        van_matrix[ii][mAbar + mBbar] = evals[ii] * power(sampleLoc[ii], mBbar) - power(sampleLoc[ii], mAbar);
    }

    mat_ZZ_p copyv_matrix(van_matrix); // unadulterated copy of van_matrix
    rank = gauss(van_matrix, mAbar + mBbar); // the last column just goes along for the ride

    // compare # of independent variables (rank) to total permitted degree of the interpolated function
    if (rank > mAbar + mBbar) { // case 1.  rank > tot. degree => error
        Logger::gLog(Logger::METHOD, "1. function interpolation failed, more sample points needed.\n");
        return false;
    } else if (rank < mAbar + mBbar) {
        // case 2. rank is smaller than tot. degree => recreate the matrix with the correct size
        // ... we do this by taking mAbar - mDiff columns from among the first mAbar
        // ...                  and mBbar - mDiff columns from index mAbar to mAbar-mBar -1
        // ...                  and the last column of the matrix
        long mDiff = mAbar + mBbar - rank; // difference between rank and upper bounds
        // ... adjust upper bounds mAbar and mBbar accordingly
        mAbar -= mDiff;
        mBbar -= mDiff;

        van_matrix.SetDims(mAbar + mBbar, mAbar + mBbar + 1);

        // recreate based on the original matrix computations
        for (ii = 0; ii < mAbar + mBbar; ii++) {
            for (jj = 0; jj < mAbar; jj++)
                van_matrix[ii][jj] = copyv_matrix[ii][jj + mDiff];
            for (jj = 0; jj < mBbar; jj++)
                van_matrix[ii][jj + mAbar] = copyv_matrix[ii][jj + mAbar + mDiff + mDiff];
            van_matrix[ii][mAbar + mBbar] = evals[ii] * power(sampleLoc[ii], mBbar) - power(sampleLoc[ii], mAbar);
        }

        // row-reduce the resulting matrix
        rank = gauss(van_matrix, mAbar + mBbar); // the last column just goes along for the ride      
    }

    // store the solution to the linear system in coefficient_vec
    coefficient_vec.SetLength(rank);
    for (ii = rank - 1; ii >= 0; ii--) {
        coefficient_vec[ii] = van_matrix[ii][mAbar + mBbar] / van_matrix[ii][ii];
        for (jj = 0; jj < ii; jj++) // subtract out the coefficient from the previous entries
            van_matrix[jj][mAbar + mBbar] -= coefficient_vec[ii] * van_matrix[jj][ii];
    }

    // 2. Store the result of the interpolation in P_vec and Q_vec
    P_vec.SetLength(mAbar + 1); // adding 1 is for p0 and q0
    Q_vec.SetLength(mBbar + 1);

    // the first mAbar coefficients (in reverse order) are P_vec
    P_vec[mAbar] = 1;
    for (ii = 0; ii < mAbar; ii++)
        P_vec[ii] = coefficient_vec[mAbar - ii - 1];

    // the next mBbar coefficients (in reverse order) are Q_vec
    Q_vec[mBbar] = 1;
    for (ii = 0; ii < rank - mAbar; ii++)
        Q_vec[ii] = coefficient_vec[rank - ii - 1];

    // 3. Free up memory and return
    coefficient_vec.kill();
    van_matrix.kill();
    copyv_matrix.kill();

    return true;
}

bool CPISync::find_roots(vec_ZZ_p& P_vec, vec_ZZ_p& Q_vec, vec_ZZ_p& numerator, vec_ZZ_p& denominator) {
Logger::gLog(Logger::METHOD,"Entering CPISync::find_roots");
    // 0. initialization
    ZZ_pX P_poly, Q_poly, gcd_poly;

    // ... convert to ZZ_pX polynomials
    conv(P_poly, P_vec);
    conv(Q_poly, Q_vec);

    // ... bring to a fraction in lowest terms
    gcd_poly = GCD(P_poly, Q_poly);
    if (deg(gcd_poly) > 0) {
        P_poly = P_poly / gcd_poly;
        Q_poly = Q_poly / gcd_poly;
    }
    gcd_poly.kill(); // free up its memory

    // 1. Check that the polynomials are square free - is gcd(poly, derivative(poly))==1?
    if (!IsOne(GCD(P_poly, diff(P_poly))) ||
            !IsOne(GCD(Q_poly, diff(Q_poly)))) {
        Logger::gLog(Logger::METHOD, "Polynomial is not square free!\n");
        return false;
    }

    // 2. Factor the two polynomials
    // SFBerlekamp function defined in ZZ_pXFactoring module - "Berlekamp" factoring approach [Shoup, J. Symbolic Comp. 20:363-397, 1995].
    vec_ZZ_pX nn, dd;
    nn = SFBerlekamp(P_poly);
    if (nn.length() > 0)
        for (const ZZ_pX& fact : nn)
            if (deg(fact) > 1) { // ended with a non-linear factor
                Logger::gLog(Logger::METHOD, "Cannot reduce P_poly to linear factors..\n");
                return false;
            }

    dd = SFBerlekamp(Q_poly);
    if (dd.length() > 0)
        for (const ZZ_pX& fact : dd)
            if (deg(fact) > 1) { // ended with a non-linear factor
                Logger::gLog(Logger::METHOD, "Cannot reduce Q_poly to linear factors.\n");
                return false;
            }

    // 3. Put the results into the numerator and denominator vectors and return
    numerator.SetLength(deg(P_poly));
    denominator.SetLength(deg(Q_poly));

    for (int ii = 0; ii < nn.length(); ii++)
        numerator[ii] = -ConstTerm(nn[ii]);
    for (int ii = 0; ii < dd.length(); ii++)
        denominator[ii] = -ConstTerm(dd[ii]);

    // free up memory
    P_poly.kill();
    Q_poly.kill();

    nn.kill();
    dd.kill();

    return true;
}

bool CPISync::set_reconcile(const long otherSetSize, const vec_ZZ_p &otherEvals, vec_ZZ_p &delta_self, vec_ZZ_p &delta_other) {
Logger::gLog(Logger::METHOD,"Entering CPISync::set_reconcile");
    if (otherSetSize < 1) {
        // Jin's optimization:  if the other set has nothing, just send over my evaluations
        map<ZZ, DataObject * >::iterator itCPI;

        for (itCPI = CPI_hash.begin(); itCPI != CPI_hash.end(); itCPI++)
            append(delta_self, to_ZZ_p(itCPI->first));
    } else // otherSetSize >=1 - the other set has something
        if (CPI_hash.empty()) { // I have nothing new
        return true;
    } else { // we both have something new
        vec_ZZ_p coefficient_P, coefficient_Q;

        // compute rational function evals
        vec_ZZ_p ratFuncEvals;
        long metalength = min(otherEvals.length(), currDiff);
        for (long ii = 0; ii < metalength; ii++)
          append(ratFuncEvals, otherEvals[ii] / CPI_evals[ii]);

        // attempt to interpolate based on these evals
        if (!ratFuncInterp(ratFuncEvals, otherSetSize, CPI_hash.size(), coefficient_P, coefficient_Q))
            return false;

        // attempt to find roots of the numerator and denominator of the rational function
        vec_ZZ_p numerator, denominator;
        if (!find_roots(coefficient_P, coefficient_Q, numerator, denominator))
            return false;
        append(delta_other, numerator);
        append(delta_self, denominator);

        coefficient_P.kill();
        coefficient_Q.kill();
    }
    return true;
}

void CPISync::sendSetElem(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, const ZZ_p& element) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::sendSetElem");
    if (!hashQ || oneWay) // these cases don't require an additional round of string exchanges
        selfMinusOther.push_back(invHash(element));
    else {
        // Translate to an actual string and send it to the client
        DataObject *dop = CPI_hash[rep(element)];
        if (dop == nullptr)
            throw SyncFailureException("Element not found - decrease probability of error requirement for sync.");
        commSync->commSend(*dop);

        Logger::gLog(Logger::METHOD, string("Translating ") + toStr(element) + " to " + dop->to_string());
        selfMinusOther.push_back(dop); // save the string
    }
}

void CPISync::recvSetElem(const shared_ptr<Communicant>& commSync, list<DataObject*> &otherMinusSelf, ZZ_p element) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::recvSetElem");
    if (!hashQ || oneWay) // these cases don't require an additional round of string exchanges
        otherMinusSelf.push_back(invHash(std::move(element)));
    else {
        // receive the actual string from the client
        DataObject *dop = commSync->commRecv_DataObject();

        Logger::gLog(Logger::METHOD, string("Received string " + dop->to_string()));
        otherMinusSelf.push_back(dop);
    }
}

void CPISync::makeStructures(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf, vec_ZZ_p &delta_self, vec_ZZ_p &delta_other) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::makeStructures");
    // Send self minus other
    try {
        for (const ZZ_p& dop : delta_self)
            sendSetElem(commSync, selfMinusOther, dop);
    } catch (const SyncFailureException& s) {
        throw (s); // rethrow the exception onward
    }

    // Receive other minus self
    for (const auto &ii : delta_other)
        recvSetElem(commSync, otherMinusSelf, ii);
}

void CPISync::SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::SendSyncParam");
    // take care of parent sync method
    SyncMethod::SendSyncParam(commSync, oneWay);

    // ... sync ID, mbar, bits, and epsilon
    commSync->commSend(enumToByte(SyncID));
    commSync->commSend(maxDiff);
    commSync->commSend(bitNum);
    commSync->commSend(probEps);
    if (!oneWay && (commSync->commRecv_byte() == SYNC_FAIL_FLAG))
        throw SyncFailureException("Sync parameters do not match.");
    Logger::gLog(Logger::COMM, "Sync parameters match");
}

void CPISync::RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::RecvSyncParam");
    // take care of parent sync method
    SyncMethod::RecvSyncParam(commSync, oneWay);

    // ... sync ID, mbar, bits, and epsilon        
    byte theSyncID = commSync->commRecv_byte();
    long mbarClient = commSync->commRecv_long();
    long bitsClient = commSync->commRecv_long();
    int epsilonClient = commSync->commRecv_int();

    if (theSyncID != enumToByte(SyncID) ||
            mbarClient != maxDiff ||
            bitsClient != bitNum ||
            epsilonClient != probEps) {
        // report a failure to establish sync parameters
        if (!oneWay)
            commSync->commSend(SYNC_FAIL_FLAG);
        Logger::gLog(Logger::COMM, "Sync parameters differ from client to server: Client has (" +
                toStr(mbarClient) + "," + toStr(bitsClient) + "," + toStr(epsilonClient) +
                ").  Server has (" + toStr(maxDiff) + "," + toStr(bitNum) + "," + toStr(probEps) + ").");
        throw SyncFailureException("Sync parameters do not match.");
    }
    if (!oneWay)
        commSync->commSend(SYNC_OK_FLAG);
    Logger::gLog(Logger::COMM, "Sync parameters match");
}

bool CPISync::SyncClient(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther, list<DataObject*> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::SyncClient");

	//Reset currDiff to 1 at the start of the sync so that the correct upper bound can be found if the dataset has changed
    if(probCPI) currDiff = 1;

    // local variables
    vec_ZZ_p delta_self, /** items I have that the other does not, based on the last synchronization. */
            delta_other; /** items the other has that I do not, based on the last synchronization. */

    // 0. Initialization;
    try {
        SyncMethod::SyncClient(commSync, selfMinusOther, otherMinusSelf); // call the base method - sets some fields to 0

        // 0.5  verify commonality initial parameters
        // ... connect to the other party
        if (!keepAlive) {
            commSync->commConnect();

            // ... check that the other side is doing the same synchronization
            SendSyncParam(commSync, oneWay);
        }

        // 1. Transmit characteristic polynomial values
        commSync->commSend((long) CPI_hash.size()); // ... first outputs how many set elements the client has

        // ... produce the values in a list:  [x1 x2 x3 ... ]
        vec_ZZ_p valList;
        for (int ii = 0; ii < currDiff; ii++)
            append(valList, CPI_evals[ii]);
        for (int ii = 0; ii < redundant_k; ii++)
            append(valList, CPI_evals[currDiff + ii]);
        commSync->commSend(valList);
        valList.kill();
      
        // 2. Get more characteristic polynomial values if needed
        while (!oneWay && (commSync->commRecv_byte() == SYNC_FAIL_FLAG)) {
            if (!probCPI || currDiff == maxDiff) {
                // CPISync failed
                delta_other.kill();
                delta_self.kill();
                if (!keepAlive)
                    commSync->commClose();
                return false;
            } else {
                // Send more samples and try again
                vec_ZZ_p tmp_vec;
                for (long ii = 0; ii < min(currDiff, maxDiff - currDiff); ii++)
                    append(tmp_vec, CPI_evals[currDiff +redundant_k + ii]);
                commSync->commSend(tmp_vec);
                currDiff = min(currDiff * 2, maxDiff);
                tmp_vec.kill();
            }
        }

        if (!oneWay) {
            delta_other = commSync->commRecv_vec_ZZ_p();
            delta_self = commSync->commRecv_vec_ZZ_p();

            Logger::gLog(Logger::METHOD, string("CPISync succeeded.\n")
                    + "   self - other =  " + toStr<vec_ZZ_p > (delta_self) + "\n"
                    + "   other - self =  " + toStr<vec_ZZ_p > (delta_other) + "\n"
                    + "\n");
	    string results= string("... results:\n")
                        + "   self - other =  " + toStr<vec_ZZ_p > (delta_self) + "\n"
                        + "   other - self =  " + toStr<vec_ZZ_p > (delta_other) + "\n"
                        + "\n";
        }

        // create selfMinusOther and otherMinusSelf structures to report the result of reconciliation
        makeStructures(commSync, selfMinusOther, otherMinusSelf, delta_self, delta_other);
        delta_self.kill();
        delta_other.kill();
        if (!keepAlive)
            commSync->commClose();

        return true;
    } catch (SyncFailureException& s) {
        Logger::gLog(Logger::METHOD_DETAILS, s.what());
        throw (s);
    }
}

bool CPISync::SyncServer(const shared_ptr<Communicant>& commSync, list<DataObject*>& selfMinusOther, list<DataObject*>& otherMinusSelf) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::SyncServer");

    //Reset currDiff to 1 at the start of the sync so that the correct upper bound can be found if the dataset has changed
	if(probCPI) currDiff = 1;

	string mystring;
    vector<long> self_hash;
    vector<long> recv_hash;
    vec_ZZ_p recv_meta;
    clock_t serverStart = clock();
    long otherSetSize;

    vec_ZZ_p delta_self, /** items I have that the other does not, based on the last synchronization. */
            delta_other; /** items the other has that I do not, based on the last synchronization. */

    SyncMethod::SyncServer(commSync, selfMinusOther, otherMinusSelf); // call the base method - sets some fields to 0

    // Verify commonality initial parameters
    if (!keepAlive) {
        // Set up listening on the port
        Logger::gLog(Logger::METHOD, "Server: Started listening to: " + commSync->getName());

        commSync->commListen();

        // ... verify sync parameters
        RecvSyncParam(commSync, oneWay);
    }


    // Perform synchronization
    // .. listen for data
    otherSetSize = commSync->commRecv_long();
    recv_meta = commSync->commRecv_vec_ZZ_p();

    bool result = true; // continues looping while result is true
    do {
        delta_other.kill();
        delta_self.kill();

        vec_ZZ_p meta_other, meta_self;
        for (long ii = 0; ii < redundant_k; ii++) {
                append(meta_other, recv_meta[currDiff + ii]);
                append(meta_self, CPI_evals[currDiff + ii]);
        }
        
        // attempt to reconcile with the presumed number of differences
        bool succeed = set_reconcile(otherSetSize, recv_meta, delta_self, delta_other);
        if (succeed) { // the node reconciliation might have been successful
            
            // PERFORM some added checks
            vec_ZZ_p value_self = meta_self;
            vec_ZZ_p value_other = meta_other;
            // perform a check with the redundant data
            for (long jj = 0; jj < redundant_k; jj++) {
                for (const auto &ii : delta_other) {
                    //value_self[jj] *= (sampleLoc[maxDiff + jj] - delta_other[ii]);
                     value_self[jj] *= (sampleLoc[currDiff + jj] - ii);
                }
            }

            for (long jj = 0; jj < redundant_k; jj++) {
                for (const auto &ii : delta_self) {
                    //value_other[jj] *= (sampleLoc[maxDiff + jj] - delta_self[ii]);
                    value_other[jj] *= (sampleLoc[currDiff + jj] - ii);
                }
            }

            for (long jj = 0; jj < redundant_k; jj++) {
                if (value_self[jj] != value_other[jj]) {
                    succeed = false;
                    break; // no need to keep going
                }
            }
            value_other.kill();
            value_self.kill();

            if (succeed) { // only do this if reconciliation has succeeded
                Logger::gLog(Logger::METHOD, "CPISync succeeded.\n");

                if (!oneWay) {
					commSync->commSend(SYNC_OK_FLAG); // sync succeeded
					commSync->commSend(delta_self);
                    commSync->commSend(delta_other);
                }

                Logger::gLog(Logger::METHOD, string("... results:\n")
                        + "   self - other =  " + toStr<vec_ZZ_p > (delta_self) + "\n"
                        + "   other - self =  " + toStr<vec_ZZ_p > (delta_other) + "\n"
                        + "\n");

                // create selfMinusOther and otherMinusSelf structures to report the result of reconciliation
                try {
                    makeStructures(commSync, selfMinusOther, otherMinusSelf, delta_self, delta_other);
                } catch (SyncFailureException& s) {
                    Logger::gLog(Logger::METHOD_DETAILS, s.what());
                    throw (s);
                }

                break; // break out of the while loop - this has been settled
            }
        }

        if (!succeed) { // if synchronization has failed for some reason
            Logger::gLog(Logger::METHOD, "Not successfully synced!\n");
            if (!oneWay)
                commSync->commSend(SYNC_FAIL_FLAG); // send just one character with the flag
            if (!probCPI || currDiff == maxDiff) {
                result = false;
                break;
            } else {
                vec_ZZ_p recv_new = commSync->commRecv_vec_ZZ_p();
                append(recv_meta, recv_new);
                currDiff = min(currDiff * 2, maxDiff);
            }
        }
        meta_self.kill();
        meta_other.kill();
    } while (result); //end of while	


    if (!keepAlive)
        commSync->commClose();
    delta_other.kill();
    delta_self.kill();
    self_hash.clear();
    recv_hash.clear();

    return result;
}

void CPISync::sendAllElem(const shared_ptr<Communicant>& commSync, list<DataObject*> &selfMinusOther) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::sendAllElem");
    commSync->commSend((long) CPI_hash.size()); // first send the size

    map< ZZ, DataObject * >::iterator it;
    for (it = CPI_hash.begin();
            it != CPI_hash.end();
            it++) {
        commSync->commSend(*(it->second));
        selfMinusOther.push_back(it->second);
        Logger::gLog(Logger::COMM_DETAILS, "Sent all node elements.");
    }
}

void CPISync::receiveAllElem(const shared_ptr<Communicant>& commSync, list<DataObject*> &otherMinusSelf) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::receiveAllElem");
    long size = commSync->commRecv_long();

    for (int ii = 0; ii < size; ii++) {
        DataObject* dop = commSync->commRecv_DataObject();
        otherMinusSelf.push_back(dop);
    }

    Logger::gLog(Logger::COMM_DETAILS, "Received all node elements.");
}

DataObject * CPISync::invHash(const ZZ_p num) const {
    Logger::gLog(Logger::METHOD,"Entering CPISync::invHash");
    const ZZ &numZZ = rep(num);
    auto *result = new DataObject(numZZ);
    return result;
}

ZZ_p CPISync::makeData(ZZ_p num) const {
    return to_ZZ_p(rep(num) % DATA_MAX);
}

ZZ_p CPISync::hash(const DataObject * datum) const {
    ZZ num = datum->to_ZZ(); // convert the datum to a ZZ

    if (!hashQ && (num >= DATA_MAX))
        Logger::error_and_quit("Cannot add element (" + datum->to_string() + ") "
            + " whose encoding (" + toStr(num) + ") is larger than  (" + toStr(DATA_MAX) + " - max field element) "
            + " when using nohash synchronization.  Please increase modulus to at least " + toStr(ceil(log(DATA_MAX + redundant_k) / log(2))) + " bit elements.");

    return to_ZZ_p(num % (DATA_MAX)); // reduce to bit_num bits and make into a ZZ_p
}

ZZ_p CPISync::hash2(const long num) const {
    return to_ZZ_p(to_ZZ(num)*101 % (DATA_MAX));
}

// update metadata when add an element

bool CPISync::addElem(DataObject * datum) {
    Logger::gLog(Logger::METHOD,"Entering CPISync::addElem");
    int ii;
    
    // call the parent class to take care of bookkeeping
    bool result = SyncMethod::addElem(datum);

    // put real data into the hash table
    ZZ_p hashID;
    ZZ hashNum;
    int count = 0;
    do {
        if (hashQ) {
            hashID = makeData(hash(datum) + hash2(count++)); // a double hash to allow repeated elements
            hashNum = rep(hashID);
        } else { // noHash is enabled
            hashID = hash(datum); // a simpler, potentially reversable hash
            hashNum = rep(hashID);
            if (CPI_hash.find(hashNum) != CPI_hash.end()) { // an item with this hash already exists
                Logger::error("Item with hash " + toStr(hashNum) + " already exists in the set.  Under the noHash option, duplicate elements are not permitted.");
                return false;
            }
        }
    } while (CPI_hash.find(hashNum) != CPI_hash.end() && // keep looking until a hash entry is found that is not currently in use
            CPI_hash.size() < DATA_MAX); // or the map is full

    if (CPI_hash.size() >= DATA_MAX) {
        Logger::gLog(Logger::METHOD, " Unable to add item " + datum->to_string() + "; please increase number of bits per element.");
        return false;
    }

    CPI_hash[hashNum] = datum;

    for (ii = 0; ii < sampleLoc.length(); ii++)
        CPI_evals[ii] *= (sampleLoc[ii] - hashID);

    Logger::gLog(Logger::METHOD_DETAILS, "... (CPISync) added item " + datum->to_string() + " with hash = " + toStr(hashNum));

    return result;
}

// update metadata when delete an element by index
bool CPISync::delElem(DataObject * newDatum) {
    Logger::gLog(Logger::METHOD, "Entering CPISync::delElem");

    // call the parent method to take care of bookkeeping
    if(!SyncMethod::delElem(newDatum)) {
	Logger::error("Couldn't find " + newDatum->to_string() + ".");
	return false;
    }

    ZZ_p hashID;
    // remove data from the hash table. to find the value, a linear search is required
	auto itr = CPI_hash.begin();
	while (itr != CPI_hash.end()) {
		if (itr->second == newDatum) {
			hashID = to_ZZ_p(itr->first);
			CPI_hash.erase(itr++);
		} else {
			++itr;
		}
	}

		// update cpi evals
    for(int ii = 0; ii < sampleLoc.length(); ii++) {
        CPI_evals[ii] /= (sampleLoc[ii] - hashID);
    }

    Logger::gLog(Logger::METHOD_DETAILS, "... (CPISync) removed item " + newDatum->print() + ".");
    return true;
}

string CPISync::printElem() {
    stringstream result("");

    map< ZZ, DataObject * >::iterator it;
    for (it = CPI_hash.begin(); it != CPI_hash.end(); it++)
        result << (it->second)->to_string() << " [hash=" << (it->first) << "], ";
    return result.str();
}