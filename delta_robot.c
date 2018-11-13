#include "common/delta_robot.h"
#include "common/utilities.h"
#include "common/error.h"
#include "common/matrix.h"
#include "common/parameters.h"
#include <math.h>
#include <stdbool.h>

// square root of 3
static const float root3 = 1.73205080756887729353f;

// sqrt(3)/2
static const float root3_div_2 = 0.86602540378443864676f;


// physical parameters for the delta robot
const struct delta_robot DELTA_ROBOT =
{
    .base_radius = PARAMETERS_DELTA_BASE_RADIUS,
    .platform_radius = PARAMETERS_DELTA_PLATFORM_RADIUS,
    .lower_leg_length = PARAMETERS_DELTA_LOWER_LEG_LENGTH,
    .upper_leg_length = PARAMETERS_DELTA_UPPER_LEG_LENGTH
};

/// define some helper functions that compute variables used
/// in several equations


/// @brief compute E1 (equation 10) in delta_robot.pdf
static inline float dk_E1(float L, float r, float q, float x)
{
    return 2.0f * L * (r - q - x);
}

/// @brief compute E2 (equation 11) in delta_robot.pdf
static inline float dk_E2(float L, float r, float q, float x, float y)
{
    return 2.0f * L * (r - q + x/2.0f - y * root3_div_2);
}

/// @brief compute E3 (equation 12) in delta_robot.pdf
static inline float dk_E3(float L, float r, float q, float x, float y)
{
    return 2.0f * L * (r - q + x/2.0f + y * root3_div_2);
}

/// @brief compute F1, F2, and F3 (equation 13) in delta_robot.pdf
static inline float dk_F(float L, float z)
{
    return -2.0f * L * z;
}

/// @brief compute A_ii (equation 54)
/// Ei - E1, E2, or E3 from equation 10, 11, or 12 respectively
/// F - the F value (equation 13)
/// Si - sine of theta_i
/// Ci - cosine of theta_i
static inline float dk_Aii(float Ei, float F, float Si, float Ci)
{
    return Ei*Si - F*Ci;
}

/// upper arm vector one, index 1 @see (equation 5)
/// @param r_minus_q - set this to r - q
/// @param L - lower leg vector length
/// @param x - x coordinate of the platform
/// @param C1 - cos(theta1)
static inline float dk_M11(float r_minus_q, float L, float x, float C1)
{
    return x - r_minus_q - L*C1;
}

/// upper arm vector one, index 2  @see (equation 5)
static inline float dk_M12(float y)
{
    return y;
}


/// upper arm vecotr i, index 3 @see (equations 5, 6, 7)
/// @param L - lower arm length
/// @param z - platform z coordinate
/// @param Si - sine of theta_i
static inline float dk_Mi3(float L, float z, float Si)
{
    return z - L*Si;
}

/// upper arm vector two , index 1 @see (equation 6)
/// @param r_minus_q - set this to r - q
/// @param L - lower leg vector length
/// @param x - x coordinate of the platform
/// @param C2 - cos(theta2)
static inline float dk_M21(float r_minus_q, float L, float x, float C2)
{
    return x + r_minus_q/2.0f + (L/2.0f)*C2;
}

/// upper arm vector two, index 2  @see (equation 6)
/// @param r_minus_q - set this to r - q
/// @param L - lower leg vector length
/// @param y - y coordinate of the platform
/// @param C2 - cos(theta2)
static inline float dk_M22(float r_minus_q, float L, float y, float C2)
{
    return y - root3_div_2*(r_minus_q + L*C2);
}

/// upper arm vector three , index 1 @see (equation 7)
/// @param r_minus_q - set this to r - q
/// @param L - lower leg vector length
/// @param x - x coordinate of the platform
/// @param C3 - cos(theta3)
static inline float dk_M31(float r_minus_q, float L, float x, float C3)
{
    return x + r_minus_q/2.0f + (L/2.0f)*C3;
}

/// upper arm vector three, index 2  @see (equation 6)
/// @param r_minus_q - set this to r - q
/// @param L - lower leg vector length
/// @param y - y coordinate of the platform
/// @param C3 - cos(theta2)
static inline float dk_M32(float r_minus_q, float L, float y, float C3)
{
    return y + root3_div_2*(r_minus_q + L*C3);
}

