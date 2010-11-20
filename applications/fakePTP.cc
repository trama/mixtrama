//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//

#include "fakePTP.h"
#include "NetwToMacControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>
#include "tutils.h"
#include "NetPkt_m.h"

Define_Module(fakePTP);

void fakePTP::initialize(int stage)
{
    BaseLayer::initialize(stage);

    if (stage == 0)
    {
        delayTimer = new cMessage("delay-timer", SEND_BROADCAST_TIMER);

        myNetwAddr = par("myAddr");

        packetLength = par("packetLength");

        packetTime = par("packetTime");
        //pppt = par("packetsPerPacketTime");
        burstSize = par("burstSize"); // numero di pkt da spedire

        destination = par("destination");

        nbPacketDropped = 0;

        lowerGateOut1 = findGate("lowerGateOut1");
        lowerGateOut2 = findGate("lowerGateOut2");
        lowerGateIn1 = findGate("lowerGateIn1");
        lowerGateIn2 = findGate("lowerGateIn2");

    } else if (stage == 1){
        if (burstSize > 0) {
            remainingBurst = burstSize;
            scheduleAt(0.5, delayTimer);//dblrand() * packetTime * burstSize / pppt, delayTimer);

        }
//    if (stage!=3)
//        return;
//
//    counter = 0;
//    numSent = 0;
//    numReceived = 0;
//    WATCH(numSent);
//    WATCH(numReceived);
//
      currlocalPort = localPort1 = par("localPort1"); // should be 819
      localPort2 = par("localPort2"); //should be 820

      WATCH(currlocalPort);
      WATCH(remainingBurst);

//    destPort = par("destPort");
//
//      const char *destAddrs = par("destAddresses");
//      cStringTokenizer tokenizer(destAddrs);
//      const char *token;
//      while ((token = tokenizer.nextToken())!=NULL)
//          destAddresses.push_back(IPAddressResolver().resolve(token));
//
//    if (destAddresses.empty())
//        return;
//
      bindToPort(localPort1,false);
      bindToPort(localPort2,false);
    }
}

void fakePTP::bindToPort(int port,bool isControlPort)
{
    EVT << "Binding to transport port " << port << endl;

    cMessage *msg = new cMessage("UDP_C_BIND", UDP_C_BIND);
    transpCInfo *ctrl = new transpCInfo();
    ctrl->setSrcPort(port);
    ctrl->setIsControlPort(isControlPort);
    //ctrl->setSockId(getId());
    msg->setControlInfo(ctrl);
    if(isControlPort)
    	send(msg, "lowerControlOut");
    else if(port==localPort1){
    	send(msg, "lowerGateOut1");
    }
    else
    	send(msg, "lowerGateOut2");

}

void fakePTP::finish()
{
    recordScalar("dropped", nbPacketDropped);

    cancelAndDelete(delayTimer);
}

void fakePTP::handleMessage(cMessage* msg)
{
	if(msg->getArrivalGateId()==lowerGateIn1 || msg->getArrivalGateId()==lowerGateIn2 ){
        recordPacket(PassedMessage::INCOMING,PassedMessage::LOWER_CONTROL,msg);
        handleLowerMsg(msg);
    } else
    	BaseLayer::handleMessage(msg);
}

void fakePTP::handleSelfMsg(cMessage * msg)
{
    switch (msg->getKind())
    {
    case SEND_BROADCAST_TIMER:
        assert(msg == delayTimer);


        sendBroadcast();

        if (--remainingBurst > 0){
            scheduleAt(simTime() + packetTime, msg);
        }

        break;
    default:
        EVT << "Unkown selfmessage! -> delete, kind: " << msg->getKind() << endl;
        delete msg;
    }
}


void fakePTP::handleLowerMsg(cMessage * msg)
{
	EVT << "New message arrived from down...";

	NetPkt *net = check_and_cast<NetPkt *>(msg);

	int srcPort = net->getSrcPort();

	EVT << "Message is for port " << srcPort <<" Deleting it!\n";

    delete msg;
    msg = 0;
}


void fakePTP::handleLowerControl(cMessage * msg)
{
    if (msg->getKind() == BaseMacLayer::PACKET_DROPPED)
    {
        nbPacketDropped++;
    }
    delete msg;
    msg = 0;
}

void fakePTP::sendBroadcast()
{

	transpCInfo *ctrl = new transpCInfo();
	//ctrl->setSockId(getId());
	ctrl->setSrcPort(currlocalPort);
	ctrl->setDestination(destination);
	//ctrl->setDestPort(currlocalPort);
	ctrl->setSource(myNetwAddr);

    NetwPkt *pkt = new NetwPkt("BROADCAST_MESSAGE", BROADCAST_MESSAGE);
    pkt->setBitLength(packetLength);

    pkt->setControlInfo(ctrl);

    if(currlocalPort==820){
    	send(pkt,lowerGateOut2);
    	currlocalPort--;
    }
    else{
    	send(pkt,lowerGateOut1);
    	currlocalPort++;
    }




}
