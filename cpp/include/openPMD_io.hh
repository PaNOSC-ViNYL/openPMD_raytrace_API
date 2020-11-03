#ifndef RAYTRACE_API_HH
#define RAYTRACE_API_HH
///\file
#include "openpmd_output_formats.h" // enum with the available openPMD backends
#include "ray.hh"
#include <openPMD/openPMD.hpp> // openPMD C++ API
#include <string>

namespace raytracing {
#define ITER 1

/** \class openPMD_io
 * \brief I/O API for the ray trace extension of the openPMD format
 *
 * This API is a higher level API of the official openPMD API. \n
 * This is meant to be used to save/read a coherent set of rays to/from file. \n
 * Coherent in this case means that a single type of rays (particles) are going to used.
 */
class openPMD_io {
	// Auxiliary classes that are private, so not part of the public API
private:
	/** \class Rays
	 * \brief stores the rays' properties
	 *
	 * This class is meant to keep in memory the rays' quantities before effectively writing
	 * them on disk by the store_chunk method, or to load in memory the rays' quantities after
	 * reading the openPMD file from disk with the load_chunk method
	 *
	 *
	 * \todo add a check that the non-photon polarization is not filled if storing photons
	 * \todo check that polarizationAmplitudes are not filled for non-photons
	 *
	 *  \dot
	 *  digraph example {
	 *      rankdir="LR";
	 *      node [shape=record, fontname=Helvetica, fontsize=10];
	 *      Rays [  URL="\ref Rays"];
	 *      u    [label="User's program" shape=ellipse URL="\ref Ray"];
	 *      pmd  [ label="openPMD file", shape=ellipse];
	 *      u    -> Rays [ arrowhead="", style="",label="push Ray" ];
	 *      Rays -> u  [ label="pop Ray", dir=""];
	 *  }
	 *  \enddot
	 */

	class Rays {
	public: // public to make it available to openPMD_io methods
		/**\class Record
		 * \brief template utility class to simplify implementation
		 * It is a vector that also stores min and max values while filling
		 */
		template <typename T> class Record {
			std::vector<T> _vals;
			T _min, _max;

		public:
			Record(): _vals(), _min(), _max() { clear(); }
			const std::vector<T>& vals(void) const { return _vals; };
			T min(void) const { return _min; };
			T max(void) const { return _max; };
			void push_back(T val) {
				_vals.push_back(val);
				if (_min > val) (_min) = val;
				if (_max < val) (_max) = val;
			}
			void clear(void) {
				std::numeric_limits<T> lim;
				_max = lim.min();
				_min = lim.max();
				_vals.clear();
			}
			const T operator[](size_t i) const { return _vals[i]; }; // cannot modify
		};

		//------------------------------ public memebers
	public:
		// I make the Recors public members to avoid writing methods to access them
		//
		// the 3d components are in separate vectors because this is the way the openPMD API
		// wants them to be
		Record<float> _x, _y, _z,                   // position
		        _dx, _dy, _dz,                      // direction (vx^2+vy^2+vz^2) = 1
		        _sx, _sy, _sz,                      // non-photon polarization
		        _sPolAx, _sPolAy, _sPolAz, _sPolPh, // photon s-polarization amplitude
		        _pPolAx, _pPolAy, _pPolAz, _pPolPh, // photon p-polarization amplitude
		        _wavelength,                        // wavelength
		        _time, _weight;                     // ray time, weight

		Record<unsigned long long int> _id; // id
		Record<int> _status;                // alive status

		//------------------------------ private memebers
	private:
		size_t _size; // number of stored rays
		size_t _read; // current index when reading

		//------------------------------ public methods
	public:
		/// \brief default constructor
		Rays();

		/** \brief append a new ray
		 * \param[in] this_ray : a ray object
		 */
		void push(const Ray& this_ray);

		/** \brief pop first ray
		 * \param[in] next : if true, it returns the current ray and advance the counter by
		 * one if false, at the next pop() it will retrieve the same ray \return ray : a ray
		 * object
		 */
		Ray pop(bool next = false);
		/** \brief reset the container, removing all the rays */
		void clear(void) {
			_size = 0;
			_read = 0;

			_x.clear();
			_y.clear();
			_z.clear();

			_dx.clear();
			_dy.clear();
			_dz.clear();

			_sx.clear();
			_sy.clear();
			_sz.clear();

			_sPolAx.clear();
			_sPolAy.clear();
			_sPolAz.clear();
			_pPolAx.clear();
			_pPolAy.clear();
			_pPolAz.clear();

			_sPolPh.clear();
			_pPolPh.clear();

			_wavelength.clear();

			_time.clear();
			_weight.clear();

			_id.clear();
			_status.clear();
		};

		/** \brief returns the number of stored rays */
		size_t size() const { return _size; };

		/** \brief check if all the data have been already retrieved
		 * \return bool : true if all the data stored have been retrieved
		 * it return true also if it is empty
		 */
		bool is_chunk_finished(void) { return _read == _size; }

