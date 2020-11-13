//#include "rays.hh" // helper class that stores the rays and returns 1D vectors
#include "openPMD_io.hh"
#include <iostream>
#include <openPMD/openPMD.hpp> // openPMD C++ API
#define DEBUG

#include <exception>
///\file
// using namespace raytracing;
// using raytracing::openPMD_io;
/** \brief defines the maximum number of rays that can be stored in memory before dumping to file
 * \todo The CHUNK_SIZE should not be hardcoded
 * it should also be optimized with tests
 */
namespace raytracing {
// constexpr size_t CHUNK_SIZE = 10000;
constexpr size_t CHUNK_SIZE = 3;
} // namespace raytracing

/** \todo use particlePatches ... but I don't understand if/how */

//------------------------------------------------------------
// filaname extensions base on the declared format
const std::map<raytracing::openPMD_output_format_t, std::string>
        raytracing::openPMD_io::output_format_names = {
                {JSON, "json"}, {HDF5, "h5"}, {AUTO, ""} //
};

//------------------------------------------------------------
raytracing::openPMD_io::openPMD_io(const std::string& filename, std::string mc_code_name,
                                   std::string mc_code_version, std::string instrument_name,
                                   std::string name_current_component):
    //, int repeat):
    _name(filename),
    _mc_code_name(mc_code_name),
    _mc_code_version(mc_code_version),
    _instrument_name(instrument_name),
    _name_current_component(name_current_component),
    _i_repeat(0),
    _n_repeat(1),
    _offset({0}),
    _series(nullptr){};

//------------------------------------------------------------
void
raytracing::openPMD_io::init_ray_prop(std::string name, openPMD::Dataset& dataset, bool isScalar,
                                      std::map<openPMD::UnitDimension, double> const& dims,
                                      double unitSI) {
	auto rays = rays_pmd();

	rays[name].setUnitDimension(dims);

	if (isScalar) {
		rays[name][openPMD::RecordComponent::SCALAR].resetDataset(dataset);
		rays[name][openPMD::RecordComponent::SCALAR].setUnitSI(unitSI);
	} else {
		// now define the RECORD_COMPONENT
		for (auto dim : {"x", "y", "z"}) {
			rays[name][dim].resetDataset(dataset);
			rays[name][dim].setUnitSI(unitSI);
		}
	}
}

//------------------------------------------------------------
/**
 * \internal \remark
 * The openPMD ray tracing extension settings are set here for the rays' properties.\n
 * n_rays is the ncount of McStas, so the number of rays to be
 * simulated
 **/
void
raytracing::openPMD_io::init_rays(std::string particle_species, unsigned long long int n_rays,
                                  unsigned int iter) {
	_max_allowed_rays = n_rays;
	_nrays =0;
#ifdef DEBUG
	std::cout << "--------------- [INIT RAYS]" << std::endl;
#endif
	auto rays = rays_pmd(particle_species);

	// these are a single entry to mark some general properties of the particles
	openPMD::Dataset dataset_single_float =
	        openPMD::Dataset(openPMD::Datatype::FLOAT, openPMD::Extent{1});
	init_ray_prop("directionOfGravity", dataset_single_float, false);
	init_ray_prop("horizontalCoordinate", dataset_single_float, false);
	init_ray_prop("mass", dataset_single_float, true, {{openPMD::UnitDimension::M, 1.}});

	rays.setAttribute("speciesType", particle_species);
	rays.setAttribute("PDGID", particle_species);
	rays.setAttribute("numParticles", 0);

	openPMD::Dataset dataset_float =
	        openPMD::Dataset(openPMD::Datatype::FLOAT, openPMD::Extent{n_rays});
	openPMD::Dataset dataset_int =
	        openPMD::Dataset(openPMD::Datatype::INT, openPMD::Extent{n_rays});
	openPMD::Dataset dataset_ulongint =
	        openPMD::Dataset(openPMD::Datatype::ULONGLONG, openPMD::Extent{n_rays});

	init_ray_prop("position", dataset_float, false, {{openPMD::UnitDimension::L, 1.}}, 1e-2);
	init_ray_prop("direction", dataset_float, false);

	init_ray_prop("nonPhotonPolarization", dataset_float, false);
	init_ray_prop("photonSPolarizationAmplitude", dataset_float, false);
	init_ray_prop("photonSPolarizationPhase", dataset_float, true);
	init_ray_prop("photonPPolarizationAmplitude", dataset_float, false);
	init_ray_prop("photonPPolarizationPhase", dataset_float, true);

	init_ray_prop("wavelength", dataset_float, true, {{openPMD::UnitDimension::L, -1}},
	              0); // 1.6021766e-13); // MeV
	init_ray_prop("weight", dataset_float, true);
	init_ray_prop("rayTime", dataset_float, true, {{openPMD::UnitDimension::T, 1.}}, 1e-3);

	init_ray_prop("id", dataset_ulongint, true);
	init_ray_prop("particleStatus", dataset_int, true);
#ifdef DEBUG
	std::cout << "------------------------------ [INIT RAYS] Done" << std::endl;
#endif
}

