// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Vector4.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the TVector4 class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/VectorFunc.h"
#include "Math/Geometry/VectorSwizzle.h"
#include "Math/Math.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TVector4
/// @brief      The TVector4 class provides a simple 4D vector class for vector
///             geometry applications.
/// @details    The TVector4 class is a template class that provides basic 4D
///             vector functionnalities, it can be used with any numeric types,
///             ranging from integer values to floating point values.
///
///             Swizzle operators are also available for all vector types, you
///             can call vec.xxx() or vec.xyyy(), etc.
///
///             For example:
///             \snippet math/geometry/vectorswizzle_snippet.h SwizzleSample
///
///             <a href="md_documentation_manual__operator_overloading.html">Following these guidelines for operator overloading.</a>
template< typename T >
struct TVector4
{
    // Members
    /*! The x component of the 4D vector. */
    T x;

    /*! The y component of the 4D vector. */
    T y;

    /*! The z component of the 4D vector. */
    T z;

    /*! The w component of the 4D vector. */
    T w;



    // Constructors
    /*! Default constructor for the vector, with default zero values. */
    ULIS_VECTOR_FUNC TVector4();

    /*! Construct the vector from input value ( both x and y will have value iValue ) */
    ULIS_VECTOR_FUNC TVector4( T iValue );

    /*! Construct the vector from input values. */
    ULIS_VECTOR_FUNC TVector4( T iX, T iY, T iZ = 0, T iW = 0 );



    // Conversion
    /*! Construct the vector from TVector2. */
    ULIS_VECTOR_FUNC TVector4( const TVector2< T >& iVec, T iZ = 0, T iW = 0 );

    /*! Construct the vector from TVector4. */
    ULIS_VECTOR_FUNC TVector4( const TVector3< T >& iVec, T iW = 0 );

    /*! Construct the vector from input vector of any convertible type. */
    template< typename U >
    ULIS_VECTOR_FUNC TVector4( const TVector4< U >& iOther );

    /*! Construct the vector from foreign type input values. */
    template< typename U, typename V, typename W, typename X >
    ULIS_VECTOR_FUNC TVector4( U iX, V iY, W iZ, X iW );


    // Named Functions
    /*! Return the euclidean distance of the vector. */
    ULIS_VECTOR_FUNC T Distance() const;

    /*!
    Return the squared euclidean distance of the vector.
    This can be useful for testing against another squared distance, thus
    saving a square root calculation.
    */
    ULIS_VECTOR_FUNC T DistanceSquared() const;

    /*! Return the manhattan distance of the vector. ( abs( x ) + abs( y ) ) */
    ULIS_VECTOR_FUNC T ManhattanDistance() const;

    /*! Return the dot product of the vector */
    ULIS_VECTOR_FUNC T DotProduct( const TVector4& iOther ) const;

    /*! Normalize this vector. */
    ULIS_VECTOR_FUNC const TVector4& Normalize();

    /*! Return the normalized version of this vector. */
    ULIS_VECTOR_FUNC TVector4 Normalized() const;

    // Comparison operators
    /*! Compare this vector to the other vector for strict equality. */
    ULIS_VECTOR_FUNC bool operator==( const TVector4& iOther ) const;

    /*! Compare this vector to the other vector for strict inequality. */
    ULIS_VECTOR_FUNC bool operator!=( const TVector4& iOther ) const;



    // Unary Negative Operator
    ULIS_VECTOR_FUNC TVector4 operator-() const;



    // Unary Operators with scalar
    /*! Add the input value to this vector and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator+=( T iValue );

    /*! Substract the input value to this vector and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator-=( T iValue );

    /*! Multiply this vector by the input value and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator*=( T iValue );

    /*! Divide this vector by the input value and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator/=( T iValue );



    // Unary Operators with vector
    /*! Add the input to this vector and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator+=(const TVector4& iOther );

    /*! Substract the input to this vector and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator-=(const TVector4& iOther );

    /*! Multiply this vector by the input vector and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator*=(const TVector4& iOther );

    /*! Divide this vector by the input vector and return a reference to this vector. */
    ULIS_VECTOR_FUNC TVector4& operator/=(const TVector4& iOther );



    // Other Operators
    /*! Access component x or y by index, return a reference to the component. */
    ULIS_VECTOR_FUNC T& operator[]( int iIndex );

    /*! Access component x or y by index, return a const reference to the component. */
    ULIS_VECTOR_FUNC const T& operator[]( int iIndex ) const;

