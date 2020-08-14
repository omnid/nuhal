#ifndef COMMON_PID_H_INCLUDE_GUARD
#define COMMON_PID_H_INCLUDE_GUARD
/// @brief Implementation of a PID controller, suitable for use on embedded
/// systems
#include<stdint.h>

/// @brief gains for a pid controller
struct pid_gains
{
    float kp;    /// proportional gain
    float ki;    /// integral gain
    float kd;    /// derivative gain
    float u_max; /// maximum control effort
    float u_min; /// minimum control effort
};

/// @brief the current state of the pid controller
struct pid_state
{
    float p_error; /// proportional error
    float i_error; /// integral error
    float d_error; /// derivative error
};

/// @brief signals for the pid controller
struct pid_signals
{
    float reference;   // the reference signal
    float measurement; // the measurement signal
    float effort;      // the control effort signal
};

/// @brief information used to debug pid controllers
struct pid_debug_info
{
    struct pid_state state;       // state of the pid controller
    struct pid_signals signals;   // values of the signals
    uint8_t sequence; // sequence number, increments by one each cycle
    uint8_t missed;   // number of cycles missed since the previous pid command
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief compute the pid control effort
/// @param gains - the gains for the controller
/// @param st [in/out] - the current state of the pid controller
/// @param reference - the reference signal
/// @param measurement - the measurement signal
/// @return the control effort
float pid_compute(const struct pid_gains * gains,
                  struct pid_state * st,
                  float reference,
                  float measurement);

struct bytestream;

/// @brief serialize pid gains
/// @param bs - the bytestream into which the gains should be inserted
/// @param gains - the gains to write to the bytestream
/// @pre there must be enough space in the bytestream for the gains
void pid_gains_inject(struct bytestream * bs, const struct pid_gains * gains);

/// @brief deserialize pid gains
/// @param bs - the bytestream fromw hich to load the gains
/// @param gains [out] - data from the bytestream is used to write the gains
/// @pre the bytestream must contain the data corresponding to gains
void pid_gains_extract(struct bytestream * bs, struct pid_gains * gains);

/// @brief serialize pid state
/// @param bs - bytestream into which the state should be inserted
/// @param state - the state to insert into the bytestream
/// @pre - there must be enough space in the bytestream for the gains
void pid_state_inject(struct bytestream * bs, const struct pid_state * state);

/// @brief deserialize pid state
/// @param bs - the bytestream fromw hich to load the state
/// @param state [out] - data from the bytestream is used to write the state
/// @pre the bytestream must contain the data corresponding to state
void pid_state_extract(struct bytestream * bs, struct pid_state * state);

/// @brief serialize pid signals
/// @param bs - bytestream into which the state should be inserted
/// @param signals - the state to insert into the bytestream
/// @pre - there must be enough space in the bytestream for the gains
void pid_signals_inject(struct bytestream * bs, const struct pid_signals * signals);

/// @brief deserialize pid signals 
/// @param bs - the bytestream fromw hich to load the signals
/// @param signals [out] - data from the bytestream is used to write the signal
/// @pre the bytestream must contain the data corresponding to signals
void pid_signals_extract(struct bytestream * bs, struct pid_signals * signals);


/// @brief serialize pid debug_info
/// @param bs - bytestream into which the state should be inserted
/// @param debug_info - the state to insert into the bytestream
/// @pre - there must be enough space in the bytestream for the gains
void pid_debug_info_inject(struct bytestream * bs, const struct pid_debug_info * debug_info);

/// @brief deserialize pid debug_info 
/// @param bs - the bytestream fromw hich to load the debug_info
/// @param debug_info [out] - data from the bytestream is used to write the signal
/// @pre the bytestream must contain the data corresponding to debug_info
void pid_debug_info_extract(struct bytestream * bs, struct pid_debug_info * state);
#ifdef __cplusplus
}
#endif
#endif
