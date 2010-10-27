package test;

import org.mixim.modules.power.battery.SimpleBattery;
import org.mixim.modules.power.battery.BatteryStats;
import org.mixim.modules.mobility.ConstSpeedMobility;
import org.mixim.base.modules.BaseUtility;


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
                @display("p=47,42;i=app");
        }
        nic: Nic802154_TI_CC2420 {
            parameters:
                @display("p=73,309;i=iface");
        }
        transport: transport {
            @display("p=69,167");
        }
    connections:
        appl.lowerGateOut --> transport.upperGateIn[0];
        appl.lowerControlOut --> transport.upperControlIn[0];
        transport.upperControlOut[0] --> appl.lowerControlIn;
        transport.upperGateOut[0] --> appl.lowerGateIn;

        radioIn --> nic.radioIn;

        transport.lowerControlOut --> nic.upperControlIn;
        transport.lowerGateOut --> nic.upperGateIn;
        nic.upperControlOut --> transport.lowerControlIn;
        nic.upperGateOut --> transport.lowerGateIn;

}
