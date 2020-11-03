c++ API for the openPMD ray trace extension 

# Devel instructions
```
git clone -b devel git@github.com:PaNOSC-ViNYL/openPMD_raytrace_API.git
cd openPMD_raytrace_API/

cmake ..

cmake --build .
cmake --build . --target doc
```

# Usage

## Writing

A Ray object should be constructed and filled with all the relevant information.

The ray is then queued for writing using the openPMD_io class:
\include test_write.cpp

The Ray class is providing all the conversion/utility operations on the quantities stored in the openPMD file according to the RAYTRACE extension


## Reading

\include test_read.cpp

## Todo
 - [ ] Setter and getter for gravity direction
 - [ ] Setter and getter for horizontalCoordinate
 - [ ] Setter and getter for numParticles
 - [ ] Interal usage of numParticles
 - [ ] Binding of wavelength, time, weight, id, status getters
 - [ ] Test the python binding
 - [ ] Cmake install for the python binding
 - [ ] PyPi package
 - [ ] ostream friend for the ray class to make easier to debug

## Examples
Examples can be found 


\example test_write.cpp




