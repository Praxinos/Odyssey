// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Matrix3x3.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the TMatrix3 class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/MatrixFunc.h"
#include "Math/Geometry/MatrixN.h"
#include "Math/Geometry/Vector.h"
#include "Math/Math.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TMatrix3
/// @brief      The TMatrix3 class provides a simple 3x3 square matrix class for
///             geometry applications.
/// @details    The TMatrix3 class is a template class that provides basic matrix
///             functionnalities, it can be used with any numeric types,
///             ranging from integer values to floating point values.
///
///             TMatrix3 are column-major ordered.
template< typename T >
class TMatrix3
{
public:
    typedef TVector3< T > tRow;
    typedef TVector3< T > tColumn;

private:
    /*! The columns of the matrix. */
    tColumn mCols[3];

public:
    // Constructors
    /*! Default constructor ( identity matrix ). */
    ULIS_MATRIX_FUNC TMatrix3();

    /*! Constructor from input diagonal value. */
    ULIS_MATRIX_FUNC TMatrix3( T iValue );

    /*! Constructor from input cells values. */
    ULIS_MATRIX_FUNC TMatrix3(
          T iM00, T iM01, T iM02
        , T iM10, T iM11, T iM12
        , T iM20, T iM21, T iM22
    );

    /*! Constructor from row vector values. */
    ULIS_MATRIX_FUNC TMatrix3(
          const tColumn& iCol0
        , const tColumn& iCol1
        , const tColumn& iCol2
    );

    /*! Copy constructor. */
    ULIS_MATRIX_FUNC TMatrix3( const TMatrix3& iOther );

    /*! Assignment operator from matrix. */
    ULIS_MATRIX_FUNC TMatrix3& operator=( const TMatrix3& iOther );



    // Conversion
    /*! Conversion constructor from other matrix type. */
    template< typename U >
    ULIS_MATRIX_FUNC TMatrix3( const TMatrix3< U >& iOther );



    // Named Functions
    /*! Return a pointer to the base storage. */
    ULIS_MATRIX_FUNC T* Data();

    /*! Return a pointer to the base storage. */
    ULIS_MATRIX_FUNC const T* Data() const;

    /*! Obtain the inverse of the matrix. */
    ULIS_MATRIX_FUNC TMatrix3 Inverse() const;

    /*! Obtain the transpose of the matrix. */
    ULIS_MATRIX_FUNC TMatrix3 Transpose() const;

    /*! Obtain the determinant of the matrix. */
    ULIS_MATRIX_FUNC T Determinant() const;

    /*! Decompose the matrix if it represents an affine transform */
    ULIS_MATRIX_FUNC void Decompose( T* oTx, T* oTy, T* oRotation, T* oScaleX, T* oScaleY, T* oSkewX, T* oSkewY ) const;

    /*! Apply the projective perspective transform or homography to the point */
    ULIS_MATRIX_FUNC TVector2< T > ApplyHomography( const TVector2< T >& iPoint ) const;

    // Transform Functions
    /*! Static maker for 2D 3x3 rotation matrix. */
    static ULIS_MATRIX_FUNC TMatrix3 MakeRotationMatrix( T iAngleRad );

    /*! Static maker for 2D 3x3 scale matrix. */
    static ULIS_MATRIX_FUNC TMatrix3 MakeScaleMatrix( T iScaleX, T iScaleY );

    /*! Static maker for 2D 3x3 skew matrix. */
    static ULIS_MATRIX_FUNC TMatrix3 MakeSkewMatrix( T iSkewX, T iSkewY );

    /*! Static maker for 2D 3x3 translation matrix. */
    static ULIS_MATRIX_FUNC TMatrix3 MakeTranslationMatrix( T iX, T iY );

    /*! Static maker for homography, 2D perspective transform. */
    static ULIS_MATRIX_FUNC TMatrix3 MakeHomography( const TVector2< T > iSrc[], const TVector2< T > iDst[] );



    // Boolean operators
    /*! Equality operator. */
    ULIS_MATRIX_FUNC bool operator==( const TMatrix3& iOther )  const;

    /*! Inequality operator. */
    ULIS_MATRIX_FUNC bool operator!=( const TMatrix3& iOther )  const;



