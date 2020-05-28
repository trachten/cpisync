/** MAIN FILE FOR CPISYNC EXECUTIBLE
 * NOTE:  This is a main file designed to demonstrate the functioning of the cpisynclib.
 * The cpisynclib library is not meant to run through this main.cpp file, but rather is
 * designed to be included a library in other application code.
 */

#include <ctime>
#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <getopt.h>
#include <ctime>

#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Communicants/Communicant.h>
#include <CPISync/Communicants/CommSocket.h>
#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Data/DataObject.h>
#include <CPISync/Syncs/GenSync.h>
#include <CPISync/Syncs/CPISync.h>
#include <CPISync/Syncs/CPISync_HalfRound.h>
#include <CPISync/Syncs/CPISync_OneLessRound.h>
#include <CPISync/Syncs/ProbCPISync.h>
#include <CPISync/Syncs/InterCPISync.h>
#include <CPISync/Aux/Logger.h>
#include <CPISync/Communicants/CommString.h>

using namespace std;



// global variables and structs
static int help_flag = 0; /** Assume no help requested. */

// ... Communication mode
static const int CLIENT = 0; /** I am a client connecting to a server. */
static const int SERVER = 1; /** I am a server listening for a client. */
static int comm_flag = CLIENT;

// ... Synchronization mode
static const int CPISYNC = 0; /** Standard, interactive CPISync. */
static const int ONEWAY_CPISYNC = 1; /** One-way (non-interactive) CPISync. */
static const int PROB_CPISYNC = 2; /** Probabilistic CPISync. */
static const int INTER_CPISYNC = 3; /** Interactive, expected-linear-time/comm CPISYNC. */
static const int IBLT_CPISYNC = 4; /** IBLT-based sync. */
static int sync_flag = INTER_CPISYNC; /** Defaults to InterCPISync. */
static int nohash_flag = 0; /** Defaults to 0 = do not allow hashing. */
static int int_flag = 0;    /** Defaults to 0 = do *not* parse string data as big Integers. */
static int numElems = 1;    /** Defaults to 1 = at most 1 element is expected in the data structure. */


timespec diff(timespec start, timespec end);
// Command-line options
static struct option long_options[] = {
        // Comm mode
        {"client", no_argument, &comm_flag,   CLIENT},
        {"server", no_argument, &comm_flag,   SERVER},

        // Sync mode
        {"cpi", no_argument, &sync_flag,      static_cast<int>(GenSync::SyncProtocol::CPISync)},
        {"oneway", no_argument, &sync_flag,   static_cast<int>(GenSync::SyncProtocol::OneWayCPISync)},
        {"prob", no_argument, &sync_flag,     static_cast<int>(GenSync::SyncProtocol::ProbCPISync)},
        {"inter", no_argument, &sync_flag,    static_cast<int>(GenSync::SyncProtocol::InteractiveCPISync)},
        {"iblt", no_argument, &sync_flag,     static_cast<int>(GenSync::SyncProtocol::IBLTSync)},
        {"nohash", no_argument, &nohash_flag, 1}, // if set, then no hashing is done to reduce the element size to a fixed bitlength
        {"integer", no_argument, &int_flag,   1},   // if set, then all set element strings are parsed as arbitrarily-sized Integers

        {"file", required_argument, 0,        'f'}, // a file from which to get elements to be synced
        {"string", required_argument, 0,      's'}, // a string from which to get a sync hash to which to synchronize
        {"host", required_argument, 0,        'h'}, // a host to which to synchronize
        {"port", required_argument, 0, 'o'}, // a port to which to synchronize or on which to list for other synchronizing clients

        // sync parameters
        {"perr", required_argument, 0, 'e'}, // maximum probability of error allowed, for sync algorithms that take this as a parameter
        {"mbar", required_argument, 0, 'm'}, // maximum number of differences in an atomic sync operation
        {"bits", required_argument, 0, 'b'}, // number of bits used to represent each set element internally
        {"partition", required_argument, 0, 'p'}, // the number of partitions in interactive CPISync
        {"numElems", required_argument, 0, 'n'},  // maximum number of elements expected in the sync object (esp. for IBLT)

        // other
        {"help", no_argument, &help_flag, 1} // get help
};

/**
 * Display usage
 * @param str The command used to run this program
 */
string display_usage(char *progName) {
    stringstream strstr;
    strstr << "Usage:  " << progName
           << "   [(-f | --file) <filename>]" << endl
           << "   [(-s | --string) <string>]" << endl
           << "   [(-h | --host) <hostname>]" << endl
           << "   [(-o | --port] <port number>]" << endl
           << "   [(-e | --perr) <max error prob> " << endl
           << "   [(-m | --mbar) <int>" << endl
           << "   [(-b | --bits) <int>" << endl
           << "   [(-p | --partition) <int>" << endl
           << "   [(-n | --numElems) <int>" << endl
           << "   [--cpi | --oneway | --prob | --inter | --iblt]" << endl
           << "   [--nohash]" << endl
           << "   [--integer]" << endl
           << "   [--client | --server]" << endl
           << "   [--help]" << endl
           << endl;

    return strstr.str();
}

