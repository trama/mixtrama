/*
 * transport.cc
 *
 *  Created on: 25/ott/2010
 *      Author: trama
 */

#include "transport.h"
#include <BaseMacLayer.h>
#include "NetPkt_m.h"
#include "NetwToMacControlInfo.h"
#include "tutils.h"
#include "Constant.h"
#include "PTPpacket_m.h"

enum TrafficGenMessageKinds{

	SEND_BROADCAST_TIMER = 1,
	BROADCAST_MESSAGE
};

static std::ostream & operator<<(std::ostream & os, const transport::sck& sd)
{
	//os << "sockId=" << sd.sockId;
	os << " appGateIndex=" << sd.appGateIndex;
	os << " localPort=" << sd.localPort;
	return os;
}

Define_Module(transport);

void transport::initialize(int stage){
	BaseLayer::initialize(stage);
		if(stage == 0) {
			numApplLayer = par("numApplLayer");
			//debug = par("debug").boolValue();
			elab_time = par("elaborationTime").doubleValue();

			EVT << "Transport elab_time "<<elab_time<<endl;
			//for now, only one app layer supported

			delayTimer = new cMessage("delay-timer",2662);

	        upperGateIn  = gateBaseId("upperGateIn");
	        upperGateOut = gateBaseId("upperGateOut");
	        upperControlIn  = gateBaseId("upperControlIn");
	        upperControlOut = gateBaseId("upperControlOut");
	        //this are the base address gate, i.e. the ones associated to gate 0
	        //one can obtain the other gate with baseaddr + index

			EVT << "the gates present in this modules are:\nHigher ports\n"<<
			upperGateIn<<" "<<
			upperGateOut <<" "<<
	        upperControlIn<<" "<<
	        upperControlOut <<" \nLower ports\n"<<
	        lowerGateIn  <<" "<<
	        lowerGateOut<<" "<<
	        lowerControlIn  <<" "<<
	        lowerControlOut <<endl;

			EVT << "The gates upperGateIn/Out and ControlIn/Out are vector gates with size"<<
					(isGateVector("upperGateIn") ? gateSize("upperGateIn") : -20 )<<endl;

	        nbPacketDropped = 0;

		}
		else if (stage == 1){

			pqueue = new cQueue("Packet Queue");

			EVT << "Created packet queue, with dimension "<<pqueue->length()<<endl;

			//WATCH(pqueue->length());


		}
}

void transport::bind(int gateIndex, transpCInfo *ctrl)
{
    // create and fill in description of a socket
	// tha app layer send a bind packet with its ID and the port number
	// it uses. For each port a ctrl message is sent, so we have to check
	// if we already created an entry in the socket map. TO BE CORRECTED
    sck *sd = new sck();
    //sd->sockId = ctrl->getSockId();
    sd->appGateIndex = gateIndex;
    sd->isControlPort = ctrl->getIsControlPort();
    sd->localPort = ctrl->getSrcPort();

    if (sd->localPort<=0)
        error("Local port could not be 0, or var in BIND message not filled in");
    //if (sd->localPort1==0)
      //  error("Local port could not be 0"); //sd->localPort = getEphemeralPort();

    EVT << "Binding socket: " << *sd << "\n";

    // add to socketsByIdMap
    ASSERT(scksID.find(sd->localPort)==scksID.end());
    scksID[sd->localPort] = sd;
    EVT << "Added socket to map.\n";

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
    } else if(msg->arrivedOn("upperGateIn")) {
    //else if(msg->getArrivalGateId()==upperGateIn) {
        recordPacket(PassedMessage::INCOMING,PassedMessage::UPPER_DATA,msg);
        handleUpperMsg(msg);
    } else if(msg->arrivedOn("upperControlIn")) {
        //else if(msg->getArrivalGateId()==upperControlIn) {
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

void transport::handleSelfMsg(cMessage* msg){

	if(msg->getKind()==2662){
		EVT << "Another packet has to be sent from the queue\n";
		exec_step();
	}

}

void transport::handleLowerMsg(cMessage *msg){
	EVT<<"Received packet from down -- Analyzing!\n";//\nSending packet up\n";

	NetPkt *net = check_and_cast<NetPkt *>(msg);

	int srcPort = net->getSrcPort();
	std::map<int,sck*>::iterator it = scksID.find(srcPort);
	int gateIdx = it->second->appGateIndex;
	EVT << "Message is for port " << srcPort << " which correspond to ID "<< upperGateOut + gateIdx <<endl;

	if(msg->getKind() < 4){ // PTP messages
			EVT << "Message to PTP application of type " <<
					msg->getKind() << endl;

			PTPpacket *ptp = check_and_cast<PTPpacket*>(net->decapsulate());
			sendDelayed(ptp,elab_time,upperGateOut + gateIdx);
	} else
		sendDelayed(msg,elab_time,upperGateOut + gateIdx);
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
    	EVT<<"Received control from down\nSending control up\n";
    	sendControlUp(msg);
    }
}

