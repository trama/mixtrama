/*
 * transport.cc
 *
 *  Created on: 25/ott/2010
 *      Author: trama
 */

#include "transport.h"
#include <BaseMacLayer.h>
#include "NetwPkt_m.h"
#include "NetwToMacControlInfo.h"

enum TrafficGenMessageKinds{

	SEND_BROADCAST_TIMER = 1,
	BROADCAST_MESSAGE
};

static std::ostream & operator<<(std::ostream & os, const transport::sck& sd)
{
	os << "sockId=" << sd.sockId;
	os << " appGateIndex=" << sd.appGateIndex;
	os << " localPort=" << sd.localPort1;
	return os;
}

Define_Module(transport);

//void UDPAppBase::bindToPort(int port)
//{
//    EV << "Binding to UDP port " << port << endl;
//
//    // TODO UDPAppBase should be ported to use UDPSocket sometime, but for now
//    // we just manage the UDP socket by hand...
//    cMessage *msg = new cMessage("UDP_C_BIND", UDP_C_BIND);
//    UDPControlInfo *ctrl = new UDPControlInfo();
//    ctrl->setSrcPort(port);
//    ctrl->setSockId(getId());
//    msg->setControlInfo(ctrl);
//    send(msg, "udpOut");
//}

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
	        //this are the base address gate, i.e. the ones associated to gate 0
	        //one can obtain the other gate with baseaddr + index

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

void transport::bind(int gateIndex, transpCInfo *ctrl)
{
    // create and fill in description of a socket
	// tha app layer send a bind packet with its ID and the port number
	// it uses. For each port a ctrl message is sent, so we have to check
	// if we already created an entry in the socket map. TO BE CORRECTED
    sck *sd = new sck();
    sd->sockId = ctrl->getSockId();
    sd->appGateIndex = gateIndex;
    sd->isControlPort = ctrl->getIsControlPort();
    sd->localPort1 = ctrl->getSrcPort();

    if (sd->sockId==-1)
        error("sockId in BIND message not filled in");
    if (sd->localPort1==0)
        error("Local port could not be 0"); //sd->localPort = getEphemeralPort();

    EV << "Binding socket: " << *sd << "\n";

    // add to socketsByIdMap
    //ASSERT(scksID.find(sd->sockId)==scksID.end());
    std::map<int,sck*>::iterator it = scksID.find(sd->sockId);
    if(it==scksID.end() && !sd->isControlPort){
    	scksID[sd->sockId] = sd;
    	EV << "Added socket to map.\n";
    }else{
    	if(sd->isControlPort){
    		if(it->second->isControlPort){
    			it->second->controlPort2 = sd->localPort1;
    			EV << "Added second control port to socket\n";
    		} else {
    			it->second->controlPort1 = sd->localPort1;
    			EV << "Added first control port to socket\n";
    		}
    	} else {
    		it->second->localPort2=sd->localPort1;
    		EV << "Added second message port to socket\n";
    	}

    }

    //first time sck is created it is added to map. Any other time the same module tries to bind to this
    //one, other infos are added to the same map entry.

    // add to socketsByPortMap
    //SockDescList& list = socketsByPortMap[sd->localPort]; // create if doesn't exist
    //list.push_back(sd);
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

void transport::handleSelfMsg(cMessage* msg){}

void transport::handleUpperMsg(cMessage *msg){

	if(msg->getKind() == UDP_C_BIND){
			transpCInfo *ctrl = check_and_cast<transpCInfo *>(msg->removeControlInfo());
			bind(msg->getArrivalGate()->getIndex(), ctrl);
		    delete ctrl;
		    delete msg;
		}

	transpCInfo *ctrl = check_and_cast<transpCInfo *>(msg->removeControlInfo());

    NetwPkt *pkt = new NetwPkt("BROADCAST_MESSAGE", BROADCAST_MESSAGE);

    pkt->setBitLength(static_cast<cPacket*>(msg)->getBitLength());

    pkt->setSrcAddr(ctrl->getSource());

    pkt->setDestAddr(ctrl->getDestination());

    // necessario per il mac perché si prende la destinazione da qui!
    pkt->setControlInfo(new NetwToMacControlInfo(ctrl->getDestination()));

	EV<<"Received packet from up\nSending packet down\n";
	sendDelayed(pkt,elab_time,lowerGateOut);
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
	if(msg->getKind() == UDP_C_BIND){
		transpCInfo *ctrl = check_and_cast<transpCInfo *>(msg->removeControlInfo());
		bind(msg->getArrivalGate()->getIndex(), ctrl);
	    delete ctrl;
	    delete msg;
	}else{
	EV<<"Received control from up\nSending control down\n";
	sendControlDown(msg);
	}
}
