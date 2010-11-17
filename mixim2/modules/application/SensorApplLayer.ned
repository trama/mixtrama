package mixtrama.mixim2.modules.application;

// Application layer to test lower layer implementations. Generate periodic traffic addressed to a sink.
//
// @sa SensorApplLayer
// @author Jerome Rousselot, Amre El-Hoiydi
simple SensorApplLayer
{
    parameters:
        bool debug = default(false); // debug switch
        bool stats = default(true);   // stats generation switch
        bool broadcastPackets = default(false); // send packets in broadcast mode
        double nbPackets = default(0);
        int destAddr = default(0);
        string trafficType;
        double trafficParam;
        double initializationTime @unit(s) = default(1 min); // minimum time before generation of the first packet
        int headerLength @unit(byte) = default(2 byte);
    gates:
        input lowerGateIn; // from network layer
        input lowerControlIn;
        output lowerGateOut; // to network layer
        output lowerControlOut;
}
