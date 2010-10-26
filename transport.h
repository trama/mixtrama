/*
 * transport.h
 *
 *  Created on: 25/ott/2010
 *      Author: trama
 */

#ifndef TRANSPORT_H_
#define TRANSPORT_H_

#include "BaseLayer.h"

class transport: public BaseLayer {
public:
	virtual void initialize(int);
	virtual void finish();
	virtual ~transport();
protected:
		virtual void handleMessage(cMessage* msg);
	    virtual void handleSelfMsg(cMessage* msg);
	    virtual void handleUpperMsg(cMessage *msg);
	    virtual void handleLowerMsg(cMessage *msg);
	    virtual void handleLowerControl(cMessage *msg);
	    virtual void handleUpperControl(cMessage *msg);

	    bool debug;
	    int nbPacketDropped;
	    int numApplLayer;
	    simtime_t elab_time;

};

#endif /* TRANSPORT_H_ */