    // Unary Negative Operator
    /*! Unary negative operator. */
    ULIS_MATRIX_FUNC TMatrix3 operator-() const;



    // Unary arithmetic operator with scalar
    /*! Unary addition operator with value. */
    ULIS_MATRIX_FUNC TMatrix3& operator+=( T iValue );

    /*! Unary substraction operator with value. */
    ULIS_MATRIX_FUNC TMatrix3& operator-=( T iValue );

    /*! Unary multiplication operator with value. */
    ULIS_MATRIX_FUNC TMatrix3& operator*=( T iValue );

    /*! Unary division operator with value. */
    ULIS_MATRIX_FUNC TMatrix3& operator/=( T iValue );



    // Unary arithmetic operator with matrix
    /*! Unary addition operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix3& operator+=( const TMatrix3& iOther );

    /*! Unary substraction operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix3& operator-=( const TMatrix3& iOther );

    /*! Unary multiplication operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix3& operator*=( const TMatrix3& iOther );

    /*! Unary division operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix3& operator/=( const TMatrix3& iOther );



    // Other Operators
    /*! Subscript accessor for matrix columns. */
    ULIS_MATRIX_FUNC tColumn& operator[]( int iIndex );

    /*! Subscript accessor for const matrix columns. */
    ULIS_MATRIX_FUNC const tColumn& operator[]( int iIndex ) const;
};