void delta_robot_inverse_kinematics(const struct delta_robot * params,
                           const struct type_linear_position * pos,
                           struct type_angular_position * out)
{
    /// @see docs/delta_robot/delta_robot.pdf for details, in particular
    // Section 2.1 Inverse Kinematics

    if(!params || !pos || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    // these values are the magnitudes of the leg lengths and base radii
    const float L = params->lower_leg_length;
    const float M = params->upper_leg_length;
    const float r = params->base_radius;
    const float q = params->platform_radius;

    const float x = pos->x; // this is [p]_1 in delta_robot.pdf
    const float y = pos->y; // this is [p]_2 in delta_robot.pdf
    const float z = pos->z; // this is [p]_3 in delta_robot.pdf


    // E, F, and G variables (equations 10 - 16) in delta_robot.pdf
    const float E1 = dk_E1(L, r, q, x);
    const float E2 = dk_E2(L, r, q, x, y);
    const float E3 = dk_E3(L, r, q, x, y);

    // all the F variables are the same
    const float F = dk_F(L, z);

    const float baseG = L*L - M*M + (r - q)*(r - q);
    // magnitude of position vector, squared
    const float p_mag_sq = x*x + y*y + z*z;

    const float G1 = baseG - 2.0f*(r - q)*x + p_mag_sq;
    const float G2 = baseG + (r - q)*(x - y * root3) + p_mag_sq;
    const float G3 = baseG + (r - q)*(x + y * root3) + p_mag_sq;

    // implementing equation (19) in delta_robot.pdf
    // s_i = -F_i +- sqrt(Fi^2 + E_i^2 - G_i^2)/ G_i - E_i)
    // compute discriminants
    const float disc1 = sqrt(F*F + E1*E1 - G1*G1);
    const float disc2 = sqrt(F*F + E2*E2 - G2*G2);
    const float disc3 = sqrt(F*F + E3*E3 - G3*G3);

    const float s1_den = G1 - E1;
    const float s2_den = G2 - E2;
    const float s3_den = G3 - E3;

    // compute both the positive and negative solutions
    const float s1_num_plus = -F + disc1;
    const float s2_num_plus = -F + disc2;
    const float s3_num_plus = -F + disc3;

    const float s1_num_minus = -F - disc1;
    const float s2_num_minus = -F - disc2;
    const float s3_num_minus = -F - disc3;

    const float s1_plus = s1_num_plus/s1_den;
    const float s2_plus = s2_num_plus/s2_den;
    const float s3_plus = s3_num_plus/s3_den;


    const float s1_minus = s1_num_minus/s1_den;
    const float s2_minus = s2_num_minus/s2_den;
    const float s3_minus = s3_num_minus/s3_den;

    // compute the angles for both solutions
    const float theta1_plus = 2.0f * atan(s1_plus);
    const float theta2_plus = 2.0f * atan(s2_plus);
    const float theta3_plus = 2.0f * atan(s3_plus);

    const float theta1_minus = 2.0f * atan(s1_minus);
    const float theta2_minus = 2.0f * atan(s2_minus);
    const float theta3_minus = 2.0f * atan(s3_minus);

    // compute the knee bend inequalities to determine the correct angle to use
    // see equations (22), (23), and (24) in delta_robot.pdf. 
    const float knee1_lhs = (q - r + x)*sin(theta1_plus); 
    const float knee2_lhs = (q -r - x/2.0f + y *root3_div_2)*sin(theta2_plus); 
    const float knee3_lhs = (q -r - x/2.0f - y *root3_div_2)*sin(theta3_plus); 

    const float knee1_rhs = z * cos(theta1_plus);
    const float knee2_rhs = z * cos(theta2_plus);
    const float knee3_rhs = z * cos(theta3_plus);

    // use the angle corresponding to the outward knee bend
    out->theta1 = knee1_lhs < knee1_rhs ? theta1_plus : theta1_minus;
    out->theta2 = knee2_lhs < knee2_rhs ? theta2_plus : theta2_minus;
    out->theta3 = knee3_lhs < knee3_rhs ? theta3_plus : theta3_minus;
}



void delta_robot_inverse_velocity(const struct delta_robot * params,
                         const struct type_linear_position * pos,
                         const struct type_angular_position * angle,
                         const struct type_linear_velocity * vel,
                         struct type_angular_velocity * out)
{
    if(!vel || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }

