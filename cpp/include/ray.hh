#ifndef RAY_CLASS_HH
#define RAY_CLASS_HH
//#include <doctest/doctest.h>

#include <cmath>
#include <ostream>

namespace raytracing {
// enum particleStatus_t : int { kDead = 0, kAlive = 1 };

/** \typedef particleStatus_t
 * particle statues are defined by the openPMD extension as integers with
 *  - 1: active/alive particle
 *  - other: any other value is used for "dead" particles, the effective value and meaining
 * depends on the simulation software used
 *
 * In the ray tracing extension only raytracing::kDead and raytracing::kAlive values are used
 */
typedef int particleStatus_t;
constexpr int kDead  = 0; ///<  particleStatus_t: dead ray
constexpr int kAlive = 1; ///< alive ray

/** \class Ray
 * \brief Generic ray, containing all the ray information being stored by the API into the openPMD
 * file
 *
 * This is base class provides several methods to set and manipulate the ray information in the
 * appropriate way.
 *
 * Other specific ray classes should inherit from this in order to provide more
 * convenient methods for the user. Derived classes can be created from this and used by the users
 * in their programs.
 *
 * Unit conversions should be applied here, so that the Rays class stores information in a coherent way.
 * There is a small inefficiency if rays are used multiple times, but the time spent in unit conversions
 * is anyway negligible w.r.t. the simulation time.
 */
class Ray {
public:
	static const size_t DIM = 3;
	static const size_t X   = 0;
	static const size_t Y   = 1;
	static const size_t Z   = 2;
	static const size_t PHASEDIM = DIM + 1;
	static const size_t PHASE = 3;

private:
	float _position[DIM];
	float _direction[DIM];
	float _polarization[DIM]; // for non-photons
	float _sPolarization[PHASEDIM];
	float _pPolarization[PHASEDIM];
	float _wavelength;
	float _time;
	float _weight;
	unsigned long long int _id;
	//particleStatus_t _status;
	particleStatus_t _status;

public:
	Ray():
	    _position(),
	    _direction(),
	    _polarization(),
	    _sPolarization(),
	    _pPolarization(),
	    _wavelength(0),
	    _time(0),
	    _weight(1),
	    _id(0),
	    _status(kAlive){};

	/// \name Getters
	///@{

	/// \name Get position
	///@{
	float x() const { return _position[X]; };
	float y() const { return _position[Y]; };
	float z() const { return _position[Z]; };
	void get_position(float* xx, float* yy, float* zz) const {
		(*xx) = x();
		(*yy) = y();
		(*zz) = z();
	}
	//@}

	/// \name Get direction
	///@{
	float dx() const { return _direction[X]; };
	float dy() const { return _direction[Y]; };
	float dz() const { return _direction[Z]; };
	void get_direction(float* x, float* y, float* z) const {
		*x = dx();
		*y = dy();
		*z = dz();
	}
	///@}

	/// \name Get polarization for non-photons
	///@{
	float sx() const { return _polarization[X]; };
	float sy() const { return _polarization[Y]; };
	float sz() const { return _polarization[Z]; };
	void get_polarization(float* x, float* y, float* z) const {
		*x = sx();
		*y = sy();
		*z = sz();
	}
	///@}

	/// \name Get s-polarization amplituded and phase for photons
	///@{
	float sPolAx() const { return _sPolarization[X]; };
	float sPolAy() const { return _sPolarization[Y]; };
	float sPolAz() const { return _sPolarization[Z]; };
	float sPolPh() const { return _sPolarization[PHASE]; };
	void get_sPolarizationAmplitude(float* x, float* y, float* z) const {
		*x = sPolAx();
		*y = sPolAy();
		*z = sPolAz();
	}
	void get_sPolarization(float* x, float* y, float* z, float* phase) const {
		get_sPolarizationAmplitude(x,y,z);
		*phase = sPolPh();
	}

	///@}
	/// \name Get p-polarization amplitude and phase for photons
	float pPolAx() const { return _pPolarization[X]; };
	float pPolAy() const { return _pPolarization[Y]; };
	float pPolAz() const { return _pPolarization[Z]; };
	float pPolPh() const { return _pPolarization[PHASE]; };
	void get_pPolarizationAmplitude(float* x, float* y, float* z) const {
		*x = pPolAx();
		*y = pPolAy();
		*z = pPolAz();
	}

