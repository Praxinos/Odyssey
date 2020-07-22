// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         ConvBuffer.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the ConvBuffer entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// Typedefs
typedef void (*fpConversionInvocation)( const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
ULIS3_API fpConversionInvocation QueryDispatchedConversionInvocation( uint32 iSrcFormat, uint32 iDstFormat );

/////////////////////////////////////////////////////
// Explicit Conv Entry Points
//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ To Grey
template< typename T1, typename T2 > void ConvBufferGreyToGrey( const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToGrey( const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToGrey(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToRGB
template< typename T1, typename T2 > void ConvBufferGreyToRGB(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToRGB(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToRGB(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToHSV
template< typename T1, typename T2 > void ConvBufferGreyToHSV(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToHSV(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToHSV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToHSL
template< typename T1, typename T2 > void ConvBufferGreyToHSL(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToHSL(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToHSL(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToCMY
template< typename T1, typename T2 > void ConvBufferGreyToCMY(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToCMY(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToCMY(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- ToCMYK
template< typename T1, typename T2 > void ConvBufferGreyToCMYK( const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToCMYK( const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToCMYK(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToYUV
template< typename T1, typename T2 > void ConvBufferGreyToYUV(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToYUV(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToYUV(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToLab
template< typename T1, typename T2 > void ConvBufferGreyToLab(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToLab(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToLab(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToXYZ
template< typename T1, typename T2 > void ConvBufferGreyToXYZ(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToXYZ(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToXYZ(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------- ToYxy
template< typename T1, typename T2 > void ConvBufferGreyToYxy(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferRGBToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSVToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferHSLToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferCMYKToYxy(  const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYUVToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferLabToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferXYZToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );
template< typename T1, typename T2 > void ConvBufferYxyToYxy(   const FFormatInfo* iSrcFormat, const tByte* iSrc, const FFormatInfo* iDstFormat, tByte* iDst, tSize iLen );

ULIS3_NAMESPACE_END

