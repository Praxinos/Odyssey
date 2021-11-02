// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Color.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FColor class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Image/Sample.h"
#pragma push_macro( "RGB" )

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FColor
/// @brief      The FColor class provides a mean of storing and manipulating
///             colors in various formats.
/// @details    A FColor allocates its own storage space and is responsible for
///             its memory.
///
///             \sa ISample
///             \sa FPixel
class ULIS_API FColor final
    : public ISample
{
public:
    /*! Destructor, manages lifetime of internal buffer. */
    ~FColor();

    /*! Default constructor, with default Format eFormat::Format_RGBA8 */
    FColor();

    /*! Constructor, from format and optional colorspace. */
    FColor( eFormat iFormat, const FColorSpace* iColorSpace = nullptr );

    /*! Constructor, from external data, format and optional colorspace. */
    FColor( const uint8* iData, eFormat iFormat, const FColorSpace* iColorSpace = nullptr );

    /*! Constructor, from pixel. */
    FColor( const FPixel& iPixel );

    /*! Copy Constructor */
    FColor( const FColor& iValue );

    /*! Constructor from Sample */
    FColor( const ISample& iValue );

    /*! Move Constructor. */
    FColor( FColor&& iValue );

    /*! Assignment Operator. */
    FColor& operator=( const FColor& iOther );

    /*! Initializer list constructor */
    template< typename T >
    FColor( eFormat iFormat, std::initializer_list< T > iValues, const FColorSpace* iColorSpace = nullptr );

    /*! Static maker, build a color from RGBA8 values, the color will have format eFormat::Format_RGBA8 */
    static FColor RGB( uint8 iR, uint8 iG, uint8 iB, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from GA8 values, the color will have format eFormat::Format_GA8 */
    static FColor GreyA8( uint8 iGrey, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from GA16 values, the color will have format eFormat::Format_GA16 */
    static FColor GreyA16( uint16 iGrey, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from GA32 values, the color will have format eFormat::Format_GA32 */
    //DISABLED:UINT32static FColor GreyA32( uint32 iGrey, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from GAF values, the color will have format eFormat::Format_GAF */
    static FColor GreyAF( ufloat iGrey, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from GAD values, the color will have format eFormat::Format_GAD */
    //DISABLED:DOUBLEstatic FColor GreyAD( udouble iGrey, udouble iA = 1.0 );

    /*! Static maker, build a color from RGBA8 values, the color will have format eFormat::Format_RGBA8 */
    static FColor RGBA8( uint8 iR, uint8 iG, uint8 iB, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from RGBA16 values, the color will have format eFormat::Format_RGBA16 */
    static FColor RGBA16( uint16 iR, uint16 iG, uint16 iB, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from RGBA32 values, the color will have format eFormat::Format_RGBA32 */
    //DISABLED:UINT32static FColor RGBA32( uint32 iR, uint32 iG, uint32 iB, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from RGBAF values, the color will have format eFormat::Format_RGBAF */
    static FColor RGBAF( ufloat iR, ufloat iG, ufloat iB, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from RGBAD values, the color will have format eFormat::Format_RGBAD */
    //DISABLED:DOUBLEstatic FColor RGBAD( udouble iR, udouble iG, udouble iB, udouble iA = 1.0 );

    /*! Static maker, build a color from HSVA8 values, the color will have format eFormat::Format_HSVA8 */
    static FColor HSVA8( uint8 iH, uint8 iS, uint8 iV, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from HSVA16 values, the color will have format eFormat::Format_HSVA16 */
    static FColor HSVA16( uint16 iH, uint16 iS, uint16 iV, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from HSVA32 values, the color will have format eFormat::Format_HSVA32 */
    //DISABLED:UINT32static FColor HSVA32( uint32 iH, uint32 iS, uint32 iV, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from HSVAF values, the color will have format eFormat::Format_HSVAF */
    static FColor HSVAF( ufloat iH, ufloat iS, ufloat iV, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from HSVAD values, the color will have format eFormat::Format_HSVAD */
    //DISABLED:DOUBLEstatic FColor HSVAD( udouble iH, udouble iS, udouble iV, udouble iA = 1.0 );

    /*! Static maker, build a color from HSLA8 values, the color will have format eFormat::Format_HSLA8 */
    static FColor HSLA8( uint8 iH, uint8 iS, uint8 iL, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from HSLA16 values, the color will have format eFormat::Format_HSLA16 */
    static FColor HSLA16( uint16 iH, uint16 iS, uint16 iL, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from HSLA32 values, the color will have format eFormat::Format_HSLA32 */
    //DISABLED:UINT32static FColor HSLA32( uint32 iH, uint32 iS, uint32 iL, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from HSLAF values, the color will have format eFormat::Format_HSLAF */
    static FColor HSLAF( ufloat iH, ufloat iS, ufloat iL, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from HSLAD values, the color will have format eFormat::Format_HSLAD */
    //DISABLED:DOUBLEstatic FColor HSLAD( udouble iH, udouble iS, udouble iL, udouble iA = 1.0 );

    /*! Static maker, build a color from CMYA8 values, the color will have format eFormat::Format_CMYA8 */
    static FColor CMYA8( uint8 iC, uint8 iM, uint8 iY, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from CMYA16 values, the color will have format eFormat::Format_CMYA16 */
    static FColor CMYA16( uint16 iC, uint16 iM, uint16 iY, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from CMYA32 values, the color will have format eFormat::Format_CMYA32 */
    //DISABLED:UINT32static FColor CMYA32( uint32 iC, uint32 iM, uint32 iY, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from CMYAF values, the color will have format eFormat::Format_CMYAF */
    static FColor CMYAF( ufloat iC, ufloat iM, ufloat iY, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from CMYAD values, the color will have format eFormat::Format_CMYAD */
    //DISABLED:DOUBLEstatic FColor CMYAD( udouble iC, udouble iM, udouble iY, udouble iA = 1.0 );

    /*! Static maker, build a color from CMYKA8 values, the color will have format eFormat::Format_CMYKA8 */
    static FColor CMYKA8( uint8 iC, uint8 iM, uint8 iY, uint8 iK, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from CMYKA16 values, the color will have format eFormat::Format_CMYKA16 */
    static FColor CMYKA16( uint16 iC, uint16 iM, uint16 iY, uint16 iK, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from CMYKA32 values, the color will have format eFormat::Format_CMYKA32 */
    //DISABLED:UINT32static FColor CMYKA32( uint32 iC, uint32 iM, uint32 iY, uint32 iK, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from CMYKAF values, the color will have format eFormat::Format_CMYKAF */
    static FColor CMYKAF( ufloat iC, ufloat iM, ufloat iY, ufloat iK, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from CMYKAD values, the color will have format eFormat::Format_CMYKAD */
    //DISABLED:DOUBLEstatic FColor CMYKAD( udouble iC, udouble iM, udouble iY, udouble iK, udouble iA = 1.0 );

    /*! Static maker, build a color from YUVA8 values, the color will have format eFormat::Format_YUVA8 */
    static FColor YUVA8( uint8 iY, uint8 iU, uint8 iV, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from YUVA16 values, the color will have format eFormat::Format_YUVA16 */
    static FColor YUVA16( uint16 iY, uint16 iU, uint16 iV, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from YUVA32 values, the color will have format eFormat::Format_YUVA32 */
    //DISABLED:UINT32static FColor YUVA32( uint32 iY, uint32 iU, uint32 iV, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from YUVAF values, the color will have format eFormat::Format_YUVAF */
    static FColor YUVAF( ufloat iY, ufloat iU, ufloat iV, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from YUVAD values, the color will have format eFormat::Format_YUVAD */
    //DISABLED:DOUBLEstatic FColor YUVAD( udouble iY, udouble iU, udouble iV, udouble iA = 1.0 );

    /*! Static maker, build a color from LabA8 values, the color will have format eFormat::Format_LabA8 */
    static FColor LabA8( uint8 iL, uint8 ia, uint8 ib, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from LabA16 values, the color will have format eFormat::Format_LabA16 */
    static FColor LabA16( uint16 iL, uint16 ia, uint16 ib, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from LabA32 values, the color will have format eFormat::Format_LabA32 */
    //DISABLED:UINT32static FColor LabA32( uint32 iL, uint32 ia, uint32 ib, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from LabAF values, the color will have format eFormat::Format_LabAF */
    static FColor LabAF( ufloat iL, ufloat ia, ufloat ib, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from LabAD values, the color will have format eFormat::Format_LabAD */
    //DISABLED:DOUBLEstatic FColor LabAD( udouble iL, udouble ia, udouble ib, udouble iA = 1.0 );

    /*! Static maker, build a color from XYZA8 values, the color will have format eFormat::Format_XYZA8 */
    static FColor XYZA8( uint8 iX, uint8 iY, uint8 iZ, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from XYZA16 values, the color will have format eFormat::Format_XYZA16 */
    static FColor XYZA16( uint16 iX, uint16 iY, uint16 iZ, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from XYZA32 values, the color will have format eFormat::Format_XYZA32 */
    //DISABLED:UINT32static FColor XYZA32( uint32 iX, uint32 iY, uint32 iZ, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from XYZAF values, the color will have format eFormat::Format_XYZAF */
    static FColor XYZAF( ufloat iX, ufloat iY, ufloat iZ, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from XYZAD values, the color will have format eFormat::Format_XYZAD */
    //DISABLED:DOUBLEstatic FColor XYZAD( udouble iX, udouble iY, udouble iZ, udouble iA = 1.0 );

    /*! Static maker, build a color from YxyA8 values, the color will have format eFormat::Format_YxyA8 */
    static FColor YxyA8( uint8 iY, uint8 ix, uint8 iy, uint8 iA = UINT8_MAX );

    /*! Static maker, build a color from YxyA16 values, the color will have format eFormat::Format_YxyA16 */
    static FColor YxyA16( uint16 iY, uint16 ix, uint16 iy, uint16 iA = UINT16_MAX );

    //DISABLED:UINT32/*! Static maker, build a color from YxyA32 values, the color will have format eFormat::Format_YxyA32 */
    //DISABLED:UINT32static FColor YxyA32( uint32 iY, uint32 ix, uint32 iy, uint32 iA = UINT32_MAX );

    /*! Static maker, build a color from YxyAF values, the color will have format eFormat::Format_YxyAF */
    static FColor YxyAF( ufloat iY, ufloat ix, ufloat iy, ufloat iA = 1.f );

    //DISABLED:DOUBLE/*! Static maker, build a color from YxyAD values, the color will have format eFormat::Format_YxyAD */
    //DISABLED:DOUBLEstatic FColor YxyAD( udouble iY, udouble ix, udouble iy, udouble iA = 1.0 );

public:
    static const FColor Black;
    static const FColor White;
    static const FColor Red;
    static const FColor Green;
    static const FColor Blue;
    static const FColor Yellow;
    static const FColor Magenta;
    static const FColor Cyan;
    static const FColor Transparent;
};

extern template FColor::FColor( eFormat, std::initializer_list< int >, const FColorSpace* );
extern template FColor::FColor( eFormat, std::initializer_list< uint8 >, const FColorSpace* );
extern template FColor::FColor( eFormat, std::initializer_list< uint16 >, const FColorSpace* );
//DISABLED:UINT32extern template FColor::FColor( eFormat, std::initializer_list< uint32 >, const FColorSpace* );
extern template FColor::FColor( eFormat, std::initializer_list< ufloat >, const FColorSpace* );
//DISABLED:DOUBLEextern template FColor::FColor( eFormat, std::initializer_list< udouble >, const FColorSpace* );

ULIS_NAMESPACE_END

#pragma pop_macro( "RGB" )

