#ifndef COMMON_TYPES_H_INCLUDE
#define COMMON_TYPES_H_INCLUDE
/// @brief types that are commonly used/shared by multiple modules
#include<stdint.h>

/// Types of control efforts
enum type_control_effort
{
    // control effort for the joint corresponds to motor velocity
    TYPE_CONTROL_EFFORT_MOTOR_VELOCITY,
    // control effort for the joint corresponds to motor torque
    TYPE_CONTROL_EFFORT_MOTOR_TORQUE,

    /// control is handled by setting the arm velocity (after the spring)
    TYPE_CONTROL_EFFORT_ARM_VELOCITY,

    /// control is handled by setting the arm torque (after the spring
    TYPE_CONTROL_EFFORT_ARM_TORQUE,

    /// control mode that does not result in any actual control effort
    /// being exerted (for debugging purposes)
    TYPE_CONTROL_EFFORT_NULL,
};

/// @brief identifier for the processor
enum type_processor_id
{
    TYPE_JC_1, /// joint controller 1
    TYPE_JC_2, /// joint controller 2
    TYPE_JC_3, /// joint controller 3
    TYPE_UC_1, /// main controller 1
    TYPE_GC_1, /// gimbal controller 1
    TYPE_WC_1, /// wheel controller 1
    TYPE_WC_2  /// wheel controller 2

};


/// @brief the position of the end effector platform
struct type_linear_position
{
    float x; /// [p]_1 in delta_robot.pdf
    float y; /// [p]_2 in delta_robot.pdf
    float z; /// [p]_3 in delta_robot.pdf
};

/// @brief the velocity of the end effector platform
struct type_linear_velocity
{
    float xdot; /// \dot{[p]}_1 in delta_robot.pdf
    float ydot; /// \dot{[p]}_2 in delta_robot.pdf
    float zdot; /// \dot{[p]}_3 in delta_robot.pdf
};

/// @brief linear forces on the end-effectors
struct type_linear_force
{
    float fx;
    float fy;
    float fz;
};

/// @brief torques at the joints
struct type_torques
{
    float tau1;
    float tau2;
    float tau3;
};


/// @brief the joint angles (that is the joint after the spring)
struct type_angular_position
{
    float theta1;
    float theta2;
    float theta3;
};

/// @brief angular velocity of the delta robot
struct type_angular_velocity
{
    float theta1dot;
    float theta2dot;
    float theta3dot;
};

/// @brief the position, velocity and force on a joint
struct type_joint_state
{
    /// joint angle, in radians
    float radians;

    /// velocity, in rad/sec
    float rad_sec;

    // torque, in newton meters
    float newton_meters;
};

/// @brief state of the joints before and after the spring
struct type_arm_state
{
    struct type_joint_state before;
    struct type_joint_state after;
};

/// @brief get the joint encoder values
struct type_joint_encoders
{
    // ticks and radians of the before the joint encoder
    uint32_t before_raw;
    float before_radians;

    // ticks and radians of the after the joint encoder
    uint32_t after_raw;
    float after_radians;
};

struct type_delta_state
{
    // state of each of the arms
    struct type_arm_state arms[3];
    // platform position
    struct type_linear_position platform_pos;
    // platform velocity
    struct type_linear_velocity platform_vel;
    // platform force
    struct type_linear_force platform_force;
};

struct type_twist
{
  float wz;
  float vx;
  float vy;
};

struct type_omni_velocities
{
  // Front left
  float uFR;
  // Front right
  float uFL;
  // Rear left
  float uRR;
  // Rear right
  float uRL;
};

struct type_wheel_velocities
{
  float right;
  float left;
};

/// @brief colors for leds. lower 3 bits of an 8 bit byte
/// correspond to the red, green, and blue leds being on (1) or off (0)
enum type_led_color
{
    TYPE_LED_COLOR_BLACK = 0x0,
    TYPE_LED_COLOR_RED   = 0x4,
    TYPE_LED_COLOR_GREEN = 0x2,
    TYPE_LED_COLOR_BLUE  = 0x1,
    TYPE_LED_COLOR_YELLOW = 0x6,
    TYPE_LED_COLOR_CYAN = 0x3,
    TYPE_LED_COLOR_MAGENTA = 0x5,
    TYPE_LED_COLOR_WHITE = 0x7,
};

struct bytestream;

