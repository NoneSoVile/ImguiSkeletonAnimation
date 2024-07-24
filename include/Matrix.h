#ifndef NV_MATRIX_H
#define NV_MATRIX_H


#include <math.h>

/// \file
/// Basic matrix classes with math operations

namespace MB {

template <class T> class vec2;
template <class T> class vec3;
template <class T> class vec4;


// Define a struct to hold the components based on the template Size
    template<class T, int Size>
    struct Components {};

    // Partial specialization for Size = 3
    template<class T>
    struct Components<T, 3>
    {
        T _11, _12, _13;
        T _21, _22, _23;
        T _31, _32, _33;
    };

    // Partial specialization for Size = 4 (default)
    template<class T>
    struct Components<T, 4>
    {
        T _11, _12, _13, _14;
        T _21, _22, _23, _24;
        T _31, _32, _33, _34;
        T _41, _42, _43, _44;
    };

////////////////////////////////////////////////////////////////////////////////
//
//  Matrix
//
////////////////////////////////////////////////////////////////////////////////
template<class T, int Size = 4>
class matrix4
{

public:
    int MatrixSize = Size;
    matrix4() { make_identity(); }

    matrix4( T t )
    { set_value(t); }

    matrix4( const T * m )
    { set_value(m); }

    matrix4( T a00, T a01, T a02, T a03,
        T a10, T a11, T a12, T a13,
        T a20, T a21, T a22, T a23,
        T a30, T a31, T a32, T a33 ) 

    {
        _components._11 = (a00); _components._12 = (a01); _components._13 = (a02);_components._14 = (a03),
        _components._21 = (a10), _components._22=(a11), _components._23=(a12), _components._24=(a13),
        _components._31=(a20), _components._32=(a21), _components._33=(a22), _components._34=(a23),
        _components._41=(a30), _components._42=(a31), _components._43=(a32), _components._44=(a33);
    }

    void print(){

    }


    void get_value( T * mp ) const {
        int32_t c = 0;
        for(int32_t j=0; j < Size; j++)
            for(int32_t i=0; i < Size; i++)
                mp[c++] = element(i,j);
    }

    const T * get_value() const {
        return _array;
    }

    void set_value( T * mp) {
        int32_t c = 0;
        for(int32_t j=0; j < Size; j++)
            for(int32_t i=0; i < Size; i++)
                element(i,j) = mp[c++];
    }

    void set_value( T r ) {
        for(int32_t i=0; i < Size; i++)
            for(int32_t j=0; j < Size; j++)
                element(i,j) = r;
    }

    void make_identity() {
        /*element(0, 0) = 1.0;
        element(0,1) = 0.0;
        element(0,2) = 0.0;
        element(0,3) = 0.0;

        element(1,0) = 0.0;
        element(1,1) = 1.0;
        element(1,2) = 0.0;
        element(1,3) = 0.0;

        element(2,0) = 0.0;
        element(2,1) = 0.0;
        element(2,2) = 1.0;
        element(2,3) = 0.0;

        element(3,0) = 0.0;
        element(3,1) = 0.0;
        element(3,2) = 0.0;
        element(3,3) = 1.0;*/

        for (int i = 0; i < MatrixSize; i++)
        {
            for (int j = 0; j < MatrixSize; j++)
                element(i, j) = (i == j) ? T(1) : T(0);
        }
    }

    // set a uniform scale
    void set_scale( T s ) {
        element(0,0) = s;
        element(1,1) = s;
        element(2,2) = s;
    }

    void set_scale( const vec3<T> & s ) {
        for (int32_t i = 0; i < 3; i++) element(i,i) = s[i];
    }


    void set_translate( const vec3<T> & t ) {
        for (int32_t i = 0; i < 3; i++) element(i,3) = t[i];
    }

    void set_row(int32_t r, const vec4<T> & t) {
        for (int32_t i = 0; i < Size; i++) 
            element(r,i) = t[i];
    }

    void set_column(int32_t c, const vec4<T> & t) {
        for (int32_t i = 0; i < Size; i++) 
            element(i,c) = t[i];
    }

    vec4<T> get_row(int32_t r) const {
        vec4<T> v;
        for (int32_t i = 0; i < Size; i++) 
            v[i] = element(r,i);
        return v;
    }

    vec4<T> get_column(int32_t c) const {
        vec4<T> v;
        for (int32_t i = 0; i < Size; i++) 
            v[i] = element(i,c);
        return v;
    }

