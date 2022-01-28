// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Rect.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the TRectangle class.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Vector.h"
#include "Math/Geometry/Matrix.h"
#include "Math/Math.h"
#include "Memory/Array.h"
#include <vector>

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TRectangle
/// @brief      The TRectangle class provides a simple 2D rectangle class for box
///             geometry applications.
/// @details    The TRectangle class is a template class that provides basic 2D
///             rectangle functionnalities, it can be used with any numeric
///             types, ranging from integer values to floating point values.
///
///             The rectangle representation is stored as x, y, w, h.
template< typename T >
struct TRectangle
{
    /*! The x coordinate of the rectangle. */
    T x;
    /*! The y coordinate of the rectangle. */
    T y;
    /*! The width of the rectangle. */
    T w;
    /*! The height of the rectangle. */
    T h;

    static const TRectangle Auto;

    /*! Default constructor, initializes values to zero. */
    TRectangle< T >()
        : x( 0 )
        , y( 0 )
        , w( 0 )
        , h( 0 )
    {}

    /*! Constructor, from input values. */
    TRectangle< T >( T iX, T iY, T iW, T iH )
        : x( iX )
        , y( iY )
        , w( iW )
        , h( iH )
    {}

    /*! Construct the Rect from input Rect of any convertible type. */
    template< typename U >
    TRectangle< T >( const TRectangle< U >& iOther )
        : x( static_cast< T >( iOther.x ) )
        , y( static_cast< T >( iOther.y ) )
        , w( static_cast< T >( iOther.w ) )
        , h( static_cast< T >( iOther.h ) )
    {}


    /*! Static named maker from input values. */
    static TRectangle< T > FromXYWH( T iX, T iY, T iW, T iH ) {
        return  TRectangle< T >( iX, iY, iW, iH );
    }

    /*! Static named maker from min max input values. */
    static TRectangle< T > FromMinMax( T iXMin, T iYMin, T iXMax, T iYMax ) {
        return  TRectangle< T >( iXMin, iYMin, iXMax - iXMin, iYMax - iYMin );
    }

    /*! Static named maker from pos and size input values. */
    static TRectangle< T > FromPositionAndSize( const TVector2< T >& iPos, const TVector2< T >& iSize ) {
        return  TRectangle< T >( iPos.x, iPos.y, iSize.x, iSize.y );
    }

    /*! Collision test with TVector2. */
    bool HitTest( const TVector2< T >& iV ) const {
        return  iV.x >= x
            &&  iV.y >= y
            &&  iV.x <  (x + h )
            &&  iV.y <  ( y + h );
    }

    /*! Vertical collision test with y coordinate value. */
    bool InVerticalRange( T iV ) const {
        return  iV >= y
            &&  iV <  ( y + h );
    }

    /*! Horizontal collision test with x coordinate value. */
    bool InHorizontalRange( T iV ) const {
        return  iV >= x
            &&  iV <  ( x + w );
    }

    /*! Compute intersection of this Rect with input Rect and return result Rect. */
    TRectangle< T > operator&( const TRectangle< T >& iOther ) const {
        T x1 = FMath::Max( x, iOther.x );
        T y1 = FMath::Max( y, iOther.y );
        T x2 = FMath::Min( x + w, iOther.x + iOther.w );
        T y2 = FMath::Min( y + h, iOther.y + iOther.h );
        return  FromMinMax( x1, y1, x2, y2 );
    }

    /*! Compute union of this Rect with input Rect and return result Rect. */
    TRectangle< T > operator|( const TRectangle< T >& iOther ) const {
        T x1 = FMath::Min( x, iOther.x );
        T y1 = FMath::Min( y, iOther.y );
        T x2 = FMath::Max( x + w, iOther.x + iOther.w );
        T y2 = FMath::Max( y + h, iOther.y + iOther.h );
        return  FromMinMax( x1, y1, x2, y2 );
    }

    /*! Compute exclude of this Rect with input Rect and return result Rect. */
    TRectangle< T > operator-( const TRectangle< T >& iOther ) const {
        TRectangle< T > inter = *this & iOther;
        if( inter.Area() == 0 )
            return  *this;

        if( inter == *this )
            return  TRectangle< T >();

        T x1  = x;
        T y1  = y;
        T x2  = x + w;
        T y2  = y + h;
        T ux1 = inter.x;
        T uy1 = inter.y;
        T ux2 = inter.x + inter.w;
        T uy2 = inter.y + inter.h;

        TRectangle< T > sides[4] = {
              FromMinMax( ux1, y1, ux2, uy1 ) // top
            , FromMinMax( x1, uy1, ux1, uy2 ) // left
            , FromMinMax( ux2, uy1, x2, uy2 ) // right
            , FromMinMax( ux1, uy2, ux2, y2 ) // bot
        };

        std::vector< TRectangle< T >* > vec;
        for( int i = 0; i < 4; ++i )
            if( sides[i].Area() )
                vec.push_back( &sides[i] );

        TRectangle< T > res = *vec[0];
        for( int i = 1; i < vec.size(); ++i )
            res = res | *vec[i];

        return  res;
    }

