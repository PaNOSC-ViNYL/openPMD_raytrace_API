#ifndef RAY_CLASS_HH
#define RAY_CLASS_HH
///\file
#include <vector>

/** \class rays
 * \brief stores the rays' properties
 *
 * \todo add a check that the non-photon polarization is not filled if storing photons
 * \todo check that polarizationAmplitudes are not filled for non-photons
 */

class rays {

private:
	// the 3d components are in separate vectors because this is the way the openPMD API wants
	// them to be
	std::vector<float> _x, _y, _z, // position
	        _dx, _dy, _dz,         // direction (vx^2+vy^2+vz^2) = 1
	        _sx, _sy, _sz,         // non-photon polarization
	        _wavelength,           // wavelength
	        _time, _weight,        // ray time, weight
	        _abs_v, _ekin;         // velocity, kinetic energy
	size_t _size;                  // number of stored neutrons
	size_t _read;                  // current index when reading

public:
	/// \brief default constructor
	rays();

	/** \brief append a new ray  (\b doing \b conversions)
	 * \param[in] x,y,z :  position          -> it is converted to cm ( * 100)
	 * \param[in] dx, dy, dz : direction
	 * \param[in] sx, sy, sz : non-photon polarization
	 * \param[in] t : ray time
	 * \param[in] p : weight
	 */
	void
	add_non_photon(double x, double y, double z,    // position
	               double dx, double dy, double dz, // direction
	               double sx, double sy, double sz, // polarization
	               double t, double p);             // ray time and weight

	/** \brief append a new photon ray
	 * \param[in] x,y,z : position
	 * \param[in] dx,dy,dz : direction
	 * \param[in] sPx, sPy, sPz : s-polarization
	 * \param[in] pPx, pPy, pPz : p-polarization
	 */
	void
	add_photon(double x, double y, double z,       // position
	           double dx, double dy, double dz,    // direction
	           double sPx, double sPy, double sPz, // s-polarization
	           double pPx, double pPy, double pPz, // p-polarization
	           double t, double p);                // ray time and weight

	void
	push_back(); // this can have all the possible arguments, for photons and
	             // non-photons

	/** \brief append a new ray reading from *openPMD data*
	 * \param[in] x,y,z : neutron position in cm
	 * \param[in] sx, sy, sz : neutron polarization (not working yet)
	 * \param[in] dx, dy, dz : neutron direction
	 * \param[in] ray time : time in ms
	 * \param[in] weight : weight
	 * \param[in] ekin : kinetic energy
	 */
	void
	store(double x, double y, double z,    //  position
	      double sx, double sy, double sz, //  polarization
	      double dx, double dy, double dz, //  velocity
	      double time, double weight,      //  ray time and weight
	      double ekin);                    //  kinetic energy

	/** \brief read a new neutron from openPMD data
	 * It increaments the reader count
	 * \param[out] x,y,z : neutron position in m
	 * \param[out] sx, sy, sz : neutron polarization (not working yet)
	 * \param[out] vx, vy, vz : neutron velocity in m/s
	 * \param[out] t : time in s
	 * \param[out] p : weight
	 */
	void
	retrieve_next(double* x, double* y, double* z,    // position
	              double* sx, double* sy, double* sz, // polarization
	              double* vx, double* vy, double* vz, // velocity
	              double* t, double* p) { // ray time and weight //, uint32_t userflag = 0);)
		retrieve(x, y, z, sx, sy, sz, vx, vy, vz, t, p);
		++_read;
	}

	void
	retrieve(double* x, double* y, double* z,    // position
	         double* sx, double* sy, double* sz, // polarization
	         double* vx, double* vy, double* vz, // velocity
	         double* t, double* p);              // ray time and weight //, uint32_t userflag = 0);)

	/** \brief reset the container, removing all the neutrons */
	void
	clear(void) {
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
		_time.clear();
		_ekin.clear();
		_weight.clear();
	};

	/** \brief returns the number of stored neutrons */
	size_t
	size() const {
		return _size;
	};

	/** \brief check if all the data have been already retrieved
	 * \return bool : true if all the data stored have been retrieved
	 * it return true also if it is empty
	 */
	bool
	is_chunk_finished(void) {
		return _read == _size;
	}

	// clang-format off
/** \name 1D vector quantities of stored neutrons
| Variable   | Comment                        | Units                    |
| ---------  | ----------------               | ------------------------ |
| x,y,z      | position in                    | [cm]                     |
| dx,dy,dz   | direction                      | (normalized velocity)    |
| sx,sy,sz   | polarization of non-photons    |                          |
| time       | ray time w.r.t. ray generation | [ms]                     |
| wavelength |                                | [Ang]                    |
| weight     | weight                         |                          |
*/
	// clang-format on

	const std::vector<float>&
	x(void) const {
		return _x;
	};
	const std::vector<float>&
	y(void) const {
		return _y;
	};
	const std::vector<float>&
	z(void) const {
		return _z;
	};
	const std::vector<float>&
	dx(void) const {
		return _dx;
	};
	const std::vector<float>&
	dy(void) const {
		return _dy;
	};
	const std::vector<float>&
	dz(void) const {
		return _dz;
	};
	const std::vector<float>&
	sx(void) const {
		return _sx;
	};
	const std::vector<float>&
	sy(void) const {
		return _sy;
	};
	const std::vector<float>&
	sz(void) const {
		return _sz;
	};
	const std::vector<float>&
	time(void) const {
		return _time;
	};
	const std::vector<float>&
	ekin(void) const {
		return _ekin;
	};
	const std::vector<float>&
	weight(void) const {
		return _weight;
	};
	///@}
};

#endif
