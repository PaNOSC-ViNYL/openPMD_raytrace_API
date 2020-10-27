#include <openPMD_io.hh>

int
main(int argc, char* argv[]) {

	std::string filename = "test_file";
	raytracing::openPMD_io iol(filename, "test code");

	
	iol.init_write(raytracing::JSON, "2112", 3, 1);

	raytracing::Ray myray;
	myray.position(1,2,3);
	//...
	myray.direction(1,1,1,1./sqrt(3));

	// append 
	iol.trace_write(myray);

	
	iol.save_write();
	return 0;
}
