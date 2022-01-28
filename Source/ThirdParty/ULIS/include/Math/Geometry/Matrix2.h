// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Matrix2x2.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the TMatrix2 class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/MatrixFunc.h"
#include "Math/Geometry/Vector.h"
#include "Math/Math.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TMatrix2
/// @brief      The TMatrix2 class provides a simple 2x2 square matrix class for
///             geometry applications.
/// @details    The TMatrix2 class is a template class that provides basic matrix
///             functionnalities, it can be used with any numeric types,
///             ranging from integer values to floating point values.
///
///             TMatrix2 are column-major ordered.
template< typename T >
class TMatrix2
{
public:
    typedef TVector2< T > tRow;
    typedef TVector2< T > tColumn;

private:
    /*! The columns of the matrix. */
    tColumn mCols[2];

public:
    // Constructors
    /*! Default constructor ( identity matrix ). */
    ULIS_MATRIX_FUNC TMatrix2();

    /*! Constructor from input diagonal value. */
    ULIS_MATRIX_FUNC TMatrix2( T iValue );

    /*! Constructor from input cells values. */
    ULIS_MATRIX_FUNC TMatrix2( T iM00, T iM01, T iM10, T iM11 );

    /*! Constructor from row vector values. */
    ULIS_MATRIX_FUNC TMatrix2( const tColumn& iCol0, const tColumn& iCol1 );

    /*! Copy constructor. */
    ULIS_MATRIX_FUNC TMatrix2( const TMatrix2& iOther );

    /*! Assignment operator from matrix. */
    ULIS_MATRIX_FUNC TMatrix2& operator=( const TMatrix2& iOther );



    // Conversion
    /*! Conversion constructor from other matrix type. */
    template< typename U >
    ULIS_MATRIX_FUNC TMatrix2( const TMatrix2< U >& iOther );



    // Named Functions
    /*! Return a pointer to the base storage. */
    ULIS_MATRIX_FUNC T* Data();

    /*! Return a pointer to the base storage. */
    ULIS_MATRIX_FUNC const T* Data() const;

    /*! Obtain the inverse of the matrix. */
    ULIS_MATRIX_FUNC TMatrix2 Inverse() const;

    /*! Obtain the transpose of the matrix. */
    ULIS_MATRIX_FUNC TMatrix2 Transpose() const;

    /*! Obtain the determinant of the matrix. */
    ULIS_MATRIX_FUNC T Determinant() const;



    // Transform Functions
    /*! Static maker for 2D 2x2 rotation matrix. */
    static ULIS_MATRIX_FUNC TMatrix2 MakeRotationMatrix( T iAngleRad );

    /*! Static maker for 2D 2x2 scale matrix. */
    static ULIS_MATRIX_FUNC TMatrix2 MakeScaleMatrix( T iScaleX, T iScaleY );

    /*! Static maker for 2D 2x2 skew matrix. */
    static ULIS_MATRIX_FUNC TMatrix2 MakeSkewMatrix( T iSkewX, T iSkewY );



    // Boolean operators
    /*! Equality operator. */
    ULIS_MATRIX_FUNC bool operator==( const TMatrix2& iOther );

    /*! Inequality operator. */
    ULIS_MATRIX_FUNC bool operator!=( const TMatrix2& iOther );



    // Unary Negative Operator
    /*! Unary negative operator. */
    ULIS_MATRIX_FUNC TMatrix2 operator-() const;



    // Unary arithmetic operator with scalar
    /*! Unary addition operator with value. */
    ULIS_MATRIX_FUNC TMatrix2& operator+=( T iValue );

    /*! Unary substraction operator with value. */
    ULIS_MATRIX_FUNC TMatrix2& operator-=( T iValue );

    /*! Unary multiplication operator with value. */
    ULIS_MATRIX_FUNC TMatrix2& operator*=( T iValue );

    /*! Unary division operator with value. */
    ULIS_MATRIX_FUNC TMatrix2& operator/=( T iValue );



    // Unary arithmetic operator with matrix
    /*! Unary addition operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix2& operator+=( const TMatrix2& iOther );

    /*! Unary substraction operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix2& operator-=( const TMatrix2& iOther );

    /*! Unary multiplication operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix2& operator*=( const TMatrix2& iOther );

    /*! Unary division operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix2& operator/=( const TMatrix2& iOther );



    // Other Operators
    /*! Subscript accessor for matrix columns. */
    ULIS_MATRIX_FUNC tColumn& operator[]( int iIndex );

