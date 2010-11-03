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


Define_Module(fakePTP);

void fakePTP::initialize(int stage)
{
    BaseLayer::initialize(stage);

    if (stage == 0)
    {
        delayTimer = new cMessage("delay-timer", SEND_BROADCAST_TIMER);

        //arp = BaseArpAccess().get();
        //myNetwAddr = arp->myNetwAddr(this);

        myNetwAddr = par("myAddr");

        packetLength = par("packetLength");

        packetTime = par("packetTime");
        //pppt = par("packetsPerPacketTime");
        burstSize = par("burstSize"); // numero di pkt da spedire

        destination = par("destination");

        nbPacketDropped = 0;

    } else if (stage == 1){
        if (burstSize > 0) {
            remainingBurst = burstSize;
            scheduleAt(70, delayTimer);//dblrand() * packetTime * burstSize / pppt, delayTimer);
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
      bindToPort(localPort1);
      bindToPort(localPort2);
    }
}

void fakePTP::bindToPort(int port)
{
    EV << "Binding to transport  port " << port << endl;

    cMessage *msg = new cMessage("UDP_C_BIND", UDP_C_BIND);
    transpCInfo *ctrl = new transpCInfo();
    ctrl->setSrcPort(port);
    //ctrl->setSockId(getId());
    msg->setControlInfo(ctrl);
    send(msg, "lowerControlOut");
}

void fakePTP::finish()
{
    recordScalar("dropped", nbPacketDropped);

    cancelAndDelete(delayTimer);
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
        EV << "Unkown selfmessage! -> delete, kind: " << msg->getKind() << endl;
        delete msg;
    }
}


void fakePTP::handleLowerMsg(cMessage * msg)
{
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

    if(currlocalPort==820)
    	currlocalPort--;
    else
    	currlocalPort++;


    sendDown(pkt);
}
