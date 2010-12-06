package mixtrama.applications.ptp;

simple PTPNode
{
    parameters:
        volatile double syncInterval;
        volatile int MACaddress;
        volatile int MasterMACaddress;
        volatile int ptpNode;
        bool debug; // debug switch
        int localPort1 = default(819);
        int localPort2 = default(820);
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
