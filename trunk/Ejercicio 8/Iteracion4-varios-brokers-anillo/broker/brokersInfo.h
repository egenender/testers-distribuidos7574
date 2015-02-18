/* 
 * File:   brokersInfo.h
 * Author: ferno
 *
 */

#ifndef BROKERSINFO_H
#define	BROKERSINFO_H

typedef struct TBrokerInfo {
    int idBroker;
    char* ipBroker;
} TBrokerInfo;

//const TBrokerInfo IP_BROKERS[CANT_BROKERS] = {{1, "127.0.0.1"}, {2, "192.168.2.3"}, {3, "192.168.2.9"}};
const TBrokerInfo IP_BROKERS[CANT_BROKERS] = {{1, "127.0.0.1"}, {2, "192.168.2.3"}};
#endif	/* BROKERSINFO_H */