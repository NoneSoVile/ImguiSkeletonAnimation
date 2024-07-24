//----------------------------------------------------------------------------------


#ifndef NV_MATH_H
#define NV_MATH_H



#include <math.h>

#include "Vector.h"
#include "Matrix.h"
#include "Quaternion.h"

/// \file
/// Overall math class header

#define NV_PI   float(3.1415926535897932384626433832795)

namespace MB {

typedef vec2<float> vec2f; ///< float 2-vectors
typedef vec2<int32_t> vec2i; ///< signed integer 2-vectors
typedef vec2<uint32_t> vec2ui; ///< unsigned integer 2-vectors
typedef vec3<float> vec3f; ///< float 3-vectors
typedef vec3<int32_t> vec3i; ///< signed integer 3-vectors
typedef vec3<uint32_t> vec3ui; ///< unsigned integer 4-vectors
typedef vec4<float> vec4f; ///< float 4-vectors
typedef vec4<int32_t> vec4i; ///< signed integer 4-vectors
typedef vec4<uint32_t> vec4ui; ///< unsigned integer 4-vectors
typedef matrix4<float> matrix4f; ///< float 4x4 matrices
typedef quaternion<float> quaternionf; ///< float quaternions
typedef matrix4<float, 3> matrix3f; ///< float 4x4 matrices


};

#endif
