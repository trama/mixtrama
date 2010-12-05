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

package xsimulator;

network PtpNetwork
{
    @display("bgb=261,233");
    submodules:
        slave: PTPNode {
            @display("p=41,44");
            syncInterval = 1;
            MACaddress = 1;
            MasterMACaddress = 2;
            ptpNode = 0;
        }
        master: PTPNode {
            @display("p=41,162");
            syncInterval = 1;
            MACaddress = 2;
            MasterMACaddress = 2;
            ptpNode = 1;
        }
        Sclock: Clock {
            @display("p=175,44");
            offset = 0;
            skew = 1;
            T = 1;
            std_gamma = 0;
            std_theta = 0;
            std_noise = 0;
        }
        Mclock: Clock {
            @display("p=175,162");
            offset = 0;
            skew = 0;
            T = 1;
            std_gamma = 0;
            std_theta = 0;
            std_noise = 0;
        }
    connections allowunconnected:
        Sclock.out_clock --> slave.in_clock;
        slave.out_clock --> Sclock.in_clock;
        Mclock.out_clock --> master.in_clock;
        master.out_clock --> Mclock.in_clock;
        slave.out_event --> master.in_event;
        master.out_event --> slave.in_event;
        slave.out_general --> master.in_general;
        master.out_general --> slave.in_general;
}
