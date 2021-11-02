// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasBlock.tpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the THasBlock class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Layer/Components/HasBlock.h"

#define TEMPLATE template< class BlockType, class BlockAllocatorType >
#define CLASS THasBlock< BlockType, BlockAllocatorType >

ULIS_NAMESPACE_BEGIN
TEMPLATE
CLASS::~THasBlock() {
    if( mBlock )
        BlockAllocatorType::Delete( mBlock );

    ULIS_DEBUG_PRINTF( "THasBlock Destroyed" )
}

TEMPLATE
CLASS::THasBlock(
      uint16 iWidth
    , uint16 iHeight
    , eFormat iFormat
    , const FColorSpace* iColorSpace
    , const TOnBlockChanged< BlockType >& iDelegate
)
    : TOnBlockChanged< BlockType >( iDelegate )
    , mBlock( nullptr )
{
    if( iWidth && iHeight )
        mBlock = BlockAllocatorType::New( iWidth, iHeight, iFormat, iColorSpace );
    Invoke( mBlock );
    ULIS_DEBUG_PRINTF( "THasBlock Created" )
}

TEMPLATE
CLASS::THasBlock(
      BlockType* iBlock
    , const TOnBlockChanged< BlockType >& iDelegate
)
    : TOnBlockChanged< BlockType >( iDelegate )
    , mBlock( iBlock )
{
    Invoke( mBlock );
    ULIS_DEBUG_PRINTF( "THasBlock Created" )
}

TEMPLATE
BlockType*
CLASS::Block() {
    return  mBlock;
}

TEMPLATE
const
BlockType*
CLASS::Block() const {
    return  mBlock;
}

TEMPLATE
void
CLASS::Realloc(
      uint16 iWidth
    , uint16 iHeight
    , eFormat iFormat
    , const FColorSpace* iColorSpace
)
{
    if( mBlock )
        BlockAllocatorType::Delete( mBlock );
    mBlock = BlockAllocatorType::New( iWidth, iHeight, iFormat, iColorSpace );
    Invoke( mBlock );
}

TEMPLATE
void
CLASS::Replace( BlockType* iValue )
{
    if( mBlock )
        BlockAllocatorType::Delete( mBlock );
    mBlock = iValue;
    Invoke( mBlock );
}

ULIS_NAMESPACE_END

// Template Macro Utility
#undef TEMPLATE
#undef CLASS

