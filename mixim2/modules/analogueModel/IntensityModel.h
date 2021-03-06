/* -*- mode:c++ -*- ********************************************************
 * file:        IntensityModel.h
 *
 * author:      Jerome Rousselot
 *
 * copyright:   (C) 2008 Centre Suisse d'Electronique et Microtechnique (CSEM) SA
 *              Real-Time Software and Networking
 *              Jaquet-Droz 1, CH-2002 Neuchatel, Switzerland.
 *
 *              This program is free software; you can redistribute it
 *              and/or modify it under the terms of the GNU General Public
 *              License as published by the Free Software Foundation; either
 *              version 2 of the License, or (at your option) any later
 *              version.
 *              For further information see file COPYING
 *              in the top level directory
 * description: this AnalogueModel models the spreading of radiated power
 * 				over a sphere centered on the emitter.
 ***************************************************************************/
#ifndef INTENSITYMODEL_H_
#define INTENSITYMODEL_H_

#include "AnalogueModel.h"
#include "Mapping.h"
#include "Signal_.h"
#include "BaseWorldUtility.h"
#include <math.h>

#define PI 3.1415926

/**
 * @brief TODO: Short description for this AnalogueModel
 *
 * @ingroup analogueModels
 */
class IntensityModel : public AnalogueModel {

public:
	IntensityModel() { }
	void filterSignal(Signal& s) {
		TimeMapping<Linear>* attMapping = new TimeMapping<Linear> ();

		// Determine distance between sender and receiver
		Move srcMove = s.getMove();
		Coord srcCoord, rcvCoord;
		double distance = 0;
		srcCoord = srcMove.getPositionAt(s.getSignalStart());
		rcvCoord = move->getPositionAt(s.getSignalStart());
		distance = rcvCoord.distance(srcCoord);

		Argument arg;
		attMapping->setValue(arg, 4*PI*pow(distance, 2));
		s.addAttenuation(attMapping);
	}
};

#endif /* INTENSITYMODEL_H_ */
