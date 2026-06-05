#! /bin/sh

#salloc --nodes=1 --exclusive --time=03:30:00 --account=epic --partition=u1-h100 --qos=gpuwf --x11
salloc --nodes=1 --exclusive --time=03:30:00 --account=epic --partition=u1-mi300x --qos=gpuwf --x11

exit 0

#salloc --nodes=1 --exclusive --time=03:30:00 --account=epic --partition=u1-h100 --qos=gpuwf
#srun --pty --x11=first --partition=u1-h100 --account=epic /bin/bash
#ssh -Y -J Wei.Huang@ursa.noaa.gov Wei.Huang@u22g08

salloc --nodes=1 --exclusive --time=03:30:00 --account=epic --partition=u1-mi300x --qos=gpuwf
srun --pty --x11=first --partition=u1-mi300x --account=epic /bin/bash
ssh -Y -J Wei.Huang@ursa.noaa.gov Wei.Huang@u22g08


Partition  QOS             Allowed Billing TRES Factor Description

u1-compute batch,windfall, debug, urgent, long 100,  General compute resource. Default if no partition is specified.

u1-h100    gpu, gpuwf 100 For jobs that require nodes with the Nvidia H-100 GPUs.

u1-gh      gpuwf      100 For jobs that require nodes with the Nvidia Grace-Hopper processors.

u1-mi300x  gpuwf      100 For jobs that require nodes with the AMD MI300X GPUs.

u1-service batch, windfall 100

Serial jobs (max 64 cores and/or 250 GB of memory per user), with a 24 hr wall time limit. Jobs will be run on service nodes that have external network connectivity. Useful for data transfers or access to external resources like databases. If your workflow requires pushing or pulling data to/from the HSMS(HPSS), it should be run there. This partition is also a good choice for doing your compilations and builds of your applications and libraries rather than doing it on the login nodes.
