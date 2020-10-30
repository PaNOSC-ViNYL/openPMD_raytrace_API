#ifndef RAYS_CLASS_HH
#define RAYS_CLASS_HH
///\file

// #include "openpmd_output_formats.h" // enum with the available openPMD backends
#include <vector>
#include <utility>
#include <limits>
#include "ray.hh"
namespace raytracing {
/** \class Rays
 * \brief stores the rays' properties
 *
 * This class is meant to keep in memory the rays' quantities before effectively writing them on
 * disk by the store_chunk method, or to load in memory the rays' quantities after reading the
 * openPMD file from disk with the load_chunk method
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
template <typename T> class Record {
	std::vector<T> _vals;
	T _min, _max;

public:
	Record():
		_vals(), _min(), _max(){
		std::numeric_limits<T> lim;
		_max =lim.min();
		_min = lim.max();
	}
	const std::vector<T>& vals(void) const { return _vals; };
	T min(void) const { return _min; };
	T max(void) const { return _max; };
	void push_back(T val) {
		_vals.push_back(val);
		if (_min > val) (_min) = val;
		if (_max < val) (_max) = val;
	}
	void clear(void) {
		_vals.clear();
		_min = 0;
		_max = 0;
	}
	const T operator[](size_t i) const { return _vals[i];}; //cannot modify
};

class Rays {

private:
	// the 3d components are in separate vectors because this is the way the openPMD API
	// wants them to be
	std::vector<float> _x, _y, _z,           // position
	        _dx, _dy, _dz,                   // direction (vx^2+vy^2+vz^2) = 1
	        _sx, _sy, _sz,                   // non-photon polarization
	        _sPolx, _sPoly, _sPolz,          // photon s-polarization amplitude
	        _pPolx, _pPoly, _pPolz,          // photon p-polarization amplitude
	        _sPolPhx, _sPolPhy, _sPolPhz,    // photon s-polarization Phase
	        _pPolPhx, _pPolPhy, _pPolPhz,    // photon p-polarization Phase
	        _wavelength,                     // wavelength
	        _time; //, _weight;                  // ray time, weight

	Record<float> _weight;

	float _xmin, _xmax, _ymin, _ymax, _zmin, _zmax,                                       //
	        _dxmin, _dxmax, _dymin, _dymax, _dzmin, _dzmax,                               //
	        _sxmin, _sxmax, _symin, _symax, _szmin, _szmax,                               //
	        _sPolxmin, _sPolxmax, _sPolymin, _sPolymax, _sPolzmin, _sPolzmax,             //
	        _pPolxmin, _pPolxmax, _pPolymin, _pPolymax, _pPolzmin, _pPolzmax,             //
	        _sPolPhxmin, _sPolPhxmax, _sPolPhymin, _sPolPhymax, _sPolPhdmin, _sPolPhzmax, //
	        _pPolPhxmin, _pPolPhxmax, _pPolPhymin, _pPolPhymax, _pPolPhdmin, _pPolPhzmax, //
	        _wavelengthmin, _wavelengthmax,                                               //
	        _timemin, _timemax,                                                           //
	        _weightmin, _weightmax;

	std::vector<unsigned long long int> _id; // id
	std::vector<particleStatus_t> _status;   // alive status
	size_t _size;                            // number of stored rays
	size_t _read;                            // current index when reading

public:
	/// \brief default constructor
	Rays();

	/** \brief append a new ray
	 * \param[in] this_ray : a ray object
	 */
	void push(const Ray& this_ray);
	//_rays.push_back(this_ray); };

	/** \brief pop first ray
	 * \param[in] next : if true, it returns the current ray and advance the counter by one
	 *     if false, at the next pop() it will retrieve the same ray
	 * \return ray : a ray object
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

		_sPolx.clear();
		_sPoly.clear();
		_sPolz.clear();
		_pPolx.clear();
		_pPoly.clear();
		_pPolz.clear();

		_sPolPhx.clear();
		_sPolPhy.clear();
		_sPolPhz.clear();
		_pPolPhx.clear();
		_pPolPhy.clear();
		_pPolPhz.clear();

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
| sPolx,sPoly,sPolz | s-polarization amplitude of photons      |                          |
| pPolx,pPoly,pPolz | p-polarization amplitude of photons      |                          |
| sPolx,sPoly,sPolz | s-polarization amplitude of photons      |                          |
| pPolx,pPoly,pPolz | p-polarization amplitude of photons      |                          |
| time        | ray time w.r.t. ray generation | [ms]                     |
| wavelength  |                                | [Ang]                    |
| weight      | weight                         |                          |
*/
	// clang-format on
	const std::vector<float>& x(void) const { return _x; };
	float x_min() const { return _xmin; };
	float x_max() const { return _xmax; };
	const std::vector<float>& y(void) const { return _y; };
	float y_min() const { return _ymin; };
	float y_max() const { return _ymax; };
	const std::vector<float>& z(void) const { return _z; };
	float z_min() const { return _zmin; };
	float z_max() const { return _zmax; };
	const std::vector<float>& dx(void) const { return _dx; };
	float dx_min() const { return _dxmin; };
	float dx_max() const { return _dxmax; };
	const std::vector<float>& dy(void) const { return _dy; };
	float dy_min() const { return _dymin; };
	float dy_max() const { return _dymax; };
	const std::vector<float>& dz(void) const { return _dz; };
	float dz_min() const { return _dzmin; };
	float dz_max() const { return _dzmax; };
	const std::vector<float>& sx(void) const { return _sx; };
	float sx_min() const { return _sxmin; };
	float sx_max() const { return _sxmax; };
	const std::vector<float>& sy(void) const { return _sy; };
	float sy_min() const { return _symin; };
	float sy_max() const { return _symax; };
	const std::vector<float>& sz(void) const { return _sz; };
	float sz_min() const { return _szmin; };
	float sz_max() const { return _szmax; };
	const std::vector<float>& sPolx(void) const { return _sPolx; };
	float sPolx_min() const { return _sPolxmin; };
	float sPolx_max() const { return _sPolxmax; };
	const std::vector<float>& sPoly(void) const { return _sPoly; };
	float sPoly_min() const { return _sPolymin; };
	float sPoly_max() const { return _sPolymax; };
	const std::vector<float>& sPolz(void) const { return _sPolz; };
	float sPolz_min() const { return _sPolzmin; };
	float sPolz_max() const { return _sPolzmax; };
	const std::vector<float>& pPolx(void) const { return _pPolx; };
	float pPolx_min() const { return _pPolxmin; };
	float pPolx_max() const { return _pPolxmax; };
	const std::vector<float>& pPoly(void) const { return _pPoly; };
	float pPoly_min() const { return _pPolymin; };
	float pPoly_max() const { return _pPolymax; };
	const std::vector<float>& pPolz(void) const { return _pPolz; };
	float pPolz_min() const { return _pPolzmin; };
	float pPolz_max() const { return _pPolzmax; };
	const std::vector<float>& wavelength(void) const { return _wavelength; };
	float wavelength_min() const { return _wavelengthmin; };
	float wavelength_max() const { return _wavelengthmax; };
	auto time(void) const { return _time; };
        auto weight(void) const { return _weight.vals(); };

