package mixtrama;

import mixtrama.mixim2.modules.nic.Nic80211;
import mixtrama.mixim2.modules.mobility.ConstSpeedMobility;
import mixtrama.mixim2.modules.application.BurstApplLayer;
import mixtrama.mixim2.base.modules.BaseUtility;
import mixtrama.mixim2.base.modules.BaseNetwLayer;
import mixtrama.mixim2.base.modules.BaseArp;


module Host
{
    parameters:
        @display("p=10,10;o=white");

    gates:
        input radioIn; // gate for sendDirect

    submodules:
        blackboard: BaseUtility {
            @display("p=200,75;b=30,25;i=block/blackboard");
        }

        mobility: ConstSpeedMobility {
            @display("p=200,150;b=30,25;i=block/cogwheel");
        }

        appl: BurstApplLayer {
            @display("p=91,75;b=22,20,rect;i=block/app");
        }

        arp: BaseArp {
            @display("p=200,225;b=30,25;i=block/process");
        }

        net: BaseNetwLayer {
            @display("p=91,150;b=25,20,rect;i=block/layer");
        }

        nic: Nic80211 {
            @display("p=91,225;b=22,20,rect;i=block/ifcard");
        }

    connections:
        nic.upperGateOut --> net.lowerGateIn;
        nic.upperGateIn <-- net.lowerGateOut;
        nic.upperControlOut --> net.lowerControlIn;
        nic.upperControlIn <-- net.lowerControlOut;

        net.upperGateOut --> appl.lowerGateIn;
        net.upperGateIn <-- appl.lowerGateOut;
        net.upperControlOut --> appl.lowerControlIn;
        net.upperControlIn <-- appl.lowerControlOut;

        radioIn --> nic.radioIn;

}
