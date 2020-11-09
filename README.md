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
 - [X] Setter and getter for gravity direction
 - [X] Setter and getter for horizontalCoordinate
 - [ ] Setter and getter for numParticles
 - [ ] Interal usage of numParticles
 - [X] Binding of wavelength, time, weight, id, status getters
 - [ ] Test the python binding
 - [ ] Cmake install for the python binding
 - [ ] PyPi package
 - [ ] ostream friend for the ray class to make easier to debug
 - [X] Repeating ray multiple times if requested when reading the openPMD file
 - [X] Writing in chunks
 - [X] Throwing runtime_error when over the maximum foreseen number of rays
 
## Examples
Examples can be found 


\example test_write.cpp




## Quantities

	
| Variable             | Comment                             | Units                    |
| ---------            | ----------------                    | ------------------------ |
| x,y,z                | position in                         | [cm]                     |
| dx,dy,dz             | direction                           | (normalized velocity)    |
| sx,sy,sz             | polarization of non-photons         |                          |
| sPolAx,sPolAy,sPolAz | s-polarization amplitude of photons |                          |
| pPolAx,pPolAy,pPolAz | p-polarization amplitude of photons |                          |
| sPolx,sPoly,sPolz    | s-polarization amplitude of photons |                          |
| pPolx,pPoly,pPolz    | p-polarization amplitude of photons |                          |
| time                 | ray time w.r.t. ray generation      | [ms]                     |
| wavelength           |                                     | [Ang]                    |
| weight               | weight                              |                          |
	