    struct matrix_3x3 Jinv;
    delta_robot_inverse_jacobian(params, pos, angle, &Jinv);

    const struct matrix_3x1 v = {{vel->xdot, vel->ydot, vel->zdot}};
    struct matrix_3x1 u;
    matrix_3x3_multiply_vector(&Jinv, &v, &u);
    out->theta1dot = u.data[0];
    out->theta2dot = u.data[1];
    out->theta3dot = u.data[2];
}

void delta_robot_inverse_force(const struct delta_robot * params,
                         const struct type_linear_position * pos,
                         const struct type_angular_position * angle,
                         const struct type_linear_force * force,
                         struct type_torques * out)
{
    if(!force || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    struct matrix_3x3 Jt;
    delta_robot_jacobian(params, pos, angle, &Jt);
    matrix_3x3_transpose(&Jt);

    const struct matrix_3x1 v = {{force->fx, force->fy, force->fz}};
    struct matrix_3x1 u;

    matrix_3x3_multiply_vector(&Jt, &v, &u);
    out->tau1 = u.data[0];
    out->tau2 = u.data[1];
    out->tau3 = u.data[2];
}

void delta_robot_forward_kinematics(const struct delta_robot * params,
                           const struct type_angular_position * angles,
                           struct type_linear_position * out)
{
    if(!params || !angles || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }


    /// see Section 2.2 Forward Kinematics in delta_robot.tex
    const float L = params->lower_leg_length;
    const float M = params->upper_leg_length;
    const float r = params->base_radius;
    const float q = params->platform_radius;

    // sines and cosines of joint angles
    const float C1 = cos(angles->theta1);
    const float S1 = sin(angles->theta1);
    const float C2 = cos(angles->theta2);
    const float S2 = sin(angles->theta2);
    const float C3 = cos(angles->theta3);
    const float S3 = sin(angles->theta3);

    // center of the spheres (equations 26, 27, 28)
    const float c1x = r + L*C1 - q;
    const float c1y = 0.0f;
    const float c1z = L * S1;

    const float c2x = (q - r - L*C2)/2.0f;
    const float c2y = -root3 * c2x;
    const float c2z = L*S2;

    const float c3x = (q - r - L*C3)/2.0f;
    const float c3y = root3 * c3x;
    const float c3z = L*S3;

    const float c21x = c2x - c1x;
    const float c21y = c2y - c1y;
    const float c21z = c2z - c1z;
    
    // compute d (equation 30)
    const float d = sqrt(c21x*c21x + c21y*c21y + c21z*c21z);

    // compute xhat (equation 29)
    const float xhat0 = c21x/d;
    const float xhat1 = c2y/d;
    const float xhat2 = c21z/d;

    // compute c3 - c1
    const float c31x = c3x - c1x;
    const float c31y = c3y - c1y;
    const float c31z = c3z - c1z;

    // compute a (equation 31)
    const float a = c31x*xhat0 + c31y*xhat1 + c31z*xhat2;

    // compute yhat (equation 33)
    // first unormalized
    const float yhat0u = c31x - a * xhat0;
    const float yhat1u = c31y - a * xhat1;
    const float yhat2u = c31z - a * xhat2;
    // now normalize
    const float ynorm = sqrt(yhat0u*yhat0u + yhat1u*yhat1u + yhat2u*yhat2u);
    const float yhat0 = yhat0u/ynorm;
    const float yhat1 = yhat1u/ynorm;
    const float yhat2 = yhat2u/ynorm;

    // compute b (equation 32)
    const float b = c31x*yhat0 + c31y*yhat1 + c31z*yhat2;

    const float b2 = b*b;
    const float a2 = a*a;

    // equation (41)
    const float xc = d/2.0f;
    // equation (42)
    const float yc = (a2 + b2 - a*d)/(2.0f*b);

    // equation (43), use the positive version
    const float amd = a - d;
    const float amd2 = amd * amd;
    const float M2 = M*M;
    // these steps are ordered to minimize numerical error
    const float zc = sqrt(4.0f*b2*M2 - (a2 + b2)*(b2 + amd2) )/(2.0f *b);

