// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         VecSwizzle.h
* @author       Clement Berthaud
* @brief        This file provides the macros for the Vec swizzle functions.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/VectorFunc.h"

// Vec2
#define ULIS_DECLARE_VEC2_SWIZZLE_FUNCTION( CLASS, E0, E1 ) \
    /*! Swizzle E0 ## E1 */                                 \
    ULIS_VECTOR_FUNC TVector2< T > E0 ## E1 () const;

#define ULIS_DEFINE_VEC2_SWIZZLE_FUNCTION( CLASS, E0, E1 )              \
    template< typename T >                                              \
    ULIS_VECTOR_FUNC TVector2< T > CLASS < T >:: E0 ## E1 () const {    \
        return  TVector2< T >( E0, E1 );                                \
    }

// Vec3
#define ULIS_DECLARE_VEC3_SWIZZLE_FUNCTION( CLASS, E0, E1, E2 ) \
    /*! Swizzle E0 ## E1 ## E2 */                               \
    ULIS_VECTOR_FUNC TVector3< T > E0 ## E1 ## E2 () const;

#define ULIS_DEFINE_VEC3_SWIZZLE_FUNCTION( CLASS, E0, E1, E2 )              \
    template< typename T >                                                  \
    ULIS_VECTOR_FUNC TVector3< T > CLASS < T >:: E0 ## E1 ## E2 () const {  \
        return  TVector3< T >( E0, E1, E2 );                                \
    }

// Vec4
#define ULIS_DECLARE_VEC4_SWIZZLE_FUNCTION( CLASS, E0, E1, E2, E3 ) \
    /*! Swizzle E0 ## E1 ## E2 ## E3 */                             \
    ULIS_VECTOR_FUNC TVector4< T > E0 ## E1 ## E2 ## E3 () const;

#define ULIS_DEFINE_VEC4_SWIZZLE_FUNCTION( CLASS, E0, E1, E2, E3 )                  \
    template< typename T >                                                          \
    ULIS_VECTOR_FUNC TVector4< T > CLASS < T >:: E0 ## E1 ## E2 ## E3 () const {    \
        return  TVector4< T >( E0, E1, E2, E3 );                                    \
    }

// Vec2
#define ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ACTION )    \
    ACTION ( CLASS, x, x )                                              \
    ACTION ( CLASS, x, y )                                              \
    ACTION ( CLASS, y, x )                                              \
    ACTION ( CLASS, y, y )

#define ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ACTION )    \
    ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ACTION )        \
    ACTION ( CLASS, x, z )                                              \
    ACTION ( CLASS, y, z )                                              \
    ACTION ( CLASS, z, x )                                              \
    ACTION ( CLASS, z, y )                                              \
    ACTION ( CLASS, z, z )

#define ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS, ACTION )    \
    ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ACTION )        \
    ACTION ( CLASS, x, w )                                              \
    ACTION ( CLASS, y, w )                                              \
    ACTION ( CLASS, z, w )                                              \
    ACTION ( CLASS, w, x )                                              \
    ACTION ( CLASS, w, y )                                              \
    ACTION ( CLASS, w, z )                                              \
    ACTION ( CLASS, w, w )

// Vec3
#define ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ACTION )    \
    ACTION ( CLASS, x, x, x )                                           \
    ACTION ( CLASS, x, x, y )                                           \
    ACTION ( CLASS, x, y, x )                                           \
    ACTION ( CLASS, x, y, y )                                           \
    ACTION ( CLASS, y, x, x )                                           \
    ACTION ( CLASS, y, x, y )                                           \
    ACTION ( CLASS, y, y, x )                                           \
    ACTION ( CLASS, y, y, y )

#define ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ACTION )    \
    ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ACTION )        \
    ACTION ( CLASS, x, x, z )                                           \
    ACTION ( CLASS, x, y, z )                                           \
    ACTION ( CLASS, x, z, x )                                           \
    ACTION ( CLASS, x, z, y )                                           \
    ACTION ( CLASS, x, z, z )                                           \
    ACTION ( CLASS, y, x, z )                                           \
    ACTION ( CLASS, y, y, z )                                           \
    ACTION ( CLASS, y, z, x )                                           \
    ACTION ( CLASS, y, z, y )                                           \
    ACTION ( CLASS, y, z, z )                                           \
    ACTION ( CLASS, z, x, x )                                           \
    ACTION ( CLASS, z, x, y )                                           \
    ACTION ( CLASS, z, x, z )                                           \
    ACTION ( CLASS, z, y, x )                                           \
    ACTION ( CLASS, z, y, y )                                           \
    ACTION ( CLASS, z, y, z )                                           \
    ACTION ( CLASS, z, z, x )                                           \
    ACTION ( CLASS, z, z, y )                                           \
    ACTION ( CLASS, z, z, z )                                           \

