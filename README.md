# openPMD_raytrace_API
c++ API for the openPMD ray trace extension 

## Devel instructions
```
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