    // Swizzle declaration
    ULIS_DECLARE_ALL_SWIZZLE_FUNCTIONS_VEC4
};



// Constructors
template< typename T >
ULIS_VECTOR_FUNC TVector4< T >::TVector4()
    : x( 0 )
    , y( 0 )
    , z( 0 )
    , w( 0 )
{}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >::TVector4( T iValue )
    : x( iValue )
    , y( iValue )
    , z( iValue )
    , w( iValue )
{}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >::TVector4( T iX, T iY, T iZ, T iW )
    : x( iX )
    , y( iY )
    , z( iZ )
    , w( iW )
{}



// Conversion
template< typename T >
template< typename U >
ULIS_VECTOR_FUNC TVector4< T >::TVector4( const TVector4< U >& iOther )
    : x( static_cast< T >( iOther.x ) )
    , y( static_cast< T >( iOther.y ) )
    , z( static_cast< T >( iOther.z ) )
    , w( static_cast< T >( iOther.w ) )
{}

template< typename T >
template< typename U, typename V, typename W, typename X >
ULIS_VECTOR_FUNC TVector4< T >::TVector4( U iX, V iY, W iZ, X iW )
    : x( static_cast< T >( iX ) )
    , y( static_cast< T >( iY ) )
    , z( static_cast< T >( iZ ) )
    , w( static_cast< T >( iW ) )
{}

// Named Functions
template< typename T >
ULIS_VECTOR_FUNC T TVector4< T >:: Distance() const {
    return  FMath::Sqrt( x * x + y *y + z * z + w * w );
}

template< typename T >
ULIS_VECTOR_FUNC T TVector4< T >::DistanceSquared() const {
    return  x * x + y * y + z *z + w * w;
}

template< typename T >
ULIS_VECTOR_FUNC T TVector4< T >::ManhattanDistance() const {
    return  FMath::Abs( x ) + FMath::Abs( y ) + FMath::Abs( z ) + FMath::Abs( w );
}

template< typename T >
ULIS_VECTOR_FUNC T TVector4< T >::DotProduct( const TVector4& iOther ) const {
    return  x * iOther.x + y * iOther.y + z * iOther.z + w * iOther.w;
}