    // compute the zhat axis
    const float zhat0 = xhat1*yhat2 - xhat2*yhat1;
    const float zhat1 = xhat2*yhat0 - xhat0*yhat2;
    const float zhat2 = xhat0*yhat1 - xhat1*yhat0;
    // convert xc, yc, zc to the base frame
    out->x = c1x + xc*xhat0 + yc*yhat0 + zc*zhat0;
    out->y = c1y + xc*xhat1 + yc*yhat1 + zc*zhat1;
    out->z = c1z + xc*xhat2 + yc*yhat2 + zc*zhat2;
}

void delta_robot_jacobian(const struct delta_robot * params,
                          const struct type_linear_position * pos,
                          const struct type_angular_position * angle,
                          struct matrix_3x3 * out)
{
    if(!params || !pos || !angle || !out)
    {
        error(FILE_LINE, "NULL ptr");
    }
    // gather the parameters
    const float L = params->lower_leg_length;
    const float r = params->base_radius;
    const float q = params->platform_radius;

    /// @see section 2 Kinematic equations and section 3 Inverse jacobian 
    const float x = pos->x;
    const float y = pos->y;
    const float z = pos->z;

    /// arm angle trigonometry
    const float C1 = cos(angle->theta1);
    const float S1 = sin(angle->theta1);
    const float C2 = cos(angle->theta2);
    const float S2 = sin(angle->theta2);
    const float C3 = cos(angle->theta3);
    const float S3 = sin(angle->theta3);

    /// E and F are needed to form A (see equation (54))
    const float E1 = dk_E1(L, r, q, x);
    const float E2 = dk_E2(L, r, q, x, y);
    const float E3 = dk_E3(L, r, q, x, y);

    const float F = dk_F(L, z);

    const float rmq = r - q;
    // the upper arm vectors, indexed by arm and component (equations 5, 6, & 7)
    // upper arm 1
    const float M11 = dk_M11(rmq, L, x, C1);
    const float M12 = dk_M12(y);
    const float M13 = dk_Mi3(L, z, S1);
    // upper arm 2
    const float M21 = dk_M21(rmq, L, x, C2);
    const float M22 = dk_M22(rmq, L, y, C2);
    const float M23 = dk_Mi3(L, z, S2);
    // upper arm 3
    const float M31 = dk_M31(rmq, L, x, C3);
    const float M32 = dk_M32(rmq, L, y, C3);
    const float M33 = dk_Mi3(L, z, S3);

    // compute the determinant of B. don't include the factor of 8 here
    const float detB = M13*(M21*M32 - M22*M31)
        + M12*(M23*M31-M21*M33)
        + M11*(M22*M33-M23*M32);

    // divide by 2 detB to compensate since B is 2 * M
    const float Binv11 = (M22*M33 - M23*M32)/(2.0f*detB);
    const float Binv12 = (M13*M32 - M12*M33)/(2.0f*detB);
    const float Binv13 = (M12*M23 - M13*M22)/(2.0f*detB);
    const float Binv21 = (M23*M31 - M21*M33)/(2.0f*detB);
    const float Binv22 = (M11*M33 - M13*M31)/(2.0f*detB);
    const float Binv23 = (M13*M21 - M11*M23)/(2.0f*detB);
    const float Binv31 = (M21*M32 - M22*M31)/(2.0f*detB);
    const float Binv32 = (M12*M31 - M11*M32)/(2.0f*detB);
    const float Binv33 = (M11*M22 - M12*M21)/(2.0f*detB);

    // compute diagonal elements of A (note F1 == F2 == F3)
    const float A00 = dk_Aii(E1, F, S1, C1);
    const float A11 = dk_Aii(E2, F, S2, C2);
    const float A22 = dk_Aii(E3, F, S3, C3);

    matrix_3x3_init(out,
                    Binv11*A00, Binv12*A11, Binv13*A22,
                    Binv21*A00, Binv22*A11, Binv23*A22,
                    Binv31*A00, Binv32*A11, Binv33*A22);
}

void delta_robot_inverse_jacobian(const struct delta_robot * params,
                      const struct type_linear_position * pos,
                      const struct type_angular_position * angle,
                      struct matrix_3x3 * out)
{
    /// @see docs/delta_robot/delta_robot.pdf for details, in particular
    // Section 3 Inverse Jacobian

