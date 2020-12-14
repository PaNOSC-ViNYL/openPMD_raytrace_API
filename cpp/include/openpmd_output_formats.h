#ifndef OPENPMD_OUTPUT_FORMATS_H
#define OPENPMD_OUTPUT_FORMATS_H
/// \file

namespace raytracing {
/** \enum openPMD_output_format_t
 * \brief list of implemented openPMD backends/extensions
 */
enum openPMD_output_format_t {
	JSON, ///< JSON
	HDF5, ///< HDF5
	AUTO  ///< decide based on the file name extension
};
} // namespace raytracing
#endif
