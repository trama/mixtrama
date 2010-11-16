//**************************************************************************
// * file:        NetPkt.msg
// *
// * author:      Federico Tramarin
// *
// * copyright:   (C) 2010 Measurement and EMC group at
// *              University of Padova, Italy.
// *
// *              This program is free software; you can redistribute it 
// *              and/or modify it under the terms of the GNU General Public 
// *              License as published by the Free Software Foundation; either
// *              version 2 of the License, or (at your option) any later 
// *              version.
// *              For further information see file COPYING 
// *              in the top level directory
// **************************************************************************/
cplusplus {{
#include "NetwPkt_m.h"
}}


class NetwPkt;

// Extension of NetwPkt, added port information.
packet NetPkt extends NetwPkt
{
    int srcPort;
}

