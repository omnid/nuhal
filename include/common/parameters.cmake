# This file should be invoked from a custom_command so that the parameters.h
# file is generated at build time

# CMAKE file used for defining some parameters in common/include/parameters.h.in
# see common/include/parameters.h.in for details.
# Variables defined here can also be passed to xacro files during configure time
set(DELTA_BASE_RADIUS 0.18)
set(DELTA_BASE_THICKNESS 0.009525)
set(DELTA_BASE_MASS 2.5)
set(DELTA_BASE_OFFSET 0.046375)
set(DELTA_PLATFORM_RADIUS  0.062)
set(DELTA_PLATFORM_THICKNESS 0.01)
set(DELTA_PLATFORM_MASS 0.5)
set(DELTA_LOWER_LEG_LENGTH 0.200)
set(DELTA_UPPER_LEG_LENGTH 0.368)

# note parameters.h is placed in the build tree so we add include directories
# for it in the build interface. THE PARAMETERS_INFILE and OUTFILE arguments
# should be passed to the script
#e.g. cmake -DPARAMETERS_IN_FILE=<fname> -DPARAMETERS_OUT_FILE=<fname> -P parameters.cmake
configure_file(${PARAMETERS_INFILE} ${PARAMETERS_OUTFILE})
