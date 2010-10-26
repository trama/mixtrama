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
	transport();
	virtual ~transport();
protected:
	    virtual void handleSelfMsg(cMessage* msg);
	    virtual void handleUpperMsg(cMessage *msg);
	    virtual void handleLowerMsg(cMessage *msg);
	    virtual void handleLowerControl(cMessage *msg);
	    virtual void handleUpperControl(cMessage *msg);
};

#endif /* TRANSPORT_H_ */
