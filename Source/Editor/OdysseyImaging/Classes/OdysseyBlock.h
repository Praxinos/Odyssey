// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include <ULIS_FDECL>

/////////////////////////////////////////////////////
// FOdysseyBlock
// Block Wrapper for Odyssey
class ODYSSEYIMAGING_API FOdysseyBlock
{
public:
    // Construction / Destruction
    ~FOdysseyBlock();
    FOdysseyBlock( int                          iWidth
                 , int                          iHeight
                 , ETextureSourceFormat         iFormat     = ETextureSourceFormat::TSF_BGRA8
                 , ::ULIS::fpInvalidateFunction iInvFunc    = nullptr
                 , void*                        iInvInfo    = nullptr
                 , bool                         iInitializeData = false );

private:
    // Forbid Copy / Move / Default
    FOdysseyBlock()                                  = delete; // Default Constructor
    FOdysseyBlock( const FOdysseyBlock& )            = delete; // Copy Constructor
    FOdysseyBlock( const FOdysseyBlock&& )           = delete; // Move Constructor
    FOdysseyBlock& operator=( const FOdysseyBlock& ) = delete; // Copy Assignement Operator

public:
    // Public API
    TArray64< uint8 >&          GetArray();
    const TArray64< uint8 >&    GetArray() const;
    ::ULIS::IBlock*             GetIBlock();
    const ::ULIS::IBlock*       GetIBlock() const;
    int                         Width() const;
    int                         Height() const;
    FVector2D                   Size() const;
    ETextureSourceFormat        GetUE4TextureSourceFormat() const;
    uint32                      GetULISFormat() const;
    void                        Reallocate( int                             iWidth
                                          , int                             iHeight
                                          , ETextureSourceFormat            iFormat     = ETextureSourceFormat::TSF_BGRA8
                                          , ::ULIS::fpInvalidateFunction    iInvFunc    = nullptr
                                          , void*                           iInvInfo    = nullptr );

private:
    // Private Data Members
    ETextureSourceFormat    mUE4TextureSourceFormat;
    uint32                  mULISFormat;
    ::ULIS::IBlock*         mIBlock;
    TArray64<uint8>         mArray;
};

