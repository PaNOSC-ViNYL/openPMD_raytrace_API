#ifndef RAY_CLASS_HH
#define RAY_CLASS_HH

#include <cmath>
/** \class Ray
 * \brief Generic ray, containing all the ray information being stored by the API into the openPMD
 * file
 *
 * This is base class provides several methods to set and manipulate the ray information in the
 * appropriate way Other specific ray classes should inherit from this in order to provide more
 * convinient methods for the user. Derived classes can be created from this and used by the users
 * in their programs
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
	float _sPolarization[DIM];
	float _pPolarization[DIM];
	float _time;
	float _weight;

public:
	Ray();

	float x() const { return _position[X]; };
	float y() const { return _position[Y]; };
	float z() const { return _position[Z]; };

	float dx() const { return _direction[X]; };
	float dy() const { return _direction[Y]; };
	float dz() const { return _direction[Z]; };

	
	/// \brief scale and set the position
	void position(double x, double y, double z, double scale) {
		_position[X] = x * scale;
		_position[Y] = y * scale;
		_position[Z] = z * scale;
	}

	/// \brief scale and set the direction
	void
	direction(double x, double y, double z, double scale) {
		_direction[X] = x * scale;
		_direction[Y] = y * scale;
		_direction[Z] = z * scale;
	}

	/// \brief scale and set the direction from the velocity
	void
	velocity(double x, double y, double z) {
		double abs_v = sqrt(x * x + y * y + z * z);
		direction(x, y, z, 1. / abs_v);
	}

	/// \name To implement
	///@{ \todo to implement
	void polarization(double x, double y, double z, double scale);
	void sPolarization(double x, double y, double z, double scale);
	void pPolarization(double x, double y, double z, double scale);
	void time(double t);
	void weigth(double w);
	/// @}
};

/** \class photon
 * \brief helper class for photons
 */
class photon : public Ray {};

/** \class neutron
 *  \brief helper class for neutrons
 */
class neutron : public Ray {};

#endif