void transport::handleUpperMsg(cMessage *msg){

	if(msg->getKind() == UDP_C_BIND){
		transpCInfo *ctrl = check_and_cast<transpCInfo *>(msg->removeControlInfo());
		bind(msg->getArrivalGate()->getIndex(), ctrl);
		delete ctrl;
		delete msg;
	} else if(msg->getKind() < 4){ // PTP messages
		EVT << "Message from PTP application of type " <<
				msg->getKind() << endl;

		PTPpacket *ptp = check_and_cast<PTPpacket*>(msg);

		NetPkt *pkt = new NetPkt("BROADCAST_MESSAGE", BROADCAST_MESSAGE);

		pkt->setBitLength(static_cast<cPacket*>(msg)->getBitLength());
		pkt->setSrcPort(ptp->getPort());
		pkt->setSrcAddr(ptp->getSource());
		pkt->setDestAddr(ptp->getDestination());
		pkt->encapsulate(ptp);
	    pkt->setControlInfo(new NetwToMacControlInfo(ptp->getDestination()));

		EVT<<"From port "<< ptp->getPort()<<" --> Enqueuing packet\n";

		pqueue->insert(pkt);

		exec_step();

	} else {
		EVT << "Message from up\n.";

	transpCInfo *ctrl = check_and_cast<transpCInfo *>(msg->removeControlInfo());

    NetPkt *pkt = new NetPkt("BROADCAST_MESSAGE", BROADCAST_MESSAGE);

    pkt->setBitLength(static_cast<cPacket*>(msg)->getBitLength());
    pkt->setSrcPort(ctrl->getSrcPort());
    pkt->setSrcAddr(ctrl->getSource());
    pkt->setDestAddr(ctrl->getDestination());
    //per ora non si specifica la porta di destinazione che viene assunta uguale a quella sorgente

    // necessario per il mac perché si prende la destinazione da qui!
    pkt->setControlInfo(new NetwToMacControlInfo(ctrl->getDestination()));

	EVT<<"Received packet from up -- port "<< ctrl->getSrcPort()<<" --> Enqueuing packet\n";

	pqueue->insert(pkt);

	exec_step();

	}
}

void transport::exec_step(){

	EVT << "Transport execution next step. Analyzing queue.\n";

	if(pqueue->empty()){
		EVT << "Queue is empty. Returning.\n";
		return;
	}

	if(pqueue->length()==1){
		EVT << "Only one packet in queue, sending down and emptying queue.\n";
		cPacket *pkt = (cPacket *)pqueue->pop();
		EVT << "Now Queue has length "<<pqueue->length()<<endl;
		sendDelayed(pkt,elab_time,lowerGateOut);
		return;
	}

	if(pqueue->length()>1){

		EVT << "More than one packet in queue, sending down the oldest one.\n";
		cPacket *pkt = (cPacket *)pqueue->pop();
		EVT << "Now Queue has length "<<pqueue->length()<<endl;
		sendDelayed(pkt,elab_time,lowerGateOut);
		scheduleAt(0.001, delayTimer);
	}

}

void transport::handleUpperControl(cMessage *msg){
	if(msg->getKind() == UDP_C_BIND){
		transpCInfo *ctrl = check_and_cast<transpCInfo *>(msg->removeControlInfo());
		bind(msg->getArrivalGate()->getIndex(), ctrl);
	    delete ctrl;
	    delete msg;
	}else{
	EVT<<"Received control from up\nSending control down\n";
	sendControlDown(msg);
	}
}