    if(!params || !pos || !angle || !out )
    {
        error(FILE_LINE, "NULL ptr");
    }

    // these values are the magnitudes of the leg lengths and base radii
    const float L = params->lower_leg_length;
    const float r = params->base_radius;
    const float q = params->platform_radius;

    // end effector platform position
    const float x = pos->x;
    const float y = pos->y;
    const float z = pos->z;

    // joint angles
    const float theta1 = angle->theta1;
    const float theta2 = angle->theta2;
    const float theta3 = angle->theta3;

    /// E and F are needed to form A (see equation (54))
    const float E1 = dk_E1(L, r, q, x);
    const float E2 = dk_E2(L, r, q, x, y);
    const float E3 = dk_E3(L, r, q, x, y);

    const float F = dk_F(L, z);

    // compute some trig functions
    const float S1 = sin(theta1);
    const float C1 = cos(theta1);

    const float S2 = sin(theta2);
    const float C2 = cos(theta2);

    const float S3 = sin(theta3);
    const float C3 = cos(theta3);

    // compute diagonal elements of A (note F1 == F2 == F3)
    const float A00 = dk_Aii(E1, F, S1, C1);
    const float A11 = dk_Aii(E2, F, S2, C2);
    const float A22 = dk_Aii(E3, F, S3, C3);

    const float rmq = r - q;

    // compute the B matrix equation(55) in delta_robot.pdf. also note that it
    // can be written in terms of M
    const float B[3][3] = {
        {2.0f*dk_M11(rmq, L, x, C1),
         2.0f*dk_M12(y),
         2.0f*dk_Mi3(L, z, S1)},
        {2.0f*dk_M21(rmq, L, x, C2),
         2.0f*dk_M22(rmq, L, y, C2),
         2.0f*dk_Mi3(L, z, S2)},
        {2.0f*dk_M31(rmq, L, x, C3),
         2.0f*dk_M32(rmq, L, y, C3),
         2.0f*dk_Mi3(L, z, S3)}
    };


    matrix_3x3_init(out,
                    B[0][0]/A00, B[0][1]/A00, B[0][2]/A00,
                    B[1][0]/A11, B[1][1]/A11, B[1][2]/A11,
                    B[2][0]/A22, B[2][1]/A22, B[2][2]/A22);

}


void delta_robot_forward_velocity(const struct delta_robot * params,
                                  const struct type_linear_position * pos,
                                  const struct type_angular_position * angle,
                                  const struct type_angular_velocity * vel,
                                  struct type_linear_velocity * out)
{
    if(!vel)
    {
        error(FILE_LINE, "NULL ptr");
    }

    // compute the jacobian
    struct matrix_3x3 J;
    delta_robot_jacobian(params, pos, angle, &J);

    // joint velocity vector
    const struct matrix_3x1 v =
        {{vel->theta1dot, vel->theta2dot, vel->theta3dot}};
    /// end-effector velocity vector
    struct matrix_3x1 u;
    matrix_3x3_multiply_vector(&J, &v, &u);
    out->xdot = u.data[0];
    out->ydot = u.data[1];
    out->zdot = u.data[2];
}


void delta_robot_forward_force(const struct delta_robot * params,
                               const struct type_linear_position * pos,
                               const struct type_angular_position * angle,
                               const struct type_torques * torque,
                               struct type_linear_force * out)
{
    if(!torque)
    {
        error(FILE_LINE, "NULL ptr");
    }
    struct matrix_3x3 JinvT;
    delta_robot_inverse_jacobian(params, pos, angle, &JinvT);
    matrix_3x3_transpose(&JinvT);

    // joint torque vector
    const struct matrix_3x1 v = {{torque->tau1, torque->tau2, torque->tau3}};
    struct matrix_3x1 u;

    matrix_3x3_multiply_vector(&JinvT, &v, &u);
    out->fx = u.data[0];
    out->fy = u.data[1];
    out->fz = u.data[2];
}

void delta_robot_knees(const struct delta_robot * params,
                       const struct type_delta_state * state,
                       struct delta_robot_knee_angles * knees)
{
    if(!params || !state || !knees)
    {
        error(FILE_LINE, "NULL ptr");
    }
}
