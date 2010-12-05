package xsimulator;

simple Clock
{
    parameters:
        volatile double offset;
        volatile double skew;
        volatile double T;
        volatile double std_gamma;
        volatile double std_theta;
        volatile double std_noise;
    gates:
        input  in_clock;
        output out_clock;
}
