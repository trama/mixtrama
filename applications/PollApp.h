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

#ifndef POLLAPP_H_
#define POLLAPP_H_

#include "NetPkt_m.h"
#include "transpCInfo_m.h"
#include "SimpleAddress.h"
#include "BaseLayer.h"
#include "BaseArp.h"
#include <BaseWorldUtility.h>

#include <omnetpp.h>

/**
 * @brief A module to generate traffic for the NIC, used for testing purposes.
 *
 * @ingroup exampleIEEE802154Narrow
 */
class PollApp : public BaseLayer
{
public:

	enum TrafficGenMessageKinds{

		SEND_BROADCAST_TIMER = 1,
		BROADCAST_MESSAGE
	};

protected:

	int packetLength;
	simtime_t packetTime;
	double pppt;
	int burstSize;
	int remainingBurst;
	int destination;

	int catPacket;

	long nbPacketDropped;

	int localPort, localCPort;

	BaseArp* arp;
	int myNetwAddr;

	cMessage *delayTimer;

	BaseWorldUtility* world;

public:


protected:

	virtual void initialize(int stage);

	virtual void finish();

	/** @brief Handle self messages such as timer... */
	virtual void handleSelfMsg(cMessage *msg);

	/** @brief Handle messages from lower layer */
	virtual void handleLowerMsg(cMessage *msg);

	/** @brief Handle control messages from lower layer */
	virtual void handleLowerControl(cMessage *msg);

	/** @brief Handle messages from upper layer */
	virtual void handleUpperMsg(cMessage *msg)
	{
		opp_error("NetworkStackTrafficGen has no upper layers!");
		delete msg;
	}

	/** @brief Handle control messages from upper layer */
	virtual void handleUpperControl(cMessage *msg)
	{
		opp_error("NetworkStackTrafficGen has no upper layers!");
		delete msg;
	}

	/** @brief Send a broadcast message to lower layer. */
	virtual void sendBroadcast();

	virtual void bindToPort(int port, bool);
};

#endif
