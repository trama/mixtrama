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

#include "PollApp.h"
#include "NetwToMacControlInfo.h"
#include <cassert>
#include <Packet.h>
#include <BaseMacLayer.h>
#include "tutils.h"

Define_Module(PollApp);

void PollApp::initialize(int stage)
{
    BaseLayer::initialize(stage);

    if (stage == 0)
    {
        world = FindModule < BaseWorldUtility * >::findGlobalModule();
        delayTimer = new cMessage("delay-timer", SEND_BROADCAST_TIMER);

        myNetwAddr = par("myAddr");

        packetLength = par("packetLength");

        packetTime = par("packetTime");

        pppt = par("packetsPerPacketTime");

        burstSize = par("burstSize");

        destination = par("destination");

        nbPacketDropped = 0;

        Packet p(1);
        catPacket = world->getCategory(&p);
    } else if (stage == 1){
        if (burstSize > 0) {
            remainingBurst = burstSize;
            scheduleAt(dblrand() * packetTime * burstSize / pppt, delayTimer);
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
      localPort = par("localPort");
      localCPort = par("localCPort");
//    destPort = par("destPort");
//
//    const char *destAddrs = par("destAddresses");
//    cStringTokenizer tokenizer(destAddrs);
//    const char *token;
//    while ((token = tokenizer.nextToken())!=NULL)
//        destAddresses.push_back(IPAddressResolver().resolve(token));
//
//    if (destAddresses.empty())
//        return;
//
      bindToPort(localPort,false);
      bindToPort(localCPort,true);
    }
}

void PollApp::bindToPort(int port,bool isControlPort)
{
    EVT << "Binding to transport port " << port << endl;

    cMessage *msg = new cMessage("UDP_C_BIND", UDP_C_BIND);
    transpCInfo *ctrl = new transpCInfo();
    ctrl->setSrcPort(port);
    ctrl->setIsControlPort(isControlPort);
    //ctrl->setSockId(getId());
    msg->setControlInfo(ctrl);
    isControlPort ? send(msg, "lowerControlOut") : send(msg, "lowerGateOut");
}

void PollApp::finish()
{
    recordScalar("dropped", nbPacketDropped);

    cancelAndDelete(delayTimer);
}

void PollApp::handleSelfMsg(cMessage * msg)
{
    switch (msg->getKind())
    {
    case SEND_BROADCAST_TIMER:
        assert(msg == delayTimer);


        sendBroadcast();

        remainingBurst--;

        if (remainingBurst == 0)
        {
            remainingBurst = burstSize;
            scheduleAt(simTime() + (dblrand() * 1.4 + 0.3) * packetTime * burstSize / pppt, msg);
        }
        else
        {
            scheduleAt(simTime() + packetTime * 2, msg);
        }

        break;
    default:
        EVT << "Unkown selfmessage! -> delete, kind: " << msg->getKind() << endl;
        delete msg;
    }
}


void PollApp::handleLowerMsg(cMessage * msg)
{
    Packet p(packetLength, 1, 0);
    world->publishBBItem(catPacket, &p, -1);

    delete msg;
    msg = 0;
}


void PollApp::handleLowerControl(cMessage * msg)
{
    if (msg->getKind() == BaseMacLayer::PACKET_DROPPED)
    {
        nbPacketDropped++;
    }
    delete msg;
    msg = 0;
}

void PollApp::sendBroadcast()
{

	transpCInfo *ctrl = new transpCInfo();
	//ctrl->setSockId(getId());
	ctrl->setSrcPort(localPort);
	ctrl->setDestination(destination);
	//ctrl->setDestPort(localPort);
	ctrl->setSource(myNetwAddr);

    NetPkt *pkt = new NetPkt("BROADCAST_MESSAGE", BROADCAST_MESSAGE);
    pkt->setBitLength(packetLength);

    pkt->setControlInfo(ctrl);

    sendDown(pkt);
}
