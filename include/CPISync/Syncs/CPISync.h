/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */
 
 // CPISYNC.h -- class for sync using CPI algorithm 
#ifndef CPI_SYNC_H
#define CPI_SYNC_H

#include <NTL/RR.h>
#include <NTL/ZZ_pX.h>
#include <NTL/vec_ZZ_p.h>
#include <NTL/ZZ_pXFactoring.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Syncs/GenSync.h>

// namespaces

using namespace NTL;

/**
 * Implements a data structure for storing mathematical multi-sets of data
 *    in a manner that is consistent with fast synchronization.
 *    More precisely, two remote multi-sets SA and SB (of b-bit vectors)
 *    with m differences between them can be synchronized using roughly
 *    m (b-bit) packets of communication.
 * 
 * Note that this data structure is really designed for sets, rather than multi-sets.
 * It can handle multi-sets, but there are more efficient ways of dealing with them.
 * 
 *
 */

class CPISync : public SyncMethod {
public:

  /**
   * Specific class constructor.
   * 
   * @param m_bar Maximum number of differences guaranteed to sync; in some cases,
   *    more than m_bar differences can be synced.  Must be <= 2^bits.
   * @param bits  The number of bits used to represent each set element. Must be >= 2.
   *    The data structure cannot store more than 2^bits elements.
   * @param epsilon An upper bound on the probability of error of the synchronization,
   *    expressed in its negative log.  In other words, the actual probability of error
   *    is upper bounded by 2^-epsilon.  This parameter is ignored if redundant is non-zero.
   * @param redundant The number of redundant points used for verification.  If this is zero,
   *    then this redundancy is computed directly from the allowed probability of error.
   * @param hashes Should data be stored as is, or first reduced via a hash

   * 
   *    Internal parameters are tweaked to guarantee this, subject to an assumption
   *    that an internal hash does not collide, and that there be at least 2 items
   *    in the union of the two sets that are being synchronized int.
   */
  CPISync(long m_bar, long bits, int epsilon, int redundant = 0, bool hashes = false);

  /**
   * General class destructor
   */
  ~CPISync() override;