#define ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS, ACTION )    \
    ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ACTION )        \
    ACTION ( CLASS, x, x, w )                                           \
    ACTION ( CLASS, x, y, w )                                           \
    ACTION ( CLASS, x, z, w )                                           \
    ACTION ( CLASS, x, w, x )                                           \
    ACTION ( CLASS, x, w, y )                                           \
    ACTION ( CLASS, x, w, z )                                           \
    ACTION ( CLASS, x, w, w )                                           \
    ACTION ( CLASS, y, x, w )                                           \
    ACTION ( CLASS, y, y, w )                                           \
    ACTION ( CLASS, y, z, w )                                           \
    ACTION ( CLASS, y, w, x )                                           \
    ACTION ( CLASS, y, w, y )                                           \
    ACTION ( CLASS, y, w, z )                                           \
    ACTION ( CLASS, y, w, w )                                           \
    ACTION ( CLASS, z, x, w )                                           \
    ACTION ( CLASS, z, y, w )                                           \
    ACTION ( CLASS, z, z, w )                                           \
    ACTION ( CLASS, z, w, x )                                           \
    ACTION ( CLASS, z, w, y )                                           \
    ACTION ( CLASS, z, w, z )                                           \
    ACTION ( CLASS, z, w, w )                                           \
    ACTION ( CLASS, w, x, x )                                           \
    ACTION ( CLASS, w, x, y )                                           \
    ACTION ( CLASS, w, x, z )                                           \
    ACTION ( CLASS, w, x, w )                                           \
    ACTION ( CLASS, w, y, x )                                           \
    ACTION ( CLASS, w, y, y )                                           \
    ACTION ( CLASS, w, y, z )                                           \
    ACTION ( CLASS, w, y, w )                                           \
    ACTION ( CLASS, w, z, x )                                           \
    ACTION ( CLASS, w, z, y )                                           \
    ACTION ( CLASS, w, z, z )                                           \
    ACTION ( CLASS, w, z, w )                                           \
    ACTION ( CLASS, w, w, x )                                           \
    ACTION ( CLASS, w, w, y )                                           \
    ACTION ( CLASS, w, w, z )                                           \
    ACTION ( CLASS, w, w, w )

// Vec4
#define ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ACTION )    \
    ACTION ( CLASS, x, x, x, x )                                        \
    ACTION ( CLASS, x, x, x, y )                                        \
    ACTION ( CLASS, x, x, y, x )                                        \
    ACTION ( CLASS, x, x, y, y )                                        \
    ACTION ( CLASS, x, y, x, x )                                        \
    ACTION ( CLASS, x, y, x, y )                                        \
    ACTION ( CLASS, x, y, y, x )                                        \
    ACTION ( CLASS, x, y, y, y )                                        \
    ACTION ( CLASS, y, x, x, x )                                        \
    ACTION ( CLASS, y, x, x, y )                                        \
    ACTION ( CLASS, y, x, y, x )                                        \
    ACTION ( CLASS, y, x, y, y )                                        \
    ACTION ( CLASS, y, y, x, x )                                        \
    ACTION ( CLASS, y, y, x, y )                                        \
    ACTION ( CLASS, y, y, y, x )                                        \
    ACTION ( CLASS, y, y, y, y )

