// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Types.h
* @author       Clement Berthaud
* @brief        This file provides core types definitions for the ULIS library.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Platform.h"
#include <cstdint>
#include <limits>
#include <cstring>
#include <memory>
ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// Domain Range Typdefs
typedef  uint8_t    uint8;      ///< Used for the smallest adressable type, unsigned 8bit images domain range values and main type for image bits memory storage.
typedef  uint16_t   uint16;     ///< Used for unsigned 16bit images domain range values.
typedef  uint32_t   uint32;     ///< Used for unsigned 32bit images domain range values and sizes such as width and height.
typedef  unsigned long long   uint64;     ///< Used for unsigned 64bit images domain range values and whole buffer sizes that might overflow 32bit range.
typedef  int8_t     int8;       ///< Used for signed 8bit images domain range values.
typedef  int16_t    int16;      ///< Used for signed 16bit images domain range values.
typedef  int32_t    int32;      ///< Used for signed 32bit images domain range values.
typedef  int64_t    int64;      ///< Used for signed 64bit images domain range values.
typedef  float      ufloat;     ///< Used for float images domain range values, indicates normalized range [0;1]
typedef  double     udouble;    ///< Used for double images domain range values, indicates normalized range [0;1]
typedef  int64      ulError;    ///< Used for errors reports defined below in this file
typedef  unsigned long long int uint64_literal; ///< Used for user defined literals mandated by C++14 standard.

/////////////////////////////////////////////////////
// MAX
#define ULIS_UINT16_MAX 0xFFFF
#define ULIS_UINT32_MAX 0xFFFFFFFF
#define ULIS_UINT64_MAX 0xFFFFFFFFFFFFFFFF
#define ULIS_INT_MAX    0x7FFFFFFF

#ifndef FLT_MAX
#define FLT_MAX __FLT_MAX__
#endif

#define ULIS_FLOAT_MAX FLT_MAX

/////////////////////////////////////////////////////
// Type codes
#define ULIS_TYPE_UINT8    0x0
#define ULIS_TYPE_UINT16   0x1
#define ULIS_TYPE_UFLOAT   0x2
//DISABLED:UINT32#define ULIS_TYPE_UINT32   0x3
//DISABLED:DOUBLE#define ULIS_TYPE_UDOUBLE  0x4

/////////////////////////////////////////////////////
// Type enum
enum eType : uint8 {
      Type_uint8     = ULIS_TYPE_UINT8
    , Type_uint16    = ULIS_TYPE_UINT16
    , Type_ufloat    = ULIS_TYPE_UFLOAT
    //DISABLED:UINT32, Type_uint32    = ULIS_TYPE_UINT32
    //DISABLED:DOUBLE, TYPE_UDOUBLE   = ULIS_TYPE_UDOUBLE
};

/////////////////////////////////////////////////////
// constexpr eTypeFromT
template< typename T >  ULIS_FORCEINLINE   eType   eTypeFromT(void)        { return  Type_uint8;   }
template<> constexpr    ULIS_FORCEINLINE   eType   eTypeFromT< uint8 >()   { return  Type_uint8;   }
template<> constexpr    ULIS_FORCEINLINE   eType   eTypeFromT< uint16 >()  { return  Type_uint16;  }
//DISABLED:UINT32template<> constexpr    ULIS_FORCEINLINE   eType   eTypeFromT< uint32 >()  { return  Type_uint32;  }
template<> constexpr    ULIS_FORCEINLINE   eType   eTypeFromT< ufloat >()  { return  Type_ufloat;  }
//DISABLED:DOUBLEtemplate<> constexpr    ULIS_FORCEINLINE   eType   eTypeFromT< udouble >() { return  TYPE_UDOUBLE; }

/////////////////////////////////////////////////////
// constexpr _min _max _clamp
template< typename T > static constexpr ULIS_FORCEINLINE T _min( T iA, T iB ) { return  iA < iB ? iA : iB; }
template< typename T > static constexpr ULIS_FORCEINLINE T _max( T iA, T iB ) { return  iA > iB ? iA : iB; }
template< typename T > static constexpr ULIS_FORCEINLINE T _clamp( T iValue, T iMin, T iMax ) { return  _max( iMin, _min( iValue, iMax ) ); }