void
raytracing::openPMD_io::init_write(std::string particle_species, unsigned long long int n_rays,
                                   openPMD_output_format_t extension, unsigned int iter) {
	_iter                = iter;
	std::string filename = _name;
	//	size_t extension_pos = filename.rfind('.', 5);
	if (extension != AUTO) {
		std::string a = output_format_names.find(extension)->second;
		filename += std::string(".") + a;
	}
	// assign the global variable to keep track of it
	_series = std::unique_ptr<openPMD::Series>(
	        new openPMD::Series(filename, openPMD::Access::CREATE));

	_series->setAuthor("openPMD raytracing API");
	// latticeName: name of the instrument
	// latticeFile: name of the instrument file
	// branchIndex: unique index number assigned to the latice branch the[article is in.
	// (it should be per particle
	//
	std::cout << "Filename: " << filename << std::endl; // remove
	// auto i = _series->iterations[1];
	auto i = iter_pmd(iter);
	_series->flush();

	//	openPMD::Record directionOfGravity;
	///\todo I don't how to add the directionOfGravity
	// float directionOfGravity[3] = {0.,0.,0.}; // this ensures that the attribute is
	// float type _series->setAttribute("directionOfGravity_X", directionOfGravity[0]);
	//_series->setAttribute("directionOfGravity_Y", directionOfGravity[1]);
	//_series->setAttribute("directionOfGravity_Z", directionOfGravity[2]);

	std::cout << "Iter: " << _iter << std::endl;
	// set the mccode, mccode_version, component name, instrument name

	init_rays(particle_species, n_rays, iter);
	std::cout << "init rays done" << std::endl;
	// openPMD::Record mass = rays["mass"];
	// openPMD::RecordComponent mass_scalar = mass[openPMD::RecordComponent::SCALAR];

	//	mass_scalar.resetDataset(dataset);

	_series->flush();
	std::cout << "flush done" << std::endl;
}

//------------------------------------------------------------
template <typename T>
void
save_write_single(openPMD::ParticleSpecies& rays, std::string field, std::string record,
                  raytracing::openPMD_io::Rays::Record<T>& rec, openPMD::Offset& offset,
                  openPMD::Extent& extent) {
	rays[field][record].storeChunk(openPMD::shareRaw(rec.vals()), offset, extent);
	rays[field][record].setAttribute("minValue", rec.min());
	rays[field][record].setAttribute("maxValue", rec.max());
}
//------------------------------------------------------------

