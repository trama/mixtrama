/*
 * transport.h
 *
 *  Created on: 25/ott/2010
 *      Author: trama
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "BaseLayer.h"
#include <map>
#include <list>
#include "transpCInfo_m.h"

class transport: public BaseLayer {

public:
	virtual void initialize(int);
	virtual void finish();
	virtual ~transport();

    struct sck //socket definition
    {
        //int sockId; // supposed to be unique across apps-- module ID
        int appGateIndex;
        int localPort;
        bool isControlPort;
        //int localPort2;
        //int controlPort1;
        //int controlPort2;
    };

    typedef std::list<sck *> sckl; // list of sockets
    typedef std::map<int,sck *> sckIdMap;

protected:
		virtual void handleMessage(cMessage* msg);
	    virtual void handleSelfMsg(cMessage* msg);
	    virtual void handleUpperMsg(cMessage *msg);
	    virtual void handleLowerMsg(cMessage *msg);
	    virtual void handleLowerControl(cMessage *msg);
	    virtual void handleUpperControl(cMessage *msg);

	    //bool debug;
	    int nbPacketDropped;
	    int numApplLayer;
	    simtime_t elab_time;

	    enum queue_status{
	    	IDLE=1,
	    	TX
	    };

	    transp_status status;

	    sckIdMap scksID;

	    // bind socket
	    virtual void bind(int gateIndex, transpCInfo *ctrl);
};

#endif /* TRANSPORT_H_ */