#define ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ACTION )    \
    ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ACTION )        \
    ACTION ( CLASS, x, x, x, z )                                        \
    ACTION ( CLASS, x, x, y, z )                                        \
    ACTION ( CLASS, x, x, z, x )                                        \
    ACTION ( CLASS, x, x, z, y )                                        \
    ACTION ( CLASS, x, x, z, z )                                        \
    ACTION ( CLASS, x, y, x, z )                                        \
    ACTION ( CLASS, x, y, y, z )                                        \
    ACTION ( CLASS, x, y, z, x )                                        \
    ACTION ( CLASS, x, y, z, y )                                        \
    ACTION ( CLASS, x, y, z, z )                                        \
    ACTION ( CLASS, x, z, x, x )                                        \
    ACTION ( CLASS, x, z, x, y )                                        \
    ACTION ( CLASS, x, z, x, z )                                        \
    ACTION ( CLASS, x, z, y, x )                                        \
    ACTION ( CLASS, x, z, y, y )                                        \
    ACTION ( CLASS, x, z, y, z )                                        \
    ACTION ( CLASS, x, z, z, x )                                        \
    ACTION ( CLASS, x, z, z, y )                                        \
    ACTION ( CLASS, x, z, z, z )                                        \
    ACTION ( CLASS, y, x, x, z )                                        \
    ACTION ( CLASS, y, x, y, z )                                        \
    ACTION ( CLASS, y, x, z, x )                                        \
    ACTION ( CLASS, y, x, z, y )                                        \
    ACTION ( CLASS, y, x, z, z )                                        \
    ACTION ( CLASS, y, y, x, z )                                        \
    ACTION ( CLASS, y, y, y, z )                                        \
    ACTION ( CLASS, y, y, z, x )                                        \
    ACTION ( CLASS, y, y, z, y )                                        \
    ACTION ( CLASS, y, y, z, z )                                        \
    ACTION ( CLASS, y, z, x, x )                                        \
    ACTION ( CLASS, y, z, x, y )                                        \
    ACTION ( CLASS, y, z, x, z )                                        \
    ACTION ( CLASS, y, z, y, x )                                        \
    ACTION ( CLASS, y, z, y, y )                                        \
    ACTION ( CLASS, y, z, y, z )                                        \
    ACTION ( CLASS, y, z, z, x )                                        \
    ACTION ( CLASS, y, z, z, y )                                        \
    ACTION ( CLASS, y, z, z, z )                                        \
    ACTION ( CLASS, z, x, x, x )                                        \
    ACTION ( CLASS, z, x, x, y )                                        \
    ACTION ( CLASS, z, x, x, z )                                        \
    ACTION ( CLASS, z, x, y, x )                                        \
    ACTION ( CLASS, z, x, y, y )                                        \
    ACTION ( CLASS, z, x, y, z )                                        \
    ACTION ( CLASS, z, x, z, x )                                        \
    ACTION ( CLASS, z, x, z, y )                                        \
    ACTION ( CLASS, z, x, z, z )                                        \
    ACTION ( CLASS, z, y, x, x )                                        \
    ACTION ( CLASS, z, y, x, y )                                        \
    ACTION ( CLASS, z, y, x, z )                                        \
    ACTION ( CLASS, z, y, y, x )                                        \
    ACTION ( CLASS, z, y, y, y )                                        \
    ACTION ( CLASS, z, y, y, z )                                        \
    ACTION ( CLASS, z, y, z, x )                                        \
    ACTION ( CLASS, z, y, z, y )                                        \
    ACTION ( CLASS, z, y, z, z )                                        \
    ACTION ( CLASS, z, z, x, x )                                        \
    ACTION ( CLASS, z, z, x, y )                                        \
    ACTION ( CLASS, z, z, x, z )                                        \
    ACTION ( CLASS, z, z, y, x )                                        \
    ACTION ( CLASS, z, z, y, y )                                        \
    ACTION ( CLASS, z, z, y, z )                                        \
    ACTION ( CLASS, z, z, z, x )                                        \
    ACTION ( CLASS, z, z, z, y )                                        \
    ACTION ( CLASS, z, z, z, z )