void
raytracing::openPMD_io::save_write(void) {
	if (_rays.size() == 0) return;

#ifdef DEBUG
	std::cout << "Number of saved rays: " << _rays.size() << "\t" << _rays._x.vals().size()
	          << std::endl;
#endif
	auto rays = rays_pmd();

	unsigned long long int extent_size = (_offset[0] + _rays.size() > _max_allowed_rays)
	                                             ? _max_allowed_rays - _offset[0]
	                                             : _rays.size();

	_nrays+=extent_size;
	
	// this check is here and not in the trace_write because I believe that loosing time for a
	// CHUNKSIZE simulating rays that are not store is much less frequent that.
	if (_nrays >= _max_allowed_rays)
		throw std::runtime_error("Maximum number of foreseen rays reached, stopping");

	openPMD::Extent extent = {extent_size};

	save_write_single(rays, "position", "x", _rays._x, _offset, extent);
	save_write_single(rays, "position", "y", _rays._y, _offset, extent);
	save_write_single(rays, "position", "z", _rays._z, _offset, extent);

	save_write_single(rays, "direction", "x", _rays._dx, _offset, extent);
	save_write_single(rays, "direction", "y", _rays._dy, _offset, extent);
	save_write_single(rays, "direction", "z", _rays._dz, _offset, extent);

	save_write_single(rays, "nonPhotonPolarization", "x", _rays._sx, _offset, extent);
	save_write_single(rays, "nonPhotonPolarization", "y", _rays._sy, _offset, extent);
	save_write_single(rays, "nonPhotonPolarization", "z", _rays._sz, _offset, extent);

	save_write_single(rays, "photonSPolarizationAmplitude", "x", _rays._sPolAx, _offset,
	                  extent);
	save_write_single(rays, "photonSPolarizationAmplitude", "y", _rays._sPolAy, _offset,
	                  extent);
	save_write_single(rays, "photonSPolarizationAmplitude", "z", _rays._sPolAz, _offset,
	                  extent);
	save_write_single(rays, "photonSPolarizationPhase", openPMD::RecordComponent::SCALAR,
	                  _rays._sPolPh, _offset, extent);

	save_write_single(rays, "photonPPolarizationAmplitude", "x", _rays._pPolAx, _offset,
	                  extent);
	save_write_single(rays, "photonPPolarizationAmplitude", "y", _rays._pPolAy, _offset,
	                  extent);
	save_write_single(rays, "photonPPolarizationAmplitude", "z", _rays._pPolAz, _offset,
	                  extent);
	save_write_single(rays, "photonPPolarizationPhase", openPMD::RecordComponent::SCALAR,
	                  _rays._pPolPh, _offset, extent);

	save_write_single(rays, "rayTime", openPMD::RecordComponent::SCALAR, _rays._time, _offset,
	                  extent);
	save_write_single(rays, "wavelength", openPMD::RecordComponent::SCALAR, _rays._wavelength,
	                  _offset, extent);
	save_write_single(rays, "weight", openPMD::RecordComponent::SCALAR, _rays._weight, _offset,
	                  extent);

	save_write_single(rays, "id", openPMD::RecordComponent::SCALAR, _rays._id, _offset, extent);
	save_write_single(rays, "particleStatus", openPMD::RecordComponent::SCALAR, _rays._status,
	                  _offset, extent);

	rays.setAttribute("numParticles", _nrays + 1);

	_series->flush();
	_rays.clear();

	for (size_t i = 0; i < extent.size(); ++i)
		_offset[i] += extent[i];
}

//------------------------------------------------------------
//------------------------------------------------------------
template <typename T>
void
read_single(openPMD::ParticleSpecies& rays, std::string field, std::string record,
            raytracing::openPMD_io::Rays::Record<T>& rec, openPMD::Offset& offset,
            openPMD::Extent& chunk_size) {

	auto data  = rays[field][record];
	rec.vals().reserve(chunk_size[0]);
	rec.vals().resize(chunk_size[0]);
	data.loadChunk<T>(openPMD::shareRaw(rec.vals()), offset, chunk_size);//data.loadChunk<T>(offset, chunk_size);
	// if (field == "position") {
	// 	for (auto i = 0; i < chunk_size[0]; ++i) {
	// 		std::cout << "Data " << record << ": " << i << "\t" << rec.vals()[i] << "\t"
	// 		          << rec.vals().size() << std::endl;
	// 	}
	// }
	        // rec.store((ddata.get()), chunk_size[0],                              //
	        //           rays[field][record].getAttribute("minValue").get<float>(), //
	        //           rays[field][record].getAttribute("maxValue").get<float>());
}
//------------------------------------------------------------