    /*! Subscript accessor for const matrix columns. */
    ULIS_MATRIX_FUNC const tColumn& operator[]( int iIndex ) const;
};



// Constructors
template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >::TMatrix2()
    : mCols {
          tColumn( 1, 0 )
        , tColumn( 0, 1 )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >::TMatrix2( T iValue )
    : mCols {
          tColumn( iValue, 0 )
        , tColumn( 0, iValue )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >::TMatrix2( T iM00, T iM01, T iM10, T iM11 )
    : mCols {
          tColumn( iM00, iM01 )
        , tColumn( iM10, iM11 )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >::TMatrix2( const tColumn& iCol0, const tColumn& iCol1 )
    : mCols {
          iCol0
        , iCol1
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >::TMatrix2( const TMatrix2< T >& iOther )
    : mCols {
          iOther.mCols[0]
        , iOther.mCols[1]
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator=( const TMatrix2< T >& iOther ) {
    mCols[0] = iOther.mCols[0];
    mCols[1] = iOther.mCols[1];
    return  *this;
}


// Conversion
template< typename T >
template< typename U >
ULIS_MATRIX_FUNC TMatrix2< T >::TMatrix2( const TMatrix2< U >& iOther )
    : mCols {
          tColumn( iOther.mCols[0] )
        , tColumn( iOther.mCols[1] )
    }
{}



// Named Functions
template< typename T >
ULIS_MATRIX_FUNC
T*
TMatrix2< T >::Data() {
    return  &(mCols[0][0]);
}

template< typename T >
ULIS_MATRIX_FUNC
const T*
TMatrix2< T >::Data() const {
    return  &(mCols[0][0]);
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > TMatrix2< T >::Inverse() const {
    T inv_det = static_cast< T >( 1 ) / Determinant();
    return  TMatrix2< T >(
          + mCols[1][1] * inv_det
        , - mCols[0][1] * inv_det
        , - mCols[1][0] * inv_det
        , + mCols[0][0] * inv_det
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > TMatrix2< T >::Transpose() const {
    return  TMatrix2< T >(
          mCols[0][0]
        , mCols[0][1]
        , mCols[1][0]
        , mCols[1][1]
    );
}

template< typename T >
ULIS_MATRIX_FUNC T TMatrix2< T >::Determinant() const {
    return  mCols[0][0] * mCols[1][1] - mCols[1][0] * mCols[0][1];
}



// Transform Functions
template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix2< T >
TMatrix2< T >::MakeRotationMatrix( T iAngleRad ) {
    const T cos_angle = cos( iAngleRad );
    const T sin_angle = sin( iAngleRad );
    return  TMatrix2< T >(
          cos_angle
        , sin_angle
        , -sin_angle
        , cos_angle
    );
}

template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix2< T >
TMatrix2< T >::MakeScaleMatrix( T iScaleX, T iScaleY ) {
    return  TMatrix2< T >(
          iScaleX
        , 0
        , 0
        , iScaleY
    );
}

template< typename T >
//static
ULIS_MATRIX_FUNC
TMatrix2< T >
TMatrix2< T >::MakeSkewMatrix( T iSkewX, T iSkewY ) {
    return  TMatrix2< T >(
          1
        , iSkewY
        , iSkewX
        , 1
    );
}



// Boolean operators
template< typename T >
ULIS_MATRIX_FUNC bool TMatrix2< T >::operator==( const TMatrix2< T >& iOther ) {
    return  mCols[0] == iOther.mCols[0] && mCols[1] == iOther.mCols[1];
}

template< typename T >
ULIS_MATRIX_FUNC bool TMatrix2< T >::operator!=( const TMatrix2< T >& iOther ) {
    return  !( *this == iOther );
}



// Unary Negative Operator
template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > TMatrix2< T >::operator-() const {
    return  TMatrix2< T >( -mCols[0], -mCols[1] );
}



// Unary arithmetic operator with scalar
template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator+=( T iValue ) {
    mCols[0] += iValue;
    mCols[1] += iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator-=( T iValue ) {
    mCols[0] -= iValue;
    mCols[1] -= iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator*=( T iValue ) {
    mCols[0] *= iValue;
    mCols[1] *= iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator/=( T iValue ) {
    mCols[0] /= iValue;
    mCols[1] /= iValue;
    return  *this;
}



// Unary arithmetic operator with matrix
template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator+=( const TMatrix2& iOther ) {
    mCols[0] += iOther.mCols[0];
    mCols[1] += iOther.mCols[1];
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator-=( const TMatrix2& iOther ) {
    mCols[0] -= iOther.mCols[0];
    mCols[1] -= iOther.mCols[1];
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator*=( const TMatrix2& iOther ) {
    return  ( *this = *this * iOther );
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T >& TMatrix2< T >::operator/=( const TMatrix2& iOther ) {
    return  ( *this *= iOther.Inverse() );
    return  *this;
}



// Other Operators
template< typename T >
ULIS_MATRIX_FUNC typename TMatrix2< T >::tColumn& TMatrix2< T >::operator[]( int iIndex ) {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 2, "Bad Index" );
    return  mCols[ iIndex ];
}

template< typename T >
ULIS_MATRIX_FUNC const typename TMatrix2< T >::tColumn& TMatrix2< T >::operator[]( int iIndex ) const {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 2, "Bad Index" );
    return  mCols[ iIndex ];
}



// Binary arithmetic operator with scalar on left hand side
template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator+( T iValue, const TMatrix2< T >& iMat ) {
    return  TMatrix2< T >(
          iValue + iMat[0]
        , iValue + iMat[1]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator-( T iValue, const TMatrix2< T >& iMat ) {
    return  TMatrix2< T >(
          iValue - iMat[0]
        , iValue - iMat[1]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator*( T iValue, const TMatrix2< T >& iMat ) {
    return  TMatrix2< T >(
          iValue * iMat[0]
        , iValue * iMat[1]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator/( T iValue, const TMatrix2< T >& iMat ) {
    return  TMatrix2< T >(
          iValue / iMat[0]
        , iValue / iMat[1]
    );
}



// Binary arithmetic operator with scalar on right hand side
template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator+( const TMatrix2< T >& iMat, T iValue ) {
    return  TMatrix2< T >(
          iMat[0] + iValue
        , iMat[1] + iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator-( const TMatrix2< T >& iMat, T iValue ) {
    return  TMatrix2< T >(
          iMat[0] - iValue
        , iMat[1] - iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator*( const TMatrix2< T >& iMat, T iValue ) {
    return  TMatrix2< T >(
          iMat[0] * iValue
        , iMat[1] * iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator/( const TMatrix2< T >& iMat, T iValue ) {
    return  TMatrix2< T >(
          iMat[0] / iValue
        , iMat[1] / iValue
    );
}



// Binary arithmetic operator with other matrix
template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator+( const TMatrix2< T >& iMat, const TMatrix2< T >& iOther ) {
    return  TMatrix2< T >(
          iMat[0] + iOther[0]
        , iMat[1] + iOther[1]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator-( const TMatrix2< T >& iMat, const TMatrix2< T >& iOther ) {
    return  TMatrix2< T >(
          iMat[0] - iOther[0]
        , iMat[1] - iOther[1]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator*( const TMatrix2< T >& iMat, const TMatrix2< T >& iOther ) {
    return  TMatrix2< T >(
          iMat[0][0] * iOther[0][0] + iMat[1][0] * iOther[0][1]
        , iMat[0][1] * iOther[0][0] + iMat[1][1] * iOther[0][1]
        , iMat[0][0] * iOther[1][0] + iMat[1][0] * iOther[1][1]
        , iMat[0][1] * iOther[1][0] + iMat[1][1] * iOther[1][1]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix2< T > operator/( const TMatrix2< T >& iMat, const TMatrix2< T >& iOther ) {
    return  iMat * iOther.Inverse();
}



// Binary matrix vector multiplication operators.
template< typename T >
ULIS_MATRIX_FUNC typename TMatrix2< T >::tColumn operator*( const TMatrix2< T >& iMat, const typename TMatrix2< T >::tRow& iRow ) {
    return  TVector2< T >(
          iMat[0][0] * iRow.x + iMat[1][0] * iRow.y
        , iMat[0][1] * iRow.x + iMat[1][1] * iRow.y
    );
}

template< typename T >
ULIS_MATRIX_FUNC typename TMatrix2< T >::tRow operator*( const typename TMatrix2< T >::tColumn& iColumn, const TMatrix2< T >& iMat ) {
    return  TVector2< T >(
          iColumn.x * iMat[0][0] + iColumn.y * iMat[0][1]
        , iColumn.x * iMat[1][0] + iColumn.y * iMat[1][1]
    );
}

ULIS_NAMESPACE_END