    matrix4 & operator *= ( const matrix4 & rhs ) {
        matrix4 mt(*this);
        set_value(T(0));

        for(int32_t i=0; i < Size; i++)
            for(int32_t j=0; j < Size; j++)
                for(int32_t c=0; c < Size; c++)
                    element(i,j) += mt(i,c) * rhs(c,j);
        return *this;
    }

    friend matrix4 operator * ( const matrix4 & lhs, const matrix4 & rhs ) {
        matrix4 r(T(0));

        for(int32_t i=0; i < Size; i++)
            for(int32_t j=0; j < Size; j++)
                for(int32_t c=0; c < Size; c++)
                    r.element(i,j) += lhs(i,c) * rhs(c,j);
        return r;
    }

    // dst = M * src
    vec4<T> operator *( const vec4<T> &src) const {
        vec4<T> r;
        for ( int32_t i = 0; i < Size; i++)
            r[i]  = ( src[0] * element(i,0) + src[1] * element(i,1) +
                      src[2] * element(i,2) + src[3] * element(i,3));
        return r;
    }

    // dst = src * M
    friend vec4<T> operator *( const vec4<T> &lhs, const matrix4 &rhs) {
        vec4<T> r;
        for ( int32_t i = 0; i < 4; i++)
            r[i]  = ( lhs[0] * rhs.element(0,i) + lhs[1] * rhs.element(1,i) +
                      lhs[2] * rhs.element(2,i) + lhs[3] * rhs.element(3,i));
        return r;
    }

    T & operator () (int32_t row, int32_t col) {
        return element(row,col);
    }

    const T & operator () (int32_t row, int32_t col) const {
        return element(row,col);
    }

    T & element (int32_t row, int32_t col) {
        return _array[row + (col * Size)];
    }

    const T & element (int32_t row, int32_t col) const {
        return _array[row + (col*Size)];
    }

    matrix4 & operator *= ( const T & r ) {
        for (int32_t i = 0; i < Size; ++i) {
            for (size_t j = 0; j < Size; j++)
            {
                element(j, i) *= r;
            }
        }
        return *this;
    }

    matrix4 & operator += ( const matrix4 & mat ) {
        for (int32_t i = 0; i < Size; ++i) {
            for (size_t j = 0; j < Size; j++)
            {
                element(j, i) += mat.element(j, i);
            }
        }
        return *this;
    }


    friend bool operator == ( const matrix4 & lhs, const matrix4 & rhs ) {
        bool r = true;
        for (int32_t i = 0; i < Size*Size; i++)
            r &= lhs._array[i] == rhs._array[i];
        return r;
    }

    friend bool operator != ( const matrix4 & lhs, const matrix4 & rhs )  {
        for (int32_t i = 0; i < Size * Size; i++) {
            if (lhs._array[i] != rhs._array[i])
                return true;
        }
        return false;
    }


  

    union {
        // Use the specialized struct as a member
        Components<T, Size> _components;
        T _array[Size*Size];     // array access
    };
    
};


//////////////////////////////////////////////////////////////////////
//
//  Friend functions
//
//   Not actually declared friends due to VC's template handling
//
//////////////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4> inverse( const matrix4<T, 4> & m) {
    matrix4<T, 4> minv;

    T r1[8], r2[8], r3[8], r4[8];
    T *s[4], *tmprow;

    s[0] = &r1[0];
    s[1] = &r2[0];
    s[2] = &r3[0];
    s[3] = &r4[0];

    int32_t i,j,p,jj;
    for(i=0;i<4;i++) {
        for(j=0;j<4;j++) {
            s[i][j] = m.element(i,j);
            if(i==j) s[i][j+4] = 1.0;
            else     s[i][j+4] = 0.0;
        }
    }
    T scp[4];
    for(i=0;i<4;i++) {
        scp[i] = T(fabs(s[i][0]));
        for(j=1;j<4;j++)
            if(T(fabs(s[i][j])) > scp[i]) scp[i] = T(fabs(s[i][j]));
        if(scp[i] == 0.0) return minv; // singular matrix!
    }

    int32_t pivot_to;
    T scp_max;
    for(i=0;i<4;i++) {
        // select pivot row
        pivot_to = i;
        scp_max = T(fabs(s[i][i]/scp[i]));
        // find out which row should be on top
        for(p=i+1;p<4;p++)
            if (T(fabs(s[p][i]/scp[p])) > scp_max) {
                scp_max = T(fabs(s[p][i]/scp[p]));
                pivot_to = p;
            }
        // Pivot if necessary
        if(pivot_to != i) {
            tmprow = s[i];
            s[i] = s[pivot_to];
            s[pivot_to] = tmprow;
            T tmpscp;
            tmpscp = scp[i];
            scp[i] = scp[pivot_to];
            scp[pivot_to] = tmpscp;
        }

        T mji;
        // perform gaussian elimination
        for(j=i+1;j<4;j++) {
            mji = s[j][i]/s[i][i];
            s[j][i] = 0.0;
            for(jj=i+1;jj<8;jj++)
                s[j][jj] -= mji*s[i][jj];
        }
    }
    if(s[3][3] == 0.0) return minv; // singular matrix!

