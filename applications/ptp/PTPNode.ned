package xsimulator;

simple PTPNode
{
    parameters:
        volatile double syncInterval;
        volatile int MACaddress;
        volatile int MasterMACaddress;
        volatile int ptpNode;
    gates:
        input in_event;
        input in_general;
        input in_clock;
        input in_lowLayer;
        output out_event;
        output out_general;
        output out_clock;
        output out_lowLayer;
}