/////////////////////////////////////////////////////
// constexptr ConvType
template< typename T1, typename T2 > constexpr T2 ULIS_FORCEINLINE ConvType( T1 iValue ) { return static_cast< T2 >( iValue ); }
template<> constexpr uint16     ULIS_FORCEINLINE ConvType< uint8,  uint16  >( uint8 iValue  )  { return iValue * 0x101;                                                             }
template<> constexpr uint32     ULIS_FORCEINLINE ConvType< uint8,  uint32  >( uint8 iValue  )  { return iValue * 0x1010101;                                                         }
template<> constexpr ufloat     ULIS_FORCEINLINE ConvType< uint8,  ufloat  >( uint8 iValue  )  { return iValue / (ufloat)0xFF;                                                      }
template<> constexpr udouble    ULIS_FORCEINLINE ConvType< uint8,  udouble >( uint8 iValue  )  { return iValue / (udouble)0xFF;                                                     }
//template<> constexpr uint8      ULIS_FORCEINLINE ConvType< uint16, uint8   >( uint16 iValue )  { return ( iValue + 1 + ( iValue>>8 ) ) >> 8;                                      }
template<> constexpr uint8      ULIS_FORCEINLINE ConvType< uint16, uint8   >( uint16 iValue )  { return iValue >> 8;                                                                }
template<> constexpr uint32     ULIS_FORCEINLINE ConvType< uint16, uint32  >( uint16 iValue )  { return iValue * 0x10001;                                                           }
template<> constexpr ufloat     ULIS_FORCEINLINE ConvType< uint16, ufloat  >( uint16 iValue )  { return iValue / (ufloat)0xFFFF;                                                    }
template<> constexpr udouble    ULIS_FORCEINLINE ConvType< uint16, udouble >( uint16 iValue )  { return iValue / (udouble)0xFFFF;                                                   }
template<> constexpr uint8      ULIS_FORCEINLINE ConvType< uint32, uint8   >( uint32 iValue )  { return iValue >> 24;                                                               }
//template<> constexpr uint16     ULIS_FORCEINLINE ConvType< uint32, uint16  >( uint32 iValue )  { return ( iValue + 1 + ( iValue>>16 ) ) >> 16;                                    }
template<> constexpr uint16     ULIS_FORCEINLINE ConvType< uint32, uint16  >( uint32 iValue )  { return iValue >> 16;                                                               }
template<> constexpr ufloat     ULIS_FORCEINLINE ConvType< uint32, ufloat  >( uint32 iValue )  { return iValue / (ufloat)0xFFFFFFFF;                                                }
template<> constexpr udouble    ULIS_FORCEINLINE ConvType< uint32, udouble >( uint32 iValue )  { return iValue / (udouble)0xFFFFFFFF;                                               }
template<> constexpr uint8      ULIS_FORCEINLINE ConvType< ufloat,  uint8  >( ufloat iValue  ) { return uint8(  iValue * 0xFF       );                                              }
template<> constexpr uint16     ULIS_FORCEINLINE ConvType< ufloat,  uint16 >( ufloat iValue  ) { return uint16( iValue * 0xFFFF     );                                              }
template<> constexpr uint32     ULIS_FORCEINLINE ConvType< ufloat,  uint32 >( ufloat iValue  ) { return ConvType< uint16, uint32 >( ConvType< float, uint16 >( iValue ) );          }
template<> constexpr uint8      ULIS_FORCEINLINE ConvType< udouble, uint8  >( udouble iValue ) { return uint8(  iValue * 0xFF       );                                              }
template<> constexpr uint16     ULIS_FORCEINLINE ConvType< udouble, uint16 >( udouble iValue ) { return uint16( iValue * 0xFFFF     );                                              }
template<> constexpr uint32     ULIS_FORCEINLINE ConvType< udouble, uint32 >( udouble iValue ) { return ConvType< uint16, uint32 >( ConvType< udouble, uint16 >( iValue ) );        }
template<> constexpr uint8      ULIS_FORCEINLINE ConvType< int,  uint8     >( int iValue  )    { return _clamp( iValue, 0, (int)UINT8_MAX );                                        }
template<> constexpr uint16     ULIS_FORCEINLINE ConvType< int,  uint16    >( int iValue  )    { return ConvType< uint8, uint16 >(  (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );   }
template<> constexpr uint32     ULIS_FORCEINLINE ConvType< int,  uint32    >( int iValue  )    { return ConvType< uint8, uint32 >(  (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );   }
template<> constexpr ufloat     ULIS_FORCEINLINE ConvType< int,  ufloat    >( int iValue  )    { return ConvType< uint8, ufloat  >( (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );   }
template<> constexpr udouble    ULIS_FORCEINLINE ConvType< int,  udouble   >( int iValue  )    { return ConvType< uint8, udouble >( (uint8)_clamp( iValue, 0, (int)UINT8_MAX ) );   }

/////////////////////////////////////////////////////
// constexptr TypeToFloat
template< typename T > constexpr ULIS_FORCEINLINE ufloat _TypeToFloat( const uint8* iSrc, uint8 iIndex ) { return  ConvType< T, ufloat >( *( ( (T*)( iSrc ) ) + iIndex ) ); }
template< typename T > constexpr ULIS_FORCEINLINE void _FloatToType( uint8* iSrc, uint8 iIndex, ufloat iValue ) { *( ( (T*)( iSrc ) ) + iIndex ) = ConvType< ufloat, T >( iValue ); }
#define TYPE2FLOAT( iSrc, iIndex )          _TypeToFloat< T >( iSrc, iIndex )
#define FLOAT2TYPE( iSrc, iIndex, iValue )  _FloatToType< T >( iSrc, iIndex, iValue )

/////////////////////////////////////////////////////
// MinType MaxType
template< typename T > T constexpr ULIS_FORCEINLINE MinType() { return static_cast< T >(0); }
template< typename T > T constexpr ULIS_FORCEINLINE MidType() { return static_cast< T >(0); }
template< typename T > T constexpr ULIS_FORCEINLINE MaxType() { return static_cast< T >(1); }
template<> uint8    constexpr ULIS_FORCEINLINE MinType< uint8   >() { return 0;            }
template<> uint16   constexpr ULIS_FORCEINLINE MinType< uint16  >() { return 0;            }
template<> uint32   constexpr ULIS_FORCEINLINE MinType< uint32  >() { return 0;            }
template<> ufloat   constexpr ULIS_FORCEINLINE MinType< ufloat  >() { return 0.0f;         }
template<> udouble  constexpr ULIS_FORCEINLINE MinType< udouble >() { return 0.0;          }
template<> uint8    constexpr ULIS_FORCEINLINE MidType< uint8   >() { return 128;          }
template<> uint16   constexpr ULIS_FORCEINLINE MidType< uint16  >() { return 32768;        }
template<> uint32   constexpr ULIS_FORCEINLINE MidType< uint32  >() { return 2147483648;   }
template<> ufloat   constexpr ULIS_FORCEINLINE MidType< ufloat  >() { return 0.5f;         }
template<> udouble  constexpr ULIS_FORCEINLINE MidType< udouble >() { return 0.5;          }
template<> uint8    constexpr ULIS_FORCEINLINE MaxType< uint8   >() { return UINT8_MAX;    }
template<> uint16   constexpr ULIS_FORCEINLINE MaxType< uint16  >() { return UINT16_MAX;   }
template<> uint32   constexpr ULIS_FORCEINLINE MaxType< uint32  >() { return UINT32_MAX;   }
template<> ufloat   constexpr ULIS_FORCEINLINE MaxType< ufloat  >() { return 1.0f;         }
template<> udouble  constexpr ULIS_FORCEINLINE MaxType< udouble >() { return 1.0;          }

/////////////////////////////////////////////////////
// Macro for switch enumerators
#define ULIS_COMP_OP_CASE_DO( _CASE, _ACTION, _E1, _E2, _E3 )              case _CASE: { _ACTION( _CASE, _E1, _E2, _E3 ); break; }
#define ULIS_SWITCH_FOR_ALL_DO( iValue, _SUBSET, _ACTION, _E1, _E2, _E3 )  switch( iValue ) { _SUBSET( ULIS_COMP_OP_CASE_DO, _ACTION, _E1, _E2, _E3 ) }

/////////////////////////////////////////////////////
// Macro for all types for template instanciation
#define ULIS_FOR_ALL_TYPES_DO( X, _E0, _E1, _E2, _E3 )      \
    X( uint8,   _E0, _E1, _E2, _E3 )                        \
    X( uint16,  _E0, _E1, _E2, _E3 )                        \
    X( ufloat,  _E0, _E1, _E2, _E3 )

/////////////////////////////////////////////////////
// Macro for all types ID for template instanciation
#define ULIS_FOR_ALL_TYPES_ID_DO( X, _E0, _E1, _E2, _E3 )   \
    X( Type_uint8,      _E0, uint8, _E2, _E3 )              \
    X( Type_uint16,     _E0, uint16, _E2, _E3 )             \
    X( Type_ufloat,     _E0, ufloat, _E2, _E3 )

/////////////////////////////////////////////////////
// Macro for all types for combination template instanciation
#define ULIS_FOR_ALL_TYPES_COMBINATIONS_DO( X, _E0, _E1 )   \
    X( uint8,   uint8   , _E0, _E1 )                        \
    X( uint8,   uint16  , _E0, _E1 )                        \
    X( uint8,   ufloat  , _E0, _E1 )                        \
    X( uint16,  uint8   , _E0, _E1 )                        \
    X( uint16,  uint16  , _E0, _E1 )                        \
    X( uint16,  ufloat  , _E0, _E1 )                        \
    X( ufloat,  uint8   , _E0, _E1 )                        \
    X( ufloat,  uint16  , _E0, _E1 )                        \
    X( ufloat,  ufloat  , _E0, _E1 )

/////////////////////////////////////////////////////
// Model codes
#define ULIS_ColorModel_ANY    0
#define ULIS_ColorModel_GREY   1
#define ULIS_ColorModel_RGB    2
#define ULIS_ColorModel_HSV    3
#define ULIS_ColorModel_HSL    4
#define ULIS_ColorModel_CMY    5
#define ULIS_ColorModel_CMYK   6
#define ULIS_ColorModel_YUV    7
#define ULIS_ColorModel_Lab    8
#define ULIS_ColorModel_XYZ    9
#define ULIS_ColorModel_Yxy    10

/////////////////////////////////////////////////////
// Profile codes
#define ULIS_None                  0
#define ULIS_LinearGrey            1
#define ULIS_sGrey                 2
#define ULIS_Grey2_2_D65           3
#define ULIS_LinearRGB             4
#define ULIS_sRGB                  5
#define ULIS_AdobeRGB              6
#define ULIS_YUV                   7
#define ULIS_XYZ                   8
#define ULIS_LabD50                9
#define ULIS_LabD65                10
#define ULIS_Yxy                   11

/////////////////////////////////////////////////////
// Channels codes
#define ULIS_MAX_CHANNELS          5
#define ULIS_CHANNEL_1_BIT         1
#define ULIS_CHANNEL_2_BIT         2
#define ULIS_CHANNEL_3_BIT         4
#define ULIS_CHANNEL_4_BIT         8
#define ULIS_CHANNEL_5_BIT         16

/////////////////////////////////////////////////////
// Model enum
enum eColorModel {
      ColorModel_GREY   = ULIS_ColorModel_GREY
    , ColorModel_RGB    = ULIS_ColorModel_RGB
    , ColorModel_HSV    = ULIS_ColorModel_HSV
    , ColorModel_HSL    = ULIS_ColorModel_HSL
    , ColorModel_CMY    = ULIS_ColorModel_CMY
    , ColorModel_CMYK   = ULIS_ColorModel_CMYK
    , ColorModel_YUV    = ULIS_ColorModel_YUV
    , ColorModel_Lab    = ULIS_ColorModel_Lab
    , ColorModel_XYZ    = ULIS_ColorModel_XYZ
    , ColorModel_Yxy    = ULIS_ColorModel_Yxy
};


/////////////////////////////////////////////////////
// Models Combinations
#define ULIS_FOR_ALL_COLOR_MODELS_COMBINATIONS_DO( X )  \
    X( Grey,    Grey )                                  \
    X( RGB,     Grey )                                  \
    X( HSV,     Grey )                                  \
    X( HSL,     Grey )                                  \
    X( CMY,     Grey )                                  \
    X( CMYK,    Grey )                                  \
    X( YUV,     Grey )                                  \
    X( Lab,     Grey )                                  \
    X( XYZ,     Grey )                                  \
    X( Yxy,     Grey )                                  \
    X( Grey,    RGB )                                   \
    X( RGB,     RGB )                                   \
    X( HSV,     RGB )                                   \
    X( HSL,     RGB )                                   \
    X( CMY,     RGB )                                   \
    X( CMYK,    RGB )                                   \
    X( YUV,     RGB )                                   \
    X( Lab,     RGB )                                   \
    X( XYZ,     RGB )                                   \
    X( Yxy,     RGB )                                   \
    X( Grey,    HSV )                                   \
    X( RGB,     HSV )                                   \
    X( HSV,     HSV )                                   \
    X( HSL,     HSV )                                   \
    X( CMY,     HSV )                                   \
    X( CMYK,    HSV )                                   \
    X( YUV,     HSV )                                   \
    X( Lab,     HSV )                                   \
    X( XYZ,     HSV )                                   \
    X( Yxy,     HSV )                                   \
    X( Grey,    HSL )                                   \
    X( RGB,     HSL )                                   \
    X( HSV,     HSL )                                   \
    X( HSL,     HSL )                                   \
    X( CMY,     HSL )                                   \
    X( CMYK,    HSL )                                   \
    X( YUV,     HSL )                                   \
    X( Lab,     HSL )                                   \
    X( XYZ,     HSL )                                   \
    X( Yxy,     HSL )                                   \
    X( Grey,    CMY )                                   \
    X( RGB,     CMY )                                   \
    X( HSV,     CMY )                                   \
    X( HSL,     CMY )                                   \
    X( CMY,     CMY )                                   \
    X( CMYK,    CMY )                                   \
    X( YUV,     CMY )                                   \
    X( Lab,     CMY )                                   \
    X( XYZ,     CMY )                                   \
    X( Yxy,     CMY )                                   \
    X( Grey,    CMYK )                                  \
    X( RGB,     CMYK )                                  \
    X( HSV,     CMYK )                                  \
    X( HSL,     CMYK )                                  \
    X( CMY,     CMYK )                                  \
    X( CMYK,    CMYK )                                  \
    X( YUV,     CMYK )                                  \
    X( Lab,     CMYK )                                  \
    X( XYZ,     CMYK )                                  \
    X( Yxy,     CMYK )                                  \
    X( Grey,    YUV )                                   \
    X( RGB,     YUV )                                   \
    X( HSV,     YUV )                                   \
    X( HSL,     YUV )                                   \
    X( CMY,     YUV )                                   \
    X( CMYK,    YUV )                                   \
    X( YUV,     YUV )                                   \
    X( Lab,     YUV )                                   \
    X( XYZ,     YUV )                                   \
    X( Yxy,     YUV )                                   \
    X( Grey,    Lab )                                   \
    X( RGB,     Lab )                                   \
    X( HSV,     Lab )                                   \
    X( HSL,     Lab )                                   \
    X( CMY,     Lab )                                   \
    X( CMYK,    Lab )                                   \
    X( YUV,     Lab )                                   \
    X( Lab,     Lab )                                   \
    X( XYZ,     Lab )                                   \
    X( Yxy,     Lab )                                   \
    X( Grey,    XYZ )                                   \
    X( RGB,     XYZ )                                   \
    X( HSV,     XYZ )                                   \
    X( HSL,     XYZ )                                   \
    X( CMY,     XYZ )                                   \
    X( CMYK,    XYZ )                                   \
    X( YUV,     XYZ )                                   \
    X( Lab,     XYZ )                                   \
    X( XYZ,     XYZ )                                   \
    X( Yxy,     XYZ )                                   \
    X( Grey,    Yxy )                                   \
    X( RGB,     Yxy )                                   \
    X( HSV,     Yxy )                                   \
    X( HSL,     Yxy )                                   \
    X( CMY,     Yxy )                                   \
    X( CMYK,    Yxy )                                   \
    X( YUV,     Yxy )                                   \
    X( Lab,     Yxy )                                   \
    X( XYZ,     Yxy )                                   \
    X( Yxy,     Yxy )

/////////////////////////////////////////////////////
// Extrapolation Method
enum eBorderMode {
      Border_Transparent
    , Border_Constant
    , Border_Extend
    , Border_Wrap
};

/////////////////////////////////////////////////////
// Format Masks
#define ULIS_FORMAT_MASK_LO     0x000FFFFF
#define ULIS_FORMAT_MASK_HI     0xFFF00000
#define ULIS_FORMAT_MASK_LAYOUT 0b11111111111111111001111111111111

/////////////////////////////////////////////////////
// Properties Format Macro Operators
// Format Properties: Write
#define ULIS_W_TYPE( i )        ( i & 0xF )
#define ULIS_W_CHANNELS( i )    ( ( i & 0xF ) << 4 )
#define ULIS_W_MODEL( i )       ( ( i & 0xF ) << 8 )
#define ULIS_W_FLOATING( i )    ( ( i & 1 ) << 12 )
#define ULIS_W_REVERSE( i )     ( ( i & 1 ) << 13 )
#define ULIS_W_SWAP( i )        ( ( i & 1 ) << 14 )
#define ULIS_W_ALPHA( i )       ( ( i & 1 ) << 15 )
#define ULIS_W_DEPTH( i )       ( ( i & 0xF ) << 16 )
#define ULIS_W_PROFILE( i )     ( ( i & 0xF ) << 20 )
#define ULIS_W_PREMULT( i )     ( ( i & 1 ) << 24 )
#define ULIS_W_LINEAR( i )      ( ( i & 1 ) << 25 )
#define ULIS_W_PLANAR( i )      ( ( i & 1 ) << 26 )
// Format Properties: Erase
#define ULIS_E_TYPE             ( ~0xF )
#define ULIS_E_CHANNELS         ( ~( 0xF << 4 ) )
#define ULIS_E_MODEL            ( ~( 0xF << 8 ) )
#define ULIS_E_FLOATING         ( ~( 0x1 << 12 ) )
#define ULIS_E_REVERSE          ( ~( 0x1 << 13 ) )
#define ULIS_E_SWAP             ( ~( 0x1 << 14 ) )
#define ULIS_E_ALPHA            ( ~( 0x1 << 15 ) )
#define ULIS_E_DEPTH            ( ~( 0xF << 16 ) )
#define ULIS_E_PROFILE          ( ~( 0xF << 20 ) )
#define ULIS_E_PREMULT          ( ~( 0x1 << 24 ) )
#define ULIS_E_LINEAR           ( ~( 0x1 << 25 ) )
#define ULIS_E_PLANAR           ( ~( 0x1 << 26 ) )
// Format Properties: Read
#define ULIS_R_TYPE( i )        ( i & 0xF )
#define ULIS_R_CHANNELS( i )    ( ( i >> 4 ) & 0xF )
#define ULIS_R_MODEL( i )       ( ( i >> 8 ) & 0xF )
#define ULIS_R_FLOATING( i )    ( ( i >> 12 ) & 0x1 )
#define ULIS_R_REVERSE( i )     ( ( i >> 13 ) & 0x1 )
#define ULIS_R_SWAP( i )        ( ( i >> 14 ) & 0x1 )
#define ULIS_R_ALPHA( i )       ( ( i >> 15 ) & 0x1 )
#define ULIS_R_DEPTH( i )       ( ( i >> 16 ) & 0xF )
#define ULIS_R_RS( i )          ( ( i >> 13 ) & 0x3 )
#define ULIS_R_PROFILE( i )     ( ( i >> 20 ) & 0xF )
#define ULIS_R_PREMULT( i )     ( ( i >> 24 ) & 0x1 )
#define ULIS_R_LINEAR( i )      ( ( i >> 25 ) & 0x1 )
#define ULIS_R_PLANAR( i )      ( ( i >> 26 ) & 0x1 )

/////////////////////////////////////////////////////
// eBlendMode
enum eBlendMode : uint8
{
      Blend_Normal
    , Blend_Top
    , Blend_Back
    , Blend_Behind
    , Blend_Dissolve
    , Blend_BayerDither8x8
    , Blend_Darken
    , Blend_Multiply
    , Blend_ColorBurn
    , Blend_LinearBurn
    , Blend_DarkerColor
    , Blend_Lighten
    , Blend_Screen
    , Blend_ColorDodge
    , Blend_LinearDodge
    , Blend_LighterColor
    , Blend_Overlay
    , Blend_SoftLight
    , Blend_HardLight
    , Blend_VividLight
    , Blend_LinearLight
    , Blend_PinLight
    , Blend_HardMix
    , Blend_Phoenix
    , Blend_Reflect
    , Blend_Glow
    , Blend_Difference
    , Blend_Exclusion
    , Blend_Add
    , Blend_Substract
    , Blend_Divide
    , Blend_Average
    , Blend_Negation
    , Blend_Hue
    , Blend_Saturation
    , Blend_Color
    , Blend_Luminosity
    , Blend_PartialDerivative
    , Blend_Whiteout
    , Blend_AngleCorrected
    , NumBlendModes
};

static const char* kwBlendMode[] =
{
      "Normal"
    , "Top"
    , "Back"
    , "Behind"
    , "Dissolve"
    , "Dither"
    , "Darken"
    , "Multiply"
    , "ColorBurn"
    , "LinearBurn"
    , "DarkerColor"
    , "Lighten"
    , "Screen"
    , "ColorDodge"
    , "LinearDodge"
    , "LighterColor"
    , "Overlay"
    , "SoftLight"
    , "HardLight"
    , "VividLight"
    , "LinearLight"
    , "PinLight"
    , "HardMix"
    , "Phoenix"
    , "Reflect"
    , "Glow"
    , "Difference"
    , "Exclusion"
    , "Add"
    , "Substract"
    , "Divide"
    , "Average"
    , "Negation"
    , "Hue"
    , "Saturation"
    , "Color"
    , "Luminosity"
    , "PartialDerivative"
    , "Whiteout"
    , "AngleCorrected"
    , "Invalid"
};

/////////////////////////////////////////////////////
// eAlphaMode
enum eAlphaMode : uint8
{
      Alpha_Normal
    , Alpha_Erase
    , Alpha_Top
    , Alpha_Back
    , Alpha_Sub
    , Alpha_Add
    , Alpha_Mul
    , Alpha_Min
    , Alpha_Max
    , NumAlphaModes
};

static const char* kwAlphaMode[] =
{
      "Normal"
    , "Erase"
    , "Top"
    , "Back"
    , "Sub"
    , "Add"
    , "Mul"
    , "Min"
    , "Max"
    , "Invalid"
};

/////////////////////////////////////////////////////
// eBlendQualifier
enum eBlendQualifier : uint8
{
      BlendQualifier_Separable
    , BlendQualifier_NonSeparable
    , BlendQualifier_Misc
};

static
inline
constexpr
eBlendQualifier
BlendingModeQualifier( eBlendMode iBlendingMode )
{
    switch( iBlendingMode ) {
        case Blend_Dissolve             : return  BlendQualifier_Misc;
        case Blend_BayerDither8x8       : return  BlendQualifier_Misc;
        case Blend_DarkerColor          : return  BlendQualifier_NonSeparable;
        case Blend_LighterColor         : return  BlendQualifier_NonSeparable;
        case Blend_Hue                  : return  BlendQualifier_NonSeparable;
        case Blend_Saturation           : return  BlendQualifier_NonSeparable;
        case Blend_Color                : return  BlendQualifier_NonSeparable;
        case Blend_Luminosity           : return  BlendQualifier_NonSeparable;
        case Blend_PartialDerivative    : return  BlendQualifier_NonSeparable;
        case Blend_Whiteout             : return  BlendQualifier_NonSeparable;
        case Blend_AngleCorrected       : return  BlendQualifier_NonSeparable;
        default                         : return  BlendQualifier_Separable;
    }
}

/////////////////////////////////////////////////////
// Blend Loop Macros
#define ULIS_FOR_ALL_SEPARABLE_BM_DO( X, _E0, _E1, _E2, _E3 )   \
    X( Blend_Normal            , _E0, _E1, _E2, _E3 )           \
    X( Blend_Top               , _E0, _E1, _E2, _E3 )           \
    X( Blend_Back              , _E0, _E1, _E2, _E3 )           \
    X( Blend_Behind            , _E0, _E1, _E2, _E3 )           \
    X( Blend_Darken            , _E0, _E1, _E2, _E3 )           \
    X( Blend_Multiply          , _E0, _E1, _E2, _E3 )           \
    X( Blend_ColorBurn         , _E0, _E1, _E2, _E3 )           \
    X( Blend_LinearBurn        , _E0, _E1, _E2, _E3 )           \
    X( Blend_Lighten           , _E0, _E1, _E2, _E3 )           \
    X( Blend_Screen            , _E0, _E1, _E2, _E3 )           \
    X( Blend_ColorDodge        , _E0, _E1, _E2, _E3 )           \
    X( Blend_LinearDodge       , _E0, _E1, _E2, _E3 )           \
    X( Blend_Overlay           , _E0, _E1, _E2, _E3 )           \
    X( Blend_SoftLight         , _E0, _E1, _E2, _E3 )           \
    X( Blend_HardLight         , _E0, _E1, _E2, _E3 )           \
    X( Blend_VividLight        , _E0, _E1, _E2, _E3 )           \
    X( Blend_LinearLight       , _E0, _E1, _E2, _E3 )           \
    X( Blend_PinLight          , _E0, _E1, _E2, _E3 )           \
    X( Blend_HardMix           , _E0, _E1, _E2, _E3 )           \
    X( Blend_Phoenix           , _E0, _E1, _E2, _E3 )           \
    X( Blend_Reflect           , _E0, _E1, _E2, _E3 )           \
    X( Blend_Glow              , _E0, _E1, _E2, _E3 )           \
    X( Blend_Difference        , _E0, _E1, _E2, _E3 )           \
    X( Blend_Exclusion         , _E0, _E1, _E2, _E3 )           \
    X( Blend_Add               , _E0, _E1, _E2, _E3 )           \
    X( Blend_Substract         , _E0, _E1, _E2, _E3 )           \
    X( Blend_Divide            , _E0, _E1, _E2, _E3 )           \
    X( Blend_Average           , _E0, _E1, _E2, _E3 )           \
    X( Blend_Negation          , _E0, _E1, _E2, _E3 )

#define ULIS_FOR_ALL_NONSEPARABLE_BM_DO( X, _E0, _E1, _E2, _E3 )    \
    X( Blend_DarkerColor       , _E0, _E1, _E2, _E3 )               \
    X( Blend_LighterColor      , _E0, _E1, _E2, _E3 )               \
    X( Blend_Hue               , _E0, _E1, _E2, _E3 )               \
    X( Blend_Saturation        , _E0, _E1, _E2, _E3 )               \
    X( Blend_Color             , _E0, _E1, _E2, _E3 )               \
    X( Blend_Luminosity        , _E0, _E1, _E2, _E3 )               \
    X( Blend_PartialDerivative , _E0, _E1, _E2, _E3 )               \
    X( Blend_Whiteout          , _E0, _E1, _E2, _E3 )               \
    X( Blend_AngleCorrected    , _E0, _E1, _E2, _E3 )

#define ULIS_FOR_ALL_MISC_BM_DO( X, _E0, _E1, _E2, _E3 )    \
    X( Blend_Dissolve          , _E0, _E1, _E2, _E3 )       \
    X( Blend_BayerDither8x8    , _E0, _E1, _E2, _E3 )

#define ULIS_FOR_ALL_BM_DO( X, _E0, _E1, _E2, _E3 )             \
    ULIS_FOR_ALL_SEPARABLE_BM_DO(      X, _E0, _E1, _E2, _E3 )  \
    ULIS_FOR_ALL_NONSEPARABLE_BM_DO(   X, _E0, _E1, _E2, _E3 )  \
    ULIS_FOR_ALL_MISC_BM_DO(           X, _E0, _E1, _E2, _E3 )

#define ULIS_FOR_ALL_AM_DO( X, _E0, _E1, _E2, _E3 ) \
    X( Alpha_Normal        , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Erase         , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Top           , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Back          , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Sub           , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Add           , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Mul           , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Min           , _E0, _E1, _E2, _E3 )   \
    X( Alpha_Max           , _E0, _E1, _E2, _E3 )

/////////////////////////////////////////////////////
// eResamplingMethod
enum eResamplingMethod
{
      Resampling_NearestNeighbour
    , Resampling_Bilinear
    , Resampling_Bicubic
    , Resampling_Area
};

static const char* kwResamplingMethod[] =
{
      "NearestNeighbour"
    , "Bilinear"
    , "Bicubic"
    , "Area"
};

/////////////////////////////////////////////////////
// eMipsLayout
enum eMipsLayout
{
      MipsLayout_Standard
    , MipsLayout_MipsOnly
};

static const char* kwMipsLayout[] =
{
      "Standard"
    , "MipsOnly"
};

/////////////////////////////////////////////////////
// eImageFormat
enum eFileFormat {
      FileFormat_png
    , FileFormat_bmp
    , FileFormat_tga
    , FileFormat_jpg
    , FileFormat_hdr
};

static const char* kwImageFormat[] = {
      "png"
    , "bmp"
    , "tga"
    , "jpg"
    , "hdr"
};

/////////////////////////////////////////////////////
// eGradientType
enum eGradientType {
      Gradient_Linear
    , Gradient_Radial
    , Gradient_Angular
    , Gradient_Reflected
    , Gradient_Diamond
};

static const char* kwGradientType[] = {
      "Linear"
    , "Radial"
    , "Angular"
    , "Reflected"
    , "Diamond"
};

/////////////////////////////////////////////////////
// ulError reports
#define ULIS_NO_ERROR                       0
#define ULIS_WARNING_NO_OP                  -1000
#define ULIS_WARNING_NO_OP_GEOMETRY         -1001
#define ULIS_WARNING_NO_OP_BAD_FILE_FORMAT  -1002
#define ULIS_WARNING_NO_OP_BAD_INPUT_DATA   -1003
#define ULIS_ERROR_FORMATS_MISMATCH         1000
#define ULIS_ERROR_CONCURRENT_DATA          1001
#define ULIS_ERROR_BAD_INPUT_DATA           1002

ULIS_NAMESPACE_END

