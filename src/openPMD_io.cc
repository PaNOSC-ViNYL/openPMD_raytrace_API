#include "rays.hh" // helper class that stores the rays and returns 1D vectors
#include "openPMD_io.hh"
#include <iostream>
#include <openPMD/openPMD.hpp> // openPMD C++ API

///\file
using namespace raytracing;
/** \brief defines the maximum number of rays that can be stored in memory before dumping to file
 *\todo The CHUNK_SIZE should not be hardcoded
 * it should also be optimized with tests
 */
namespace raytracing {
constexpr size_t CHUNK_SIZE = 10000;
}

/** \todo ekin should be saved according to the openPMD standard */
/** \todo use particlePatches ... but I don't understand if/how */

//------------------------------------------------------------
const std::map<openPMD_output_format_t, std::string> openPMD_io::output_format_names = {
        {JSON, "json"},
        {HDF5, "h5"},
};

//------------------------------------------------------------
openPMD_io::openPMD_io(const std::string& filename, // openPMD::Access read_mode,
                       std::string mc_code_name, std::string mc_code_version,
                       std::string instrument_name, std::string name_current_component, int repeat):
    _name(filename),
    _mc_code_name(mc_code_name),
    _mc_code_version(mc_code_version),
    _instrument_name(instrument_name),
    _name_current_component(name_current_component),
    _i_repeat(1),
    _n_repeat(repeat),
    //    _access_mode(openPMD::Access::READ_WRITE),
    _offset({0}),
    _series(nullptr){};

//------------------------------------------------------------
/** \brief
 * \param[in] name : name of the field/property
 * \param[in] isScalar : bool indicating if it is scalar
 * \param[in] dims : Unit dimensions
 */
