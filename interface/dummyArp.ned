package mixtrama.interface;

import mixtrama.mixim2.base.modules.BaseArp;


// Module which is responsible for Addressresolution
simple dummyArp extends BaseArp
{
    parameters:
        int macAddress;
        @class(dummyArp);
}