#define ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS, ACTION )    \
    ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ACTION )        \
    ACTION ( CLASS, x, x, x, w )                                        \
    ACTION ( CLASS, x, x, y, w )                                        \
    ACTION ( CLASS, x, x, z, w )                                        \
    ACTION ( CLASS, x, x, w, x )                                        \
    ACTION ( CLASS, x, x, w, y )                                        \
    ACTION ( CLASS, x, x, w, z )                                        \
    ACTION ( CLASS, x, x, w, w )                                        \
    ACTION ( CLASS, x, y, x, w )                                        \
    ACTION ( CLASS, x, y, y, w )                                        \
    ACTION ( CLASS, x, y, z, w )                                        \
    ACTION ( CLASS, x, y, w, x )                                        \
    ACTION ( CLASS, x, y, w, y )                                        \
    ACTION ( CLASS, x, y, w, z )                                        \
    ACTION ( CLASS, x, y, w, w )                                        \
    ACTION ( CLASS, x, z, x, w )                                        \
    ACTION ( CLASS, x, z, y, w )                                        \
    ACTION ( CLASS, x, z, z, w )                                        \
    ACTION ( CLASS, x, z, w, x )                                        \
    ACTION ( CLASS, x, z, w, y )                                        \
    ACTION ( CLASS, x, z, w, z )                                        \
    ACTION ( CLASS, x, z, w, w )                                        \
    ACTION ( CLASS, x, w, x, x )                                        \
    ACTION ( CLASS, x, w, x, y )                                        \
    ACTION ( CLASS, x, w, x, z )                                        \
    ACTION ( CLASS, x, w, x, w )                                        \
    ACTION ( CLASS, x, w, y, x )                                        \
    ACTION ( CLASS, x, w, y, y )                                        \
    ACTION ( CLASS, x, w, y, z )                                        \
    ACTION ( CLASS, x, w, y, w )                                        \
    ACTION ( CLASS, x, w, z, x )                                        \
    ACTION ( CLASS, x, w, z, y )                                        \
    ACTION ( CLASS, x, w, z, z )                                        \
    ACTION ( CLASS, x, w, z, w )                                        \
    ACTION ( CLASS, x, w, w, x )                                        \
    ACTION ( CLASS, x, w, w, y )                                        \
    ACTION ( CLASS, x, w, w, z )                                        \
    ACTION ( CLASS, x, w, w, w )                                        \
    ACTION ( CLASS, y, x, x, w )                                        \
    ACTION ( CLASS, y, x, y, w )                                        \
    ACTION ( CLASS, y, x, z, w )                                        \
    ACTION ( CLASS, y, x, w, x )                                        \
    ACTION ( CLASS, y, x, w, y )                                        \
    ACTION ( CLASS, y, x, w, z )                                        \
    ACTION ( CLASS, y, x, w, w )                                        \
    ACTION ( CLASS, y, y, x, w )                                        \
    ACTION ( CLASS, y, y, y, w )                                        \
    ACTION ( CLASS, y, y, z, w )                                        \
    ACTION ( CLASS, y, y, w, x )                                        \
    ACTION ( CLASS, y, y, w, y )                                        \
    ACTION ( CLASS, y, y, w, z )                                        \
    ACTION ( CLASS, y, y, w, w )                                        \
    ACTION ( CLASS, y, z, x, w )                                        \
    ACTION ( CLASS, y, z, y, w )                                        \
    ACTION ( CLASS, y, z, z, w )                                        \
    ACTION ( CLASS, y, z, w, x )                                        \
    ACTION ( CLASS, y, z, w, y )                                        \
    ACTION ( CLASS, y, z, w, z )                                        \
    ACTION ( CLASS, y, z, w, w )                                        \
    ACTION ( CLASS, y, w, x, x )                                        \
    ACTION ( CLASS, y, w, x, y )                                        \
    ACTION ( CLASS, y, w, x, z )                                        \
    ACTION ( CLASS, y, w, x, w )                                        \
    ACTION ( CLASS, y, w, y, x )                                        \
    ACTION ( CLASS, y, w, y, y )                                        \
    ACTION ( CLASS, y, w, y, z )                                        \
    ACTION ( CLASS, y, w, y, w )                                        \
    ACTION ( CLASS, y, w, z, x )                                        \
    ACTION ( CLASS, y, w, z, y )                                        \
    ACTION ( CLASS, y, w, z, z )                                        \
    ACTION ( CLASS, y, w, z, w )                                        \
    ACTION ( CLASS, y, w, w, x )                                        \
    ACTION ( CLASS, y, w, w, y )                                        \
    ACTION ( CLASS, y, w, w, z )                                        \
    ACTION ( CLASS, y, w, w, w )                                        \
    ACTION ( CLASS, z, x, x, w )                                        \
    ACTION ( CLASS, z, x, y, w )                                        \
    ACTION ( CLASS, z, x, z, w )                                        \
    ACTION ( CLASS, z, x, w, x )                                        \
    ACTION ( CLASS, z, x, w, y )                                        \
    ACTION ( CLASS, z, x, w, z )                                        \
    ACTION ( CLASS, z, x, w, w )                                        \
    ACTION ( CLASS, z, y, x, w )                                        \
    ACTION ( CLASS, z, y, y, w )                                        \
    ACTION ( CLASS, z, y, z, w )                                        \
    ACTION ( CLASS, z, y, w, x )                                        \
    ACTION ( CLASS, z, y, w, y )                                        \
    ACTION ( CLASS, z, y, w, z )                                        \
    ACTION ( CLASS, z, y, w, w )                                        \
    ACTION ( CLASS, z, z, x, w )                                        \
    ACTION ( CLASS, z, z, y, w )                                        \
    ACTION ( CLASS, z, z, z, w )                                        \
    ACTION ( CLASS, z, z, w, x )                                        \
    ACTION ( CLASS, z, z, w, y )                                        \
    ACTION ( CLASS, z, z, w, z )                                        \
    ACTION ( CLASS, z, z, w, w )                                        \
    ACTION ( CLASS, z, w, x, x )                                        \
    ACTION ( CLASS, z, w, x, y )                                        \
    ACTION ( CLASS, z, w, x, z )                                        \
    ACTION ( CLASS, z, w, x, w )                                        \
    ACTION ( CLASS, z, w, y, x )                                        \
    ACTION ( CLASS, z, w, y, y )                                        \
    ACTION ( CLASS, z, w, y, z )                                        \
    ACTION ( CLASS, z, w, y, w )                                        \
    ACTION ( CLASS, z, w, z, x )                                        \
    ACTION ( CLASS, z, w, z, y )                                        \
    ACTION ( CLASS, z, w, z, z )                                        \
    ACTION ( CLASS, z, w, z, w )                                        \
    ACTION ( CLASS, z, w, w, x )                                        \
    ACTION ( CLASS, z, w, w, y )                                        \
    ACTION ( CLASS, z, w, w, z )                                        \
    ACTION ( CLASS, z, w, w, w )                                        \
    ACTION ( CLASS, w, x, x, x )                                        \
    ACTION ( CLASS, w, x, x, y )                                        \
    ACTION ( CLASS, w, x, x, z )                                        \
    ACTION ( CLASS, w, x, x, w )                                        \
    ACTION ( CLASS, w, x, y, x )                                        \
    ACTION ( CLASS, w, x, y, y )                                        \
    ACTION ( CLASS, w, x, y, z )                                        \
    ACTION ( CLASS, w, x, y, w )                                        \
    ACTION ( CLASS, w, x, z, x )                                        \
    ACTION ( CLASS, w, x, z, y )                                        \
    ACTION ( CLASS, w, x, z, z )                                        \
    ACTION ( CLASS, w, x, z, w )                                        \
    ACTION ( CLASS, w, x, w, x )                                        \
    ACTION ( CLASS, w, x, w, y )                                        \
    ACTION ( CLASS, w, x, w, z )                                        \
    ACTION ( CLASS, w, x, w, w )                                        \
    ACTION ( CLASS, w, y, x, x )                                        \
    ACTION ( CLASS, w, y, x, y )                                        \
    ACTION ( CLASS, w, y, x, z )                                        \
    ACTION ( CLASS, w, y, x, w )                                        \
    ACTION ( CLASS, w, y, y, x )                                        \
    ACTION ( CLASS, w, y, y, y )                                        \
    ACTION ( CLASS, w, y, y, z )                                        \
    ACTION ( CLASS, w, y, y, w )                                        \
    ACTION ( CLASS, w, y, z, x )                                        \
    ACTION ( CLASS, w, y, z, y )                                        \
    ACTION ( CLASS, w, y, z, z )                                        \
    ACTION ( CLASS, w, y, z, w )                                        \
    ACTION ( CLASS, w, y, w, x )                                        \
    ACTION ( CLASS, w, y, w, y )                                        \
    ACTION ( CLASS, w, y, w, z )                                        \
    ACTION ( CLASS, w, y, w, w )                                        \
    ACTION ( CLASS, w, z, x, x )                                        \
    ACTION ( CLASS, w, z, x, y )                                        \
    ACTION ( CLASS, w, z, x, z )                                        \
    ACTION ( CLASS, w, z, x, w )                                        \
    ACTION ( CLASS, w, z, y, x )                                        \
    ACTION ( CLASS, w, z, y, y )                                        \
    ACTION ( CLASS, w, z, y, z )                                        \
    ACTION ( CLASS, w, z, y, w )                                        \
    ACTION ( CLASS, w, z, z, x )                                        \
    ACTION ( CLASS, w, z, z, y )                                        \
    ACTION ( CLASS, w, z, z, z )                                        \
    ACTION ( CLASS, w, z, z, w )                                        \
    ACTION ( CLASS, w, z, w, x )                                        \
    ACTION ( CLASS, w, z, w, y )                                        \
    ACTION ( CLASS, w, z, w, z )                                        \
    ACTION ( CLASS, w, z, w, w )                                        \
    ACTION ( CLASS, w, w, x, x )                                        \
    ACTION ( CLASS, w, w, x, y )                                        \
    ACTION ( CLASS, w, w, x, z )                                        \
    ACTION ( CLASS, w, w, x, w )                                        \
    ACTION ( CLASS, w, w, y, x )                                        \
    ACTION ( CLASS, w, w, y, y )                                        \
    ACTION ( CLASS, w, w, y, z )                                        \
    ACTION ( CLASS, w, w, y, w )                                        \
    ACTION ( CLASS, w, w, z, x )                                        \
    ACTION ( CLASS, w, w, z, y )                                        \
    ACTION ( CLASS, w, w, z, z )                                        \
    ACTION ( CLASS, w, w, z, w )                                        \
    ACTION ( CLASS, w, w, w, x )                                        \
    ACTION ( CLASS, w, w, w, y )                                        \
    ACTION ( CLASS, w, w, w, z )                                        \
    ACTION ( CLASS, w, w, w, w )

