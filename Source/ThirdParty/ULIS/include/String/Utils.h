// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Utils.h
* @author       Clement Berthaud
* @brief        This file provides string utility tools.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
void ULIS_API ReplaceAllOccurences( std::string& iStr, const std::string& iFrom, const std::string& iTo );
size_t ULIS_API LevenshteinDistance( const char* iA, size_t iLenA, const char* iB, size_t iLenB );
ULIS_NAMESPACE_END

