# CMAKE file used for defining some parameters in common/include/parameters.h.in
# see common/include/parameters.h.in for details.
# Variables defined here can also be passed to xacro files during configure time
# This file can be invoked as a script:
# cmake -DPARAMETERS_INFILE=<infile> -DPARAMETERS_OUTFILE=<outfile> -P parameters.cmake
# when invoked this way, infile is configured using the variables defined here and cofigure_file
# the resulting output file is placed in outfile.  This method used when generating parameters.h

set(DELTA_BASE_RADIUS 0.18)
set(DELTA_BASE_THICKNESS 0.009525)
set(DELTA_BASE_MASS 2.5)
set(DELTA_BASE_OFFSET 0.046375)
set(DELTA_PLATFORM_RADIUS  0.062)
set(DELTA_PLATFORM_THICKNESS 0.01)
set(DELTA_PLATFORM_MASS 0.5)
set(DELTA_LOWER_LEG_LENGTH 0.200)
set(DELTA_UPPER_LEG_LENGTH 0.368)

# make sure we were invoked with -P to actually call configure_file
if(CMAKE_SCRIPT_MODE_FILE AND NOT CMAKE_PARENT_LIST_FILE)
  # note parameters.h is placed in the build tree so we add include directories
  # for it in the build interface. THE PARAMETERS_INFILE and OUTFILE arguments
  # should be passed to the script
  #e.g. cmake -DPARAMETERS_IN_FILE=<fname> -DPARAMETERS_OUT_FILE=<fname> -P parameters.cmake
  configure_file(${PARAMETERS_INFILE} ${PARAMETERS_OUTFILE})
endif()
