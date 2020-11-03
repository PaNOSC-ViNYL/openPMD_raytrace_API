#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
namespace py = pybind11;

#include <openPMD_io.hh>
#include <rays.hh>
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
	py::class_<Ray>(m, "Ray")
	        .def("x", &Ray::x)
	        .def("y", &Ray::y)
	        .def("z", &Ray::z)
	        ///\todo get position returning an ntuple
	        .def("dx", &Ray::dx)
	        .def("dy", &Ray::dy)
	        .def("dz", &Ray::dz)
	        ///\todo get_direction returning an ntuple
	        .def("sx", &Ray::sx)
	        .def("sy", &Ray::sy)
	        .def("sz", &Ray::sz)
	        ///\todo get_polarization returning an ntuple
	        .def("sPolAx", &Ray::sPolAx)
	        .def("sPolAy", &Ray::sPolAy)
	        .def("sPolAz", &Ray::sPolAz)
	        .def("sPolPh", &Ray::sPolPh)
	        ///\todo get_sPolarization returning an ntuple
	        .def("pPolAx", &Ray::pPolAx)
	        .def("pPolAy", &Ray::pPolAy)
	        .def("pPolAz", &Ray::pPolAz)
	        .def("pPolPh", &Ray::pPolPh)
	        ///\todo get_pPolarization returning an ntuple
	        //	        .def("weight", &Ray::weight)
	        //.def("id", &Ray::id)
	        //.def("status", &Ray::status)
	        // // Setters
	        .def("position", &Ray::position)
	        .def("direction", &Ray::direction)
	        .def("polarization", &Ray::polarization)
	        .def("sPolarization", &Ray::sPolarization)
	        .def("pPolarization", &Ray::pPolarization)
	        //
	        .def("wavelength", static_cast<void (Ray::*)(double)>(&Ray::wavelength))
	        .def("time", static_cast<void (Ray::*)(double)>(&Ray::time))
	        .def("weight", static_cast<void (Ray::*)(double)>(&Ray::weight))
	        .def("id", static_cast<void (Ray::*)(unsigned long long int)>(&Ray::id))
	        .def("status", static_cast<void (Ray::*)(particleStatus_t)>(&Ray::status))

	        //
	        ;
}