void
raytracing::openPMD_io::load_chunk(void) {

	_rays.clear(); // Necessary to set _read to zero
	auto rays                     = rays_pmd();
	unsigned long long int remaining = _nrays - _offset[0];
	openPMD::Extent chunk_size    = {remaining > raytracing::CHUNK_SIZE ? raytracing::CHUNK_SIZE
                                                                      : remaining};
#ifdef DEBUG
	std::cout << "OO: " << _nrays << "\t" << _offset[0] << "\t" << remaining << "\t" << chunk_size[0] << std::endl;
	std::cout << "  Loading chunk of size " << chunk_size[0] << "; file contains " << _nrays
		//<< " entries of type " << x_dat.getDatatype()
		  << std::endl;
#endif
	/* I don't understand....
	 * the data type info is embedded in the data... so why do we need to declare
	 * loadChunk<float>? it should overload to the right function... and return the correct
	 * datatype.
	 */

	read_single(rays, "position", "x", _rays._x, _offset, chunk_size);
	read_single(rays, "position", "y", _rays._y, _offset, chunk_size);
	read_single(rays, "position", "z", _rays._z, _offset, chunk_size);

	read_single(rays, "direction", "x", _rays._dx, _offset, chunk_size);
	read_single(rays, "direction", "y", _rays._dy, _offset, chunk_size);
	read_single(rays, "direction", "z", _rays._dz, _offset, chunk_size);

	read_single(rays, "nonPhotonPolarization", "x", _rays._sx, _offset, chunk_size);
	read_single(rays, "nonPhotonPolarization", "y", _rays._sy, _offset, chunk_size);
	read_single(rays, "nonPhotonPolarization", "z", _rays._sz, _offset, chunk_size);

	read_single(rays, "photonSPolarizationAmplitude", "x", _rays._sPolAx, _offset, chunk_size);
	read_single(rays, "photonSPolarizationAmplitude", "y", _rays._sPolAy, _offset, chunk_size);
	read_single(rays, "photonSPolarizationAmplitude", "z", _rays._sPolAz, _offset, chunk_size);
	read_single(rays, "photonSPolarizationPhase", openPMD::RecordComponent::SCALAR,
	            _rays._sPolPh, _offset, chunk_size);

	read_single(rays, "photonPPolarizationAmplitude", "x", _rays._pPolAx, _offset, chunk_size);
	read_single(rays, "photonPPolarizationAmplitude", "y", _rays._pPolAy, _offset, chunk_size);
	read_single(rays, "photonPPolarizationAmplitude", "z", _rays._pPolAz, _offset, chunk_size);
	read_single(rays, "photonPPolarizationPhase", openPMD::RecordComponent::SCALAR,
	            _rays._pPolPh, _offset, chunk_size);

	read_single(rays, "rayTime", openPMD::RecordComponent::SCALAR, _rays._time, _offset,
	            chunk_size);
	read_single(rays, "wavelength", openPMD::RecordComponent::SCALAR, _rays._wavelength,
	            _offset, chunk_size);
	read_single(rays, "weight", openPMD::RecordComponent::SCALAR, _rays._weight, _offset,
	            chunk_size);

	read_single(rays, "id", openPMD::RecordComponent::SCALAR, _rays._id, _offset, chunk_size);
	read_single(rays, "particleStatus", openPMD::RecordComponent::SCALAR, _rays._status,
	            _offset, chunk_size);

	//_rays.size(chunk_size[0]);
	
	_series->flush();

	//	std::cout << _rays._x[0] << "\t" << _rays._x[2] << std::endl;
	for (size_t i = 0; i < chunk_size.size(); ++i)
		_offset[i] += chunk_size[i];
}