void
openPMD_io::init_ray_prop(std::string name, openPMD::Dataset& dataset, bool isScalar,
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
/** n_rays is the ncount of McStas, so the number of rays to be
 * simulated */
void
openPMD_io::init_rays(unsigned long long int n_rays, unsigned int iter) {

	// series.setAttribute("dinner", "Pizza and Coke");
	// i.setAttribute("vacuum", true);

	auto rays = rays_pmd();
	rays.setAttribute("speciesType", "2112");
	rays.setAttribute("PDGID", "2112");
	// declare the dataset total size: the final size

	openPMD::Dataset dataset = openPMD::Dataset(openPMD::Datatype::FLOAT, openPMD::Extent{n_rays});

	init_ray_prop("position", dataset, false, {{openPMD::UnitDimension::L, 1.}}, 1e-2);
	init_ray_prop("direction", dataset, false);
	init_ray_prop("polarization", dataset, false);
	// now set the scalars
	init_ray_prop("weight", dataset, true);
	init_ray_prop("time", dataset, true, {{openPMD::UnitDimension::T, 1.}}, 1e-3);
	init_ray_prop("wavelength", dataset, true,
	              {{
	                       openPMD::UnitDimension::M,
	                       1,
	               },
	               {
	                       openPMD::UnitDimension::L,
	                       2,
	               },
	               {openPMD::UnitDimension::T, -2}},
	              1.6021766e-13); // MeV
}

void
openPMD_io::init_write(openPMD_output_format_t extension, unsigned long long int n_rays,
                       unsigned int iter) {
	_iter = iter;
	std::string filename = _name;
	std::string a        = output_format_names.find(extension)->second;
	filename += std::string(".") + a;

	// assign the global variable to keep track of it
	_series =
	        std::unique_ptr<openPMD::Series>(new openPMD::Series(filename, openPMD::Access::CREATE));

	_series->setAuthor("openPMD output component");
	// latticeName: name of the instrument
	// latticeFile: name of the instrument file
	// branchIndex: unique index number assigned to the latice branch the[article is in. (it should
	// be per particle
	//
	std::cout << "Filename: " << filename << std::endl; // remove
	auto i = _series->iterations[1];
	// auto i = iter_pmd(iter);
	i.setAttribute("vacuum", true);
	// set the mccode, mccode_version, component name, instrument name

	//	init_rays(n_rays, iter);

	// openPMD::Record mass = rays["mass"];
	// openPMD::RecordComponent mass_scalar =
	// mass[openPMD::RecordComponent::SCALAR];

	// mass_scalar.resetDataset(dataset);

	//_series->flush();
}

//------------------------------------------------------------
void
openPMD_io::save_write(void) {
	if (_rays.size() == 0) return;
#ifdef DEBUG
	std::cout << "Number of saved rays: " << _rays.size() << "\t" << _rays.x().size() << std::endl;
#endif
	auto rays = rays_pmd();

	openPMD::Extent extent = {_rays.size()};
	rays["position"]["x"].storeChunk(openPMD::shareRaw(_rays.x()), _offset, extent);
	rays["position"]["y"].storeChunk(openPMD::shareRaw(_rays.y()), _offset, extent);
	rays["position"]["z"].storeChunk(openPMD::shareRaw(_rays.z()), _offset, extent);

	rays["direction"]["x"].storeChunk(openPMD::shareRaw(_rays.dx()), _offset, extent);
	rays["direction"]["y"].storeChunk(openPMD::shareRaw(_rays.dy()), _offset, extent);
	rays["direction"]["z"].storeChunk(openPMD::shareRaw(_rays.dz()), _offset, extent);

	rays["polarization"]["x"].storeChunk(openPMD::shareRaw(_rays.sx()), _offset, extent);
	rays["polarization"]["y"].storeChunk(openPMD::shareRaw(_rays.sy()), _offset, extent);
	rays["polarization"]["z"].storeChunk(openPMD::shareRaw(_rays.sz()), _offset, extent);

	rays["time"][openPMD::RecordComponent::SCALAR].storeChunk(openPMD::shareRaw(_rays.time()),
	                                                          _offset, extent);
	rays["wavelength"][openPMD::RecordComponent::SCALAR].storeChunk(openPMD::shareRaw(_rays.wavelength()),
	                                                            _offset, extent);
	rays["weight"][openPMD::RecordComponent::SCALAR].storeChunk(openPMD::shareRaw(_rays.weight()),
	                                                            _offset, extent);

	_series->flush();
	_rays.clear();
	for (size_t i = 0; i < extent.size(); ++i)
		_offset[i] += extent[i];
}

//------------------------------------------------------------

void
openPMD_io::load_chunk(void) {
	_rays.clear(); // Necessary to set _read to zero

	auto ray_data = rays_pmd();

	auto x_dat = ray_data["position"]["x"];

	// Assume all have same length
	openPMD::Extent extent = x_dat.getExtent();

	if (extent[0] == 0) {
		std::cout << "[STATUS][LoadChunk] No more data to read" << std::endl;
		return;
	}
	auto y_dat      = ray_data["position"]["y"];
	auto z_dat      = ray_data["position"]["z"];
	auto dx_dat     = ray_data["direction"]["x"];
	auto dy_dat     = ray_data["direction"]["y"];
	auto dz_dat     = ray_data["direction"]["z"];
	auto sx_dat     = ray_data["polarization"]["x"];
	auto sy_dat     = ray_data["polarization"]["y"];
	auto sz_dat     = ray_data["polarization"]["z"];
	auto sPolx_dat     = ray_data["sPolarization"]["x"];
	auto sPoly_dat     = ray_data["sPolarization"]["y"];
	auto sPolz_dat     = ray_data["sPolarization"]["z"];
	auto pPolx_dat     = ray_data["pPolarization"]["x"];
	auto pPoly_dat     = ray_data["pPolarization"]["y"];
	auto pPolz_dat     = ray_data["pPolarization"]["z"];

	auto time_dat   = ray_data["time"][openPMD::RecordComponent::SCALAR];
	auto wavelength_dat = ray_data["wavelength"][openPMD::RecordComponent::SCALAR];
	auto weight_dat = ray_data["weight"][openPMD::RecordComponent::SCALAR];
	// Missing polarization vector

	openPMD::Extent chunk_size = {(extent[0] > CHUNK_SIZE) ? CHUNK_SIZE : extent[0]};
#ifdef DEBUG
	std::cout << "  Loading chunk of size " << chunk_size[0] << "; file contains " << extent[0]
	          << " entries of type " << x_dat.getDatatype() << std::endl;
#endif
	/* I don't understand....
	 * the data type info is embedded in the data... so why do we need to declare loadChunk<float>?
	 * it should overload to the right function... and return the correct datatype.
	 */
	auto all_x_data  = x_dat.loadChunk<float>(_offset, chunk_size);
	auto all_y_data  = y_dat.loadChunk<float>(_offset, chunk_size);
	auto all_z_data  = z_dat.loadChunk<float>(_offset, chunk_size);
	auto all_dx_data = dx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_dy_data = dy_dat.loadChunk<float>(_offset, chunk_size);
	auto all_dz_data = dz_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sx_data = sx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sy_data = sy_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sz_data = sz_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sPolx_data = sPolx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sPoly_data = sPoly_dat.loadChunk<float>(_offset, chunk_size);
	auto all_sPolz_data = sPolz_dat.loadChunk<float>(_offset, chunk_size);
	auto all_pPolx_data = pPolx_dat.loadChunk<float>(_offset, chunk_size);
	auto all_pPoly_data = pPoly_dat.loadChunk<float>(_offset, chunk_size);
	auto all_pPolz_data = pPolz_dat.loadChunk<float>(_offset, chunk_size);

	auto time_data   = time_dat.loadChunk<float>(_offset, chunk_size);
	auto wavelength_data = wavelength_dat.loadChunk<float>(_offset, chunk_size);
	auto weight_data = weight_dat.loadChunk<float>(_offset, chunk_size);
	// Missing polarization vector

	_series->flush();

	// Store openPMD data in rays particle instance
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
		            weight_data.get()[index] );
	}
}