template< typename T >
ULIS_VECTOR_FUNC const TVector4< T >& TVector4< T >::Normalize() {
    T distance = Distance();
    ULIS_ASSERT( distance != 0, "Division by zero" );
    x /= distance;
    y /= distance;
    z /= distance;
    w /= distance;
    return  *this;
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > TVector4< T >::Normalized() const {
    TVector4< T > result = *this;
    return  result.Normalize();
}



// Comparison operators
template< typename T >
ULIS_VECTOR_FUNC bool TVector4< T >::operator==( const TVector4< T >& iOther ) const {
    return  x == iOther.x && y == iOther.y && z == iOther.z && w == iOther.w;
}

template< typename T >
ULIS_VECTOR_FUNC bool TVector4< T >::operator!=(const TVector4< T >& iOther ) const {
    return  !( *this == iOther );
}



// Unary negative operator
template< typename T >
ULIS_VECTOR_FUNC TVector4< T > TVector4< T >::operator-() const {
    return  TVector4< T >( -x, -y, -z, -w );
}



// Unary Operators with scalar
template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator+=( T iValue ) {
    x += iValue;
    y += iValue;
    z += iValue;
    w += iValue;
    return  *this;
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator-=( T iValue ) {
    x -= iValue;
    y -= iValue;
    z -= iValue;
    w -= iValue;
    return  *this;
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator*=( T iValue ) {
    x *= iValue;
    y *= iValue;
    z *= iValue;
    w *= iValue;
    return  *this;
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator/=( T iValue ) {
    ULIS_ASSERT( iValue != 0, "Division by zero" );
    x /= iValue;
    y /= iValue;
    z /= iValue;
    w /= iValue;
    return  *this;
}



// Unary Operators with vectors
template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator+=(const TVector4< T >& iOther ) {
    x += iOther.x;
    y += iOther.y;
    z += iOther.z;
    w += iOther.w;
    return  *this;
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator-=(const TVector4< T >& iOther ) {
    x -= iOther.x;
    y -= iOther.y;
    z -= iOther.z;
    w -= iOther.w;
    return  *this;
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator*=(const TVector4< T >& iOther ) { 
    x *= iOther.x;
    y *= iOther.y;
    z *= iOther.z;
    w *= iOther.w;
    return  *this;
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T >& TVector4< T >::operator/=(const TVector4& iOther ) {
    ULIS_ASSERT( iOther.x != 0, "Division by zero" );
    ULIS_ASSERT( iOther.y != 0, "Division by zero" );
    ULIS_ASSERT( iOther.z != 0, "Division by zero" );
    ULIS_ASSERT( iOther.w != 0, "Division by zero" );
    x /= iOther.x;
    y /= iOther.y;
    z /= iOther.z;
    w /= iOther.w;
    return  *this;
}



// Other Operators
template< typename T >
ULIS_VECTOR_FUNC T& TVector4< T >::operator[]( int iIndex ) {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 4, "Bad Index" );
    return  ( &x )[ iIndex ];
}

template< typename T >
ULIS_VECTOR_FUNC const T& TVector4< T >::operator[]( int iIndex ) const {
    ULIS_ASSERT( iIndex >= 0 && iIndex < 4, "Bad Index" );
    return  ( &x )[ iIndex ];
}





// Binary Operators with scalar on left hand side
template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator+( T iValue, const TVector4< T >& iVector ) {
    return  TVector4< T >(
          iValue + iVector.x
        , iValue + iVector.y
        , iValue + iVector.z
        , iValue + iVector.w
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator-( T iValue, const TVector4< T >& iVector ) {
    return  TVector4< T >(
          iValue - iVector.x
        , iValue - iVector.y
        , iValue - iVector.z
        , iValue - iVector.w
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator*( T iValue, const TVector4< T >& iVector ) {
    return  TVector4< T >(
          iValue * iVector.x
        , iValue * iVector.y
        , iValue * iVector.z
        , iValue * iVector.w
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator/( T iValue, const TVector4< T >& iVector ) {
    ULIS_ASSERT( iVector.x != 0, "Division by zero" );
    ULIS_ASSERT( iVector.y != 0, "Division by zero" );
    ULIS_ASSERT( iVector.z != 0, "Division by zero" );
    ULIS_ASSERT( iVector.w != 0, "Division by zero" );
    return  TVector4< T >(
          iValue / iVector.x
        , iValue / iVector.y
        , iValue / iVector.z
        , iValue / iVector.w
    );
}



// Binary Operators with scalar on right hand side
template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator+( const TVector4< T >& iVector, T iValue ) {
    return  TVector4< T >(
          iVector.x + iValue
        , iVector.y + iValue
        , iVector.z + iValue
        , iVector.w + iValue
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator-( const TVector4< T >& iVector, T iValue ) {
    return  TVector4< T >(
          iVector.x - iValue
        , iVector.y - iValue
        , iVector.z - iValue
        , iVector.w - iValue
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator*( const TVector4< T >& iVector, T iValue ) {
    return  TVector4< T >(
          iVector.x * iValue
        , iVector.y * iValue
        , iVector.z * iValue
        , iVector.w * iValue
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator/( const TVector4< T >& iVector, T iValue ) {
    ULIS_ASSERT( iValue != 0, "Division by zero" );
    return  TVector4< T >(
          iVector.x / iValue
        , iVector.y / iValue
        , iVector.z / iValue
        , iVector.w / iValue
    );
}


// Binary Operators with vector
template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator+( const TVector4< T >& iVector, const TVector4< T >& iOther ) {
    return  TVector4< T >(
          iVector.x + iOther.x
        , iVector.y + iOther.y
        , iVector.z + iOther.z
        , iVector.w + iOther.w
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator-( const TVector4< T >& iVector, const TVector4< T >& iOther ) {
    return  TVector4< T >(
          iVector.x - iOther.x
        , iVector.y - iOther.y
        , iVector.z - iOther.z
        , iVector.w - iOther.w
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator*( const TVector4< T >& iVector, const TVector4< T >& iOther ) {
    return  TVector4< T >(
          iVector.x * iOther.x
        , iVector.y * iOther.y
        , iVector.z * iOther.z
        , iVector.w * iOther.w
    );
}

template< typename T >
ULIS_VECTOR_FUNC TVector4< T > operator/( const TVector4< T >& iVector, const TVector4< T >& iOther ) {
    ULIS_ASSERT( iOther.x != 0, "Division by zero" );
    ULIS_ASSERT( iOther.y != 0, "Division by zero" );
    ULIS_ASSERT( iOther.z != 0, "Division by zero" );
    ULIS_ASSERT( iOther.w != 0, "Division by zero" );
    return  TVector4< T >(
          iVector.x / iOther.x
        , iVector.y / iOther.y
        , iVector.z / iOther.z
        , iVector.w / iOther.w
    );
}

ULIS_NAMESPACE_END