    //
    // Now we have an upper triangular matrix.
    //
    //  x x x x | y y y y
    //  0 x x x | y y y y
    //  0 0 x x | y y y y
    //  0 0 0 x | y y y y
    //
    //  we'll back substitute to get the inverse
    //
    //  1 0 0 0 | z z z z
    //  0 1 0 0 | z z z z
    //  0 0 1 0 | z z z z
    //  0 0 0 1 | z z z z
    //

    T mij;
    for(i=3;i>0;i--) {
        for(j=i-1;j > -1; j--) {
            mij = s[j][i]/s[i][i];
            for(jj=j+1;jj<8;jj++)
                s[j][jj] -= mij*s[i][jj];
        }
    }

    for(i=0;i<4;i++)
        for(j=0;j<4;j++)
            minv(i,j) = s[i][j+4] / s[i][i];

    return minv;
}


//
// transpose
//
//   return the transposed matrix
////////////////////////////////////////////////////////////
template<class T, int Size=4>
matrix4<T, Size> transpose( const matrix4<T, Size> & m) {
    matrix4<T, Size> mtrans;

    for(int32_t i=0;i<Size;i++)
        for(int32_t j=0;j< Size;j++)
            mtrans(i,j) = m.element(j,i);
    return mtrans;
}

//
// Rotation matrix creation
// From rotation angle around X axis [radians]
//
//   return the rotation matrix
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4>& rotationX( matrix4<T, 4> & M, const T angle )
{
    T cosa = cos(angle);
    T sina = sin(angle);

    M.element(0,0) = 1.0;
    M.element(0,1) = 0.0;
    M.element(0,2) = 0.0;
    M.element(0,3) = 0.0;

    M.element(1,0) = 0.0;
    M.element(1,1) = cosa;
    M.element(1,2) = -sina;
    M.element(1,3) = 0.0;

    M.element(2,0) = 0.0;
    M.element(2,1) = sina;
    M.element(2,2) = cosa;
    M.element(2,3) = 0.0;

    M.element(3,0) = 0.0;
    M.element(3,1) = 0.0;
    M.element(3,2) = 0.0;
    M.element(3,3) = 1.0;

    return M;
}

//
// Rotation matrix creation
// From rotation angle around Y axis [radians]
//
//   return the rotation matrix
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4>& rotationY( matrix4<T, 4> & M, const T angle )
{
    T cosa = cos(angle);
    T sina = sin(angle);

    M.element(0,0) = cosa;
    M.element(0,1) = 0.0;
    M.element(0,2) = sina;
    M.element(0,3) = 0.0;

    M.element(1,0) = 0.0;
    M.element(1,1) = 1.0;
    M.element(1,2) = 0.0;
    M.element(1,3) = 0.0;

    M.element(2,0) = -sina;
    M.element(2,1) = 0.0;
    M.element(2,2) = cosa;
    M.element(2,3) = 0.0;

    M.element(3,0) = 0.0;
    M.element(3,1) = 0.0;
    M.element(3,2) = 0.0;
    M.element(3,3) = 1.0;

    return M;
}

//
// Rotation matrix creation
// From rotation angle around Y axis [radians]
//
//   return the rotation matrix
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4>& rotationZ( matrix4<T, 4> & M, const T angle )
{
    T cosa = cos(angle);
    T sina = sin(angle);

    M.element(0,0) = cosa;
    M.element(0,1) = -sina;
    M.element(0,2) = 0.0;
    M.element(0,3) = 0.0;

    M.element(1,0) = sina;
    M.element(1,1) = cosa;
    M.element(1,2) = 0.0;
    M.element(1,3) = 0.0;

    M.element(2,0) = 0.0;
    M.element(2,1) = 0.0;
    M.element(2,2) = 1.0;
    M.element(2,3) = 0.0;

    M.element(3,0) = 0.0;
    M.element(3,1) = 0.0;
    M.element(3,2) = 0.0;
    M.element(3,3) = 1.0;

    return M;
}

