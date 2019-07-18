# Kernel Primitive Unit Tests

This folder contains Vivado-HLS-based unit tests for kernel primitives.

To perform c-simulation and co-sim on all tests, run the following command in this directory:

```
make CSIM=1 COSIM=1 run
```

To test device other than default `xcu200-fsgd2104-2-e`, just specify the DSA name via DEVICE var:

```
make CSIM=1 COSIM=1 check DEVICE=u250
```

Other than SDx tool variables, `PLATFORM_REPO_PATHS` must be set so that the makefile can detect
the target platform and deduce the device to check against.