unsigned long long int
raytracing::openPMD_io::init_read(std::string particle_species, unsigned int iter,
                                  unsigned long long int n_rays, unsigned int repeat) {

	_n_repeat            = repeat;
	_i_repeat            = 0;
	_iter                = iter;
	_offset              = {0};
	std::string filename = _name;

	// assign the global variable to keep track of it
	_series = std::unique_ptr<openPMD::Series>(new openPMD::Series(
	        filename,
	        openPMD::Access::READ_ONLY)); ///\todo the file access type was defined in
	                                      /// the openPMD_io constructor

	std::cout << "File information: " << filename << std::endl;
	if (_series->containsAttribute("author"))
		std::cout << "  Author  : " << _series->author() << std::endl;
	std::cout << "  Filename: " << filename << std::endl;
	std::cout << "  Number of iterations: " << _series->iterations.size() << std::endl;

	if (_series->iterations.size() == 0)
		std::cout << "  ERROR, no iterations found in openPMD series" << std::endl;

	///\todo check if the iteration exists

	// check the maximum number of rays stored
	_rays.clear(); // Necessary to set _read to zero

	auto i = iter_pmd(_iter);
	_series->flush();
	auto rays = rays_pmd(particle_species);
	_nrays    = rays.getAttribute("numParticles").get<unsigned long long int>();
	std::cout << "numParticles: " << _nrays << std::endl;
	if (n_rays > _nrays) {
		std::cerr << "[ERROR] Requested a number of rays that is not available in "
		             "the "
		             "current file"
		          << std::endl;
		throw std::runtime_error("ERROR"); ///\todo make it more meaningful
	}
	if (n_rays != 0) {
		std::cout << "[WARNING] Requested " << n_rays
		          << ", while available in file: " << _nrays << std::endl;
		_nrays = n_rays;
	}
	_series->flush();
	return _nrays;
}

void
raytracing::openPMD_io::trace_write(raytracing::Ray this_ray) {
	if (_rays.size() == CHUNK_SIZE) {
#ifdef DEBUG
		std::cout << "Reached CHUNK_SIZE\t" << _offset[0] << "\t" << _rays.size() << "\t"
		          << _max_allowed_rays << std::endl;
#endif
		save_write();
		_rays.clear();
	}
	_rays.push(this_ray);
}

raytracing::Ray
raytracing::openPMD_io::trace_read(void) {
	///\todo reordering if conditions can improve performance
	std::cout << "---- " << _i_repeat << "\t" << _n_repeat << "\t";
	if(_i_repeat++==0){
		if (_rays.is_chunk_finished()) { load_chunk(); }
		_last_ray = _rays.pop();
	}
	if (_i_repeat >= _n_repeat) _i_repeat = 0;

	std::cout <<_last_ray  << std::endl; 
	return _last_ray;
}

void
raytracing::openPMD_io::get_gravity_direction(float* x, float* y, float* z) {
	auto rays = rays_pmd();
	*x        = rays["directionOfGravity"]["x"].loadChunk<float>().get()[0];
	*y        = rays["directionOfGravity"]["y"].loadChunk<float>().get()[0];
	*z        = rays["directionOfGravity"]["z"].loadChunk<float>().get()[0];
}

void
raytracing::openPMD_io::get_horizontal_direction(float* x, float* y, float* z) {
	auto rays = rays_pmd();
	*x        = rays["horizontalCoordinate"]["x"].loadChunk<float>().get()[0];
	*y        = rays["horizontalCoordinate"]["y"].loadChunk<float>().get()[0];
	*z        = rays["horizontalCoordinate"]["z"].loadChunk<float>().get()[0];
}

std::ostream&
raytracing::operator<<(std::ostream& os, const raytracing::Ray& ray) {
	return os << "(" << ray.x() << ", " << ray.y() << "\t" << ray.z() << ")";
}