  /**
   * Connect as a client to a specific communicant and computes differences between the two (without actually updating them).
   * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference).
   * %R:  Sync_Server must have been called at that communicant.
   * 
   * @param commSync The communicant to whom to connect.
   * @param selfMinusOther A result of reconciliation.  Elements that I have that the other Communicant does not.
   * @param otherMinusSelf A result of reconciliation.  Elements that the other Communicant has that I do not.
   * @return true iff the connection and subsequent synchronization appear to be successful.
   */
  bool SyncClient(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;

  /**
   * Waits for a client to connect from a specific communicant and computes differences between the two (without actually updating them).
   * All results are *added* to the selfMinusOther and otherMinusSelf parameters (passed by reference).
   *      * 
   * @param commSync The Communicant to whom to connect.
   * @param selfMinusOther A result of reconciliation.  Elements that I have that the other Communicant does not.
   * @param otherMinusSelf A result of reconciliation.  Elements that the other Communicant has that I do not.
   * @return true iff the connection and subsequent synchronization appear to be successful.
   */
  bool SyncServer(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther, list<shared_ptr<DataObject>> &otherMinusSelf) override;


  /**
   * Sends all elements stored in this object to a Communicant.
   * @param commSync The Communicant to whom to connect.
   * @param selfMinusOther All elements transmitted are added to this parameter, passed by reference.
   */
  void sendAllElem(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &selfMinusOther);

  /**
   * Receives elements from a Communicant.
   * @param commSync The Communicant to whom to connect.
   * @param otherMinusSelf All elements received are added to this parameter, passed by reference.
   * @requires A connection to the other Communicant must already be present.
   */
  static void receiveAllElem(const shared_ptr<Communicant>& commSync, list<shared_ptr<DataObject>> &otherMinusSelf);
  /*
   ** update metadata when an element is being added
   */
  bool addElem(shared_ptr<DataObject> newDatum) override;

  template <typename T>
  bool addElem(T* newDatum) {
      auto *newDO = make_shared<DataObject>(*newDatum);
      bool result = addElem(newDO);
      return result;
  }

  // update metadata when an element is being deleted (the element is supplied by index)
  bool delElem(shared_ptr<DataObject> newDatum) override;

  /**
   * @return A string with some internal information about this object.
   */
  string getName() override;

  /**
   * @return A string representing the elements (with hashes) stored in the CPISync object.
   */
  string printElem();

  
protected:
  // internal data
  bool probCPI{}; /** If true, then CPISync actually operates using the probabilistic CPISync protocol, wherein
                      *  the presumed number of differences is doubled until a correct upper bound is found.
                      *  If false, then CPISync operates with just the prescribed upper bound on differences and
                      *  fails or produces incorrect results if this is not actually an upper bound.
                      */
  bool oneWay{}; /** Enables one-way CPISync when set to true.  Otherwise, both client and server are synced.
                  * This is mutually exclusive with probCPI (i.e. they cannot both be set to true). */
  bool hashQ; /** Typically CPISync syncs hashes of elements (hashQ == true), then exchange the differing elements.
                  * With hashQ == false, trivial hashes are used (that are in one-to-one correspondence with data)
                  * so that actual element differences are computed in CPISync, and
                  * one round of communication is saved.  Under this situation, all elements, must comfortably
                  *  fit within the prescribed bit-length for internal storage, and no element duplicates are permitted.
                  *
                  * With hashQ == true, element duplicates are permitted (handled through hashing) and, potentially,
                 * element  representations are smaller.
                 */
  bool keepAlive{}; /** If this is true, the CPISync does not setup or close the communicant connection - these
                     *  can be handled manually.
                     */

  long bitNum; /** Number of bits used to represent an element of the set that is being synchronized. */
  long maxDiff; /** Maximum number of differences to synchronize (for regular CPIsync) */
  int probEps; /** Negative log of the upper bound on the probability of error for the synchronization. */
  ZZ fieldSize; /** The size of the finite field used to represent set elements. */

  vec_ZZ_p sampleLoc; /** Locations at which the set's characteristic polynomial is sampled. */
  long currDiff; /** The number of differences currently being synchronization. (initially set by the constructor) - for iterative methods. */
  int redundant_k; /** the number of redundant samples of the characteristic polynomial to evaluate.
                         *  This relates to the probability of error for the synchronization. */

  map< ZZ, shared_ptr<DataObject> > CPI_hash; /** list of pairs, one for each element in the set (to be synchronized).
                                           *  The first item in the pair is a hash (a long integer) 
                                           *  of the second item, which is the set element.
                                           *  All operations are done on the hashes, and this look-up table can be used to retrieve
                                           *  the actual element once the hashes have been synchronized.
                                           */

  // helper functions

  /**
   * Initialize <num> sample points and some default values for CPISync
   * @param num The number of sample points to initialize.
   * @modifies Initializes the sampleLoc and CPI_meta arrays with <num> initial values.
   */
  void initData(int num);

  /**
   * Interpolates a rational function with a given set of evaluations at the sample locations.
   * The numerator of the rational function is placed in P_vec and the denominator in Q_vec.
   * @param evals evaluations of the rational function that is being interpolated at the points
   *    given by the sampleLoc vector.  The i-th evaluation is taken based on the i-th sampleLoc entry.
   * @param mA An upper bound on the size of the numerator of the recovered rational function.  This should be
   *    as tight as possible to avoid unnecessary computation.
   * @param mB An upper bound on the size of the numerator of the recovered rational function.  This should be
   *    as tight as possible to avoid unnecessary computation.
   * @param P_vec Coefficients of the numerator of the interpolated rational function.  This is set by
   *    the the ratio_Interpolation upon completion of the interpolation.  The coefficients are from low
   *    to high order term ... meaning that P_vec[0] is the coefficient of the constant term, P_vec[0] the
   *    coefficient of x, ... P_vec[i] the coefficient of x^i
   * @param Q_vec Coefficients of the denominator of the interpolated rational function.  This is set by
   *    the the ratio_Interpolation upon completion of the interpolation.  The coefficients are from low
   *    to high order term ... meaning that Q_vec[0] is the coefficient of the constant term, Q_vec[0] the
   *    coefficient of x, ... Q_vec[i] the coefficient of x^i
   * @return true iff the rational function interpolation appears to have completed properly, meaning that some
   *    function was interpolated that meets the evaluations at the sample locations given by sampleLoc
   */
  bool ratFuncInterp(const vec_ZZ_p& evals, long mA, long mB, vec_ZZ_p& P_vec, vec_ZZ_p& Q_vec);

  /**
   * Simultaneously finds the roots of two polynomials (that correspond to the numerator and denominator, respectively
   * of an interpolated rational function.
   * 
   * @requires P_vec and Q_vec must be monic (not checked) and square-free (checked)
   * @param P_vec The first polynomial whose roots are to be found.
   * @param Q_vec The second polynomial whose roots are to be found.
   * @param numerator Returns a list of roots of the P_vec polynomial.
   * @param denominator Returns a list of roots of the Q_vec polynomial.
   * @return true if root-finding succeeded.  Reasons for failure include:
   *        * either P_vec or Q_vec are not square free
   *        * factoring procedure failed to factor some term down to a linear factor
   */
  static bool find_roots(vec_ZZ_p& P_vec, vec_ZZ_p& Q_vec, vec_ZZ_p& numerator, vec_ZZ_p& denominator);


  /**
   * Reconciles the set represented by this object with the another set of
   * size <otherSetSize> and characteristic polynomial evaluations <otherEvals>.
   * @param otherSetSize The size of the other set with which reconciliation is occurring.
   * @param otherEvals Evaluations of other set's characteristic polynomial on exactly the same
   *    sample locations as this set is being evaluated.
   * @param delta_self All elements found that are in my set by not in the other set are put into this vector
   * @param delta_other All elements found that are in the other set by not in my set are put into this vector
   * @return true if reconciliation succeeded.  Possible reasons for reconciliation failure include:
   *    * inability to interpolate the reconciliation rational function
   *    * inability to factor the numerator or denominator of the interpolation rational function
   */
  bool set_reconcile(long otherSetSize, const vec_ZZ_p& otherEvals, vec_ZZ_p &delta_self, vec_ZZ_p &delta_other);

  vec_ZZ_p CPI_evals; /** The ii-th entry of this vector is the evaluation of this data structure's characteristic
                        * polynomial at the ii-th sample point. */
  ZZ DATA_MAX; /** Set elements must be within the range 0..data_max-1.  Sample locations are taken between data_max and ZZ_p::modulus() */

    /**
     * Encode and transmit synchronization parameters (e.g. synchronization scheme, probability of error ...)
     * to another communicant for the purposes of ensuring that both are using the same scheme.
     * @param commSync The communicant to whom to send the parameters.
     * @require commSync must already be connected
     * @throws SyncFailureException if the parameters don't match between the synchronizing parties.
      */
  void SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay = false) override;

