// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         HostDeviceInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FCPU struct.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
struct ULIS3_API FHostDeviceInfo
{
private:
    FHostDeviceInfo() = delete;
    FHostDeviceInfo( bool iVendor_AMD
                   , bool iVendor_Intel
                   , bool iOS_x64
                   , bool iOS_AVX
                   , bool iOS_AVX512
                   , bool iHW_MMX
                   , bool iHW_x64
                   , bool iHW_ABM
                   , bool iHW_RDRAND
                   , bool iHW_BMI1
                   , bool iHW_BMI2
                   , bool iHW_ADX
                   , bool iHW_PREFETCHWT1
                   , bool iHW_MPX
                   , bool iHW_SSE
                   , bool iHW_SSE2
                   , bool iHW_SSE3
                   , bool iHW_SSSE3
                   , bool iHW_SSE41
                   , bool iHW_SSE42
                   , bool iHW_SSE4a
                   , bool iHW_AES
                   , bool iHW_SHA
                   , bool iHW_AVX
                   , bool iHW_XOP
                   , bool iHW_FMA3
                   , bool iHW_FMA4
                   , bool iHW_AVX2
                   , bool iHW_AVX512_F
                   , bool iHW_AVX512_PF
                   , bool iHW_AVX512_ER
                   , bool iHW_AVX512_CD
                   , bool iHW_AVX512_VL
                   , bool iHW_AVX512_BW
                   , bool iHW_AVX512_DQ
                   , bool iHW_AVX512_IFMA
                   , bool iHW_AVX512_VBMI );

public:
    static FHostDeviceInfo Detect();

public:
    // Public Data Members
    const bool Vendor_AMD;
    const bool Vendor_Intel;
    const bool OS_x64;
    const bool OS_AVX;
    const bool OS_AVX512;
    const bool HW_MMX;
    const bool HW_x64;
    const bool HW_ABM;
    const bool HW_RDRAND;
    const bool HW_BMI1;
    const bool HW_BMI2;
    const bool HW_ADX;
    const bool HW_PREFETCHWT1;
    const bool HW_MPX;
    const bool HW_SSE;
    const bool HW_SSE2;
    const bool HW_SSE3;
    const bool HW_SSSE3;
    const bool HW_SSE41;
    const bool HW_SSE42;
    const bool HW_SSE4a;
    const bool HW_AES;
    const bool HW_SHA;
    const bool HW_AVX;
    const bool HW_XOP;
    const bool HW_FMA3;
    const bool HW_FMA4;
    const bool HW_AVX2;
    const bool HW_AVX512_F;
    const bool HW_AVX512_PF;
    const bool HW_AVX512_ER;
    const bool HW_AVX512_CD;
    const bool HW_AVX512_VL;
    const bool HW_AVX512_BW;
    const bool HW_AVX512_DQ;
    const bool HW_AVX512_IFMA;
    const bool HW_AVX512_VBMI;
};

ULIS3_NAMESPACE_END