	/** \brief append a new ray to the internal memory reading from *openPMD data*
	 * \param[in] x,y,z :  position
	 * \param[in] dx, dy, dz : direction
	 * \param[in] sx, sy, sz : non-photon polarization
	 * \param[in] sPolx, sPoly, sPolz : s-polarization
	 * \param[in] pPolx, pPoly, pPolz : p-polarization
	 * \param[in] wavelength : ray wavelength
	 * \param[in] t : ray time
	 * \param[in] p : weight
	 */
	void store(float x, float y, float z,             // position
	           float dx, float dy, float dz,          // direction
	           float sx, float sy, float sz,          // polarization
	           float sPolx, float sPoly, float sPolz, // s-polarization
	           float pPolx, float pPoly, float pPolz, // p-polarization
	           float wavelength, float t, float p);   // ray wavelength, time and weight

	///@}

#ifdef SHERVIN
	/** \brief append a new ray to the internal memory
	 * \param[in] x,y,z :  position
	 * \param[in] dx, dy, dz : direction
	 * \param[in] sx, sy, sz : non-photon polarization
	 * \param[in] t : ray time
	 * \param[in] p : weight
	 */
	void push_back_nonphoton(double x, double y, double z,    // position
	                         double dx, double dy, double dz, // direction
	                         double sx, double sy, double sz, // polarization
	                         double t, double p);             // ray time and weight

	/** \brief append a new photon ray to the internal memory
	 * \param[in] x,y,z : position
	 * \param[in] dx,dy,dz : direction
	 * \param[in] sPx, sPy, sPz : s-polarization
	 * \param[in] pPx, pPy, pPz : p-polarization
	 * \param[in] t : ray time
	 * \param[in] p : weight
	 */
	void push_back_photon(double x, double y, double z,       // position
	                      double dx, double dy, double dz,    // direction
	                      double sPx, double sPy, double sPz, // s-polarization
	                      double pPx, double pPy, double pPz, // p-polarization
	                      double t, double p);                // ray time and weight

	/** \brief append a new ray to the internal memory reading from *openPMD data*
	 * \param[in] x,y,z :  position
	 * \param[in] dx, dy, dz : direction
	 * \param[in] sx, sy, sz : non-photon polarization
	 * \param[in] t : ray time
	 * \param[in] p : weight
	 */
	void store_nonphoton(double x, double y, double z,    // position
	                     double dx, double dy, double dz, // direction
	                     double sx, double sy, double sz, // polarization
	                     double t, double p);             // ray time and weight

	/** \brief append a new ray reading from *openPMD data*
	 * \param[in] x,y,z : position
	 * \param[in] dx,dy,dz : direction
	 * \param[in] sPx, sPy, sPz : s-polarization
	 * \param[in] pPx, pPy, pPz : p-polarization
	 * \param[in] t : ray time
	 * \param[in] p : weight
	 */
	void store_photon(double x, double y, double z,       // position
	                  double dx, double dy, double dz,    // direction
	                  double sPx, double sPy, double sPz, // s-polarization
	                  double pPx, double pPy, double pPz, // p-polarization
	                  double t, double p);                // ray time and weight

	/** \brief read a new ray from openPMD data
	 * It increaments the reader count
	 * \param[out] x,y,z : neutron position
	 * \param[out] sx, sy, sz : neutron polarization \todo polarization not working yet
	 * \param[out] vx, vy, vz : neutron velocity
	 * \param[out] t : time
	 * \param[out] p : weight
	 */
	void retrieve_next(double* x, double* y, double* z,    // position
	                   double* sx, double* sy, double* sz, // polarization
	                   double* vx, double* vy, double* vz, // velocity
	                   double* t, double* p) { // ray time and weight //, uint32_t userflag = 0);)
		retrieve(x, y, z, sx, sy, sz, vx, vy, vz, t, p);
		++_read;
	}

	void retrieve(double* x, double* y, double* z,    // position
	              double* sx, double* sy, double* sz, // polarization
	              double* vx, double* vy, double* vz, // velocity
	              double* t, double* p); // ray time and weight //, uint32_t userflag = 0);)

#endif
};
} // namespace raytracing
#endif
