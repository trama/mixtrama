package test;

import test.applications.fakePTP;
import test.interface.dummyArp;
import test.applications.SimpleAppl;
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
        dummyArp: dummyArp {
            @display("p=249,211");
        }
        fakePTP: fakePTP {
            @display("p=128,45");
        }
    connections:
        radioIn --> nic.radioIn;

        transport.lowerControlOut --> nic.upperControlIn;
        transport.lowerGateOut --> nic.upperGateIn;
        nic.upperControlOut --> transport.lowerControlIn;
        nic.upperGateOut --> transport.lowerGateIn;

        fakePTP.lowerGateOut1 --> transport.upperGateIn++;
        transport.upperGateOut++ --> fakePTP.lowerGateIn1;
        fakePTP.lowerGateOut2 --> transport.upperGateIn++;
        transport.upperGateOut++ --> fakePTP.lowerGateIn2;
        transport.upperControlOut++ --> fakePTP.lowerControlIn;
        fakePTP.lowerControlOut --> transport.upperControlIn++;
        appl.lowerGateOut --> transport.upperGateIn++;
        appl.lowerControlOut --> transport.upperControlIn++;
        transport.upperGateOut++ --> appl.lowerGateIn;
        transport.upperControlOut++ --> appl.lowerControlIn;
}