  /**
   * Receive synchronization parameters from another communicant and compare to the current object.
   * Return true iff they are the same.
   * @param commSync The communicant to whom to send the parameters.
   * @param oneWay If set to true, no response is expected from the other communicant (the sync is one-way).
   * @require commSync must already be connected
   * @throws SyncFailureException if the parameters don't match between the synchronizing parties.
   */
  void RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay = false) override;

   /**
    * Set MBar and recalculate the parameters that are dependent on mBar
    */
	void SetSymDiffs(long mBar) override;

private:

  /**
   * Computes a hash of the given datum of size bit_num, used internally within CPISync.
   * Initial synchronization actually occurs only on these hashes.
   * @param datum The datum to hash
   * @return A hash of the datum.
   * @note The hash must be between 0 and 2^(bit_num) inclusive.
   * @note If there is a collision between two elements under this hash, it is possible that the synchronization
   * will fail.  This probability is not included in the probability of error computation.
   */
  ZZ_p _hash(const shared_ptr<DataObject>datum) const;

  /**
   * Inverts the hash above when the noHash boolean is set
   */
  shared_ptr<DataObject> _invHash(ZZ_p num) const;

  /**
   * A secondary hash used for disambiguating set elements with the same hash.
   * 
   * i. It should be computationally efficient and deterministic.in num (i.e. provides
   *    the same response given the same parameter).
   * ii. It should return 0 when given parameter 0
   * @param num A parameter of the hash.
   * @return An arbitrary ZZ_p corresponding to num.
   */
  ZZ_p _hash2(long num) const;

  /**
   * Converts num to a number between 1 and DATA_MAX
   * @return 
   */
  ZZ_p _makeData(ZZ_p num) const;

  /**
   * Sends one set element, properly unhashed, to the other side
   * @param element The set element to send.  The element is stored internally as an integer;
   *      procedure applies the appropriate unhashing to send the actual element (rather than its hash).
   * @throws SyncFailureException if a synchronization error is detected.
   */
  void _sendSetElem(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther, const ZZ_p &element);

  /**
   * Receives one set element, properly unhashed, from the other side
   * @param element The set element to receive.  If no hash is used, nothing must actually be received ...
   * the element is simply appended to the otherMinusSelf list.
   */
  void _recvSetElem(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &otherMinusSelf, ZZ_p element);

  /**
   * Helper function for Sync_Client and Sync_Server.  Sends a second round to the other
   * communicant translating reconciled hashes into actual strings.
   */
  void _makeStructures(const shared_ptr<Communicant> &commSync, list<shared_ptr<DataObject>> &selfMinusOther,
					   list<shared_ptr<DataObject>> &otherMinusSelf, vec_ZZ_p &delta_self, vec_ZZ_p &delta_other);
};
#endif
