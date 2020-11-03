//#include "rays.hh" // helper class that stores the rays and returns 1D vectors
#include "openPMD_io.hh"
#include <iostream>
#include <openPMD/openPMD.hpp> // openPMD C++ API
#define DEBUG

///\file
//using namespace raytracing;
//using raytracing::openPMD_io;
/** \brief defines the maximum number of rays that can be stored in memory before dumping to file
 * \todo The CHUNK_SIZE should not be hardcoded
 * it should also be optimized with tests
 */
namespace raytracing {
constexpr size_t CHUNK_SIZE = 10000;
}

/** \todo use particlePatches ... but I don't understand if/how */

//------------------------------------------------------------
// filaname extensions base on the declared format
const std::map<raytracing::openPMD_output_format_t, std::string> raytracing::openPMD_io::output_format_names = {
        {JSON, "json"},
        {HDF5, "h5"},
	{AUTO, ""} // 
};

//------------------------------------------------------------
raytracing::openPMD_io::openPMD_io(const std::string& filename, // openPMD::Access read_mode,
                       std::string mc_code_name, std::string mc_code_version,
                       std::string instrument_name, std::string name_current_component, int repeat):
    _name(filename),
    _mc_code_name(mc_code_name),
    _mc_code_version(mc_code_version),
    _instrument_name(instrument_name),
    _name_current_component(name_current_component),
    _i_repeat(1),
    _n_repeat(repeat),
    _offset({0}),
    _series(nullptr){};