// Constructors
template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >::TMatrix3()
    : mCols {
          tColumn( 1, 0, 0 )
        , tColumn( 0, 1, 0 )
        , tColumn( 0, 0, 1 )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >::TMatrix3( T iValue )
    : mCols {
          tColumn( iValue, 0, 0 )
        , tColumn( 0, iValue, 0 )
        , tColumn( 0, 0, iValue )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >::TMatrix3(
      T iM00, T iM01, T iM02
    , T iM10, T iM11, T iM12
    , T iM20, T iM21, T iM22
    )
    : mCols {
          tColumn( iM00, iM01, iM02 )
        , tColumn( iM10, iM11, iM12 )
        , tColumn( iM20, iM21, iM22 )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >::TMatrix3(
      const tColumn& iCol0
    , const tColumn& iCol1
    , const tColumn& iCol2
    )
    : mCols {
          iCol0
        , iCol1
        , iCol2
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >::TMatrix3( const TMatrix3< T >& iOther )
    : mCols {
          iOther.mCols[0]
        , iOther.mCols[1]
        , iOther.mCols[2]
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator=( const TMatrix3< T >& iOther ) {
    mCols[0] = iOther.mCols[0];
    mCols[1] = iOther.mCols[1];
    mCols[2] = iOther.mCols[2];
    return  (*this);
}



// Conversion
template< typename T >
template< typename U >
ULIS_MATRIX_FUNC TMatrix3< T >::TMatrix3( const TMatrix3< U >& iOther )
    : mCols {
          tColumn( iOther.mCols[0] )
        , tColumn( iOther.mCols[1] )
        , tColumn( iOther.mCols[2] )
    }
{}



// Named Functions
template< typename T >
ULIS_MATRIX_FUNC
T*
TMatrix3< T >::Data() {
    return  &(mCols[0][0]);
}

template< typename T >
ULIS_MATRIX_FUNC
const T*
TMatrix3< T >::Data() const {
    return  &(mCols[0][0]);
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > TMatrix3< T >::Inverse() const {
    T inv_det = static_cast< T >( 1 ) / Determinant();

    return  TMatrix3< T >(
          + ( mCols[1][1] * mCols[2][2] - mCols[2][1] * mCols[1][2] ) * inv_det
        , - ( mCols[0][1] * mCols[2][2] - mCols[2][1] * mCols[0][2] ) * inv_det
        , + ( mCols[0][1] * mCols[1][2] - mCols[1][1] * mCols[0][2] ) * inv_det
        , - ( mCols[1][0] * mCols[2][2] - mCols[2][0] * mCols[1][2] ) * inv_det
        , + ( mCols[0][0] * mCols[2][2] - mCols[2][0] * mCols[0][2] ) * inv_det
        , - ( mCols[0][0] * mCols[1][2] - mCols[1][0] * mCols[0][2] ) * inv_det
        , + ( mCols[1][0] * mCols[2][1] - mCols[2][0] * mCols[1][1] ) * inv_det
        , - ( mCols[0][0] * mCols[2][1] - mCols[2][0] * mCols[0][1] ) * inv_det
        , + ( mCols[0][0] * mCols[1][1] - mCols[1][0] * mCols[0][1] ) * inv_det
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > TMatrix3< T >::Transpose() const {
    return  TMatrix3< T >(
          mCols[0][0]
        , mCols[1][0]
        , mCols[2][0]
        , mCols[0][1]
        , mCols[1][1]
        , mCols[2][1]
        , mCols[0][2]
        , mCols[1][2]
        , mCols[2][2]
    );
}

template< typename T >
ULIS_MATRIX_FUNC T TMatrix3< T >::Determinant() const {
    return  + mCols[0][0] * ( mCols[1][1] * mCols[2][2] - mCols[2][1] * mCols[1][2] )
            - mCols[1][0] * ( mCols[0][1] * mCols[2][2] - mCols[2][1] * mCols[0][2] )
            + mCols[2][0] * ( mCols[0][1] * mCols[1][2] - mCols[1][1] * mCols[0][2] );
}

template< typename T >
ULIS_MATRIX_FUNC
void
TMatrix3< T >::Decompose(
      T* oTx
    , T* oTy
    , T* oRotation
    , T* oScaleX
    , T* oScaleY
    , T* oSkewX
    , T* oSkewY
) const
{
    T a = mCols[0][0];
    T b = mCols[0][1];
    T c = mCols[1][0];
    T d = mCols[1][1];
    T e = mCols[2][0];
    T f = mCols[2][1];
    T delta = a * d - b * c;

    T tx = e;
    T ty = f;
    T rot = 0;
    T scx = 0;
    T scy = 0;
    T skx = 0;
    T sky = 0;
    // Apply the QR-like decomposition.
    if( a != 0.f || b != 0.f ) {
        T r = FMath::Sqrt( a * a + b * b );
        rot = FMath::Sign( b ) * acosf( a / r );
        scx = r;
        scy = delta / r;
        skx = atanf( ( a * c + b * d ) / ( r * r ) );
        sky = 0;
    } else if( c != 0 || d != 0 ) {
        float s = sqrtf( c * c + d * d );
        rot = FMath::kPIf / 2.f - ( d > 0 ? acosf( -c / s) : - acosf( c / s ) );
        scx = delta / s;
        scy = s;
        skx = 0;
        sky = atanf( ( a * c + b * d ) / ( s * s ) );
    } else {
    }
    *oTx        = tx;
    *oTy        = ty;
    *oRotation  = rot;
    *oScaleX    = scx;
    *oScaleY    = scy;
    *oSkewX     = skx;
    *oSkewY     = sky;
}

template< typename T >
ULIS_MATRIX_FUNC
TVector2< T >
TMatrix3< T >::ApplyHomography( const TVector2< T >& iPoint ) const
{
    T inv_div = 1 / ( iPoint.x * mCols[2][0] + iPoint.y * mCols[2][1] + mCols[2][2] );
    return  TVector2< T >(
          ( iPoint.x * mCols[0][0] + iPoint.y * mCols[0][1] + mCols[0][2] ) * inv_div
        , ( iPoint.x * mCols[1][0] + iPoint.y * mCols[1][1] + mCols[1][2] ) * inv_div
    );
}



// Transform Functions
template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix3< T >
TMatrix3< T >::MakeRotationMatrix( T iAngleRad ) {
    const T cos_angle = cos( iAngleRad );
    const T sin_angle = sin( iAngleRad );
    return  TMatrix3< T >(
          +cos_angle, +sin_angle, 0
        , -sin_angle, +cos_angle, 0
        , 0, 0, 1
    );
}

template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix3< T >
TMatrix3< T >::MakeScaleMatrix( T iScaleX, T iScaleY ) {
    return  TMatrix3< T >(
          iScaleX, 0, 0
        , 0, iScaleY, 0
        , 0, 0, 1
    );
}

template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix3< T >
TMatrix3< T >::MakeSkewMatrix( T iSkewX, T iSkewY ) {
    return  TMatrix3< T >(
          1, iSkewY, 0
        , iSkewX, 1, 0
        , 0, 0, 1
    );
}

template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix3< T >
TMatrix3< T >::MakeTranslationMatrix( T iX, T iY ) {
    return  TMatrix3< T >(
          1, 0, 0
        , 0, 1, 0
        , iX, iY, 1
    );
}

template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix3< T >
TMatrix3< T >::MakeHomography( const TVector2< T > iSrc[], const TVector2< T > iDst[] ) {
    //      c00*xi + c01*yi + c02
    // ui = ---------------------
    //      c20*xi + c21*yi + c22
    //      c10*xi + c11*yi + c12
    // vi = ---------------------
    //      c20*xi + c21*yi + c22
    // Coefficients are calculated by solving linear system:
    // / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \ /c00\ /u0\
    // | x1 y1  1  0  0  0 -x1*u1 -y1*u1 | |c01| |u1|
    // | x2 y2  1  0  0  0 -x2*u2 -y2*u2 | |c02| |u2|
    // | x3 y3  1  0  0  0 -x3*u3 -y3*u3 |.|c10|=|u3|
    // |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 | |c20| |v2|
    // |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 | |c11| |v0|
    // |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 | |c12| |v1|
    // \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 / \c21/ \v3/

    // CB: It would be faster if we solve the system by hand, eliminating any redundant calculations, but i don't have time for this right now.
    // Use inverse matrix approach instead:
    // /c00\ / x0 y0  1  0  0  0 -x0*u0 -y0*u0 \-1  /u0\
    // |c01| | x1 y1  1  0  0  0 -x1*u1 -y1*u1 |    |u1|
    // |c02| | x2 y2  1  0  0  0 -x2*u2 -y2*u2 |    |u2|
    // |c10|=| x3 y3  1  0  0  0 -x3*u3 -y3*u3 |  * |u3|
    // |c20| |  0  0  0 x2 y2  1 -x2*v2 -y2*v2 |    |v2|
    // |c11| |  0  0  0 x0 y0  1 -x0*v0 -y0*v0 |    |v0|
    // |c12| |  0  0  0 x1 y1  1 -x1*v1 -y1*v1 |    |v1|
    // \c21/ \  0  0  0 x3 y3  1 -x3*v3 -y3*v3 /    \v3/
    using Mat8 = TMatrixN< double, double, 8 >;
    using Vec8 = TVectorN< double, double, 8 >;
    TMatrixN< double, double, 8 > mat(
        { iSrc[0].x, iSrc[0].y, 1, 0, 0, 0, static_cast< double >( -iSrc[0].x ) * iDst[0].x, static_cast< double >( - iSrc[0].y ) * iDst[0].x
        , iSrc[1].x, iSrc[1].y, 1, 0, 0, 0, static_cast< double >( -iSrc[1].x ) * iDst[1].x, static_cast< double >( - iSrc[1].y ) * iDst[1].x
        , iSrc[2].x, iSrc[2].y, 1, 0, 0, 0, static_cast< double >( -iSrc[2].x ) * iDst[2].x, static_cast< double >( - iSrc[2].y ) * iDst[2].x
        , iSrc[3].x, iSrc[3].y, 1, 0, 0, 0, static_cast< double >( -iSrc[3].x ) * iDst[3].x, static_cast< double >( - iSrc[3].y ) * iDst[3].x
        , 0, 0, 0, iSrc[2].x, iSrc[2].y, 1, static_cast< double >( -iSrc[2].x ) * iDst[2].y, static_cast< double >( - iSrc[2].y ) * iDst[2].y
        , 0, 0, 0, iSrc[0].x, iSrc[0].y, 1, static_cast< double >( -iSrc[0].x ) * iDst[0].y, static_cast< double >( - iSrc[0].y ) * iDst[0].y
        , 0, 0, 0, iSrc[1].x, iSrc[1].y, 1, static_cast< double >( -iSrc[1].x ) * iDst[1].y, static_cast< double >( - iSrc[1].y ) * iDst[1].y
        , 0, 0, 0, iSrc[3].x, iSrc[3].y, 1, static_cast< double >( -iSrc[3].x ) * iDst[3].y, static_cast< double >( - iSrc[3].y ) * iDst[3].y }
    );

    Vec8 vec( { iDst[0].x, iDst[1].x, iDst[2].x, iDst[3].x, iDst[2].y, iDst[0].y, iDst[1].y, iDst[3].y } );
    Mat8 inverse = mat.Inverse();
    Vec8 res = inverse * vec;
    return  TMatrix3< T >(
          static_cast< T >( res[0] )
        , static_cast< T >( res[1] )
        , static_cast< T >( res[2] )
        , static_cast< T >( res[3] )
        , static_cast< T >( res[4] )
        , static_cast< T >( res[5] )
        , static_cast< T >( res[6] )
        , static_cast< T >( res[7] )
        , static_cast< T >( 1 )
    );
}



// Boolean operators
template< typename T >
ULIS_MATRIX_FUNC bool TMatrix3< T >::operator==( const TMatrix3< T >& iOther ) const {
    return  mCols[0] == iOther.mCols[0]
        &&  mCols[1] == iOther.mCols[1]
        &&  mCols[2] == iOther.mCols[2];
}

template< typename T >
ULIS_MATRIX_FUNC bool TMatrix3< T >::operator!=( const TMatrix3< T >& iOther ) const {
    return  !( *this == iOther );
}



// Unary Negative Operator
template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > TMatrix3< T >::operator-() const {
    return  TMatrix3< T >( -mCols[0], -mCols[1], -mCols[2] );
}



// Unary arithmetic operator with scalar
template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator+=( T iValue ) {
    mCols[0] += iValue;
    mCols[1] += iValue;
    mCols[2] += iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator-=( T iValue ) {
    mCols[0] -= iValue;
    mCols[1] -= iValue;
    mCols[2] -= iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator*=( T iValue ) {
    mCols[0] *= iValue;
    mCols[1] *= iValue;
    mCols[2] *= iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator/=( T iValue ) {
    mCols[0] /= iValue;
    mCols[1] /= iValue;
    mCols[2] /= iValue;
    return  *this;
}



// Unary arithmetic operator with matrix
template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator+=( const TMatrix3& iOther ) {
    mCols[0] += iOther.mCols[0];
    mCols[1] += iOther.mCols[1];
    mCols[2] += iOther.mCols[2];
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator-=( const TMatrix3& iOther ) {
    mCols[0] -= iOther.mCols[0];
    mCols[1] -= iOther.mCols[1];
    mCols[2] -= iOther.mCols[2];
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator*=( const TMatrix3& iOther ) {
    return  ( *this = *this * iOther );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T >& TMatrix3< T >::operator/=( const TMatrix3& iOther ) {
    return  ( *this *= iOther.Inverse() );
}



// Other Operators
template< typename T >
ULIS_MATRIX_FUNC typename TMatrix3< T >::tColumn& TMatrix3< T >::operator[]( int iIndex ) {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 3, "Bad Index" );
    return  mCols[ iIndex ];
}

template< typename T >
ULIS_MATRIX_FUNC const typename TMatrix3< T >::tColumn& TMatrix3< T >::operator[]( int iIndex ) const {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 3, "Bad Index" );
    return  mCols[ iIndex ];
}



// Binary arithmetic operator with scalar on left hand side
template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator+( T iValue, const TMatrix3< T >& iMat ) {
    return  TMatrix3< T >(
          iValue + iMat[0]
        , iValue + iMat[1]
        , iValue + iMat[2]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator-( T iValue, const TMatrix3< T >& iMat ) {
    return  TMatrix3< T >(
          iValue - iMat[0]
        , iValue - iMat[1]
        , iValue - iMat[2]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator*( T iValue, const TMatrix3< T >& iMat ) {
    return  TMatrix3< T >(
          iValue * iMat[0]
        , iValue * iMat[1]
        , iValue * iMat[2]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator/( T iValue, const TMatrix3< T >& iMat ) {
    return  TMatrix3< T >(
          iValue / iMat[0]
        , iValue / iMat[1]
        , iValue / iMat[2]
    );
}



// Binary arithmetic operator with scalar on right hand side
template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator+( const TMatrix3< T >& iMat, T iValue ) {
    return  TMatrix3< T >(
          iMat[0] + iValue
        , iMat[1] + iValue
        , iMat[2] + iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator-( const TMatrix3< T >& iMat, T iValue ) {
    return  TMatrix3< T >(
          iMat[0] - iValue
        , iMat[1] - iValue
        , iMat[2] - iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator*( const TMatrix3< T >& iMat, T iValue ) {
    return  TMatrix3< T >(
          iMat[0] * iValue
        , iMat[1] * iValue
        , iMat[2] * iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator/( const TMatrix3< T >& iMat, T iValue ) {
    return  TMatrix3< T >(
          iMat[0] / iValue
        , iMat[1] / iValue
        , iMat[2] / iValue
    );
}



// Binary arithmetic operator with other matrix
template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator+( const TMatrix3< T >& iMat, const TMatrix3< T >& iOther ) {
    return  TMatrix3< T >(
          iMat[0] + iOther[0]
        , iMat[1] + iOther[1]
        , iMat[2] + iOther[2]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator-( const TMatrix3< T >& iMat, const TMatrix3< T >& iOther ) {
    return  TMatrix3< T >(
          iMat[0] - iOther[0]
        , iMat[1] - iOther[1]
        , iMat[2] - iOther[2]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator*( const TMatrix3< T >& iMat, const TMatrix3< T >& iOther ) {
    return  TMatrix3< T >(
          iMat[0][0] * iOther[0][0] + iMat[1][0] * iOther[0][1] + iMat[2][0] * iOther[0][2]
        , iMat[0][1] * iOther[0][0] + iMat[1][1] * iOther[0][1] + iMat[2][1] * iOther[0][2]
        , iMat[0][2] * iOther[0][0] + iMat[1][2] * iOther[0][1] + iMat[2][2] * iOther[0][2]
        , iMat[0][0] * iOther[1][0] + iMat[1][0] * iOther[1][1] + iMat[2][0] * iOther[1][2]
        , iMat[0][1] * iOther[1][0] + iMat[1][1] * iOther[1][1] + iMat[2][1] * iOther[1][2]
        , iMat[0][2] * iOther[1][0] + iMat[1][2] * iOther[1][1] + iMat[2][2] * iOther[1][2]
        , iMat[0][0] * iOther[2][0] + iMat[1][0] * iOther[2][1] + iMat[2][0] * iOther[2][2]
        , iMat[0][1] * iOther[2][0] + iMat[1][1] * iOther[2][1] + iMat[2][1] * iOther[2][2]
        , iMat[0][2] * iOther[2][0] + iMat[1][2] * iOther[2][1] + iMat[2][2] * iOther[2][2]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix3< T > operator/( const TMatrix3< T >& iMat, const TMatrix3< T >& iOther ) {
    return  iMat * iOther.Inverse();
}



// Binary matrix vector multiplication operators.
template< typename T >
ULIS_MATRIX_FUNC typename TMatrix3< T >::tColumn operator*( const TMatrix3< T >& iMat, const typename TMatrix3< T >::tRow& iRow ) {
    return  TVector3< T >(
          iMat[0][0] * iRow.x + iMat[1][0] * iRow.y + iMat[2][0] * iRow.z
        , iMat[0][1] * iRow.x + iMat[1][1] * iRow.y + iMat[2][1] * iRow.z
        , iMat[0][2] * iRow.x + iMat[1][2] * iRow.y + iMat[2][2] * iRow.z
    );
}

template< typename T >
ULIS_MATRIX_FUNC typename TMatrix3< T >::tRow operator*( const typename TMatrix3< T >::tColumn& iColumn, const TMatrix3< T >& iMat ) {
    return  TVector3< T >(
          iMat[0][0] * iColumn.x + iMat[0][1] * iColumn.y + iMat[0][2] * iColumn.z
        , iMat[1][0] * iColumn.x + iMat[1][1] * iColumn.y + iMat[1][2] * iColumn.z
        , iMat[2][0] * iColumn.x + iMat[2][1] * iColumn.y + iMat[2][2] * iColumn.z
    );
}

ULIS_NAMESPACE_END

