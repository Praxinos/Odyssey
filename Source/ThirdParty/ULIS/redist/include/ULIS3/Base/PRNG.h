// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         PRNG.h
* @author       Clement Berthaud
* @brief        This file provides declaration for weak PRNG functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS3_NAMESPACE_BEGIN
ULIS3_API void ResetWeakPRNGSeed();
ULIS3_API void SetWeakPRNGSeed( uint32 iVal );
ULIS3_API uint32 GetWeakPRNGSeed();
ULIS3_API uint32 GenerateWeakPRNG();
ULIS3_NAMESPACE_END