		// clang-format off
/** \name 1D vector quantities of stored rays
| Variable    | Comment                        | Units                    |
| ---------   | ----------------               | ------------------------ |
| x,y,z       | position in                    | [cm]                     |
| dx,dy,dz    | direction                      | (normalized velocity)    |
| sx,sy,sz    | polarization of non-photons    |                          |
| sPolAx,sPolAy,sPolAz | s-polarization amplitude of photons      |                          |
| pPolAx,pPolAy,pPolAz | p-polarization amplitude of photons      |                          |
| sPolx,sPoly,sPolz | s-polarization amplitude of photons      |                          |
| pPolx,pPoly,pPolz | p-polarization amplitude of photons      |                          |
| time        | ray time w.r.t. ray generation | [ms]                     |
| wavelength  |                                | [Ang]                    |
| weight      | weight                         |                          |
*/
		// clang-format on

		///@}
	};

public:
	/**\brief constructor
	 *
	 * At construction, no file is created. The init_write() or init_read() methods should be
	 * called to performed the desired operations.
	 *
	 */
	explicit openPMD_io(
	        const std::string& filename, ///< filename
	        //	        openPMD::Access read_mode,        ///< file access mode
	        //(read/write/append)
	        const std::string mc_code_name    = "", ///< [optional] Name of the simulation code name
	        const std::string mc_code_version = "", ///< [optional] Simulation software version
	        const std::string instrument_name = "", ///< [optional] Name of the instrument
	        const std::string name_current_component =
	                "",    ///< [optional] current component name along the beamline
	        int repeat = 1 ///< [optional] Number of times a ray should be repeatedly retrieved
	);

	/***************************************************************/
	/// \name Writing mode
	///@{
	/** \brief initializes the "series" object from the openPMD API in WRITE MODE
	 *
	 * It is extremely important to set the n_rays as the maximum number of rays to save in the
	 * file. If it is not known in advance, the user should guess it. The file size increases
	 * according to this value, so it should be kept the lowest possible.
	 *
	 * This method calls init_rays() a first time.
	 */
	void init_write(std::string pdgId,             ///< PDG ID of the particles
	                unsigned long long int n_rays, ///< number of rays being simulated (max)
	                openPMD_output_format_t output_format=AUTO, ///< output format
	                unsigned int iter = 1 ///< openPMD iteration, always using the default value

	);

	/** \brief declare the ray particle species in the file
	 *
	 * This function must be called each time the particle species are changed and for each new
	 * iteration.
	 *
	 **/
	void
	init_rays(std::string particle_species, unsigned long long int n_rays, unsigned int iter);

	/// \brief save ray properties
	void trace_write(Ray this_ray);

	/** \brief Flushes the output to file before closing it
	 * It writes the current particle species of the current iteration.
	 *
	 **/
	void save_write(void);
	///@}

	/***************************************************************/
	/// \name Reading mode
	///@{
	/// \brief initializes the "series" object from the openPMD API in READ MODE
	unsigned long long int
	init_read(openPMD_output_format_t output_format, ///< output format
	          unsigned long long int n_rays =
	                  0,            ///< NOT USED YET 0 = all \todo to fix the maximum
	                                ///< number of rays one wants to read from the file
	          unsigned int iter = 1 ///< openPMD iteration, always using the default value
	);

	/** \brief read one ray from file
	 *
	 * \returns Ray object
	 */
	Ray trace_read(void);
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
	//	openPMD::Access _access_mode;
	openPMD::Offset _offset;
	bool _isWriteMode;
	std::unique_ptr<openPMD::Series> _series;
	Rays _rays;
	unsigned int _iter;
	std::string _particle_species;

	static const std::map<openPMD_output_format_t, std::string> output_format_names;

	//------------------------------ set of helper methods
	inline openPMD::Iteration& iter_pmd(unsigned int iter) { return _series->iterations[iter]; }

	// returns the current particle species from the current iteration
	inline openPMD::ParticleSpecies& rays_pmd(void) {
		auto i = iter_pmd(_iter);
		return i.particles[_particle_species];
	}

	// returns the given particle species from the current iteration
	inline openPMD::ParticleSpecies& rays_pmd(std::string particle_species) {
		_particle_species = particle_species;
		auto i            = iter_pmd(_iter);
		return i.particles[particle_species];
	}

	/** \brief Sets a new Record for the current particles of the current iteration
	 * \param[in] isScalar : bool indicating if it is scalar
	 * \param[in] dims : Unit dimensions
	 * \param[in]
	 */
	void init_ray_prop(std::string name,          ///< name : name of the field/property
	                   openPMD::Dataset& dataset, ///< dataset definition
	                   bool isScalar,             ///< true if it is a scalar
	                   std::map<openPMD::UnitDimension, double> const& dims =
	                           {{openPMD::UnitDimension::L, 0.}}, ///< dimensions
	                   double unitSI = 0.);                       ///< scale w.r.t. SI
};
} // namespace raytracing
#endif
