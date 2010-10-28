package test;

simple transport
{
    parameters:
        int numApplLayer = default(1);
        bool debug;
        double elaborationTime @unit(s) = default(0us);
        @display("p=10,10;o=white");

    gates:
        input upperGateIn[]; // from upper layer
        output upperGateOut[]; // to uppe layer
        input upperControlIn[]; // control from upper layer
        output upperControlOut[]; // control to upper layer
        input lowerGateIn; // gate for sendDirect
        output lowerGateOut;
        input lowerControlIn; // control from NIC
        output lowerControlOut; // control to NIC

}