    /*! Compute exclude of this Rect with input Rect and return result array. */
    void Exclusion( const TRectangle< T >& iOther, TArray< TRectangle< T > >* oResult ) const {
        oResult->Clear();

        TRectangle< T > inter = *this & iOther;
        if( inter.Area() == 0 ) {
            oResult->PushBack( *this );
            return;
        }

        if( inter == *this ) {
            oResult->PushBack( TRectangle< T >() );
            return;
        }

        T x1  = x;
        T y1  = y;
        T x2  = x + w;
        T y2  = y + h;
        T ux1 = inter.x;
        T uy1 = inter.y;
        T ux2 = inter.x + inter.w;
        T uy2 = inter.y + inter.h;

        TRectangle< T > top     = FromMinMax( ux1, y1, ux2, uy1 ); // top
        TRectangle< T > left    = FromMinMax( x1, uy1, ux1, uy2 ); // left
        TRectangle< T > right   = FromMinMax( ux2, uy1, x2, uy2 ); // right
        TRectangle< T > bot     = FromMinMax( ux1, uy2, ux2, y2 ); // bot
        if( top.Area() )    oResult->PushBack( top );
        if( left.Area() )   oResult->PushBack( left );
        if( right.Area() )  oResult->PushBack( right );
        if( bot.Area() )    oResult->PushBack( bot );
        return;
    }

    /*! Compute union of this Rect with input Rect and return result Rect, with safeguards for empty rects */
    TRectangle< T > UnionLeaveEmpty( const TRectangle< T >& iOther ) const {
        if( Area() == 0 )
            return iOther;

        if( iOther.Area() == 0 )
            return  *this;

        return  *this | iOther;
    }

    /*! Strict equality comparison. */
    bool operator==( const TRectangle< T >& iOther ) const {
        return  ( x == iOther.x && y == iOther.y && w == iOther.w && h == iOther.h );
    }

    /*! Strict inequality comparison. */
    bool operator!=( const TRectangle< T >& iOther ) const {
        return  !operator==( iOther );
    }

    /*! Return the area of the rect. */
    T Area() const {
        return  w * h;
    }

    /*! Sanitize the Rect by setting negative sizes to zero. */
    void Sanitize() {
        if( h < 0 )
            h = 0;

        if( w < 0 )
            w = 0;
    }

    /*! Return the sanitized version of this rect. */
    TRectangle< T > Sanitized() const {
        TRectangle< T > ret = *this;
        ret.Sanitize();
        return  ret;
    }

    /*! Affine transform this rect by input transform ( AABB ). */
    void TransformAffine( const FMat3F& iTransform ) {
        float src_x2 = static_cast< float >( x + w );
        float src_y2 = static_cast< float >( y + h );
        const FMat3F& mat = iTransform;
        FVec3F m00 = mat * FVec3F( x, y, 1 );
        FVec3F m10 = mat * FVec3F( src_x2, y, 1 );
        FVec3F m11 = mat * FVec3F( src_x2, src_y2, 1 );
        FVec3F m01 = mat * FVec3F( x, src_y2, 1 );
        x = static_cast< T >( FMath::RoundToNegativeInfinity( FMath::Min4( m00.x, m10.x, m11.x, m01.x ) ) );
        y = static_cast< T >( FMath::RoundToNegativeInfinity( FMath::Min4( m00.y, m10.y, m11.y, m01.y ) ) );
        w = static_cast< T >( FMath::RoundToPositiveInfinity( FMath::Max4( m00.x, m10.x, m11.x, m01.x ) ) ) - x;
        h = static_cast< T >( FMath::RoundToPositiveInfinity( FMath::Max4( m00.y, m10.y, m11.y, m01.y ) ) ) - y;
    }

    /*! Perspective transform this rect by input transform ( AABB ). */
    void TransformPerspective( const FMat3F& iTransform ) {
        float x1 = static_cast< float >( x );
        float y1 = static_cast< float >( y );
        float x2 = static_cast< float >( x + w );
        float y2 = static_cast< float >( y + h );
        const FMat3F& mat = iTransform;
        FVec2F A = mat.ApplyHomography( FVec2F( x1, y1 ) );
        FVec2F B = mat.ApplyHomography( FVec2F( x2, y1 ) );
        FVec2F C = mat.ApplyHomography( FVec2F( x2, y2 ) );
        FVec2F D = mat.ApplyHomography( FVec2F( x1, y2 ) );
        x = static_cast< T >( FMath::RoundToNegativeInfinity( FMath::Min4( A.x, B.x, C.x, D.x ) ) );
        y = static_cast< T >( FMath::RoundToNegativeInfinity( FMath::Min4( A.y, B.y, C.y, D.y ) ) );
        w = static_cast< T >( FMath::RoundToPositiveInfinity( FMath::Max4( A.x, B.x, C.x, D.x ) ) ) - x;
        h = static_cast< T >( FMath::RoundToPositiveInfinity( FMath::Max4( A.y, B.y, C.y, D.y ) ) ) - y;
    }

    /*! Return the affine transformed version of this rect ( AABB ). */
    TRectangle< T > TransformedAffine( const FMat3F& iTransform ) const {
        TRectangle< T > result = *this;
        result.TransformAffine( iTransform );
        return  result;
    }

    /*! Return the perspective transformed version of this rect ( AABB ). */
    TRectangle< T > TransformedPerspective( const FMat3F& iTransform ) const {
        TRectangle< T > result = *this;
        result.TransformPerspective( iTransform );
        return  result;
    }

    /*! Shift x and y coordinates by input vector. */
    TRectangle< T >& Shift( const TVector2< T >& iVec ) {
        x += iVec.x;
        y += iVec.y;
        return  *this;
    }

    /*! Get x and y coordinates as vector. */
    TVector2< T > Position() const {
        return  TVector2< T >( x, y );
    }

    /*! Get the rectuangle size as vector. */
    TVector2< T > Size() const {
        return  TVector2< T >( w, h );
    }
};

template< typename T >
const TRectangle< T > TRectangle< T >::Auto = TRectangle< T >( 0, 0, std::numeric_limits< T >::max(), std::numeric_limits< T >::max() );

ULIS_NAMESPACE_END

