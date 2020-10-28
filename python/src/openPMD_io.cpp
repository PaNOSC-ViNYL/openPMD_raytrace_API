#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
namespace py = pybind11;

#include <openPMD_io.hh>
using namespace raytracing;

PYBIND11_MODULE(raytracing, m) {

	m.doc() = "pybind11 example plugin"; // optional module docstring
	py::class_<openPMD_io>(m, "openPMD_io");
}
