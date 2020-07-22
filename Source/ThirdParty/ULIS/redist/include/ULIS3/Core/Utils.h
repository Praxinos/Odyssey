/*
*
*   ULIS3
*__________________
*
* @file         Utils.h
* @author       Clement Berthaud
* @brief        This file provides core utils definitions for the ULIS3 library.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Platform.h"

ULIS3_NAMESPACE_BEGIN
// eTypeFromT
template< typename T >  ULIS3_FORCEINLINE   eType   eTypeFromT(void)        { return  TYPE_UINT8;   }
template<> constexpr    ULIS3_FORCEINLINE   eType   eTypeFromT< uint8 >()   { return  TYPE_UINT8;   }
template<> constexpr    ULIS3_FORCEINLINE   eType   eTypeFromT< uint16 >()  { return  TYPE_UINT16;  }
template<> constexpr    ULIS3_FORCEINLINE   eType   eTypeFromT< uint32 >()  { return  TYPE_UINT32;  }
template<> constexpr    ULIS3_FORCEINLINE   eType   eTypeFromT< ufloat >()  { return  TYPE_UFLOAT;  }
template<> constexpr    ULIS3_FORCEINLINE   eType   eTypeFromT< udouble >() { return  TYPE_UDOUBLE; }

// _min _max _clamp
template< typename T > static constexpr ULIS3_FORCEINLINE T _min( T iA, T iB ) { return  iA < iB ? iA : iB; }
template< typename T > static constexpr ULIS3_FORCEINLINE T _max( T iA, T iB ) { return  iA > iB ? iA : iB; }
template< typename T > static constexpr ULIS3_FORCEINLINE T _clamp( T iValue, T iMin, T iMax ) { return  _max( iMin, _min( iValue, iMax ) ); }

// ConvType
template< typename T1, typename T2 > constexpr T2 ULIS3_FORCEINLINE ConvType( T1 iValue ) { return static_cast< T2 >( iValue ); }
template<> constexpr uint16     ULIS3_FORCEINLINE ConvType< uint8,  uint16  >( uint8 iValue  )  { return iValue * 0x101;                                                            }
template<> constexpr uint32     ULIS3_FORCEINLINE ConvType< uint8,  uint32  >( uint8 iValue  )  { return iValue * 0x1010101;                                                        }
template<> constexpr ufloat     ULIS3_FORCEINLINE ConvType< uint8,  ufloat  >( uint8 iValue  )  { return iValue / (ufloat)0xFF;                                                     }
template<> constexpr udouble    ULIS3_FORCEINLINE ConvType< uint8,  udouble >( uint8 iValue  )  { return iValue / (udouble)0xFF;                                                    }
template<> constexpr uint8      ULIS3_FORCEINLINE ConvType< uint16, uint8   >( uint16 iValue )  { return ( iValue + 1 + ( iValue>>8 ) ) >> 8;                                       }
template<> constexpr uint32     ULIS3_FORCEINLINE ConvType< uint16, uint32  >( uint16 iValue )  { return iValue * 0x10001;                                                          }
template<> constexpr ufloat     ULIS3_FORCEINLINE ConvType< uint16, ufloat  >( uint16 iValue )  { return iValue / (ufloat)0xFFFF;                                                   }
template<> constexpr udouble    ULIS3_FORCEINLINE ConvType< uint16, udouble >( uint16 iValue )  { return iValue / (udouble)0xFFFF;                                                  }
template<> constexpr uint8      ULIS3_FORCEINLINE ConvType< uint32, uint8   >( uint32 iValue )  { return iValue >> 24;                                                              }
template<> constexpr uint16     ULIS3_FORCEINLINE ConvType< uint32, uint16  >( uint32 iValue )  { return ( iValue + 1 + ( iValue>>16 ) ) >> 16;                                     }
template<> constexpr ufloat     ULIS3_FORCEINLINE ConvType< uint32, ufloat  >( uint32 iValue )  { return iValue / (ufloat)0xFFFFFFFF;                                               }
template<> constexpr udouble    ULIS3_FORCEINLINE ConvType< uint32, udouble >( uint32 iValue )  { return iValue / (udouble)0xFFFFFFFF;                                              }
template<> constexpr uint8      ULIS3_FORCEINLINE ConvType< ufloat,  uint8  >( ufloat iValue  ) { return uint8(  iValue * 0xFF       );                                             }
template<> constexpr uint16     ULIS3_FORCEINLINE ConvType< ufloat,  uint16 >( ufloat iValue  ) { return uint16( iValue * 0xFFFF     );                                             }
template<> constexpr uint32     ULIS3_FORCEINLINE ConvType< ufloat,  uint32 >( ufloat iValue  ) { return uint32( iValue * 0xFFFFFFFF );                                             }
template<> constexpr uint8      ULIS3_FORCEINLINE ConvType< udouble, uint8  >( udouble iValue ) { return uint8(  iValue * 0xFF       );                                             }
template<> constexpr uint16     ULIS3_FORCEINLINE ConvType< udouble, uint16 >( udouble iValue ) { return uint16( iValue * 0xFFFF     );                                             }
template<> constexpr uint32     ULIS3_FORCEINLINE ConvType< udouble, uint32 >( udouble iValue ) { return uint32( iValue * 0xFFFFFFFF );                                             }
template<> constexpr uint8      ULIS3_FORCEINLINE ConvType< int,  uint8     >( int iValue  )    { return _clamp( iValue, 0, (int)UINT8_MAX );                                       }
template<> constexpr uint16     ULIS3_FORCEINLINE ConvType< int,  uint16    >( int iValue  )    { return ConvType< uint8, uint16 >(  (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );  }
template<> constexpr uint32     ULIS3_FORCEINLINE ConvType< int,  uint32    >( int iValue  )    { return ConvType< uint8, uint32 >(  (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );  }
template<> constexpr ufloat     ULIS3_FORCEINLINE ConvType< int,  ufloat    >( int iValue  )    { return ConvType< uint8, ufloat  >( (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );  }
template<> constexpr udouble    ULIS3_FORCEINLINE ConvType< int,  udouble   >( int iValue  )    { return ConvType< uint8, udouble >( (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );  }

template< typename T > constexpr ULIS3_FORCEINLINE ufloat _TypeToFloat( const tByte* iSrc, uint8 iIndex ) { return  ConvType< T, ufloat >( *( ( (T*)( iSrc ) ) + iIndex ) ); }
template< typename T > constexpr ULIS3_FORCEINLINE void _FloatToType( tByte* iSrc, uint8 iIndex, ufloat iValue ) { *( ( (T*)( iSrc ) ) + iIndex ) = ConvType< ufloat, T >( iValue ); }
#define TYPE2FLOAT( iSrc, iIndex )          _TypeToFloat< T >( iSrc, iIndex )
#define FLOAT2TYPE( iSrc, iIndex, iValue )  _FloatToType< T >( iSrc, iIndex, iValue )

// MinType MaxType
template< typename T > T constexpr ULIS3_FORCEINLINE MinType() { return T(0); }
template< typename T > T constexpr ULIS3_FORCEINLINE MaxType() { return T(1); }
template<> uint8    constexpr ULIS3_FORCEINLINE MaxType< uint8   >() { return UINT8_MAX;    }
template<> uint16   constexpr ULIS3_FORCEINLINE MaxType< uint16  >() { return UINT16_MAX;   }
template<> uint32   constexpr ULIS3_FORCEINLINE MaxType< uint32  >() { return UINT32_MAX;   }
template<> ufloat   constexpr ULIS3_FORCEINLINE MaxType< ufloat  >() { return 1.f;          }
template<> udouble  constexpr ULIS3_FORCEINLINE MaxType< udouble >() { return 1.0;          }

// Macro for switch enumerators
#define ULIS3_COMP_OP_CASE_DO( _CASE, _ACTION, _E1, _E2, _E3 )              case _CASE: { _ACTION( _CASE, _E1, _E2, _E3 ); break; }
#define ULIS3_SWITCH_FOR_ALL_DO( iValue, _SUBSET, _ACTION, _E1, _E2, _E3 )  switch( iValue ) { _SUBSET( ULIS3_COMP_OP_CASE_DO, _ACTION, _E1, _E2, _E3 ) }

// Macro for all types for template instanciation
#define ULIS3_FOR_ALL_TYPES_DO( X, _E0, _E1, _E2, _E3 )     \
    X( uint8,   _E0, _E1, _E2, _E3 )                        \
    X( uint16,  _E0, _E1, _E2, _E3 )                        \
    X( uint32,  _E0, _E1, _E2, _E3 )                        \
    X( ufloat,  _E0, _E1, _E2, _E3 )                        \
    X( udouble, _E0, _E1, _E2, _E3 )

// Macro for all types ID for template instanciation
#define ULIS3_FOR_ALL_TYPES_ID_DO( X, _E0, _E1, _E2, _E3 )  \
    X( TYPE_UINT8,      _E0, uint8, _E2, _E3 )              \
    X( TYPE_UINT16,     _E0, uint16, _E2, _E3 )             \
    X( TYPE_UINT32,     _E0, uint32, _E2, _E3 )             \
    X( TYPE_UFLOAT,     _E0, ufloat, _E2, _E3 )             \
    X( TYPE_UDOUBLE,    _E0, udouble, _E2, _E3 )

// Macro for all types for combination template instanciation
#define ULIS3_FOR_ALL_TYPES_COMBINATIONS_DO( X )    \
    X( uint8,   uint8    )                          \
    X( uint8,   uint16   )                          \
    X( uint8,   uint32   )                          \
    X( uint8,   ufloat   )                          \
    X( uint8,   udouble  )                          \
    X( uint16,  uint8   )                           \
    X( uint16,  uint16  )                           \
    X( uint16,  uint32  )                           \
    X( uint16,  ufloat  )                           \
    X( uint16,  udouble )                           \
    X( uint32,  uint8   )                           \
    X( uint32,  uint16  )                           \
    X( uint32,  uint32  )                           \
    X( uint32,  ufloat  )                           \
    X( uint32,  udouble )                           \
    X( ufloat,  uint8   )                           \
    X( ufloat,  uint16  )                           \
    X( ufloat,  uint32  )                           \
    X( ufloat,  ufloat  )                           \
    X( ufloat,  udouble )                           \
    X( udouble, uint8   )                           \
    X( udouble, uint16  )                           \
    X( udouble, uint32  )                           \
    X( udouble, ufloat  )                           \
    X( udouble, udouble )

ULIS3_NAMESPACE_END