template<class T>
matrix4<T, 4>& rotationAxis(matrix4<T, 4> & M, const T theta, const vec3<T> u)
    {
        T cosTheta = cos(theta);
        T sinTheta = sin(theta);
        T ux = u.x;
        T uy = u.y;
        T uz = u.z;

        M.element(0,0) = cosTheta + ux * ux * (1 - cosTheta);
        M.element(0,1) = ux*uy * (1 - cosTheta) - uz*sinTheta;
        M.element(0,2) = ux*uz * (1 - cosTheta) + uy*sinTheta;
        M.element(0,3) = 0.0;

        M.element(1,0) = uy*ux*(1 - cosTheta) + uz*sinTheta;
        M.element(1,1) = cosTheta + uy*uy * (1 - cosTheta);
        M.element(1,2) = uy*uz*(1 - cosTheta) - ux * sinTheta;
        M.element(1,3) = 0.0;

        M.element(2,0) = uz * ux * (1 - cosTheta) - uy * sinTheta;
        M.element(2,1) = uz * uy * (1 - cosTheta) + ux * sinTheta;
        M.element(2,2) = cosTheta + uz * uz * (1 - cosTheta);
        M.element(2,3) = 0.0;

        M.element(3,0) = 0.0;
        M.element(3,1) = 0.0;
        M.element(3,2) = 0.0;
        M.element(3,3) = 1.0;

        return M;
    }

//
// Rotation matrix creation
// From euler angles
//      1/ Yaw around Y axis in radians
//      2/ Pitch around X axis in radians
//      3/ Roll around Z axis in radians
//
//Yaw     Pitch   Roll
//Heading Pitch   Bank
//Y       X       Z
//   return the rotation matrix [R] = [Roll].{Pitch].[Yaw]
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4>& rotationYawPitchRoll( matrix4<T, 4> & M, const T yaw , const T pitch , const T roll )
{
    M.make_identity();
    matrix4<T> rot;

    if (roll)
    {
        MB::rotationZ(M, roll );
    }
    if (pitch)
    {
        M *= MB::rotationX(rot, pitch );
    }
    if (yaw)
    {
        M *= MB::rotationY(rot, yaw );
    }

    return M;
}

//
// Translation matrix creation
// From absolute translation values along X, Y and Z axis
//
//   return the translation matrix
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4>& translation( matrix4<T, 4> & M, const T tx , const T ty , const T tz )
{
    M.element(0,0) = 1.0;
    M.element(1,0) = 0.0;
    M.element(2,0) = 0.0;
    M.element(3,0) = 0.0;

    M.element(0,1) = 0.0;
    M.element(1,1) = 1.0;
    M.element(2,1) = 0.0;
    M.element(3,1) = 0.0;

    M.element(0,2) = 0.0;
    M.element(1,2) = 0.0;
    M.element(2,2) = 1.0;
    M.element(3,2) = 0.0;

    M.element(0,3) = tx;
    M.element(1,3) = ty;
    M.element(2,3) = tz;
    M.element(3,3) = 1.0;

    return M;
}

//
// Look At matrix creation
//
//   return the inverse view matrix
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4> & lookAt( matrix4<T, 4>& M, const vec3<T>& eye, const vec3<T>& center, const vec3<T>& up)
{
    vec3<T> x, y, z;

    // make rotation matrix

    // Z vector
    z.x = eye.x - center.x;
    z.y = eye.y - center.y;
    z.z = eye.z - center.z;
    z = normalize(z);

    // Y vector
    y.x = up.x;
    y.y = up.y;
    y.z = up.z;

    // X vector = Y cross Z
    x = cross(y,z);

    // Recompute Y = Z cross X
    y = cross(z,x);

    // cross product gives area of parallelogram, which is < 1.0 for
    // non-perpendicular unit-length vectors; so normalize x, y here
    x = normalize(x);
    y = normalize(y);

    M._components._11 = x.x; M._components._21 = x.y; M._components._31 = x.z; M._components._41 = -x.x * eye.x - x.y * eye.y - x.z*eye.z;
    M._components._12 = y.x; M._components._22 = y.y; M._components._32 = y.z; M._components._42 = -y.x * eye.x - y.y * eye.y - y.z*eye.z;
    M._components._13 = z.x; M._components._23 = z.y; M._components._33 = z.z; M._components._43 = -z.x * eye.x - z.y * eye.y - z.z*eye.z;
    M._components._14 = 0.0; M._components._24 = 0.0; M._components._34 = 0.0; M._components._44 = 1.0;
    return M;
}

