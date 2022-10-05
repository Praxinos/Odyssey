// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Matrix4x4.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the TMatrix4 class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/MatrixFunc.h"
#include "Math/Geometry/Vector.h"
#include "Math/Math.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TMatrix4
/// @brief      The TMatrix4 class provides a simple 4x4 square matrix class for
///             geometry applications.
/// @details    The TMatrix4 class is a template class that provides basic matrix
///             functionnalities, it can be used with any numeric types,
///             ranging from integer values to floating point values.
///
///             TMatrix3 are column-major ordered.
template< typename T >
class TMatrix4
{
public:
    typedef TVector4< T > tRow;
    typedef TVector4< T > tColumn;

private:
    /*! The columns of the matrix. */
    tColumn mCols[4];

public:
    // Constructors
    /*! Default constructor ( identity matrix ). */
    ULIS_MATRIX_FUNC TMatrix4();

    /*! Constructor from input diagonal value. */
    ULIS_MATRIX_FUNC TMatrix4( T iValue );

    /*! Constructor from input cells values. */
    ULIS_MATRIX_FUNC TMatrix4(
          T iM00, T iM01, T iM02, T iM03
        , T iM10, T iM11, T iM12, T iM13
        , T iM20, T iM21, T iM22, T iM23
        , T iM30, T iM31, T iM32, T iM33
    );

    /*! Constructor from row vector values. */
    ULIS_MATRIX_FUNC TMatrix4(
          const tColumn& iCol0
        , const tColumn& iCol1
        , const tColumn& iCol2
        , const tColumn& iCol3
    );

    /*! Copy constructor. */
    ULIS_MATRIX_FUNC TMatrix4( const TMatrix4& iOther );

    /*! Assignment operator from matrix. */
    ULIS_MATRIX_FUNC TMatrix4& operator=( const TMatrix4& iOther );



    // Conversion
    /*! Conversion constructor from other matrix type. */
    template< typename U >
    ULIS_MATRIX_FUNC TMatrix4( const TMatrix4< U >& iOther );



    // Named Functions
    /*! Return a pointer to the base storage. */
    ULIS_MATRIX_FUNC T* Data();

    /*! Return a pointer to the base storage. */
    ULIS_MATRIX_FUNC const T* Data() const;

    /*! Obtain the inverse of the matrix. */
    ULIS_MATRIX_FUNC TMatrix4 Inverse() const;

    /*! Obtain the transpose of the matrix. */
    ULIS_MATRIX_FUNC TMatrix4 Transpose() const;

    /*! Obtain the determinant of the matrix. */
    ULIS_MATRIX_FUNC T Determinant() const;



    // Boolean operators
    /*! Equality operator. */
    ULIS_MATRIX_FUNC bool operator==( const TMatrix4& iOther );

    /*! Inequality operator. */
    ULIS_MATRIX_FUNC bool operator!=( const TMatrix4& iOther );



    // Unary Negative Operator
    /*! Unary negative operator. */
    ULIS_MATRIX_FUNC TMatrix4 operator-() const;



    // Unary arithmetic operator with scalar
    /*! Unary addition operator with value. */
    ULIS_MATRIX_FUNC TMatrix4& operator+=( T iValue );

    /*! Unary substraction operator with value. */
    ULIS_MATRIX_FUNC TMatrix4& operator-=( T iValue );

    /*! Unary multiplication operator with value. */
    ULIS_MATRIX_FUNC TMatrix4& operator*=( T iValue );

    /*! Unary division operator with value. */
    ULIS_MATRIX_FUNC TMatrix4& operator/=( T iValue );



    // Unary arithmetic operator with matrix
    /*! Unary addition operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix4& operator+=( const TMatrix4& iOther );

    /*! Unary substraction operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix4& operator-=( const TMatrix4& iOther );

    /*! Unary multiplication operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix4& operator*=( const TMatrix4& iOther );

    /*! Unary division operator with matrix. */
    ULIS_MATRIX_FUNC TMatrix4& operator/=( const TMatrix4& iOther );



    // Other Operators
    /*! Subscript accessor for matrix columns. */
    ULIS_MATRIX_FUNC tColumn& operator[]( int iIndex );

    /*! Subscript accessor for const matrix columns. */
    ULIS_MATRIX_FUNC const tColumn& operator[]( int iIndex ) const;
};



