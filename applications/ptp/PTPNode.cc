/*
Copyright(C) 2010 Giada Giorgi

    This file is part of a free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    The software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <omnetpp.h>
#include "PTPpacket_m.h"
#include "Constant.h"

class PTPNode:public cSimpleModule{
protected:
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
private:
	void handleSelfMessage(cMessage *msg);
	void handlePTPEventMessage(cMessage *msg);
	void handlePTPGeneralMessage(cMessage *msg);
	void handleClockMessage(cMessage *msg);
	void handleLowLayerMessage(cMessage *msg);
	void servo_clock();


	// ---------------------------------------------------------------------------
	// State variables
	// ---------------------------------------------------------------------------
	const char *name;
	int address;
	int master;
	int ptpNode;
	double Tsync;
	simtime_t ts_s_sync;	//Timestamp assegnato dallo slave al msg SYNC
	simtime_t ts_m_sync;	//Timestamp assegnato dal master al msg SYN
	simtime_t ts_s_dreq;	//Timestamp assegnato dallo slave al msg DREQ
	simtime_t ts_m_dreq;	//Timestamp assegnato dal master al msg DREQ
	simtime_t dprop;		//Ritardo di propagazione one-way-delay
	simtime_t dms;			//Ritardo di propagazione tra master e slave
	simtime_t dsm;			//Ritardo di propagazione tra slave e master
	double offset;			//Offset misurato tra master e slave
	double drift;			//Drift misurato tra master e slave
	//----------------------------------------------------------------------------
	//Servo clock variables
	//----------------------------------------------------------------------------

};

Define_Module(PTPNode);

void PTPNode::initialize(){
	Tsync = par("syncInterval");
	address = par("MACaddress");
	ptpNode = par("ptpNode");

	if(ptpNode==0){
		ev << "PTPNODE: SLAVE node" << endl;
		name = "slave";
		master = par("MasterMACaddress");
		scheduleAt(simTime()+intuniform(Tsync,14*Tsync)+CONSTDELAY, new cMessage("SLtimer"));
	}else{
		ev << "PTPNODE: MASTER node" << endl;
		name = "master";
		master = address;
		scheduleAt(simTime()+Tsync, new cMessage("MStimer"));
	}
}//end initialize

void PTPNode::handleMessage(cMessage *msg){
	//Timer
	if(msg->isSelfMessage()){
		handleSelfMessage(msg);
	}
	//PTP event messages: sync and delay_request
	if(msg->arrivedOn("in_event")){
		handlePTPEventMessage(msg);
	}
	//PTP general messages: follow_up and delay_response
	if(msg->arrivedOn("in_general")){
		handlePTPGeneralMessage(msg);
		}
	//Clock messages
	if(msg->arrivedOn("in_clock")){
		handleClockMessage(msg);
		}
	//Messages from the low layer (i.e. MAC and PHY)
	if(msg->arrivedOn("in_lowLayer")){
		handleLowLayerMessage(msg);
		}
	delete msg;
}//end handleMessage


void PTPNode::handleSelfMessage(cMessage *msg){
	PTPpacket *pck;
	switch(ptpNode){
		case SLAVE:
			//Create a new DREQ message
			pck = new PTPpacket("DREQ_TIME_REQ");
			pck->setPtpType(DREQ);
			pck->setSource(address);
			pck->setDestination(master);
			pck->setClockType(TIME_REQ);
			// Get the current timestamp from the local clock
			send(pck,"out_clock");
			//Schedule a new timer event
			scheduleAt(simTime()+intuniform(Tsync,14*Tsync)+CONSTDELAY, new cMessage("SLtimer"));
			break;
		case MASTER:
			//creation of a Sync message
			pck = new PTPpacket("SYNC_TIME_REQ");
			pck->setPtpType(SYNC);
			pck->setSource(address);
			pck->setDestination(-1);
			//the message is forwarded to the clock module
			//in order to obtain a timestamp
			pck->setClockType(TIME_REQ);
			send(pck,"out_clock");
			scheduleAt(simTime()+Tsync, new cMessage("MStimer"));
			break;
	}//end switch
}//end handleSelfMessage

void PTPNode::handlePTPEventMessage(cMessage *msg){
	switch(ptpNode){
	case SLAVE:
		switch (((PTPpacket *)msg)->getPtpType()){
				case SYNC://a SYNC message has been received
					ts_m_sync = ((PTPpacket *)msg)->getData();
					msg->setName("SYNC_TIME_REQ");
					((PTPpacket *)msg)->setClockType(TIME_REQ);
					send((cMessage *)msg->dup(),"out_clock");
					break;
				case DRES:
				case FUP:
				case DREQ:
					error("Invalid slave event message");
					break;
		}
		break;//end SLAVE case
	case MASTER:
		switch (((PTPpacket *)msg)->getPtpType()){
				case SYNC:
				case DRES:
				case FUP:
					error("Invalid master event message");
					break;
				case DREQ://a DREQ message has been received
					//creation of a DRES message from the master
					PTPpacket *pck = new PTPpacket("DRES_TIME_REQ");
					pck->setPtpType(DRES);
					pck->setDestination(((PTPpacket *)msg)->getSource());
					pck->setSource(address);
					//the message is forwarded to the clock module
					//in order to obtain a timestamp
					pck->setClockType(TIME_REQ);
					send(pck,"out_clock");
					break;
			}
		break; //end MASTER case
	}//end switch
}//end handlePTPEventMessage

void PTPNode::handlePTPGeneralMessage(cMessage *msg){

	switch(ptpNode){
	case SLAVE:
		switch (((PTPpacket *)msg)->getPtpType()){
				case SYNC:
				case DREQ:
					error("Invalid slave general message");
					break;
				case DRES:
					ts_m_dreq = ((PTPpacket *)msg)->getData();
					dsm = ts_m_dreq - ts_s_dreq;
					dprop = (dms + dsm)/2;
					break;
				case FUP:
					//to be defined
					break;
			}
		break; //end SLAVE case
	case MASTER://to be implemented for synchronizing master to a grand-master
		error("Invalid master general message");
		break;
		//end MASTER case
	}//end switch
}//end handlePTPGeneralMessage

void PTPNode::handleClockMessage(cMessage *msg){
	switch(ptpNode){
	case SLAVE:
		switch (((PTPpacket *)msg)->getPtpType()){
			case SYNC:
				ts_s_sync = msg->getTimestamp();
				dms = ts_s_sync - ts_m_sync;
				offset = SIMTIME_DBL(dms - dprop);
				servo_clock();
				break;
			case DREQ:
				ts_s_dreq = msg->getTimestamp();
				msg->setName("DREQ");
				((PTPpacket*)msg)->setDestination(master);
				((PTPpacket*)msg)->setClockType(OFF);
				((PTPpacket*)msg)->setPort(UDP_EVENT_PORT);
				send((cMessage *)msg->dup(),"out_event");
				break;
			case DRES:
			case FUP:
				error("Invalid clock message");
				break;
		}
		break; //end SLAVE case
	case MASTER:
		switch (((PTPpacket *)msg)->getPtpType()){
			case SYNC:
				msg->setName("SYNC");
				((PTPpacket*)msg)->setClockType(OFF);
				((PTPpacket*)msg)->setPort(UDP_EVENT_PORT);
				send((cMessage *)msg->dup(),"out_event");
				break;
			case DREQ:
				error("Invalid clock message");
				break;
			case DRES:
				msg->setName("DRES");
				((PTPpacket*)msg)->setClockType(OFF);
				((PTPpacket*)msg)->setPort(UDP_GENERAL_PORT);
				send((cMessage *)msg->dup(),"out_general");
				break;
			case FUP:
				msg->setName("FUP");
				((PTPpacket*)msg)->setClockType(OFF);
				((PTPpacket*)msg)->setPort(UDP_GENERAL_PORT);
				send((cMessage *)msg->dup(),"out_general");
				break;
		}
		break; //end MASTER case
	}//end switch
}//end handleClockMessage

void PTPNode::handleLowLayerMessage(cMessage *msg){
	//-------------------------------------------
	//to be modified
	//-------------------------------------------
	PTPpacket *pck = new PTPpacket("FUP_TIME_REQ");
	pck->setPtpType(FUP);
	pck->setByteLength(FUP_BYTE);
	pck->setDestination(-1);
	pck->setSource(address);
	//the message is forwarded to the clock module
	//in order to obtain a timestamp
	pck->setClockType(TIME_REQ);
	send(pck,"out_clock");
}//end handleLowLayerMessage


void PTPNode::finish(){
}//end finish

/*
-------------------------------------------------------------------------------
SERVO CLOCK IMPLEMENTATION.
This function must be overwritten.
-------------------------------------------------------------------------------*/
void PTPNode::servo_clock(){
	//------------------------------------------
	//to be defined
	//------------------------------------------
}
