/* This code is part of the CPISync project developed at Boston University.  Please see the README for use and references. */

#include <CPISync/Communicants/Communicant.h>
#include <CPISync/Aux/SyncMethod.h>
#include <CPISync/Aux/Exceptions.h>
#include <CPISync/Aux/Logger.h>

SyncMethod::SyncMethod() {
    SyncID = SYNC_TYPE::GenericSync; // synchronization type
}

SyncMethod::~SyncMethod() = default;

void SyncMethod::SendSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
 if (!commSync->establishModSend(oneWay)) // establish ZZ_p modulus - must be first
     throw SyncFailureException("Sync parameters do not match between communicants.");
}

void SyncMethod::RecvSyncParam(const shared_ptr<Communicant>& commSync, bool oneWay /* = false */) {
if (!commSync->establishModRecv(oneWay)) // establish ZZ_p modulus - must be first
      throw SyncFailureException("Sync parameters do not match between communicants.");   
}


//SYNC METHOD STATS RECORDING

SyncMethod::SyncStats::SyncStats(){
    xmitBytes = recvBytes = commTime = compTime = idleTime = 0;
}

void SyncMethod::SyncStats::reset(StatID statID){
    switch(statID) {
        case SyncMethod::SyncStats::XMIT:
            xmitBytes = 0;
            break;
        case SyncMethod::SyncStats::RECV:
            recvBytes = 0;
            break;
        case SyncMethod::SyncStats::COMM_TIME:
            commTime = 0;
            break;
        case SyncMethod::SyncStats::IDLE_TIME:
            idleTime = 0;
            break;
        case SyncMethod::SyncStats::COMP_TIME:
            compTime = 0;
            break;
        case SyncMethod::SyncStats::ALL:
            xmitBytes = recvBytes = commTime = compTime = idleTime = 0;
            break;
        default:
            Logger::error("Invalid type specified in reset");
    }
}

double SyncMethod::SyncStats::getStat(StatID statID){
    switch(statID) {
        case SyncMethod::SyncStats::XMIT:
            return xmitBytes;
            break;
        case SyncMethod::SyncStats::RECV:
            return recvBytes;
            break;
        case SyncMethod::SyncStats::COMM_TIME:
            return commTime;
            break;
        case SyncMethod::SyncStats::IDLE_TIME:
            return idleTime;
            break;
        case SyncMethod::SyncStats::COMP_TIME:
            return compTime;
            break;
        default:
            Logger::error("Invalid get type specified in getStat");
            return -1;
    }
}


void SyncMethod::SyncStats::increment(StatID statID, double incr){
    switch(statID) {
        case SyncMethod::SyncStats::XMIT:
            xmitBytes += incr;
            break;
        case SyncMethod::SyncStats::RECV:
            recvBytes += incr;
            break;
        case SyncMethod::SyncStats::COMM_TIME:
            commTime += incr;
            break;
        case SyncMethod::SyncStats::IDLE_TIME:
            idleTime += incr;
            break;
        case SyncMethod::SyncStats::COMP_TIME:
            compTime += incr;
            break;
        case SyncMethod::SyncStats::ALL:
            xmitBytes += floor(incr);
            recvBytes += floor(incr);
            commTime += incr;
            compTime += incr;
            idleTime += incr;
            break;
        default:
            Logger::error("Invalid type specified in increment");
    }
}


void SyncMethod::SyncStats::timerStart(StatID timerID){
    switch(timerID) {
        case SyncMethod::SyncStats::COMM_TIME:
            commStart = std::chrono::high_resolution_clock::now();
            break;
        case SyncMethod::SyncStats::IDLE_TIME:
            idleStart = std::chrono::high_resolution_clock::now();
            break;
        case SyncMethod::SyncStats::COMP_TIME:
            compStart = std::chrono::high_resolution_clock::now();
            break;
        case SyncMethod::SyncStats::ALL:
            commStart = std::chrono::high_resolution_clock::now();
            idleStart = std::chrono::high_resolution_clock::now();
            compStart = std::chrono::high_resolution_clock::now();
            break;

        default:
            Logger::error("Invalid timer type specified in timerStart");

    }
}


void SyncMethod::SyncStats::timerEnd(StatID timerID){
    switch(timerID) {
        case SyncMethod::SyncStats::COMM_TIME:
            commTime += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - commStart).count() * 1e-6);
            break;
        case SyncMethod::SyncStats::IDLE_TIME:
            idleTime += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - idleStart).count() * 1e-6);
            break;
        case SyncMethod::SyncStats::COMP_TIME:
            compTime += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - compStart).count() * 1e-6);
            break;
        case SyncMethod::SyncStats::ALL:
            commTime += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - commStart).count() * 1e-6);
            idleTime += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - idleStart).count() * 1e-6);
            compTime += (std::chrono::duration_cast<std::chrono::microseconds> (std::chrono::high_resolution_clock::now() - compStart).count() * 1e-6);
            break;

        default:
            Logger::error("Invalid timer type specified in timerEnd");

    }
}



