/*
 * transport.cc
 *
 *  Created on: 25/ott/2010
 *      Author: trama
 */

#include "transport.h"
#include <BaseMacLayer.h>

Define_Module(transport);

void transport::initialize(int stage){
	BaseLayer::initialize(stage);
		if(stage == 0) {
			numApplLayer = par("numApplLayer");
			debug = par("debug").boolValue();
			elab_time = par("elaborationTime").doubleValue();

			EV << "Transport elab_time "<<elab_time<<endl;
			//for now, only one app layer supported

	        upperGateIn  = gateBaseId("upperGateIn");
	        upperGateOut = gateBaseId("upperGateOut");
	        upperControlIn  = gateBaseId("upperControlIn");
	        upperControlOut = gateBaseId("upperControlOut");


			EV << "the gates present in this modules are:\nHigher ports\n"<<
			upperGateIn<<" "<<
			upperGateOut <<" "<<
	        upperControlIn<<" "<<
	        upperControlOut <<" \nLower ports\n"<<
	        lowerGateIn  <<" "<<
	        lowerGateOut<<" "<<
	        lowerControlIn  <<" "<<
	        lowerControlOut <<endl;

			EV << "The gates upperGateIn/Out and ControlIn/Out are vector gates with size"<<
					(isGateVector("upperGateIn") ? gateSize("upperGateIn") : -20 )<<endl;

	        nbPacketDropped = 0;

		}
}

void transport::handleMessage(cMessage* msg)
{
    if (msg->isSelfMessage()){
        handleSelfMsg(msg);
    } else if(msg->getArrivalGateId()==lowerControlIn){
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_CONTROL,msg);
        handleLowerControl(msg);
    } else if(msg->getArrivalGateId()==lowerGateIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_DATA,msg);
        handleLowerMsg(msg);
    } else if(msg->getArrivalGateId()==upperGateIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_DATA,msg);
        handleUpperMsg(msg);
    } else if(msg->getArrivalGateId()==upperControlIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_CONTROL,msg);
        handleUpperControl(msg);
    } else if(msg->getArrivalGateId()==-1) {
        /* Classes extending this class may not use all the gates, f.e.
         * BaseApplLayer has no upper gates. In this case all upper gate-
         * handles are initialized to -1. When getArrivalGateId() equals -1,
         * it would be wrong to forward the message to one of these gates,
         * as they actually don't exist, so raise an error instead.
         */
        opp_error("No self message and no gateID?? Check configuration.");
    } else {
        /* msg->getArrivalGateId() should be valid, but it isn't recognized
         * here. This could signal the case that this class is extended
         * with extra gates, but handleMessage() isn't overridden to
         * check for the new gate(s).
         */
        opp_error("Unknown gateID?? Check configuration or override handleMessage().");
    }
}

void transport::finish(){
    recordScalar("dropped", nbPacketDropped);

    //cancelAndDelete(delayTimer);
}

transport::~transport(){

}



void transport::handleSelfMsg(cMessage* msg){}
void transport::handleUpperMsg(cMessage *msg){
	EV<<"Received packet from up\nSending packet down\n";
	sendDelayed(msg,elab_time,lowerGateOut);
}

void transport::handleLowerMsg(cMessage *msg){
	EV<<"Received packet from down\nSending packet up\n";
	sendDelayed(msg,elab_time,upperGateOut);
}

void transport::handleLowerControl(cMessage *msg){

    if (msg->getKind() == BaseMacLayer::PACKET_DROPPED)
    {
        nbPacketDropped++;
        delete msg;
        msg = 0;
    }
    else
    {
    	EV<<"Received control from down\nSending control up\n";
    	sendControlUp(msg);
    }
}

void transport::handleUpperControl(cMessage *msg){
	EV<<"Received control from up\nSending control down\n";
	sendControlDown(msg);
}
