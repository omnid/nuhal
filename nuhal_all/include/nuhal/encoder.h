#ifndef COMMON_ENCODER_INCLUDE_GUARD
#define COMMON_ENCODER_INCLUDE_GUARD
/// @file
/// @brief Provide a common interface for working with encoders.
///
/// Device-specific drivers provide the raw encoder counts, the
/// rest of the conversion is performed here.
/// There are three types of angles we can get from an encoder
/// suppose an encoder has ticks_per_rev counts per revolution
/// and zero_angle_ticks corresponds to the zero angle
/// raw - this is a direct reading
/// ticks - integer units, normalized such that
/// the angles range from
// -ceil(ticks_per_rev/2) <= angle <= floor(ticks_per_rev/2)
// with angle 0 corresponding to zero_angle_ticks
/// finally radians from (-pi, pi]
/// General workflow is for a low-level function to provide encoder_raw_ticks
/// The user then normalizes the raw ticks and can convert them to radians
#include<stdint.h>

struct bytestream;

/// Encoder orientation: Specify if counting up results in smaller or larger angles.
enum encoder_orientation
{
    /// Increasing ticks -> increasing angle
    ENCODER_ORIENTATION_UP_UP,

    /// Increasing ticks -> decreasing angle
    ENCODER_ORIENTATION_UP_DOWN 
};

/// @brief Properties of an encoder
struct encoder
{
    /// The number of counts per revolution
    uint32_t ticks_per_rev;

    /// The number of ticks corresponding to the zero angle
    uint32_t zero_angle_ticks;

    /// The orientation of the encoder if increasing ticks ->
    /// increasing angle or vice versa
    enum encoder_orientation orientation;
};

/// @brief Raw data from an encoder.
/// These are the ticks of the encoder prior to going
/// through any scaling or re-zeroing
struct encoder_raw
{
    /// \brief The full-turn count for multi-turn encoders.
    ///
    /// Multi has a resolution of 1 tick per revolution.
    uint32_t multi;

    /// Single-turn  tick per revolution
    uint32_t single;
};


/// @brief encoders for the joint
struct encoder_joints
{
    /// calibrated ticks for the before-spring encoder
    int32_t before_ticks;

    /// radians of the before-spring encoder
    float before_radians;

    /// calibrated ticks for the after-spring encoder
    int32_t after_ticks;

    /// radians of the after-spring encoder
    float after_radians;
};

/// @brief encoders for the gimbal
struct encoder_gimbal
{
    /// Calibrated ticks for the x axis
    int32_t x_ticks;

    /// Calibrated ticks for the y axis
    int32_t y_ticks;


    /// Calibrated ticks for the z axis
    int32_t z_ticks;

    /// radians for the x axis
    float x_radians;

    /// radians for the y axis
    float y_radians;

    /// radians for the z axis
    float z_radians;
};

#ifdef __cplusplus
extern "C" {
#endif

/// @brief normalize an encoder so that increasing ticks correspond
/// to increasing angle and the zero angle and zero tick points are the same
/// @param enc - encoder whose ticks we are to normalize
/// @param raw - the raw count of enc
/// @return normalized encoder ticks:
///
/// The normalization means that
/// 1. -ceil(enc->ticks_per_rev/2) <= ticksN <= floor(enc->ticks_per_rev/2)
/// 1. ticksN == 0 -> ticks == enc-> zero_angle_ticks
/// essentially this is a signed angle, centered at 0, in units of ticks
/// since this is normalized, multi-turn is ignored
int32_t encoder_ticks(const struct encoder * enc, struct encoder_raw raw);

/// @brief convert a tick count to radians
/// @param enc - the encoder properties
/// @param ticks - raw ticks read from the encoder (normalized by encoder_ticks)
/// @return - the encoder count. if ticks is normalized than
/// the angle will be from -pi to pi.  however, tick counts outside the range
/// of one full cycle can still be passed to this and it will give the
/// equivalent radians
float encoder_radians(const struct encoder * enc, int32_t ticks);

/// @brief  This function answers the question:
/// given that an ENCODER'S raw count of X corresponds to Y radians,
/// what raw count corresponds to 0 radians?
/// @param enc - the encoder
/// @param raw count - the count of the encoder at a position
/// @param radians - the angle in radians of the encoder at the position
/// @return raw encoder counts that correspond to the zero angle
/// @pre -pi < radians <= pi
uint32_t encoder_zero_raw(const struct encoder * enc,
                          struct encoder_raw raw,
                          float radians);

/// @brief serialize the joint encoder values into a bytestream
/// @param bs - the bytestream
/// @param enc - the encoder data to store in the stream
void encoder_joints_inject(struct bytestream * bs,
                          const struct encoder_joints * enc);

/// @brief deserialize joint encoder data from the bytestream
/// @param[in,out] bs  The bytestream
/// @param[out] out    The encoder data read from the stream
void encoder_joints_extract(struct bytestream * bs,
                           struct encoder_joints * out);


/// @brief Serialize the gimbal encoder values into a bytestream
/// @param[in,out] bs  The bytestream to write the data to
/// @param[in]     enc The encoder data to store in the stream
void encoder_gimbal_inject(struct bytestream * bs,
                          const struct encoder_gimbal * enc);

/// @brief Deserialize gimbal encoder data from the bytestream
/// @param[in,out] bs    The bytestream
/// @param[out]    out   The encoder data read from the stream
void encoder_gimbal_extract(struct bytestream * bs,
                           struct encoder_gimbal * out);
#ifdef __cplusplus
}
#endif
#endif