// Constructors
template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >::TMatrix4()
    : mCols {
          tColumn( 1, 0, 0, 0 )
        , tColumn( 0, 1, 0, 0 )
        , tColumn( 0, 0, 1, 0 )
        , tColumn( 0, 0, 0, 1 )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >::TMatrix4( T iValue )
    : mCols {
          tColumn( iValue, 0, 0, 0 )
        , tColumn( 0, iValue, 0, 0 )
        , tColumn( 0, 0, iValue, 0 )
        , tColumn( 0, 0, 0, iValue )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >::TMatrix4(
      T iM00, T iM01, T iM02, T iM03
    , T iM10, T iM11, T iM12, T iM13
    , T iM20, T iM21, T iM22, T iM23
    , T iM30, T iM31, T iM32, T iM33
    )
    : mCols {
          tColumn( iM00, iM01, iM02, iM03 )
        , tColumn( iM10, iM11, iM12, iM13 )
        , tColumn( iM20, iM21, iM22, iM23 )
        , tColumn( iM30, iM31, iM32, iM33 )
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >::TMatrix4(
      const tColumn& iCol0
    , const tColumn& iCol1
    , const tColumn& iCol2
    , const tColumn& iCol3
    )
    : mCols {
          iCol0
        , iCol1
        , iCol2
        , iCol3
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >::TMatrix4( const TMatrix4< T >& iOther )
    : mCols {
          iOther.mCols[0]
        , iOther.mCols[1]
        , iOther.mCols[2]
        , iOther.mCols[3]
    }
{}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator=( const TMatrix4< T >& iOther ) {
    mCols[0] = iOther.mCols[0];
    mCols[1] = iOther.mCols[1];
    mCols[2] = iOther.mCols[2];
    mCols[3] = iOther.mCols[3];
    return  *this;
}



// Conversion
template< typename T >
template< typename U >
ULIS_MATRIX_FUNC TMatrix4< T >::TMatrix4( const TMatrix4< U >& iOther )
    : mCols {
          tColumn( iOther.mCols[0] )
        , tColumn( iOther.mCols[1] )
        , tColumn( iOther.mCols[2] )
        , tColumn( iOther.mCols[3] )
    }
{}



// Named Functions
template< typename T >
ULIS_MATRIX_FUNC
T*
TMatrix4< T >::Data() {
    return  &(mCols[0][0]);
}

template< typename T >
ULIS_MATRIX_FUNC
const T*
TMatrix4< T >::Data() const {
    return  &(mCols[0][0]);
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > TMatrix4< T >::Inverse() const {
    T coefficient_00 = mCols[2][2] * mCols[3][3] - mCols[3][2] * mCols[2][3];
    T coefficient_02 = mCols[1][2] * mCols[3][3] - mCols[3][2] * mCols[1][3];
    T coefficient_03 = mCols[1][2] * mCols[2][3] - mCols[2][2] * mCols[1][3];
    T coefficient_04 = mCols[2][1] * mCols[3][3] - mCols[3][1] * mCols[2][3];
    T coefficient_06 = mCols[1][1] * mCols[3][3] - mCols[3][1] * mCols[1][3];
    T coefficient_07 = mCols[1][1] * mCols[2][3] - mCols[2][1] * mCols[1][3];
    T coefficient_08 = mCols[2][1] * mCols[3][2] - mCols[3][1] * mCols[2][2];
    T coefficient_10 = mCols[1][1] * mCols[3][2] - mCols[3][1] * mCols[1][2];
    T coefficient_11 = mCols[1][1] * mCols[2][2] - mCols[2][1] * mCols[1][2];
    T coefficient_12 = mCols[2][0] * mCols[3][3] - mCols[3][0] * mCols[2][3];
    T coefficient_14 = mCols[1][0] * mCols[3][3] - mCols[3][0] * mCols[1][3];
    T coefficient_15 = mCols[1][0] * mCols[2][3] - mCols[2][0] * mCols[1][3];
    T coefficient_16 = mCols[2][0] * mCols[3][2] - mCols[3][0] * mCols[2][2];
    T coefficient_18 = mCols[1][0] * mCols[3][2] - mCols[3][0] * mCols[1][2];
    T coefficient_19 = mCols[1][0] * mCols[2][2] - mCols[2][0] * mCols[1][2];
    T coefficient_20 = mCols[2][0] * mCols[3][1] - mCols[3][0] * mCols[2][1];
    T coefficient_22 = mCols[1][0] * mCols[3][1] - mCols[3][0] * mCols[1][1];
    T coefficient_23 = mCols[1][0] * mCols[2][1] - mCols[2][0] * mCols[1][1];

    TVector4< T > factor_0( coefficient_00, coefficient_00, coefficient_02, coefficient_03 );
    TVector4< T > factor_1( coefficient_04, coefficient_04, coefficient_06, coefficient_07 );
    TVector4< T > factor_2( coefficient_08, coefficient_08, coefficient_10, coefficient_11 );
    TVector4< T > factor_3( coefficient_12, coefficient_12, coefficient_14, coefficient_15 );
    TVector4< T > factor_4( coefficient_16, coefficient_16, coefficient_18, coefficient_19 );
    TVector4< T > factor_5( coefficient_20, coefficient_20, coefficient_22, coefficient_23 );
    TVector4< T > vector_0( mCols[1][0], mCols[0][0], mCols[0][0], mCols[0][0] );
    TVector4< T > vector_1( mCols[1][1], mCols[0][1], mCols[0][1], mCols[0][1] );
    TVector4< T > vector_2( mCols[1][2], mCols[0][2], mCols[0][2], mCols[0][2] );
    TVector4< T > vector_3( mCols[1][3], mCols[0][3], mCols[0][3], mCols[0][3] );

    TVector4< T > inverse_0( vector_1 * factor_0 - vector_2 * factor_1 + vector_3 * factor_2 );
    TVector4< T > inverse_1( vector_0 * factor_0 - vector_2 * factor_3 + vector_3 * factor_4 );
    TVector4< T > inverse_2( vector_0 * factor_1 - vector_1 * factor_3 + vector_3 * factor_5 );
    TVector4< T > inverse_3( vector_0 * factor_2 - vector_1 * factor_4 + vector_2 * factor_5 );

    TVector4< T > checkerboard_patternA( +1, -1, +1, -1 );
    TVector4< T > checkerboard_patternB( -1, +1, -1, +1 );
    TMatrix4< T > inverse(
          inverse_0 * checkerboard_patternA
        , inverse_1 * checkerboard_patternB
        , inverse_2 * checkerboard_patternA
        , inverse_3 * checkerboard_patternB
    );

    TVector4< T > row_0( inverse[0][0], inverse[1][0], inverse[2][0], inverse[3][0] );

    TVector4< T > dot0( mCols[0] * row_0 );
    T dot1 = ( dot0.x + dot0.y) + ( dot0.z + dot0.w );

    T inv_det = static_cast< T >( 1 ) / dot1;

    return inverse * inv_det;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > TMatrix4< T >::Transpose() const {
    return  TMatrix4< T >(
          mCols[0][0]
        , mCols[1][0]
        , mCols[2][0]
        , mCols[3][0]
        , mCols[0][1]
        , mCols[1][1]
        , mCols[2][1]
        , mCols[3][1]
        , mCols[0][2]
        , mCols[1][2]
        , mCols[2][2]
        , mCols[3][2]
        , mCols[0][3]
        , mCols[1][3]
        , mCols[2][3]
        , mCols[3][3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC T TMatrix4< T >::Determinant() const {
    const T sub_determinant_00 = (*this)[2][2] * (*this)[3][3] - (*this)[3][2] * (*this)[2][3];
    const T sub_determinant_01 = (*this)[2][1] * (*this)[3][3] - (*this)[3][1] * (*this)[2][3];
    const T sub_determinant_02 = (*this)[2][1] * (*this)[3][2] - (*this)[3][1] * (*this)[2][2];
    const T sub_determinant_03 = (*this)[2][0] * (*this)[3][3] - (*this)[3][0] * (*this)[2][3];
    const T sub_determinant_04 = (*this)[2][0] * (*this)[3][2] - (*this)[3][0] * (*this)[2][2];
    const T sub_determinant_05 = (*this)[2][0] * (*this)[3][1] - (*this)[3][0] * (*this)[2][1];

    TVector4< T > coefficient(
          + ( (*this)[1][1] * sub_determinant_00 - (*this)[1][2] * sub_determinant_01 + (*this)[1][3] * sub_determinant_02 )
        , - ( (*this)[1][0] * sub_determinant_00 - (*this)[1][2] * sub_determinant_03 + (*this)[1][3] * sub_determinant_04 )
        , + ( (*this)[1][0] * sub_determinant_01 - (*this)[1][1] * sub_determinant_03 + (*this)[1][3] * sub_determinant_05 )
        , - ( (*this)[1][0] * sub_determinant_02 - (*this)[1][1] * sub_determinant_04 + (*this)[1][2] * sub_determinant_05 )
    );

    return  (*this)[0][0] * coefficient[0] + (*this)[0][1] * coefficient[1]
         +  (*this)[0][2] * coefficient[2] + (*this)[0][3] * coefficient[3];
}



// Boolean operators
template< typename T >
ULIS_MATRIX_FUNC bool TMatrix4< T >::operator==( const TMatrix4< T >& iOther ) {
    return  mCols[0] == iOther.mCols[0]
        &&  mCols[1] == iOther.mCols[1]
        &&  mCols[2] == iOther.mCols[2]
        &&  mCols[3] == iOther.mCols[3];
}

template< typename T >
ULIS_MATRIX_FUNC bool TMatrix4< T >::operator!=( const TMatrix4< T >& iOther ) {
    return  !( *this == iOther );
}



// Unary Negative Operator
template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > TMatrix4< T >::operator-() const {
    return  TMatrix4< T >( -mCols[0], -mCols[1], -mCols[2], -mCols[3] );
}



// Unary arithmetic operator with scalar
template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator+=( T iValue ) {
    mCols[0] += iValue;
    mCols[1] += iValue;
    mCols[2] += iValue;
    mCols[3] += iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator-=( T iValue ) {
    mCols[0] -= iValue;
    mCols[1] -= iValue;
    mCols[2] -= iValue;
    mCols[3] -= iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator*=( T iValue ) {
    mCols[0] *= iValue;
    mCols[1] *= iValue;
    mCols[2] *= iValue;
    mCols[3] *= iValue;
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator/=( T iValue ) {
    mCols[0] /= iValue;
    mCols[1] /= iValue;
    mCols[2] /= iValue;
    mCols[3] /= iValue;
    return  *this;
}



// Unary arithmetic operator with matrix
template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator+=( const TMatrix4& iOther ) {
    mCols[0] += iOther.mCols[0];
    mCols[1] += iOther.mCols[1];
    mCols[2] += iOther.mCols[2];
    mCols[3] += iOther.mCols[3];
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator-=( const TMatrix4& iOther ) {
    mCols[0] -= iOther.mCols[0];
    mCols[1] -= iOther.mCols[1];
    mCols[2] -= iOther.mCols[2];
    mCols[3] -= iOther.mCols[3];
    return  *this;
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator*=( const TMatrix4& iOther ) {
    return  ( *this = *this * iOther );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T >& TMatrix4< T >::operator/=( const TMatrix4& iOther ) {
    return  ( *this *= iOther.Inverse() );
}



// Other Operators
template< typename T >
ULIS_MATRIX_FUNC typename TMatrix4< T >::tColumn& TMatrix4< T >::operator[]( int iIndex ) {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 4, "Bad Index" );
    return  mCols[ iIndex ];
}

template< typename T >
ULIS_MATRIX_FUNC const typename TMatrix4< T >::tColumn& TMatrix4< T >::operator[]( int iIndex ) const {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 4, "Bad Index" );
    return  mCols[ iIndex ];
}



// Binary arithmetic operator with scalar on left hand side
template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator+( T iValue, const TMatrix4< T >& iMat ) {
    return  TMatrix4< T >(
          iValue + iMat[0]
        , iValue + iMat[1]
        , iValue + iMat[2]
        , iValue + iMat[3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator-( T iValue, const TMatrix4< T >& iMat ) {
    return  TMatrix4< T >(
          iValue - iMat[0]
        , iValue - iMat[1]
        , iValue - iMat[2]
        , iValue - iMat[3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator*( T iValue, const TMatrix4< T >& iMat ) {
    return  TMatrix4< T >(
          iValue * iMat[0]
        , iValue * iMat[1]
        , iValue * iMat[2]
        , iValue * iMat[3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator/( T iValue, const TMatrix4< T >& iMat ) {
    return  TMatrix4< T >(
          iValue / iMat[0]
        , iValue / iMat[1]
        , iValue / iMat[2]
        , iValue / iMat[3]
    );
}



// Binary arithmetic operator with scalar on right hand side
template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator+( const TMatrix4< T >& iMat, T iValue ) {
    return  TMatrix4< T >(
          iMat[0] + iValue
        , iMat[1] + iValue
        , iMat[2] + iValue
        , iMat[3] + iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator-( const TMatrix4< T >& iMat, T iValue ) {
    return  TMatrix4< T >(
          iMat[0] - iValue
        , iMat[1] - iValue
        , iMat[2] - iValue
        , iMat[3] - iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator*( const TMatrix4< T >& iMat, T iValue ) {
    return  TMatrix4< T >(
          iMat[0] * iValue
        , iMat[1] * iValue
        , iMat[2] * iValue
        , iMat[3] * iValue
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator/( const TMatrix4< T >& iMat, T iValue ) {
    return  TMatrix4< T >(
          iMat[0] / iValue
        , iMat[1] / iValue
        , iMat[2] / iValue
        , iMat[3] / iValue
    );
}



// Binary arithmetic operator with other matrix
template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator+( const TMatrix4< T >& iMat, const TMatrix4< T >& iOther ) {
    return  TMatrix4< T >(
          iMat[0] + iOther[0]
        , iMat[1] + iOther[1]
        , iMat[2] + iOther[2]
        , iMat[3] + iOther[3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator-( const TMatrix4< T >& iMat, const TMatrix4< T >& iOther ) {
    return  TMatrix4< T >(
          iMat[0] - iOther[0]
        , iMat[1] - iOther[1]
        , iMat[2] - iOther[2]
        , iMat[3] - iOther[3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator*( const TMatrix4< T >& iMat, const TMatrix4< T >& iOther ) {
    const TVector4< T > colA0 = iMat[0];
    const TVector4< T > colA1 = iMat[1];
    const TVector4< T > colA2 = iMat[2];
    const TVector4< T > colA3 = iMat[3];
    const TVector4< T > colB0 = iOther[0];
    const TVector4< T > colB1 = iOther[1];
    const TVector4< T > colB2 = iOther[2];
    const TVector4< T > colB3 = iOther[3];

    return  TMatrix4< T >(
          colA0 * colB0[0] + colA1 * colB0[1] + colA2 * colB0[2] + colA3 * colB0[3]
        , colA0 * colB1[0] + colA1 * colB1[1] + colA2 * colB1[2] + colA3 * colB1[3]
        , colA0 * colB2[0] + colA1 * colB2[1] + colA2 * colB2[2] + colA3 * colB2[3]
        , colA0 * colB3[0] + colA1 * colB3[1] + colA2 * colB3[2] + colA3 * colB3[3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC TMatrix4< T > operator/( const TMatrix4< T >& iMat, const TMatrix4< T >& iOther ) {
    return  iMat * iOther.Inverse();
}



// Binary matrix vector multiplication operators.
template< typename T >
ULIS_MATRIX_FUNC typename TMatrix4< T >::tColumn operator*( const TMatrix4< T >& iMat, const typename TMatrix4< T >::tRow& iRow ) {
    return  TVector4< T >(
          iMat[0][0] * iRow[0] + iMat[1][0] * iRow[1] + iMat[2][0] * iRow[2] + iMat[3][0] * iRow[3]
        , iMat[0][1] * iRow[0] + iMat[1][1] * iRow[1] + iMat[2][1] * iRow[2] + iMat[3][1] * iRow[3]
        , iMat[0][2] * iRow[0] + iMat[1][2] * iRow[1] + iMat[2][2] * iRow[2] + iMat[3][2] * iRow[3]
        , iMat[0][3] * iRow[0] + iMat[1][3] * iRow[1] + iMat[2][3] * iRow[2] + iMat[3][3] * iRow[3]
    );
}

template< typename T >
ULIS_MATRIX_FUNC typename TMatrix4< T >::tRow operator*( const typename TMatrix4< T >::tColumn& iColumn, const TMatrix4< T >& iMat ) {
    return  TVector4< T >(
          iMat[0][0] * iColumn[0] + iMat[0][1] * iColumn[1] + iMat[0][2] * iColumn[2] + iMat[0][3] * iColumn[3]
        , iMat[1][0] * iColumn[0] + iMat[1][1] * iColumn[1] + iMat[1][2] * iColumn[2] + iMat[1][3] * iColumn[3]
        , iMat[2][0] * iColumn[0] + iMat[2][1] * iColumn[1] + iMat[2][2] * iColumn[2] + iMat[2][3] * iColumn[3]
        , iMat[3][0] * iColumn[0] + iMat[3][1] * iColumn[1] + iMat[3][2] * iColumn[2] + iMat[3][3] * iColumn[3]
    );
}

ULIS_NAMESPACE_END

