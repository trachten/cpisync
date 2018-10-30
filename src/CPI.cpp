//
// Created by Bowen Song on 10/29/18.
//

#include "CPI.h"

CPI::CPI(size_t value_size, size_t num_evals)
: bits(value_size), numEvals(num_evals) {

    redundant_k = 1;

    probEps = redundant_k * bits;


    // set the lengths
    sampleLoc.SetLength(num_evals);
    CPI_evals.SetLength(num_evals);

    // populate the arrays
    for (int ii = 0; ii < num_evals; ii++) {
        sampleLoc[ii] = to_ZZ_p(DATA_MAX) + ii + 1; // i.e. from the region outside where valid data might lie
        CPI_evals[ii] = 1;
    }
}

CPI::~CPI() {
    sampleLoc.kill();
    CPI_evals.kill();
}


ZZ_p CPI::makeData(ZZ_p num) const {
    return to_ZZ_p(rep(num) % DATA_MAX);
}


bool CPI::ratFuncInterp(const vec_ZZ_p& evals, long mA, long mB, vec_ZZ_p& P_vec, vec_ZZ_p& Q_vec) {
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

bool CPI::find_roots(vec_ZZ_p& P_vec, vec_ZZ_p& Q_vec, vec_ZZ_p& numerator, vec_ZZ_p& denominator) {
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

bool CPI::set_reconcile(const long otherSetSize, const vec_ZZ_p &otherEvals, vec_ZZ_p &delta_self, vec_ZZ_p &delta_other) {
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


DataObject * CPI::invHash(const ZZ_p num) const {
    Logger::gLog(Logger::METHOD,"Entering CPISync::invHash");
    const ZZ &numZZ = rep(num);
    auto *result = new DataObject(numZZ);
    return result;
}


ZZ_p CPI::hash(const DataObject * datum) const {
    ZZ num = datum->to_ZZ(); // convert the datum to a ZZ

    if (!hashQ && (num >= DATA_MAX))
        Logger::error_and_quit("Cannot add element (" + datum->to_string() + ") "
                               + " whose encoding (" + toStr(num) + ") is larger than  (" + toStr(DATA_MAX) + " - max field element) "
                               + " when using nohash synchronization.  Please increase modulus to at least " + toStr(ceil(log(DATA_MAX + redundant_k) / log(2))) + " bit elements.");

    return to_ZZ_p(num % (DATA_MAX)); // reduce to bit_num bits and make into a ZZ_p
}

ZZ_p CPI::hash2(const long num) const {
    return to_ZZ_p(to_ZZ(num)*101 % (DATA_MAX));
}


bool CPI::addElem(DataObject * datum) {
    int ii;

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

    return true;
}

// update metadata when delete an element by index

bool CPI::delElem(DataObject * datum) {

    ZZ_p hashID;
    // remove data from the hash table. to find the value, a linear search is required
    for(auto iter = CPI_hash.begin(); iter != CPI_hash.end(); iter++) {
        if(iter->second == datum) {
            hashID = to_ZZ_p(iter->first);
            CPI_hash.erase(iter);
            break; // edit we are deleting one thing anyway
        }
    }

    // update cpi evals
    for(int ii = 0; ii < sampleLoc.length(); ii++) {
        CPI_evals[ii] /= (sampleLoc[ii] - hashID);
    }

    return true; //edit You never returned anything
}