#ifdef __cplusplus
extern "C" {
#endif

/// @brief serialize a processor id
/// @param bs - the bytestream to place the id into
/// @param id - the processor id
/// @pre there must be enough room in bs for the id
void type_inject_processor_id(struct bytestream * bs,
                              enum type_processor_id id);

/// @brief deserialize a processor id
/// @param bs - the bytestream to place the id into
/// @return the processor id
/// @pre bs must contain a valid processor id
enum type_processor_id type_extract_processor_id(struct bytestream * bs);

/// @brief inject a joint state into a bytestream
/// @param bs - bytestream which should contain space to store a joint_state
/// @param js  - the joint state to insert into the bytestream
void type_inject_joint_state(struct bytestream * bs,
                                   const struct type_joint_state * js);

/// @brief extract a joint state from a bytestream
/// @param bs - the bytestream containing the joint state
/// @param out [out] - the resulting joint state
void type_extract_joint_state(struct bytestream * bs,
                                    struct type_joint_state * out);

/// @brief inject the arm state into a bytestream
/// @param bs - the bytestream which should have enough space to store arm state
/// @param as - the arm state to insert into the stream
void type_inject_arm_state(struct bytestream * bs,
                                 const struct type_arm_state * as);

/// @brief extract an arm state from a bytestream
/// @param bs - the bytestream containing the arm state
/// @param out  [out] - the resulting arm state
void type_extract_arm_state(struct bytestream * bs,
                                  struct type_arm_state * as);


/// @brief inject the linear position into a bytestream
/// @param bs - the bytestream
/// @param lp - the struct to inject
void type_inject_linear_position(struct bytestream * bs,
                                       const struct type_linear_position * lp);

/// @brief extract linear position from a bytestream
/// @param bs - the bytestream
/// @param lp [out] - the struct to extract
void type_extract_linear_position(struct bytestream * bs,
                                        struct type_linear_position * lp);


/// @brief inject the linear velocity into a bytestream
/// @param bs - the bytestream
/// @param lp - the struct to inject
void type_inject_linear_velocity(struct bytestream * bs,
                                       const struct type_linear_velocity * lp);

/// @brief extract linear velocity from a bytestream
/// @param bs - the bytestream
/// @param lp [out] - the struct to extract
void type_extract_linear_velocity(struct bytestream * bs,
                                        struct type_linear_velocity * lp);


/// @brief serialize the encoder values into a bytestream
/// @param bs - the bytestream
/// @param enc - the encoder data to store in the stream
void type_inject_joint_encoders(struct bytestream * bs,
                                const struct type_joint_encoders * enc);

/// @brief deserialize encoder data from the bytestream
/// @param bs - the bytestream
/// @param out [out] - the encoder data read from the stream
void type_extract_joint_encoders(struct bytestream * bs,
                                 struct type_joint_encoders * out);

/// @brief serialize delta state data from the bytestram
/// @param bs - the bytestream
/// @param enc - the delta state data to store in the stream
void type_inject_delta_state(struct bytestream * bs,
                             const struct type_delta_state * ds);


/// @brief deserialize delta state data from the bytestream
/// @param bs - the bytestream
/// @param out [out] - the encoder data read from the stream
void type_extract_delta_state(struct bytestream * bs,
                                 struct type_delta_state * out);

/// @brief serialize the led color
/// @param bs - the bytestream
/// @param color -t he color to place in the bytestream
void type_inject_led_color(struct bytestream * bs, enum type_led_color color);


/// @param bs - the bytestream
/// @return the color that was stored in the bytestream
enum type_led_color type_extract_led_color(struct bytestream * bs);

/// @brief deserialize control effort from the bytestream
enum type_control_effort type_control_effort_extract(struct bytestream * bs);

/// @brief serialize control effort to the bytestream
void type_control_effort_inject(struct bytestream * bs, enum type_control_effort ef);

/// @brief inject twist values into bytestream
/// @param bs - the bytestream
/// @param v - the twist to place in the bytestream
void type_inject_twist(struct bytestream * bs, const struct type_twist * v);

/// @brief etract twist values from the bytestream
/// @param bs - the bytestream
/// @param v [out] - the output twist
void type_extract_twist(struct bytestream * bs, struct type_twist * v);

/// @brief extract wheel velocities from a single Tiva wheel board (data for two wheels only)
/// @param bs - the bytestream
/// @param u - the wheel velocities (rad/s)
void type_inject_wheel_velocities(struct bytestream * bs, struct type_wheel_velocities * u);

/// @brief injects wheel velocities from a single Tiva wheel board (data for two wheels only)
/// @param bs - the bytestream
/// @param u [out] - the wheel velocities (rad/s)
void type_extract_wheel_velocities(struct bytestream * bs, struct type_wheel_velocities * u);

/// @brief extract wheel velocities from the omni robot
/// @param bs - the bytestream
/// @param u - the wheel velocities (rad/s)
void type_inject_omni_velocities(struct bytestream * bs, struct type_omni_velocities * u);

/// @brief injects wheel velocities from the omni robot
/// @param bs - the bytestream
/// @param u [out] - the wheel velocities (rad/s)
void type_extract_omni_velocities(struct bytestream * bs, struct type_omni_velocities * u);

#ifdef __cplusplus
}
#endif
#endif
