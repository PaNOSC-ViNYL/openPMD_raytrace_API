#ifndef RAY_CLASS_HH
#define RAY_CLASS_HH

#include <cmath>

namespace raytracing {
enum particleStatus_t { kDead = 0, kAlive = 1 };

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

private:
	float _position[DIM];
	float _direction[DIM];
	float _polarization[DIM]; // for non-photons
	float _sPolarizationAmpl[DIM];
	float _sPolarizationPhase[DIM];
	float _pPolarizationAmpl[DIM];
	float _pPolarizationPhase[DIM];
	float _wavelength;
	float _time;
	float _weight;
	unsigned long long int _id;
	particleStatus_t _status;

public:
	Ray():
	    _position(),
	    _direction(),
	    _polarization(),
	    _sPolarizationAmpl(),
	    _sPolarizationPhase(),
	    _pPolarizationAmpl(),
	    _pPolarizationPhase(),
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
	///@}

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
	float sPolAx() const { return _sPolarizationAmpl[X]; };
	float sPolAy() const { return _sPolarizationAmpl[Y]; };
	float sPolAz() const { return _sPolarizationAmpl[Z]; };
	float sPolPhx() const { return _sPolarizationPhase[X]; };
	float sPolPhy() const { return _sPolarizationPhase[Y]; };
	float sPolPhz() const { return _sPolarizationPhase[Z]; };
	///@}
	/// \name Get p-polarization amplitude and phase for photons
	float pPolAx() const { return _pPolarizationAmpl[X]; };
	float pPolAy() const { return _pPolarizationAmpl[Y]; };
	float pPolAz() const { return _pPolarizationAmpl[Z]; };
	float pPolPhx() const { return _pPolarizationPhase[X]; };
	float pPolPhy() const { return _pPolarizationPhase[Y]; };
	float pPolPhz() const { return _pPolarizationPhase[Z]; };

	void get_sPolarizationAmplitude(float* x, float* y, float* z) const {
		*x = sPolAx();
		*y = sPolAy();
		*z = sPolAz();
	}
	void get_pPolarizationAmplitude(float* x, float* y, float* z) const {
		*x = pPolAx();
		*y = pPolAy();
		*z = pPolAz();
	}

	void get_sPolarizationPhase(float* x, float* y, float* z) const {
		*x = sPolPhx();
		*y = sPolPhy();
		*z = sPolPhz();
	}
	void get_pPolarizationPhase(float* x, float* y, float* z) const {
		*x = pPolPhx();
		*y = pPolPhy();
		*z = pPolPhz();
	}

	///@}

	/// \name Get wavelength, ray time, weight, id
	///@{
	float wavelength() const { return _wavelength; };
	float time() const { return _time; };
	float weight() const { return _weight; };
	unsigned long long int id(void) const { return _id; };
	particleStatus_t status(void) const { return _status; };
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
	void sPolarization(double x, double y, double z, double scale = 1);
	/// \brief scale and set p-polarization for photons  \todo to implement
	void pPolarization(double x, double y, double z, double scale = 1);
	/// \brief set wavelength
	void wavelength(double w) { _wavelength = w; };
	/// \brief set time
	void time(double t) { _time = t; };
	/// \brief set weight
	void weight(double w) { _weight = w; };

	void id(unsigned long long int id) { _id = id; };

	void status(particleStatus_t s) { _status = s; };
	///@}
};

/** \class photon
 * \brief helper class for photons
 */
class photon : public Ray {};

/** \class neutron
 *  \brief helper class for neutrons
 */
class neutron : public Ray {};
} // namespace raytracing
#endif
