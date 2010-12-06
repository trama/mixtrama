/*
Copyright(C) 2010 Giada Giorgi

    This file is part of a is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.

    It is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <string.h>
#include <omnetpp.h>
#include <fstream>
#include "PTPpacket_m.h"
#include "Constant.h"
#include "tutils.h"
#include "BaseModule.h"
using namespace std;


class Clock : public BaseModule {
protected:
	virtual void initialize(int stage);
	virtual void handleMessage(cMessage *msg);
	virtual void finish();
 	//virtual void updateDisplay();
	virtual void openfile();
	virtual void closefile();

//	bool debug;

private:
	double getTimestamp();
	void adjtimex(double value, int type);
	double offset;
	double skew;
	double std_gamma;
	double std_theta;
	double std_noise;
	double T;
	double lclock;
	double gamma;
	double currentSimTime;
	ofstream outFile;


};

Define_Module(Clock);

void Clock::initialize(int stage){
	/*
	 * Initialization of input parameters
	 */
	BaseModule::initialize(stage);
	if (stage == 0) {
	    hasPar("debug") ? debug = par("debug").boolValue() : debug = false;
		offset = par("offset");
		skew   = par("skew");
		std_gamma  = par("std_gamma");
		std_theta  = par("std_theta");
		std_noise  = par("std_noise");
		T  = par("T");
		currentSimTime=simTime().dbl();
		lclock = currentSimTime+offset;
		gamma = skew;

		openfile();
	}
}

void Clock::handleMessage(cMessage *msg){
	EVT << "CLOCK : handle message" << endl;
	if(msg->isSelfMessage()){
		/*
		 * For possible future implementations of timers
		 */
	}else{
	/*
	 * The message can be:
	 * o  a time request (TIME_REQ) message: it returns a timestamp from
	 *    the local clock;
	 * o  an adjustment message (OFFSET_ADJ or FREQ_ADJ): it contains a
	 *    clock correction;
	 */
		switch(((PTPpacket *)msg)->getClockType()){
			case TIME_REQ:		//timestamp request
				msg->setName("TIME_RES");
				msg->setTimestamp(getTimestamp());
				((PTPpacket *)msg)->setClockType(TIME_RES);
				send((cMessage *)msg->dup(),"out_clock");
				break;
			case OFFSET_ADJ:	//offset correction
				adjtimex( SIMTIME_DBL( ((PTPpacket *)msg)->getData() ),0);
				break;
			case FREQ_ADJ:		//skew correction
				adjtimex( SIMTIME_DBL( ((PTPpacket *)msg)->getData() ),1);
				break;
		}
	}
	delete msg;
}


double Clock::getTimestamp(){
	/*
	 * Clock model
	 *
	 * Simple SKM model:
	 * --------------------------------------------------------
	 * clock = offset + (1+gamma)*(simTime().dbl()-origine);
	 *
	 * Recursive model:
	 * --------------------------------------------------------
	 * gamma[i]=gamma[i-1]+normal(0,std_gamma);
	 * clock[i]=clock[i-1]+(1+gamma[i])*T+normal(0,std_theta);
	 */
	EVT << "CLOCK : get timestamp" << endl;
	while(currentSimTime>simTime().dbl()){
		gamma=gamma+normal(0,std_gamma);
		lclock=lclock+(1+gamma)*T+normal(0,std_theta);
		outFile<<lclock<<"\t"<<gamma<<endl;
		currentSimTime=currentSimTime+T;
	}
	/*
	 * Timestamping noise
	 */

	double noise = normal(0,std_noise);

	return lclock + noise;
}

void Clock::adjtimex(double value, int type){
	switch(type){
	case 0: //offset update
		EVT << "CLOCK : offset update" << endl;
		lclock = lclock - value;
		break;
	case 1: //frequency skew update
		EVT << "CLOCK : freq skew update" << endl;
		gamma = gamma - value;
		break;
	}
}

void Clock::finish(){
	closefile();
}

void Clock::openfile(){
	EVT << "CLOCK: file open\n";
	outFile.open("clockdata.txt");
}

void Clock::closefile(){
	EVT << "CLOCK: file close\n";
	outFile.close();
}
