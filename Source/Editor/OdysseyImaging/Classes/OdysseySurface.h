// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include <ULIS_FDECL>

class FOdysseySurface;
class FOdysseyBlock;

/////////////////////////////////////////////////////
// Utlity

ODYSSEYIMAGING_API void CopyUTextureDataIntoBlock( FOdysseyBlock* iBlock, UTexture2D* iTexture );
ODYSSEYIMAGING_API void CopyBlockDataIntoUTexture( FOdysseyBlock* iBlock, UTexture2D* iTexture );
ODYSSEYIMAGING_API FOdysseyBlock* NewOdysseyBlockFromUTextureData( UTexture2D* iTexture );
ODYSSEYIMAGING_API void InvalidateSurfaceFromData( FOdysseyBlock* iData, FOdysseySurface* iSurface );
ODYSSEYIMAGING_API void InvalidateSurfaceFromData( FOdysseyBlock* iData, FOdysseySurface* iSurface, int iX1, int iY1, int iX2, int iY2 );
ODYSSEYIMAGING_API void InvalidateTextureFromData( FOdysseyBlock* iData, UTexture2D* iTexture );
ODYSSEYIMAGING_API void InvalidateTextureFromData( FOdysseyBlock* iData, UTexture2D* iTexture, const ::ULIS::FRect& iRect );
ODYSSEYIMAGING_API void InvalidateTextureFromData( FOdysseyBlock* iData, UTexture2D* iTexture, int iX1, int iY1, int iX2, int iY2 );
ODYSSEYIMAGING_API void InvalidateTextureFromData( ::ULIS::IBlock* iData, UTexture2D* iTexture, const ::ULIS::FRect& iRect );
ODYSSEYIMAGING_API void InvalidateSurfaceFromData( ::ULIS::IBlock* iData, FOdysseySurface* iSurface, const ::ULIS::FRect& iRect );

struct FOdysseyLiveUpdateInfo
{
    UTexture2D* main;
    UTexture2D* live;
    bool        enabled;
};

ODYSSEYIMAGING_API void InvalidateSurfaceCallback( FOdysseyBlock* iData, void* iInfo, int iX1, int iY1, int iX2, int iY2 );
ODYSSEYIMAGING_API void InvalidateLiveSurfaceCallback( FOdysseyBlock* iData, void* iInfo, int iX1, int iY1, int iX2, int iY2 );

ODYSSEYIMAGING_API void InvalidateSurfaceCallback( ::ULIS::IBlock* iData, void* iInfo, const ::ULIS::FRect& iRect );
ODYSSEYIMAGING_API void InvalidateLiveSurfaceCallback( ::ULIS::IBlock* iData, void* iInfo, const ::ULIS::FRect& iRect );

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API FOdysseySurface
{
public:
    // Construction / Destruction

    // Destruction will delete texture if Transient ( not borrowed )
    ~FOdysseySurface();

    // Responsible for its underlyin FOdysseyBlock TArray allocation
    // With transient UTexture
    FOdysseySurface( int iWidth, int iHeight, ETextureSourceFormat iFormat = ETextureSourceFormat::TSF_BGRA8 );

    // Responsible for its underlyin FOdysseyBlock TArray allocation
    // With borrowed UTexture, not responsible for its Allocation / Deallocation
    FOdysseySurface( UTexture2D* iTexture );

    // Responsible for its underlyin UTexture2D
    // With borrowed FOdysseyBlock, not responsible for its Allocation / Deallocation
    FOdysseySurface( FOdysseyBlock* iBlock );

private:
    // Forbid Copy Constructor and Copy Assignement Operator
    FOdysseySurface( const FOdysseyBlock& )            = delete;
    FOdysseySurface& operator=( const FOdysseyBlock& ) = delete;

public:
    // Public API
    FOdysseyBlock*          Block();
    const FOdysseyBlock*    Block() const;
    UTexture2D*             Texture();
    const UTexture2D*       Texture() const;

    bool IsBorrowedTexture() const;
    void CommitBlockChangesIntoTextureBulk();

public:
    // Public Tampon Methods
    int  Width();
    int  Height();
    void Invalidate();
    void Invalidate( int iX1, int iY1, int iX2, int iY2 );
    void Invalidate( const ::ULIS::FRect& iRect );

private:
    // Private Data
    const bool mIsBorrowedTexture;
    const bool mIsBorrowedBlock;

    UPROPERTY()
    UTexture2D* mTexture;

    FOdysseyBlock* mBlock;
};
