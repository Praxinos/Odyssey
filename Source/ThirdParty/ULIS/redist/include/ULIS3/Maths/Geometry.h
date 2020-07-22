// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Geometry.h
* @author       Clement Berthaud
* @brief        This file provides geometry class declarations.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include <cmath>

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// TVec2
template< typename T >
struct TVec2
{
    T x;
    T y;

    TVec2() : x( 0 ), y( 0 ) {}
    TVec2( T iV ) : x( iV ), y( iV ) {};
    TVec2( T ix, T iy ) : x( ix ), y( iy ) {};
    template< typename U > TVec2( U ix, U iy ) : x( static_cast< T >( ix ) ), y( static_cast< T >( iy ) ) {};
    template< typename U > TVec2( const TVec2< U >& iOther ) : x( static_cast< T >( iOther.x ) ), y( static_cast< T >( iOther.y ) ) {};

    ULIS3_FORCEINLINE T Distance() const {                              return  sqrt( x * x + y *y ); }
    ULIS3_FORCEINLINE T DistanceSquared() const {                       return  x * x + y * y; }
    ULIS3_FORCEINLINE T ManhattanDistance() const {                     return  x + y; }
    ULIS3_FORCEINLINE T operator|( const TVec2& iOther ) const {        return  x * iOther.x + y * iOther.y; }
    ULIS3_FORCEINLINE T DotProduct( const TVec2& iOther ) {             return  ( *this | iOther ); }
    ULIS3_FORCEINLINE TVec2 operator+( const TVec2& iOther ) const {    return  TVec2( x + iOther.x, y + iOther.y ); }
    ULIS3_FORCEINLINE TVec2 operator-(const TVec2& iOther ) const {     return  TVec2( x - iOther.x, y - iOther.y ); }
    ULIS3_FORCEINLINE TVec2 operator-( T iValue ) const {               return  TVec2( x - iValue, y - iValue ); }
    ULIS3_FORCEINLINE TVec2 operator+( T iValue ) const {               return  TVec2( x + iValue, y + iValue ); }
    ULIS3_FORCEINLINE TVec2 operator*( T iValue ) const {               return  TVec2( x * iValue, y * iValue ); }
    ULIS3_FORCEINLINE TVec2 operator/( T iValue ) const {               return  TVec2( x / iValue, y / iValue ); }
    ULIS3_FORCEINLINE TVec2 operator*(const TVec2& iOther ) const {     return  TVec2( x * iOther.x, y * iOther.y ); }
    ULIS3_FORCEINLINE TVec2 operator/(const TVec2& iOther ) const {     return  TVec2( x / iOther.x, y / iOther.y ); }
    ULIS3_FORCEINLINE TVec2 operator%(const TVec2& iOther ) const {     return  TVec2( x % iOther.x, y % iOther.y ); }
    ULIS3_FORCEINLINE bool operator==(const TVec2& iOther ) const {     return  x == iOther.x && y == iOther.y; }
    ULIS3_FORCEINLINE bool operator!=(const TVec2& iOther ) const {     return  x != iOther.x || y != iOther.y; }
    ULIS3_FORCEINLINE TVec2 operator-() const {                         return  TVec2( -x, -y ); }
    ULIS3_FORCEINLINE TVec2 operator+=(const TVec2& iOther ) {          x += iOther.x; y += iOther.y; return  *this; }
    ULIS3_FORCEINLINE TVec2 operator-=(const TVec2& iOther ) {          x -= iOther.x; y -= iOther.y; return  *this; }
    ULIS3_FORCEINLINE TVec2 operator*=(T iValue) {                      x *= iValue; y *= iValue; return  *this; }
    ULIS3_FORCEINLINE TVec2 operator/=(T iValue ) {                     x /= iValue; y /= iValue; return  *this; }
    ULIS3_FORCEINLINE TVec2 operator*=(const TVec2& iOther ) {          x *= iOther.x; y *= iOther.y; return  *this; }
    ULIS3_FORCEINLINE TVec2 operator/=(const TVec2& iOther ) {          x /= iOther.x; y /= iOther.y; return  *this; }
    ULIS3_FORCEINLINE TVec2 operator%=(const TVec2& iOther ) {          x %= iOther.x; y %= iOther.y; return  *this; }
    ULIS3_FORCEINLINE T& operator[](int32 iIndex ) { ULIS3_ASSERT( iIndex >= 0 && iIndex < 3, "Bad Index" ); return  ( &x )[ iIndex ]; }
    ULIS3_FORCEINLINE T operator[]( int32 iIndex)const { ULIS3_ASSERT( iIndex >= 0 && iIndex < 3, "Bad Index" ); return  ( &x )[ iIndex ]; }
    ULIS3_FORCEINLINE void Normalize() { T dist = Distance(); x /= dist; y /= dist; }
};

/////////////////////////////////////////////////////
// FRect
struct ULIS3_API FRect
{
    int x;
    int y;
    int w;
    int h;

    FRect();
    FRect( int ix, int iy, int iW, int iH );
    static FRect FromXYWH( int ix, int iy, int iW, int iH );
    static FRect FromMinMax( int ixMin, int iyMin, int ixMax, int iyMax );

    template< class T >
    bool HitTest( const TVec2< T >& iV ) {
        return  iV.x >= static_cast< T >( x )
            &&  iV.x <  static_cast< T >( x + h )
            &&  iV.y >= static_cast< T >( y )
            &&  iV.y <  static_cast< T >( y + h );
    }

    template< class T >
    bool InVerticalRange( T iV ) {
        return  iV >= static_cast< T >( y )
            &&  iV <  static_cast< T >( y + h );
    }

    template< class T >
    bool InHorizontalRange( T iV ) {
        return  iV >= static_cast< T >( x )
            &&  iV <  static_cast< T >( x + w );
    }

    FRect operator&( const FRect& iOther ) const;
    FRect operator|( const FRect& iOther ) const;
    FRect operator-( const FRect& iOther ) const;
    FRect UnionLeaveEmpty( const FRect& iOther ) const;
    bool operator==( const FRect& iOther ) const;
    int Area() const;
    void Sanitize();
    FRect Sanitized();
    void TransformAffine( const FTransform2D& iTransform );
    void TransformPerspective( const FTransform2D& iTransform );
    FRect TransformedAffine( const FTransform2D& iTransform ) const;
    FRect TransformedPerspective( const FTransform2D& iTransform ) const;
    void FitInPositiveRange();
    void Shift( const FVec2I& iVec );
    FVec2I GetShift() const;
};

ULIS3_NAMESPACE_END

