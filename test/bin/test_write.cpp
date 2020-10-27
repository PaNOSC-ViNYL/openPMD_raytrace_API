#include <openPMD_io.hh>
#include <openPMD/openPMD.hpp>
#include <cstdlib>
#include <numeric>
using std::cout;
using namespace openPMD;

int
main(int argc, char* argv[]) {
	// user input: size of matrix to write, default 3x3
	size_t size          = (argc == 2 ? atoi(argv[1]) : 3);
	std::string filename = "test_file";
	raytracing::openPMD_io iol(filename, "test code");
	//	iol.init_write(HDF5, 100);
	// matrix dataset to write with values 0...size*size-1
	std::vector<double> global_data(size * size);
	std::iota(global_data.begin(), global_data.end(), 0.);

	std::cout << "CIAO" << std::endl;

	namespace io = openPMD;
	auto series  = io::Series("myOutput/data.json", io::Access::CREATE);
	std::cout << "Created an empty " << series.iterationEncoding() << " Series\n";
	openPMD::MeshRecordComponent rho = series.iterations[1].meshes["rho"][openPMD::MeshRecordComponent::SCALAR];
	std::cout << "Created a scalar mesh Record with all required openPMD attributes\n";

	Datatype datatype = determineDatatype(shareRaw(global_data));
	Extent extent     = {size, size};
	Dataset dataset   = Dataset(datatype, extent);
	cout << "Created a Dataset of size " << dataset.extent[0] << 'x' << dataset.extent[1]
	     << " and Datatype " << dataset.dtype << '\n';

	rho.resetDataset(dataset);
	cout << "Set the dataset properties for the scalar field rho in iteration 1\n";

	series.flush();
	cout << "File structure and required attributes have been written\n";

	Offset offset = {0, 0};
	rho.storeChunk(shareRaw(global_data), offset, extent);
	cout << "Stored the whole Dataset contents as a single chunk, "
	        "ready to write content\n";

	series.flush();
	cout << "Dataset content has been fully written\n";

	// auto i       = series.iterations[42];
	// series.setAuthor("Axel Huebl <a.huebl@hzdr.de>");
	// series.setMachine("Hall Probe 5000, Model 3");

	// series.setAttribute("dinner", "Pizza and Coke");
	// //	i.setAttribute("vacuum", true);

	series.flush();
	//	iol.save_write();
	return 0;
	//	Ray myray;
	// iol.trace_write(myray);


	return 0;
}
