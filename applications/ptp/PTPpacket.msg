packet PTPpacket
{
    int pckType;
    int clockType;
    int ptpType;
    int source;
    int destination;
    int port;
    simtime_t data;
}
