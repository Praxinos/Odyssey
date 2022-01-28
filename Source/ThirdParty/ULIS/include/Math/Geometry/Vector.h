// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Vector.h
* @author       Clement Berthaud
* @brief        This file provides a central point to include vector types.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Vector2.h"
#include "Math/Geometry/Vector3.h"
#include "Math/Geometry/Vector4.h"
#include "Math/Geometry/VectorFunc.h"
#include "Math/Geometry/VectorSwizzle.h"

ULIS_NAMESPACE_BEGIN
// Conversion constructors for vectors 2
template< typename T >
ULIS_VECTOR_FUNC
TVector2< T >::TVector2( const TVector3< T >& iVec )
    : x( iVec.x )
    , y( iVec.y )
{}

template< typename T >
ULIS_VECTOR_FUNC
TVector2< T >::TVector2( const TVector4< T >& iVec )
    : x( iVec.x )
    , y( iVec.y )
{}

// Conversion constructors for vectors 3
template< typename T >
ULIS_VECTOR_FUNC
TVector3< T >::TVector3( const TVector2< T >& iVec, T iZ )
    : x( iVec.x )
    , y( iVec.y )
    , z( iZ )
{}

template< typename T >
ULIS_VECTOR_FUNC
TVector3< T >::TVector3( const TVector4< T >& iVec )
    : x( iVec.x )
    , y( iVec.y )
    , z( iVec.z )
{}

// Conversion constructors for vectors 4
template< typename T >
ULIS_VECTOR_FUNC
TVector4< T >::TVector4( const TVector2< T >& iVec, T iZ, T iW )
    : x( iVec.x )
    , y( iVec.y )
    , z( iZ )
    , w( iW )
{}

template< typename T >
ULIS_VECTOR_FUNC
TVector4< T >::TVector4( const TVector3< T >& iVec, T iW )
    : x( iVec.x )
    , y( iVec.y )
    , z( iVec.z )
    , w( iW )
{}

// Swizzle definitions
ULIS_DEFINE_ALL_SWIZZLE_FUNCTIONS_VEC2
ULIS_DEFINE_ALL_SWIZZLE_FUNCTIONS_VEC3
ULIS_DEFINE_ALL_SWIZZLE_FUNCTIONS_VEC4

// Template instanciations exports
template struct ULIS_API TVector2< float >;
template struct ULIS_API TVector2< int >;
template struct ULIS_API TVector3< float >;
template struct ULIS_API TVector3< int >;
template struct ULIS_API TVector4< float >;
template struct ULIS_API TVector4< int >;

ULIS_NAMESPACE_END

