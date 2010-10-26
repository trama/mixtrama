package test;

simple transport
{
    parameters:
        int numApplLayer = default(1);
        bool debug;
        @display("p=10,10;o=white");

    gates:
        input upperGateIn[numApplLayer]; // from upper layer
        output upperGateOut[numApplLayer]; // to uppe layer
        input upperControlIn[numApplLayer]; // control from upper layer
        output upperControlOut[numApplLayer]; // control to upper layer
        input lowerGateIn; // gate for sendDirect
        output lowerGateOut;
        input lowerControlIn; // control from NIC
        output lowerControlOut; // control to NIC

}
