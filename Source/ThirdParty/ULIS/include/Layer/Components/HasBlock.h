// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasBlock.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the THasBlock class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Core/CallbackCapable.h"

ULIS_NAMESPACE_BEGIN
template< class BlockType > using TBlockChangedDelegate = TLambdaCallback< void, const BlockType* >;
template< class BlockType > using TOnBlockChanged = TCallbackCapable< TBlockChangedDelegate< BlockType > >;
/////////////////////////////////////////////////////
/// @class      THasBlock
/// @brief      Simple HasBlock class.
template< class BlockType, class BlockAllocatorType >
class THasBlock
    : public TOnBlockChanged< BlockType >
{
protected:
    ~THasBlock();
    THasBlock(
          uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat = eFormat::Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
        , const TOnBlockChanged< BlockType >& iDelegate = TOnBlockChanged< BlockType >()
    );

    THasBlock(
          BlockType* iBlock
        , const TOnBlockChanged< BlockType >& iDelegate = TOnBlockChanged< BlockType >()
    );

public:
    BlockType* Block();
    const BlockType* Block() const;

protected:
    void Realloc(
          uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat = eFormat::Format_RGBA8
        , const FColorSpace* iColorSpace = nullptr
    );
    void Replace( BlockType* iValue );

private:
    BlockType* mBlock;
};

ULIS_NAMESPACE_END

#include "Layer/Components/HasBlock.tpp"

