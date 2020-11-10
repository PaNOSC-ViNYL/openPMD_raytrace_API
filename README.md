# c++ API for the openPMD ray trace extension 

This library provides an API in C++ and Python to read and write openPMD files following the raytrace extension.


## Quantities
According to the extension, here's the list of variables stored in the file and the corresponding units.

	
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
|                      |                                     |                          |

	

## Devel instructions

```
git clone -b devel git@github.com:PaNOSC-ViNYL/openPMD_raytrace_API.git
cd openPMD_raytrace_API/

cmake ..

cmake --build .
cmake --build . --target doc
```

# Usage

The library is under the \ref raytracing namespace.

Here's a schematic of the I/O logic. 

NB: image with hyperlinks

\dot
	   digraph example {
	       rankdir="LR";
	       node [shape=record, fontname=Helvetica, fontsize=10];
		   u    [label="User's program" shape=ellipse];
		   Ray  [label="Ray object" shape=rectangle, URL="\ref raytracing::Ray"];
		   u -> Ray [label="create and fill"];
		   openPMD_io [];
		   Ray -> openPMD_io [label="trace_write", URL="\ref raytracing::openPMD_io::trace_write"];
		   openPMD_io -> pmd [label="save_write", URL="\ref raytracing::openPMD_io::save_write"];
		   pmd  [ label="openPMD file", shape=ellipse];
		   pmd -> openPMD_io [label="load_chunk (internal)"];
		   openPMD_io -> Ray [label="trace_read", URL="\ref raytracing::openPMD_io::trace_read"];
		   Ray -> u [label="get values"];
	   }
\enddot


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
 - [ ] Add doctest package for C++ unit tests



## Examples
Examples can be found 


\example test_write.cpp



