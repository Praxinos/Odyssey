// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         MemoryPoolPolicy.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FMemoryPoolPolicy class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FMemoryPoolPolicy
/// @brief      The FMemoryPoolPolicy class provides a way to control how a
///             memory pool handles its memory usage strategy.
/// @details    The FMemoryPoolPolicy specifies wether a memory pool should
///             allocate more than the target cap if necessary, or fail when
///             requesting memory that isn't available, and the way it should
///             handle defragmentation and page allocation behaviours.
class ULIS_API FMemoryPoolPolicy
{
public:
    FMemoryPoolPolicy(
        bool iAllocPageIfOutOfSpace = true
    );

    bool AllocPageIfOutOfSpace() const;
    void SetAllocPageIfOutOfSpace( bool iValue );

public:
    // Private Static Named Policies
    static const FMemoryPoolPolicy PermissiveGrowth;

private:
    // Private Data Members
    bool bAllocPageIfOutOfSpace;
};

ULIS_NAMESPACE_END

