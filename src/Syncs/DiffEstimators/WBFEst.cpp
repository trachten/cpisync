//
// Created by Zifan Wang on 2019-08-20.
//

#include <CPISync/Syncs/DiffEstimators/WBFEst.h>
#include <CPISync/Aux/Auxiliary.h>
#include <CPISync/Syncs/DiffEstimators/WrappedBloomFilter.h>

WBFEst::WBFEst(long difEstError){
    myProtocol = DIFF_EST::WrappedBloomFilter;
    errProb = difEstError;
}

WBFEst::~WBFEst(){}

long WBFEst::ClientDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData){
    commSync->commConnect();
    // determine the size for WBF
    commSync->commSend((long)myData.size());
    auto expSize = (size_t)commSync->commRecv_long();

    // build a client WBF
    WrappedBloomFilter clientWBF(expSize);
    for(auto elem: myData)
        clientWBF.insert(elem->to_ZZ());

    // send to server
    commSync->commSend(clientWBF);

    // receive the estimated differences from server
    long symmetricDiff = commSync->commRecv_long();

    commSync->commClose();
    return symmetricDiff;
}

long WBFEst::ServerDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData){
    commSync->commListen();

    // determine the size for WBF
    auto otherSize = (size_t)commSync->commRecv_long();
    auto expSize = (myData.size() > otherSize) ? myData.size() : otherSize ;
    // tell the client the size for initializing a WBF
    commSync->commSend((long)expSize);
    // receive WBF from client
    WrappedBloomFilter clientWBF = commSync->commRecv_WBF();
    // unwrap client filter using data on the server
    auto result = clientWBF.unwrap(myData);
    // add bias according to error probablity
    result += 2 * clientWBF.errCorrect(expSize,errProb,result);
    // send result to client
    commSync->commSend(result);
    commSync->commClose();
    return result;
}