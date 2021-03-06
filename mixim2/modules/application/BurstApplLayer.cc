/* -*- mode:c++ -*- ********************************************************
 * file:        BurstApplLayer.cc
 *
 * author:      Daniel Willkomm
 *
 * copyright:   (C) 2004 Telecommunication Networks Group (TKN) at
 *              Technische Universitaet Berlin, Germany.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 ***************************************************************************
 * part of:     framework implementation developed by tkn
 * description: application layer like the TestApplLayer but sends burst of
 *               messages
 **************************************************************************/


#include "BurstApplLayer.h"
#include "tutils.h"

Define_Module(BurstApplLayer);

// do some initialization
void BurstApplLayer::initialize(int stage)
{
    TestApplLayer::initialize(stage);

    if(stage==0){
        if(hasPar("burstSize"))
            burstSize = par("burstSize");
        else
            burstSize = 3;

        wait = hasPar("waitBeforeSend") ? par("waitBeforeSend") : 0;
    }
    else if (stage == 1){

    	if(delayTimer->isScheduled()){
    		cancelEvent(delayTimer);
    		scheduleAt(simTime()+wait, delayTimer);
    	}

    }
}


void BurstApplLayer::handleSelfMsg(cMessage *msg)
{
    switch(msg->getKind())
    {
    case SEND_BROADCAST_TIMER:
        for(int i=0; i<burstSize; i++) {
            sendBroadcast();
        }
        delete msg;
        break;
    default:
        EVT <<" Unkown selfmessage! -> delete, kind: "<<msg->getKind()<<endl;
    }
}
