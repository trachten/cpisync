/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references.
 * Created on June 20, 2017, 5:04 PM
 */

#ifndef CONSTS_AND_TYPES
#define CONSTS_AND_TYPES

#include <NTL/ZZ.h>
#include <NTL/RR.h>

typedef unsigned char byte;
typedef std::basic_string<unsigned char> ustring; // a string of unsigned characters

// namespaces
using namespace NTL;
using std::cout;
using std::clog;
using std::endl;
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
using std::ostream;
using std::invalid_argument;
using std::runtime_error;

// CONSTANT VALUES

const ZZ ZZ_ZERO = to_ZZ("0");
const ZZ ZZ_ONE = to_ZZ("1");
const ZZ ZZ_TWO = to_ZZ("2");
const RR RR_ONE = (RR) 1.0;
const RR RR_TWO = (RR) 2.0;
const string SYNC_FAILED = "ERR: SYNC FAILED"; // message that a sync attempt has failed
// ... these constants must copy at most one byte
const byte SYNC_FAIL_FLAG = 0; /** The sync failed. */
const byte SYNC_OK_FLAG = 1; /** The sync succeeded. */
const byte SYNC_SOME_INFO = 2; /** I have some information to sync. */
const byte SYNC_NO_INFO = 3; /** I have no information to sync. */

// ... ... synchronization type
enum class SYNC_TYPE : byte {
  GenericSync,
  CPISync,
  CPISync_HalfRound,
  CPISync_HalfRound_Hashed,
  CPISync_OneLessRound,
  CPISync_ExistingConnection,
  Interactive_CPISync,
  Priority_CPISync,
  HashSync,
  FullSync,
  IBLTSync,
  IBLTSync_HalfRound,
  IBLTSync_Multiset
};

// ... Error constants
static const int SYNC_SUCCESS = 0; /** Exit status when synchronization succeeds. */
static const int SYNC_FAILURE = -1; /** Exit status when synchronization fails. */
static const int GENERAL_ERROR = -2; /** Exit status for a general error. */

#endif /* TYPES_H */