#define ULIS_DECLARE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2 ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2( , ULIS_DECLARE_VEC2_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3 ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3( , ULIS_DECLARE_VEC2_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC4 ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC4( , ULIS_DECLARE_VEC2_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2 ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2( , ULIS_DECLARE_VEC3_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3 ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3( , ULIS_DECLARE_VEC3_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC4 ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC4( , ULIS_DECLARE_VEC3_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2 ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2( , ULIS_DECLARE_VEC4_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3 ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3( , ULIS_DECLARE_VEC4_SWIZZLE_FUNCTION )
#define ULIS_DECLARE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC4 ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC4( , ULIS_DECLARE_VEC4_SWIZZLE_FUNCTION )

#define ULIS_DEFINE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS ) ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ULIS_DEFINE_VEC2_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS ) ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ULIS_DEFINE_VEC2_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS ) ULIS_FOR_ALL_VEC2_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS, ULIS_DEFINE_VEC2_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS ) ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ULIS_DEFINE_VEC3_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS ) ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ULIS_DEFINE_VEC3_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS ) ULIS_FOR_ALL_VEC3_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS, ULIS_DEFINE_VEC3_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS ) ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2( CLASS, ULIS_DEFINE_VEC4_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS ) ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3( CLASS, ULIS_DEFINE_VEC4_SWIZZLE_FUNCTION )
#define ULIS_DEFINE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS ) ULIS_FOR_ALL_VEC4_SWIZZLE_FUNCTIONS_IN_VEC4( CLASS, ULIS_DEFINE_VEC4_SWIZZLE_FUNCTION )

