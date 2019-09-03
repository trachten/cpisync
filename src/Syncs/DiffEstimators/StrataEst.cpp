//
// Created by Zifan Wang on 2019-08-26.
//

#include <CPISync/Syncs/DiffEstimators/StrataEst.h>
#include <CPISync/Syncs/DiffEstimators/Strata.h>

StrataEst::StrataEst(long difEstError){
    estError = difEstError;
}

StrataEst::~StrataEst() = default;

long StrataEst::ClientDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData){
    commSync->commConnect();
    // determine size for strata
    auto othersize = strTo<size_t>(commSync->commRecv_string());
    size_t size = othersize + myData.size();

    // construct client strata
    Strata clientStrata(sizeof(ZZ), 50, log2(size));

    for(auto &elem : myData)
        clientStrata.insert(elem);

    // send client strata to server
    commSync->commSend(clientStrata);

    long diff = commSync->commRecv_long();

    commSync->commClose();
    return diff;
}

long StrataEst::ServerDifEst(shared_ptr<Communicant> commSync,list<shared_ptr<DataObject>> myData){
    commSync->commListen();

    // send size of server data to client to determine parameter for strata
    commSync->commSend(toStr(myData.size()));

    Strata clientStrata = commSync->commRecv_strata();
    Strata serverStrata(sizeof(ZZ),50,clientStrata.size());

    for(auto &elem : myData)
        serverStrata.insert(elem);
    clientStrata -= serverStrata ;

    long diff = clientStrata.decode();

    commSync->commSend(diff);
    commSync->commClose();

    return diff;
    
}
