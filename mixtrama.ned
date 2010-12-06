package mixtrama;

import mixtrama.mixim2.base.connectionManager.ConnectionManager;


network mixtrama
{
    parameters:
        double playgroundSizeX @unit(m); // x size of the area the nodes are in (in meters)
        double playgroundSizeY @unit(m); // y size of the area the nodes are in (in meters)
        double playgroundSizeZ @unit(m); // z size of the area the nodes are in (in meters)
        double numSlaves; // total number of hosts in the network
        double numHosts;

        @display("bgb=538,349,white;bgp=0,0;bgl=2");
    submodules:
        CM802154: ConnectionManager {
            parameters:
                @display("p=492,54;b=42,42,rect,green;i=abstract/router");
        }
        world: WorldUtilityStats {
            parameters:
                playgroundSizeX = playgroundSizeX;
                playgroundSizeY = playgroundSizeY;
                playgroundSizeZ = playgroundSizeZ;
                @display("p=70,39;i=misc/globe");
        }
        node[numSlaves]: Slave {
            parameters:
                addr = index;
                @display("p=88,153;i=device/wifilaptop");
        }
        master: Master {
            parameters:
                addr = 10;
                @display("p=140,225;i=device/wifilaptop");
        }
        CM80211: ConnectionManager {
            @display("p=211,56;i=abstract/dispatcher");
        }
        host[numHosts]: Host {
            @display("i=device/accesspoint;p=374,190");
        }
    //        master: MasterNode {
    //            parameters:
    //                addr = 1;
    //                @display("p=70,139;i=device/wifilaptop");
    //        }
    connections allowunconnected:

}
