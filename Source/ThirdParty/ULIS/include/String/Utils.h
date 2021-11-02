// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Utils.h
* @author       Clement Berthaud
* @brief        This file provides string utility tools.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
void ULIS_API ReplaceAllOccurences( std::string& iStr, const std::string& iFrom, const std::string& iTo );
size_t ULIS_API LevenshteinDistance( const char* iA, size_t iLenA, const char* iB, size_t iLenB );
ULIS_NAMESPACE_END