unsigned long long int
openPMD_io::init_read(openPMD_output_format_t extension, unsigned long long int n_rays,
                      unsigned int iter) {
	std::string filename = _name;

	// assign the global variable to keep track of it
	_series = std::unique_ptr<openPMD::Series>(new openPMD::Series(
	        filename, openPMD::Access::READ_ONLY)); ///\todo the file access type was defined in the
	                                                ///openPMD_io constructor

	std::cout << "File information: " << filename << std::endl;
	if (_series->containsAttribute("author"))
		std::cout << "  Author  : " << _series->author() << std::endl;
	std::cout << "  Filename: " << filename << std::endl;
	std::cout << "  Number of iterations: " << _series->iterations.size() << std::endl;

	if (_series->iterations.size() == 0)
		std::cout << "  ERROR, no iterations found in openPMD series" << std::endl;
	if (_series->iterations.size() > 1)
		std::cout << "  WARNING, several iterations found in openPMD series, only 1 is used."
		          << std::endl;

	// check the maximum number of rays stored
	auto ray_data          = rays_pmd();
	auto x_dat             = ray_data["position"]["x"];
	openPMD::Extent extent = x_dat.getExtent();
	_rays.clear(); // Necessary to set _read to zero
	return extent[0];
}

#ifdef SHERVIN
//------------------------------------------------------------
void
openPMD_io::trace_read(double* x, double* y, double* z, double* sx, double* sy, double* sz, double* vx,
                       double* vy, double* vz, double* t, double* p) {

	if (_rays.is_chunk_finished()) load_chunk(); // proceed with a new chunk
	if (_rays.size() == 0) return;               // there was nothing more to read

	/* returning the values of the last ray again
	 * this is to allow repeting rays to be used
	 */
	if (_i_repeat < _n_repeat) {
		_rays.retrieve(x, y, z, sx, sy, sz, vx, vy, vz, t, p);
		++_i_repeat;
	} else {
		// Get ray state from rays particle instance
		_rays.retrieve_next(x, y, z, sx, sy, sz, vx, vy, vz, t,
		                    p); // Will loop internally if more than data size is read
		_i_repeat = 1;
	}
}

//------------------------------------------------------------
void
openPMD_io::trace_write(double x, double y, double z,    //
                        double sx, double sy, double sz, //
                        double vx, double vy, double vz, //
                        double t, double p) {

	_rays.push_back(x, y, z, sx, sy, sz, vx, vy, vz, t, p);

	if (_rays.size() >= CHUNK_SIZE) save_write();
}

#endif


