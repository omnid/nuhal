#ifndef COMMON_PID_H_INCLUDE_GUARD
#define COMMON_PID_H_INCLUDE_GUARD
/// @brief Implementation of a PID controller, suitable for use on embedded
/// systems
#include<stdint.h>

/// @brief gains for a pid controller
struct pid_gains
{
    float kp; // proportional
    float ki; // integral
    float kd; // derivative
};

struct pid_state
{
    struct pid_gains gains;
    float u_max; // maximum control effort
    float u_min; // minimum control effort
    float p_error; // proportional error
    float i_error; // integral error
    float d_error; // derivative error
};



/// @brief data for pid controllers
struct pid_data
{
    float p_error;  // proprotional error
    float i_error;  // integral error
    float d_error;  // derivative error
    float u_effort; // control effort
    float r_reference; // reference signal
    float z_measurement;   // sensor signal
    uint8_t sequence; // sequence number, increments by one each cycle
    uint8_t missed;   // number of cycles missed since the previous pid command
};


#ifdef __cplusplus
extern "C" {
#endif

/// @brief compute the pid control effort
/// @param st - the current state of the pid controller
/// @param reference - the reference signal
/// @param measurement - the measurement signal
float pid_compute(struct pid_state * st, float reference, float measurement);

struct bytestream;

/// @brief serialize pid gains
/// @param bs - the bytestream into which the gains should be inserted
/// @param gains - the gains to write to the bytestream
/// @pre there must be enough space in the bytestream for the gains
void pid_inject_gains(struct bytestream * bs, const struct pid_gains * gains);

/// @brief deserialize pid gains
/// @param bs - the bytestream fromw hich to load the gains
/// @param gains [out] - data from the bytestream is used to write the gains
/// @pre the bytestream must contain the data corresponding to gains
void pid_extract_gains(struct bytestream * bs, struct pid_gains * gains);

/// @brief serialize pid data
/// @param bs - the bytestream into which the gains should be inserted
/// @param data - the pid data to write to the stream
/// @pre there must be enough space in bs for the data
void pid_inject_data(struct bytestream * bs, struct pid_data * data);


/// @brief deserialize pid data
/// @param bs - the bytestream from which to load the data
/// @param data [out] - data from the bytestream is used to write the data
/// @pre the bytestream must contain the data corresponding to data
void pid_extract_data(struct bytestream * bs, struct pid_data * data);

#ifdef __cplusplus
}
#endif
#endif
