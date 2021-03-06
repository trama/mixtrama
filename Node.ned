package mixtrama;

import mixtrama.mixim2.modules.power.battery.SimpleBattery;
import mixtrama.mixim2.modules.power.battery.BatteryStats;
import mixtrama.mixim2.modules.nic.Nic802154_TI_CC2420;
import mixtrama.mixim2.modules.mobility.ConstSpeedMobility;
import mixtrama.mixim2.base.modules.BaseUtility;
import mixtrama.interface.transport;
import mixtrama.interface.dummyArp;
import mixtrama.applications.SimpleAppl;


module Node
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
        mobility: ConstSpeedMobility {
            parameters:
                @display("p=257,305;i=cogwheel2");
        }
        appl: SimpleAppl {
            parameters:
                myAddr = addr;
                @display("p=70,41;i=app");
        }
        nic: Nic802154_TI_CC2420 {
            parameters:
                @display("p=73,309;i=iface");
        }
        transport: transport {
            @display("p=69,167");
        }
        arp: dummyArp {
            @display("p=249,211");
        }
    connections:
        radioIn --> nic.radioIn;

        transport.lowerControlOut --> nic.upperControlIn;
        transport.lowerGateOut --> nic.upperGateIn;
        nic.upperControlOut --> transport.lowerControlIn;
        nic.upperGateOut --> transport.lowerGateIn;

        appl.lowerGateOut --> transport.upperGateIn++;
        appl.lowerControlOut --> transport.upperControlIn++;
        transport.upperGateOut++ --> appl.lowerGateIn;
        transport.upperControlOut++ --> appl.lowerControlIn;
}
