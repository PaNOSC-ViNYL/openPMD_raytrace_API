//#include "rays.hh"
#include "openPMD_io.hh"
#include <array>
#include <cmath>
///\file

using raytracing::Ray;
using raytracing::openPMD_io;
//using raytracing::openPMD_io::Rays;
#ifndef V2SE
#define VS2E 5.22703725e-6 /* Convert (v[m/s])**2 to E[meV] */
#endif

openPMD_io::Rays::Rays(): _size(0), _read(0) {}

//------------------------------
void
openPMD_io::Rays::push(const Ray& this_ray) {
	_x.push_back(this_ray.x());
	_y.push_back(this_ray.y());
	_z.push_back(this_ray.z());

	_dx.push_back(this_ray.dx());
	_dy.push_back(this_ray.dy());
	_dz.push_back(this_ray.dz());

	_sx.push_back(this_ray.sx());
	_sy.push_back(this_ray.sy());
	_sz.push_back(this_ray.sz());

	_sPolAx.push_back(this_ray.sPolAx());
	_sPolAy.push_back(this_ray.sPolAy());
	_sPolAz.push_back(this_ray.sPolAz());
	_sPolPh.push_back(this_ray.sPolPh());

	_pPolAx.push_back(this_ray.pPolAx());
	_pPolAy.push_back(this_ray.pPolAy());
	_pPolAz.push_back(this_ray.pPolAz());
	_pPolPh.push_back(this_ray.pPolPh());

	_wavelength.push_back(this_ray.get_wavelength());
	_time.push_back(this_ray.get_time());
	_weight.push_back(this_ray.get_weight());

	_id.push_back(this_ray.get_id());
	_status.push_back(this_ray.get_status());
	++_size;
};

//------------------------------
Ray
openPMD_io::Rays::pop(bool next) {

	Ray r;
	r.set_position(_x[_read], _y[_read], _z[_read]);
	r.set_direction(_dx[_read], _dy[_read], _dz[_read]);

	r.set_polarization(_sx[_read], _sy[_read], _sz[_read]);

	r.set_sPolarization(_sPolAx[_read], _sPolAy[_read], _sPolAz[_read], _sPolPh[_read]);
	r.set_pPolarization(_pPolAx[_read], _pPolAy[_read], _pPolAz[_read], _pPolPh[_read]);

	r.set_wavelength(_wavelength[_read]);
	r.set_time(_time[_read]);
	r.set_weight(_weight[_read]);

	r.set_id(_id[_read]);
	r.set_status(_status[_read]);
	
	if (next) ++_read;
	return r;
}

#ifdef SHERVIN
//------------------------------
void
openPMD_io::Rays::store(float x, float y, float z,                    // position
            float dx, float dy, float dz,                 // direction
            float sx, float sy, float sz,                 // polarization
            float sPolx, float sPoly, float sPolz,        // s-polarization
            float pPolx, float pPoly, float pPolz,        // p-polarization
            float wavelength, float time, float weight) { // ray wavelength, time and weight

	_x.push_back(x);
	_y.push_back(y);
	_z.push_back(z);

	_dx.push_back(dx);
	_dy.push_back(dy);
	_dz.push_back(dz);

	_sx.push_back(sx);
	_sy.push_back(sy);
	_sz.push_back(sz);

	_sPolx.push_back(sPolx);
	_sPoly.push_back(sPoly);
	_sPolz.push_back(sPolz);

	_pPolx.push_back(pPolx);
	_pPoly.push_back(pPoly);
	_pPolz.push_back(pPolz);

	_wavelength.push_back(wavelength);
	_time.push_back(time);
	_weight.push_back(weight);

	/*
	// The data is bonkers, z is usually largest as it should be, but completely wrong orders of
	magnitude std::cout << "x,y,z:" << x << "," << y << "," << z << std::endl; std::cout <<
	"vx,vy,vz:" << dx << "," << dy << "," << dz << std::endl;
*/

	++_size;
}

#endif
#ifdef SHERVIN
void
openPMD_io::Rays::push_back_nonphoton(double x, double y, double z,    //
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

/* The unit conversion is done here. This way the content of the object is consistent
 * if push_back() is used or if store() is used. This leads to a small
 * inefficiency when re-using the same neutrons multiple times, since
 * the conversion has to be repeated for the same neutrons. It is
 * usually a small performance loss compared to the total time of a
 * typical simulation, so consistency has been preferred over
 * performance in this case.
 */
void
openPMD_io::Rays::retrieve(double* x, double* y, double* z,    //
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