	void get_pPolarization(float* x, float* y, float* z, float* phase) const {
		get_pPolarizationAmplitude(x,y,z);
		*phase = pPolPh();
	}

	///@}

	/** \name Get wavelength, ray time, weight, id and status
	 * Allowed status values are raytracing::kAlive and raytracing::kDead
	 */
	///@{
	float wavelength() const { return _wavelength; };
	inline float get_wavelength() const { return wavelength(); };
	float time() const { return _time; };
	inline float get_time() const { return time(); };
	float weight() const { return _weight; };
	inline float get_weight() const { return weight(); };
	unsigned long long int id(void) const { return _id; };
	inline unsigned long long int get_id(void) const { return id(); };
	/** \brief return the particle status
	 * \return raytracing::kAlive or raytracing::kDead
	 */
	particleStatus_t status(void) const { return _status; };
	inline particleStatus_t get_status(void) const { return status(); };

	///@}
	///@}

	/// \name Setters
	/// @{

	/// \brief scale and set the position
	void position(double x, double y, double z, double scale = 1) {
		_position[X] = x * scale;
		_position[Y] = y * scale;
		_position[Z] = z * scale;
	}

	/// \brief scale and set the direction
	void direction(double x, double y, double z, double scale = 1) {
		_direction[X] = x * scale;
		_direction[Y] = y * scale;
		_direction[Z] = z * scale;
	}

	/// \brief scale and set the direction from the velocity
	void velocity(double x, double y, double z) {
		double abs_v = sqrt(x * x + y * y + z * z);
		direction(x, y, z, 1. / abs_v);
	}

	/// \brief scale and set polarization for non-photons  \todo to implement
	void polarization(double x, double y, double z, double scale = 1) {
		_polarization[X] = x * scale;
		_polarization[Y] = y * scale;
		_polarization[Z] = z * scale;
	}
	/// \brief scale and set s-polarization for photons
	void sPolarization(double x, double y, double z, double phase, double scale = 1){
		_sPolarization[X] = x * scale;
		_sPolarization[Y] = y * scale;
		_sPolarization[Z] = z * scale;
		_sPolarization[PHASE] = phase;
	};
	/// \brief scale and set p-polarization for photons  \todo to implement
	void pPolarization(double x, double y, double z, double phase, double scale = 1){
		_pPolarization[X] = x * scale;
		_pPolarization[Y] = y * scale;
		_pPolarization[Z] = z * scale;
		_pPolarization[PHASE] = phase;
	};
	/// \brief set wavelength
	void wavelength(double w) { _wavelength = w; };
	inline void set_wavelength(double w) { wavelength(w); }; // for pybind overloading
	
	/// \brief set time
	void time(double t) { _time = t; };
	inline void set_time(double t) { time(t); }; // for pybind overloading
	/// \brief set weight
	void weight(double w) { _weight = w; };
	inline void set_weight(double w) { weight(w); }; // for pybind overloading

	void id(unsigned long long int idv) { _id = idv; };
	inline void set_id(unsigned long long int idv) { id(idv); }; // for pybind overloading

	void status(particleStatus_t s) { _status = s; };
	inline void set_status(particleStatus_t s) { status(s); }; // for pybind overloading

	///@}
public:
	friend std::ostream& operator<<(std::ostream& os, const Ray& ray);
};

	
#ifdef DD
	
TEST_CASE("") {
	Ray myray;
	CHECK(myray.get_status() == kAlive);
	myray.position(1e-5, 2e-4, 2e2, 0.5);
	CHECK(myray.x() == 0.5e-5);
}

/** \class photon
 * \brief helper class for photons
 */
class photon : public Ray {};

/** \class neutron
 *  \brief helper class for neutrons
 */
class neutron : public Ray {};

#endif
class mcstas_neutron : public Ray {
public:
	void position(double x, double y, double z){
		Ray::position(x, y, z, 1e2);
	};
};

} // namespace raytracing

#endif

