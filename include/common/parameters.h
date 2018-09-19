#ifndef COMMON_PARAMETERS_INCLUDE_GUARD_H
#define COMMON_PARAMETERS_INCLUDE_GUARD_H
/// @brief store general parameters that are useful to the whole system
#include "common/utilities.h"

/// @brief joint limits for the arm angle
#define PARAMETERS_ARM_MIN_RAD DEGREES_TO_RADIANS(10.0f)
#define PARAMETERS_ARM_MAX_RAD DEGREES_TO_RADIANS(80.0f)

/// @brief maximum velocity of the arm
#define PARAMETERS_ARM_MAX_RAD_SEC 10.0f

// amps of current,
// actual max is 6
#define PARAMETERS_ARM_MAX_CURRENT 6.0f
/// @brief workspace limits, in m.  origin is at the center of the base
/// so limits are x (-PARAMETERS_WORK_X_MM,LIMIT_WORK_X_M)
/// so limits are y (-PARAMETERS_WORK_Y_MM,LIMIT_WORK_Y_M)
/// so limits are y (-PARAMETERS_WORK_Z_MIN_MM,LIMIT_WORK_Z_MAX_M)
#define PARAMETERS_WORK_X_M 0.070f
#define PARAMETERS_WORK_Y_M 0.070f
#define PARAMETERS_WORK_Z_MIN_M 0.2f
#define PARAMETERS_WORK_Z_MAX_M 0.36f

/// physical parameters of the delta robot
#define PARAMETERS_DELTA_BASE_RADIUS  0.17f
#define PARAMETERS_DELTA_PLATFORM_RADIUS  0.067f
#define PARAMETERS_DELTA_LOWER_LEG_LENGTH 0.200f
#define PARAMETERS_DELTA_UPPER_LEG_LENGTH 0.305f

/// height of the calibration fixture (m)
/// height of the fixture is .20489875 m
/// The aluminum block pedestal is .0056896 m
/// so the total is .21058835
#define PARAMETERS_CALIBRATION_FIXTURE_HEIGHT 0.21058835f

/// mass of the robot above the spring, in kg, excluding the gimbal
#define PARAMETERS_CALIBRATION_SPRUNG_MASS 0.9f


/// Frequencies for some control loop

/// @brief joint control loop in hz. The speed of this loop is limited
/// by the speed of the motor firmware.
/// it can take up to 2 ms to get a response for
/// a motor command.  It could be possible to circumvent this limitation by
/// not waiting for the response; however, the commands would then fill the
/// motor's queue and be buffered, which does not help much
#define PARAMETERS_JOINT_CONTROL_HZ 800u

#endif
