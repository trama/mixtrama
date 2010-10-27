enum UDPCommandCode
{

    UDP_C_DATA = 0;    // datagram to send (must carry UDPControlInfo)
    UDP_C_BIND = 1;    // fill srcPort and srcAddr in UDPControlInfo
    UDP_C_CONNECT = 2; // fill destAddr and destPort in UDPControlInfo
    UDP_C_UNBIND = 3;  // fill sockId in UDPControlInfo
}

class transpCInfo
{
    int sockId = -1;   // uniquely identifies the \UDP socket
    int srcPort;   // \UDP source port in packet, or local port with BIND
    int destPort;
    int destination;
    int source;
}

