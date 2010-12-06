package mixtrama.applications.ptp;

simple Clock
{
    parameters:
        volatile double offset;
        volatile double skew;
        volatile double T;
        volatile double std_gamma;
        volatile double std_theta;
        volatile double std_noise;
        bool debug;
    gates:
        input in_clock;
        output out_clock;
}
