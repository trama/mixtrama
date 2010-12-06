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

module PTPMaster
{
    @display("bgb=154,233");
    gates:
        input lowerGateIn1; // from upper layers
        input lowerGateIn2;
        output lowerGateOut1; // to upper layers
        output lowerGateOut2; // to upper layers
        output lowerControlOut; // control connection
        input lowerControlIn;
    submodules:
        master: PTPNode {
            @display("p=80,172");
            syncInterval = 1;
            MACaddress = 2;
            MasterMACaddress = 2;
            ptpNode = 1;
        }
        Mclock: Clock {
            @display("p=80,49");
            offset = 0;
            skew = 0;
            T = 1;
            std_gamma = 0;
            std_theta = 0;
            std_noise = 0;
        }
    connections allowunconnected:
        Mclock.out_clock --> master.in_clock;
        master.out_clock --> Mclock.in_clock;
        master.out_event --> lowerGateOut1;
        master.out_general --> lowerGateOut2;
        master.out_lowLayer --> lowerControlOut;
        lowerControlIn --> master.in_lowLayer;
        lowerGateIn1 --> master.in_event;
        lowerGateIn2 --> master.in_general;
}
