#ifndef RAYTRACE_API_HH
#define RAYTRACE_API_HH
///\file
#include "openpmd_output_formats.h" // enum with the available openPMD backends
#include "ray.hh"
#include <openPMD/openPMD.hpp> // openPMD C++ API
#include <string>

#define ITER 1

class Rays; // forward declaration of the class

/** \class openPMD_io
 * \brief I/O API for the ray trace extension of the openPMD format
 *
 * This API is a higher level API of the official openPMD API. \n
 * This is meant to be used to save/read a coherent set of rays to/from file. \n
 * Coherent in this case means that a single type of rays (particles) are going to used.
 */
class openPMD_io {
public:
	///\brief constructor
	explicit openPMD_io(
	        const std::string& filename,             ///< filename
	        openPMD::Access read_mode,               ///< file access mode (read/write/append)
	        std::string mc_code_name           = "", ///< Name of the simulation code name
	        std::string mc_code_version        = "", ///< Simulation software version
	        std::string instrument_name        = "", ///< Name of the instrument
	        std::string name_current_component = "", ///< current component name along the beamline
	        int repeat = 1 ///< number of times a ray should be repeatedly retrieved
	);

	/***************************************************************/
	/// \name Writing mode
	///@{
public:
	/// \brief initializes the "series" object from the openPMD API in WRITE MODE
	void init_write(openPMD_output_format_t output_format, ///< output format
	                unsigned long long int n_rays,         ///< number of rays being simulated (max)
	                unsigned int iter = 1 ///< openPMD iteration, always using the default value
	);

#ifdef SHERVIN
	/** \brief save ray properties in a vector
	 *
	 * it writes to the file when the number of rays reaches CHUNK_SIZE as defined in the
	 * openPMD_io.cc file
	 * clang-format off
	 * \param[in] x,y,z : ray position          -> it is converted to cm ( * 100)
	 * \param[in] sx, sy, sz : ray polarization
	 * \param[in] vx, vy, vz : ray velocity     -> only the direction is stored
	 * \param[in] t : time
	 * \param[in] p : weight
	 * clang-format off
	 */
	void trace_write(double x, double y, double z,    // position
	                 double sx, double sy, double sz, // polarization
	                 double vx, double vy, double vz, // velocity
	                 double t, double p);
#endif
	/// \brief save ray properties
	void trace_write(Ray this_ray);
	/** \brief Flushes the output to file before closing it */
	void save_write(void);

private:
	///@}

	/***************************************************************/
	/// \name Reading mode
	///@{
public:
	/// \brief initializes the "series" object from the openPMD API in READ MODE
	unsigned long long int
	init_read(openPMD_output_format_t output_format, ///< output format
	          unsigned long long int n_rays,         ///< number of rays being simulated (max)
	          unsigned int iter = 1 ///< openPMD iteration, always using the default value
	);

#ifdef SHERVIN
	/** \brief save ray properties in a vector
	 *
	 * Reads ray data from particle instance in format useful for McStas
	 * \param[out] x,y,z : ray position in m
	 * \param[out] sx, sy, sz : ray polarization (not working yet)
	 * \param[out] vx, vy, vz : ray velocity m/s
	 * \param[out] t : time [s]
	 * \param[out] p : weight
	 */
	void trace_read(double* x, double* y, double* z,    // position
	                double* sx, double* sy, double* sz, // polarization
	                double* vx, double* vy, double* vz, // velocity
	                double* t, double* p);
#endif
	/** \brief read one ray from file
	 *
	 * \returns Ray object
	 */
	Ray trace_read(void) const;
	///@}
private:
	void load_chunk(void);

private:
	// parameters defined at construction
	std::string _name;
	std::string _mc_code_name;
	std::string _mc_code_version;
	std::string _instrument_name;
	std::string _name_current_component;
	int _i_repeat, _n_repeat;

	// internal usage
	openPMD::Access _access_mode;
	openPMD::Offset _offset;
	bool _isWriteMode;
	std::unique_ptr<openPMD::Series> _series;
	Rays _rays;

	static const std::map<openPMD_output_format_t, std::string> output_format_names;
	/** declare the ray particle species in the file */
	void init_rays(unsigned int iter, unsigned long long int n_rays);

	//------------------------------ set of helper methods
	inline openPMD::Iteration& iter_pmd(void) { return _series->iterations[ITER]; }

	inline openPMD::ParticleSpecies& rays_pmd(void) {
		auto i = iter_pmd();
		return i.particles["ray"];
	}

	void
	init_ray_prop(std::string name, openPMD::Dataset& dataset, bool isScalar,
	              std::map<openPMD::UnitDimension, double> const& dims = {{openPMD::UnitDimension::L,
	                                                                       0.}},
	              double unitSI                                        = 0.);
};

#endif
