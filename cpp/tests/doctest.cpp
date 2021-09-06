#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <openPMD_io.hh>
using namespace raytracing;

#include <doctest/doctest.h>
TEST_CASE("[Ray] Read-Write") {
	Ray myray;
	CHECK(myray.get_status() == kAlive);
	myray.set_position(1e-5, 2e-4, 2e2, 0.5);
	CHECK(myray.x() == doctest::Approx(5e-6));
	CHECK(myray.y() == doctest::Approx(1e-4));
	CHECK(myray.z() == doctest::Approx(1e2));

	float x,y,z;
	myray.get_position(&x, &y, &z);
	CHECK(x == doctest::Approx(5e-6));
	x =y =z =0;
	myray.get_position(x, y, z);

	///\todo add more checks for the raytracing::Ray class
}

TEST_CASE("[openPMD_io] Write") {

	std::string filename = "test_file.";
	SUBCASE("JSON") { filename += "json"; };
	SUBCASE("HDF5") { filename += "h5"; };

	raytracing::openPMD_io iol(filename, "test code");
	unsigned long long int n_rays_max = 11;
	unsigned int iter                 = 2;
	iol.init_write("2112", n_rays_max, raytracing::AUTO, iter);
	std::cout << "filename = " << filename << std::endl;

	iol.set_gravity_direction(0.33, 0.33, 0.33);
	raytracing::Ray myray;
	myray.set_position(1, 2, 3);
	//...
	myray.set_direction(1, 1, 1, 1. / sqrt(3));

	// append
	for (size_t i = 0; i < n_rays_max; ++i) { // don't write less than those set during init
		myray.set_position(i, i, i);
		iol.trace_write(myray);
	}

	std::cout << "trace write done" << std::endl;
	iol.save_write();
	std::cout << "save write done" << std::endl;

	unsigned int nrepeat = 3;
	auto nrays           = iol.init_read("2112", iter, 2, nrepeat);
	float x = 3, y = 5, z = 7;
	iol.get_gravity_direction(&x, &y, &z);
	std::cout << x << "\t" << y << "\t" << z << std::endl;
	CHECK(x == doctest::Approx(0.33));
	CHECK(y == doctest::Approx(0.33));
	CHECK(z == doctest::Approx(0.33));

	for (unsigned int i = 0; i < nrays * nrepeat; ++i) {
		std::cout << i << "/" << nrays << "\t" << iol.trace_read();
	}
}