#define ULIS_DECLARE_ALL_SWIZZLE_FUNCTIONS_VEC2 \
    ULIS_DECLARE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2 \
    ULIS_DECLARE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2 \
    ULIS_DECLARE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2

#define ULIS_DECLARE_ALL_SWIZZLE_FUNCTIONS_VEC3 \
    ULIS_DECLARE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3 \
    ULIS_DECLARE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3 \
    ULIS_DECLARE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3

#define ULIS_DECLARE_ALL_SWIZZLE_FUNCTIONS_VEC4 \
    ULIS_DECLARE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC4 \
    ULIS_DECLARE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC4 \
    ULIS_DECLARE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC4


#define ULIS_DEFINE_ALL_SWIZZLE_FUNCTIONS_VEC2              \
    ULIS_DEFINE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC2( TVector2 )  \
    ULIS_DEFINE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC2( TVector2 )  \
    ULIS_DEFINE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC2( TVector2 )

#define ULIS_DEFINE_ALL_SWIZZLE_FUNCTIONS_VEC3              \
    ULIS_DEFINE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC3( TVector3 )  \
    ULIS_DEFINE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC3( TVector3 )  \
    ULIS_DEFINE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC3( TVector3 )

#define ULIS_DEFINE_ALL_SWIZZLE_FUNCTIONS_VEC4              \
    ULIS_DEFINE_VEC2_SWIZZLE_FUNCTIONS_IN_VEC4( TVector4 )  \
    ULIS_DEFINE_VEC3_SWIZZLE_FUNCTIONS_IN_VEC4( TVector4 )  \
    ULIS_DEFINE_VEC4_SWIZZLE_FUNCTIONS_IN_VEC4( TVector4 )

