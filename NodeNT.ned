package mixtrama;

import mixtrama.mixim2.modules.power.battery.SimpleBattery;
import mixtrama.mixim2.modules.power.battery.BatteryStats;
import mixtrama.mixim2.modules.nic.Nic802154_TI_CC2420;
import mixtrama.mixim2.modules.netw.ArpHost;
import mixtrama.mixim2.modules.mobility.ConstSpeedMobility;
import mixtrama.mixim2.base.modules.BaseUtility;
import mixtrama.applications.SimpleAppl;


module NodeNT
{
    parameters:
        int addr;
        @display("bgb=388,386,white;bgl=3");
    gates:
        input radioIn; // gate for sendDirect
    submodules:
        batteryStats: BatteryStats {
            @display("p=243,57;i=block/circle");
        }
        battery: SimpleBattery {
            @display("p=320,57;i=block/control");
        }
        utility: BaseUtility {
            parameters:
                @display("p=320,140;b=24,24,rect,black;i=block/blackboard");
        }
        arp: ArpHost {
            parameters:
                @display("p=283,238;b=24,24,rect,blue");
        }
        mobility: ConstSpeedMobility {
            parameters:
                @display("p=257,305;i=cogwheel2");
        }
        appl: SimpleAppl {
            parameters:
                myAddr = addr;
                @display("p=77,109;i=app");
        }
        nic: Nic802154_TI_CC2420 {
            parameters:
                @display("p=73,309;i=iface");
        }
    connections:

        radioIn --> nic.radioIn;
        nic.upperControlOut --> appl.lowerControlIn;
        appl.lowerControlOut --> nic.upperControlIn;
        nic.upperGateOut --> appl.lowerGateIn;
        appl.lowerGateOut --> nic.upperGateIn;
}