/**
 * Implements a full set of command options
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, char *argv[]) {
    // constants


    // ... parse the command-line arguments
    int option_index = 0;
    int choice;

    struct timespec timerStart, timerEnd;
    //clock_gettime(CLOCK_REALTIME, &timerStart);

    // ... default values
    list<shared_ptr<DataObject>> data;
    string cStr; // initially unused
    string host; // for Communication Sockets
    int port = 10000; // default port for Communication Sockets
    double perr = 0.00005; // max error probability
    long mbar = 5;
    long bits = 32;
    long pFactor = 2;
    // the maximum number of elements expected in the data structure
    bool dataInFile = false;
    string fileName; // the file to tie to the synchronization

    // 0. Parse the command-line options
    do {
        choice = getopt_long(argc, argv, "f:s:h:o:p:e:m:b:n:?",
                             long_options, &option_index);
        switch (choice) {
            case 0:
                // long option without a short version
                // ... so far, nothing to do here
                break;
            case 'f':
            {// read data from a file
                dataInFile = true;
                fileName = optarg;
                string str;
                break;
            }
            // broken below:
            //case 's': // read sync hash info from a base64 string
            //    cStr = new CommString(optarg, true);
            //    break;

            case 'h':
                host = optarg;
                break;

            case 'o':
                port = atoi(optarg);
                break;

            case 'p':
                pFactor = atoi(optarg);
                break;

            case 'e':
                perr = atof(optarg);
                break;

            case 'm':
                mbar = atol(optarg);
                break;

            case 'n':


            case 'b':
                bits = atol(optarg);
                break;

            case '?':
                Logger::error_and_quit(display_usage(argv[0]));
                break;

        }
    } while (choice != -1);

    // 1. Command-line post-processing
    if (help_flag == 1) // user requested help?
        Logger::error_and_quit(display_usage(argv[0]));

    // ... if some arguments were not processed ... call an error and halt
    if (optind < argc) {
        string args = "Unprocessed arguments: ";
        while (optind < argc)
            args += argv[optind++] + string(" ");
        Logger::error_and_quit(args + "\n" + display_usage(argv[0]));
    }
    // ... check for command-line conflicts
    if (!cStr.empty() && (!host.empty()))
        Logger::error_and_quit("Cannot specify both sync hash string AND host. " + display_usage(argv[0]));

    // 2. Set up the sync
    // .... sync protocol
    GenSync::SyncProtocol proto;
    switch (sync_flag) {
        case CPISYNC:
            if (nohash_flag)
                proto=GenSync::SyncProtocol::CPISync_OneLessRound;
            else
                proto=GenSync::SyncProtocol::CPISync;
            break;
        case ONEWAY_CPISYNC:
            proto=GenSync::SyncProtocol::CPISync_HalfRound;
            break;
        case PROB_CPISYNC:
            proto=GenSync::SyncProtocol::ProbCPISync;
            break;
        case INTER_CPISYNC:
            proto=GenSync::SyncProtocol::InteractiveCPISync;
            break;
        case IBLT_CPISYNC:
            proto=GenSync::SyncProtocol::OneWayIBLTSync;
            break;
    }
    //Logger::gLog(Logger::METHOD, "Sync Method:  " + toStr(proto));


    // ... communicants
    GenSync::SyncComm comm;
    if (!cStr.empty()) // are we syncing with a string
        comm=GenSync::SyncComm::string; // cStr contains the string
    else // we are syncing with a socket
        comm=GenSync::SyncComm::socket;

    int negLogPerr = (int) -log(perr)/log(2); // the negative log of perr ... this is how the constructors expect their error value

    // 3. Deploy the sync
    GenSync theSync = GenSync::Builder().
            setProtocol(proto).
            setComm(comm).
            setIoStr(cStr).
            setPort(port).
            setErr(negLogPerr).
            setMbar(mbar).
            setBits(bits).
            setNumPartitions(pFactor).
            setDataFile(fileName).
            build();

    // ... add data
    if (!dataInFile) {
        cout << "Please enter set data, one by one, ending with an empty string:" << endl;
        string str; // input string
        int ii = 0;
        cout << "Set element #0: ";
        getline(cin, str);
        while (!str.empty()) {
            theSync.addElem(make_shared<DataObject>(str)); // add this datum to our list
            cout << "Set element #" << ++ii << ": ";
            getline(cin, str);
        }
        cout << "Input noted." << endl;
    }

    bool syncSuccess;
    if (comm_flag == CLIENT) {
        Logger::gLog(Logger::METHOD, "\n[CLIENT]");
        syncSuccess = theSync.clientSyncBegin(0);
    } else {
        Logger::gLog(Logger::METHOD, "\n[SERVER]");
        syncSuccess = theSync.serverSyncBegin(0);
    }

    // 4. Results
    if (syncSuccess) {
        if (!cStr.empty()) // syncing with a string, dump the current string contents
            cout << "Current string contents: " << ((CommString *) comm)->getString() << endl;
        //clock_gettime(CLOCK_REALTIME, &timerEnd);
        timespec timeTaken = diff(timerStart, timerEnd);
//        double totalTime = (timeTaken.tv_sec) + (double) timeTaken.tv_nsec / 1000000000;
//        // Report statistics
//        cout << endl << "   Data transferred: " << comm->getXferBytesTot() << " bytes" << endl
//             << "   Data received:    " << comm->getRecvBytesTot() << " bytes" << endl
//             << "   Total Time elapsed:    " << (double) totalTime <<" seconds." <<endl
//             << "   CPU Time elapsed:      " << (double) (clock() - comm->getLastTimeTot()) / CLOCKS_PER_SEC << " seconds." << endl;
        exit(SYNC_SUCCESS);
    }
    else
        exit(SYNC_FAILURE);
}

timespec diff(timespec start, timespec end)
{
    timespec temp{};
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}
