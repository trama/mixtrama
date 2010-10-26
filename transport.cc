/*
 * transport.cc
 *
 *  Created on: 25/ott/2010
 *      Author: trama
 */

#include "transport.h"

Define_Module(transport);

void transport::initialize(int stage){
	BaseLayer::initialize(stage);
		if(stage == 0) {
			numApplLayer = par("numApplLayer");

		}

}


