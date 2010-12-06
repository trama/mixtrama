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

package mixtrama.applications.ptp;

module PTPSlave
{
    @display("bgb=133,201");
    gates:
        input lowerGateIn; // from upper layers
        output lowerGateOut1; // to upper layers
        output lowerGateOut2; // to upper layers
        output lowerControlOut; // control connection
        input lowerControlIn;
        //input lowerLayerIn;
    submodules:
        slave: PTPNode {
            @display("p=63,141");
            syncInterval = 1;
            MACaddress = 1;
            MasterMACaddress = 2;
            ptpNode = 0;
        }
        Sclock: Clock {
            @display("p=63,32");
            offset = 0;
            skew = 1;
            T = 1;
            std_gamma = 0;
            std_theta = 0;
            std_noise = 0;
        }
    connections allowunconnected:
        Sclock.out_clock --> slave.in_clock;
        slave.out_clock --> Sclock.in_clock;
        lowerGateIn --> slave.in_general;
        slave.out_general --> lowerGateOut1;
        lowerControlIn --> slave.in_event;
        slave.out_event --> lowerControlOut;
        //lowerLayerIn --> slave.in_lowLayer;
}