//
// Projection matrix creation (Right Handed, OpenGL standard)
// From the frustum definition
//
//   return the projection matrix
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4>& frustum( matrix4<T, 4> & M, const T l, const T r, const T b, const T t, const T n, const T f)
{
    M.element(0,0) = ((T)(2.0))*n / (r-l);
    M.element(1,0) = 0.0;
    M.element(2,0) = 0.0;
    M.element(3,0) = 0.0;

    M.element(0,1) = 0.0;
    M.element(1,1) = ((T)(2.0))*n / (t-b);
    M.element(2,1) = 0.0;
    M.element(3,1) = 0.0;

    M.element(0,2) = (r+l) / (r-l);
    M.element(1,2) = (t+b) / (t-b);
    M.element(2,2) = -(f+n) / (f-n);
    M.element(3,2) = -1.0;

    M.element(0,3) = 0.0;
    M.element(1,3) = 0.0;
    M.element(2,3) = -(((T)(2.0))*f*n) / (f-n);
    M.element(3,3) = 0.0;

    return M;
}

//
// Projection matrix creation (Right Handed, OpenGL standard)
// From the fovy in radians, aspect ratio and near far definition
//
//   return the projection matrix
////////////////////////////////////////////////////////////
template<class T>
matrix4<T, 4>& perspective(matrix4<T, 4> & M, const T fovy, const T aspect, const T n, const T f)
{
	T xmin, xmax, ymin, ymax;

	ymax = n * (T)tan(fovy * 0.5);
	ymin = -ymax;

	xmin = ymin * aspect;
	xmax = ymax * aspect;

	return frustum(M, xmin, xmax, ymin, ymax, n, f);
}

template<class T>
matrix4<T, 4>& perspectivex( matrix4<T, 4> & M, const T fovx, const T aspect, const T near, const T far)
{
    float e = 1.0f / ::tanf(fovx / 2.0f);
    float aspectInv = 1.0f / aspect;
    float fovy = 2.0f * atanf(aspectInv / e);
    float xScale = 1.0f / tanf(0.5f * fovy);
    float yScale = xScale / aspectInv;

    M._array[0] = xScale;
    M._array[1] = 0.0f;
    M._array[2] = 0.0f;
    M._array[3] = 0.0f;

    M._array[4] = 0.0f;
    M._array[5] = yScale;
    M._array[6] = 0.0f;
    M._array[7] = 0.0f;

    M._array[8] = 0.0f;
    M._array[9] = 0.0f;
    M._array[10] = (far + near) / (near - far);
    M._array[11] = -1.0f;

    M._array[12] = 0.0f;
    M._array[13] = 0.0f;
    M._array[14] = (2.0f * far * near) / (near - far);
    M._array[15] = 0.0f;

    return M;
}


template<class T>
matrix4<T, 4>& ortho2D(matrix4<T, 4> & M, T left, T right, T bottom, T top)
{
    float zNear=-1.0; float zFar=1.0;

    float sx = 2.0f / (right - left);
    float sy = 2.0f / (top - bottom);
    float sz = 2.0f / (zFar - zNear);

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(zFar + zNear) / (zFar - zNear);

    //matrix is stored column major
    M._array[0] = sx,     M._array[4] = 0.0f, M._array[ 8] = 0.0f,  M._array[12] = tx;
    M._array[1] = 0.0f,  M._array[5] = sy,     M._array[ 9] = 0.0f,  M._array[13] = ty;
    M._array[2] = 0.0f,  M._array[6] = 0.0f, M._array[10] = sz,       M._array[14] = tz;
    M._array[3] = 0.0f,  M._array[7] = 0.0f, M._array[11] = 0.0f,  M._array[15] = 1.0f;

    return M;
}

template<class T>
matrix4<T, 4>& ortho3D(matrix4<T, 4> & M, T left, T right, T bottom, T top, T zNear, T zFar)
{
    float sx = 2.0f / (right - left);
    float sy = 2.0f / (top - bottom);
    float sz = 2.0f / (zFar - zNear);

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(zFar + zNear) / (zFar - zNear);

    //matrix is stored column major
    M._array[0] = sx,     M._array[4] = 0.0f, M._array[ 8] = 0.0f,  M._array[12] = tx;
    M._array[1] = 0.0f,  M._array[5] = sy,     M._array[ 9] = 0.0f,  M._array[13] = ty;
    M._array[2] = 0.0f,  M._array[6] = 0.0f, M._array[10] = sz,       M._array[14] = tz;
    M._array[3] = 0.0f,  M._array[7] = 0.0f, M._array[11] = 0.0f,  M._array[15] = 1.0f;

    return M;
}

};

#endif
