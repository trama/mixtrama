package test.interface;

import org.mixim.base.modules.BaseArp;

// Module which is responsible for Addressresolution
simple dummyArp extends BaseArp
{
    parameters:
        int macAddress;
        @class(dummyArp);
}

