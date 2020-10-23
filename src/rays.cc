#include "rays.hh"
#include <array>
#include <cmath>
#define DEBUG
#ifdef DEBUG
#include <iostream> // for debug
#include <cassert>
#endif
///\file
#ifndef V2SE
#define VS2E 5.22703725e-6 /* Convert (v[m/s])**2 to E[meV] */
#endif

Rays::Rays(): _size(0), _read(0) {}

//------------------------------
void
Rays::push(Ray this_ray) {
	_x.push_back(this_ray.x());
	_y.push_back(this_ray.y());
	_z.push_back(this_ray.z());

	_dx.push_back(this_ray.dx());
	_dy.push_back(this_ray.dy());
	_dz.push_back(this_ray.dz());

	/// \todo implement the rest
};

//------------------------------
Ray
Rays::pop(bool next) {
	/// \todo implement the rest

	Ray r;
	r.position(_x[_read], _y[_read], _z[_read]);
	r.direction(_dx[_read], _dy[_read], _dz[_read]);

	r.polarization(_sx[_read], _sy[_read], _sz[_read]);

	r.time(_time[_read]);
	r.weight(_weight[_read]);

	if (next) ++_read;
	return r;
}

#ifdef SHERVIN
void
Rays::push_back_nonphoton(double x, double y, double z,    //
                          double dx, double dy, double dz, //
                          double sx, double sy, double sz, //
                          double t, double p) {
	double abs_v = sqrt(dx * dx + dy * dy + dz * dz);
	double ekin  = VS2E * abs_v * abs_v / 1e9;

	_x.push_back(x); // cm
	_y.push_back(y);
	_z.push_back(z);

	_sx.push_back(sx);
	_sy.push_back(sy);
	_sz.push_back(sz);

	_dx.push_back(dx / abs_v);
	_dy.push_back(dy / abs_v);
	_dz.push_back(dz / abs_v);

	_time.push_back(t * 1e3);

	_weight.push_back(p);

	_ekin.push_back(ekin);
	//  _userflag.push_back(userflag);
	++_size;
}

void
Rays::store(double x, double y, double z,    //
            double sx, double sy, double sz, //
            double dx, double dy, double dz, //
            double time, double weight, double ekin) {

	_x.push_back(x);
	_y.push_back(y);
	_z.push_back(z);

	_sx.push_back(sx);
	_sy.push_back(sy);
	_sz.push_back(sz);

	_vx.push_back(dx);
	_vy.push_back(dy);
	_vz.push_back(dz);

	_time.push_back(time);

	_weight.push_back(weight);

	_ekin.push_back(ekin);

	/*
    // The data is bonkers, z is usually largest as it should be, but completely wrong orders of
    magnitude std::cout << "x,y,z:" << x << "," << y << "," << z << std::endl; std::cout <<
    "vx,vy,vz:" << dx << "," << dy << "," << dz << std::endl;
    */

	++_size;
}

/* The unit conversion is done here. This way the content of the object is consistent
 * if push_back() is used or if store() is used. This leads to a small
 * inefficiency when re-using the same neutrons multiple times, since
 * the conversion has to be repeated for the same neutrons. It is
 * usually a small performance loss compared to the total time of a
 * typical simulation, so consistency has been preferred over
 * performance in this case.
 */
void
Rays::retrieve(double* x, double* y, double* z,    //
               double* sx, double* sy, double* sz, //
               double* vx, double* vy, double* vz, //
               double* t, double* p) {             //, uint32_t userflag) {

	assert(_read <= _size);
	// Convert position from cm to m
	*x = _x[_read] / 100;
	*y = _y[_read] / 100;
	*z = _z[_read] / 100;

	// Polarization is in the correct format (not written by write component yet)
	*sx = _sx[_read];
	*sy = _sy[_read];
	*sz = _sz[_read];

	// Calculate velocity from energy
	double abs_v = sqrt(_ekin[_read] * 1e9 / VS2E);

	// Use magnitude of velocity to convert direction to velocity
	*vx = _vx[_read] * abs_v;
	*vy = _vy[_read] * abs_v;
	*vz = _vz[_read] * abs_v;

	// Time from ms to s
	*t = _time[_read] * 1e-3;

	// Weight is in the correct format
	*p = _weight[_read];

	//	++_read;

	//	if (_read >= _size)
	//	_read = 0; // Start over if entire dataset is read
}
#endif
