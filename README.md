c++ API for the openPMD ray trace extension 

# Devel instructions
```
git clone -b devel git@github.com:PaNOSC-ViNYL/openPMD_raytrace_API.git
cd openPMD_raytrace_API/
git clone --depth 1 git@github.com:openPMD/openPMD-api.git
cd openPMD-api
patch -p1 < ../cmake_api.patch
cd -
mkdir build/
cd build/
cmake .. -DopenPMD_USE_PYTHON=OFF -DopenPMD_INSTALL=OFF -DopenPMD_USE_INTERNAL_CATCH=ON -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_CLI_TOOLS=OFF


cmake --build .
cmake --build . --target doc
```

# Usage

## Writing
```
include <ray.hh>
```
A Ray object should be constructed and filled with all the relevant information.

The ray is then queued for writing using the openPMD_io class:
```
std::string filename = "test_file";
raytracing::openPMD_io iol(filename, "test code");
	
iol.init_write(raytracing::JSON, "2112", 3, 1);

raytracing::Ray myray;
myray.position(1,2,3);
//...
myray.direction(1,1,1,1./sqrt(3));

// append 
iol.trace_write(myray);

	
iol.save_write();
```

The Ray class is providing all the conversion/utility operations on the quantities stored in the openPMD file according to the RAYTRACE extension


## Reading



## Todo
 - [ ] Setter and getter for gravity direction
 - [ ] Setter and getter for horizontalCoordinate
 - [ ] Setter and getter for numParticles
 - [ ] Interal usage of numParticles
 - [ ] Binding of wavelength, time, weight, id, status getters
 - [ ] Test the python binding
 - [ ] Cmake install for the python binding
 - [ ] PyPi package