//------------------------------------------------------------
void
raytracing::openPMD_io::init_ray_prop(std::string name, openPMD::Dataset& dataset, bool isScalar,
                          std::map<openPMD::UnitDimension, double> const& dims, double unitSI) {
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
#ifdef DEBUG
	std::cout << "--------------- [INIT RAYS]" << std::endl;
#endif
	auto rays = rays_pmd(particle_species);

	// these are a single entry to mark some general properties of the particles
	openPMD::Dataset dataset_single_float =
	        openPMD::Dataset(openPMD::Datatype::FLOAT, openPMD::Extent{1});
	init_ray_prop("directionOfGravity", dataset_single_float, false);
	init_ray_prop("mass", dataset_single_float, true, {{openPMD::UnitDimension::M, 1.}});

	rays.setAttribute("speciesType", particle_species);
	rays.setAttribute("PDGID", particle_species);

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
	if (extension != AUTO){
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
	///\todo set minValue and maxValue
	openPMD::Extent extent = {_rays.size()};

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

	_series->flush();
	_rays.clear();

	for (size_t i = 0; i < extent.size(); ++i)
		_offset[i] += extent[i];
}

//------------------------------------------------------------

void
raytracing::openPMD_io::load_chunk(void) {
	_rays.clear(); // Necessary to set _read to zero
	auto ray_data = rays_pmd();

	auto x_dat = ray_data["position"]["x"];

	// Assume all have same length
	openPMD::Extent extent = x_dat.getExtent();

	if (extent[0] == 0) {
		std::cout << "[STATUS][LoadChunk] No more data to read" << std::endl;
		return;
	}
	auto y_dat     = ray_data["position"]["y"];
	auto z_dat     = ray_data["position"]["z"];
	auto dx_dat    = ray_data["direction"]["x"];
	auto dy_dat    = ray_data["direction"]["y"];
	auto dz_dat    = ray_data["direction"]["z"];
	auto sx_dat    = ray_data["polarization"]["x"];
	auto sy_dat    = ray_data["polarization"]["y"];
	auto sz_dat    = ray_data["polarization"]["z"];
	auto sPolx_dat = ray_data["sPolarization"]["x"];
	auto sPoly_dat = ray_data["sPolarization"]["y"];
	auto sPolz_dat = ray_data["sPolarization"]["z"];
	auto pPolx_dat = ray_data["pPolarization"]["x"];
	auto pPoly_dat = ray_data["pPolarization"]["y"];
	auto pPolz_dat = ray_data["pPolarization"]["z"];

	auto time_dat       = ray_data["time"][openPMD::RecordComponent::SCALAR];
	auto wavelength_dat = ray_data["wavelength"][openPMD::RecordComponent::SCALAR];
	auto weight_dat     = ray_data["weight"][openPMD::RecordComponent::SCALAR];
	// Missing polarization vector

	openPMD::Extent chunk_size = {(extent[0] > raytracing::CHUNK_SIZE) ? raytracing::CHUNK_SIZE : extent[0]};
#ifdef DEBUG
	std::cout << "  Loading chunk of size " << chunk_size[0] << "; file contains " << extent[0]
	          << " entries of type " << x_dat.getDatatype() << std::endl;
#endif
	/* I don't understand....
	 * the data type info is embedded in the data... so why do we need to declare
	 * loadChunk<float>? it should overload to the right function... and return the correct
	 * datatype.
	 */
	auto all_x_data     = x_dat.loadChunk<float>(_offset, chunk_size);
	auto all_y_data     = y_dat.loadChunk<float>(_offset, chunk_size);
	auto all_z_data     = z_dat.loadChunk<float>(_offset, chunk_size);
	auto all_dx_data    = dx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_dy_data    = dy_dat.loadChunk<float>(_offset, chunk_size);
	auto all_dz_data    = dz_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sx_data    = sx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sy_data    = sy_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sz_data    = sz_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sPolx_data = sPolx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sPoly_data = sPoly_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sPolz_data = sPolz_dat.loadChunk<float>(_offset, chunk_size);
	auto all_pPolx_data = pPolx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_pPoly_data = pPoly_dat.loadChunk<float>(_offset, chunk_size);
	auto all_pPolz_data = pPolz_dat.loadChunk<float>(_offset, chunk_size);

	auto time_data       = time_dat.loadChunk<float>(_offset, chunk_size);
	auto wavelength_data = wavelength_dat.loadChunk<float>(_offset, chunk_size);
	auto weight_data     = weight_dat.loadChunk<float>(_offset, chunk_size);
	// Missing polarization vector

	_series->flush();

	// Store openPMD data in rays particle instance
#ifdef SHERVIN
	for (size_t index = 0; index < chunk_size[0]; index++) {
		_rays.store(all_x_data.get()[index], all_y_data.get()[index],
		            all_z_data.get()[index], //
		            all_dx_data.get()[index], all_dy_data.get()[index],
		            all_dz_data.get()[index], //
		            all_sx_data.get()[index], all_sy_data.get()[index],
		            all_sz_data.get()[index], //
		            all_sPolx_data.get()[index], all_sPoly_data.get()[index],
		            all_sPolz_data.get()[index], //
		            all_pPolx_data.get()[index], all_pPoly_data.get()[index],
		            all_pPolz_data.get()[index], //
		            wavelength_data.get()[index], time_data.get()[index],
		            weight_data.get()[index]);
	}
#endif
}

unsigned long long int
raytracing::openPMD_io::init_read(raytracing::openPMD_output_format_t extension, unsigned long long int n_rays,
                      unsigned int iter) {
	std::string filename = _name;

	// assign the global variable to keep track of it
	_series = std::unique_ptr<openPMD::Series>(new openPMD::Series(
	        filename, openPMD::Access::READ_ONLY)); ///\todo the file access type was defined in
	                                                ///the openPMD_io constructor

	std::cout << "File information: " << filename << std::endl;
	if (_series->containsAttribute("author"))
		std::cout << "  Author  : " << _series->author() << std::endl;
	std::cout << "  Filename: " << filename << std::endl;
	std::cout << "  Number of iterations: " << _series->iterations.size() << std::endl;

	if (_series->iterations.size() == 0)
		std::cout << "  ERROR, no iterations found in openPMD series" << std::endl;
	if (_series->iterations.size() > 1)
		std::cout
		        << "  WARNING, several iterations found in openPMD series, only 1 is used."
		        << std::endl;

	// check the maximum number of rays stored
	auto ray_data          = rays_pmd();
	auto x_dat             = ray_data["position"]["x"];
	openPMD::Extent extent = x_dat.getExtent();
	_rays.clear(); // Necessary to set _read to zero
	return extent[0];
}

void
raytracing::openPMD_io::trace_write(raytracing::Ray this_ray) {
	_rays.push(this_ray);
}

raytracing::Ray
raytracing::openPMD_io::trace_read(void) {
	return _rays.pop(true);
}
