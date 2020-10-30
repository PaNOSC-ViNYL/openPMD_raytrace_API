#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
namespace py = pybind11;

#include <openPMD_io.hh>
using namespace raytracing;

PYBIND11_MODULE(raytracing, m) {

	m.doc() = "pybind11 example plugin"; // optional module docstring
	py::class_<openPMD_io>(m, "openPMD_io")
	        .def("init_write", &openPMD_io::init_write)
	        .def("init_rays", &openPMD_io::init_rays)
	        .def("trace_write", &openPMD_io::trace_write)
	        .def("save_write", &openPMD_io::save_write)
	        .def("init_read", &openPMD_io::init_read)
	        .def("trace_read", &openPMD_io::trace_read) //
	        ;
}
