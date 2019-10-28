// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
#pragma once
#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include <ULIS_FDECL>

class FOdysseySurface;
class FOdysseyBlock;

/////////////////////////////////////////////////////
// Utlity


ODYSSEYIMAGING_API void CopyUTextureDataIntoBlock( FOdysseyBlock* block, UTexture2D* texture );
ODYSSEYIMAGING_API void CopyBlockDataIntoUTexture( FOdysseyBlock* block, UTexture2D* texture );
ODYSSEYIMAGING_API FOdysseyBlock* NewOdysseyBlockFromUTextureData( UTexture2D* texture );
ODYSSEYIMAGING_API void InvalidateSurfaceFromData( FOdysseyBlock* data, FOdysseySurface* surface );
ODYSSEYIMAGING_API void InvalidateSurfaceFromData( FOdysseyBlock* data, FOdysseySurface* surface, int x1, int y1, int x2, int y2 );
ODYSSEYIMAGING_API void InvalidateTextureFromData( FOdysseyBlock* data, UTexture2D* texture);
ODYSSEYIMAGING_API void InvalidateTextureFromData( FOdysseyBlock* data, UTexture2D* texture, const  ::ULIS::FRect& iRect );
ODYSSEYIMAGING_API void InvalidateTextureFromData( FOdysseyBlock* data, UTexture2D* texture, int x1, int y1, int x2, int y2 );
ODYSSEYIMAGING_API void InvalidateTextureFromData( ::ULIS::IBlock* iData, UTexture2D* iTexture, const  ::ULIS::FRect& iRect );
ODYSSEYIMAGING_API void InvalidateSurfaceFromData( ::ULIS::IBlock* iData, FOdysseySurface* iSurface, const  ::ULIS::FRect& iRect );

struct  FOdysseyLiveUpdateInfo
{
    UTexture2D*  main;
    UTexture2D*  live;
    bool         enabled;
};

ODYSSEYIMAGING_API void InvalidateSurfaceCallback( FOdysseyBlock* data, void* info, int x1, int y1, int x2, int y2 );
ODYSSEYIMAGING_API void InvalidateLiveSurfaceCallback( FOdysseyBlock* data, void* info, int x1, int y1, int x2, int y2 );

ODYSSEYIMAGING_API void InvalidateSurfaceCallback( ::ULIS::IBlock* iData, void* iInfo, const ::ULIS::FRect& iRect );
ODYSSEYIMAGING_API void InvalidateLiveSurfaceCallback( ::ULIS::IBlock* iData, void* iInfo, const ::ULIS::FRect& iRect );

/////////////////////////////////////////////////////
// FOdysseySurface


// Odyssey Surface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API FOdysseySurface
{
public:
    // Construction / Destruction

    // Destruction will delete texture if Transient ( not borrowed )
    ~FOdysseySurface();

    // Responsible for its underlyin FOdysseyBlock TArray allocation
    // With transient UTexture
    FOdysseySurface( int iWidth, int iHeight, ETextureSourceFormat fmt = ETextureSourceFormat::TSF_BGRA8 );

    // Responsible for its underlyin FOdysseyBlock TArray allocation
    // With borrowed UTexture, not responsible for its Allocation / Deallocation
    FOdysseySurface( UTexture2D* iTex );

    // Responsible for its underlyin UTexture2D
    // With borrowed FOdysseyBlock, not responsible for its Allocation / Deallocation
    FOdysseySurface( FOdysseyBlock* iBlock );

private:
    // Forbid Copy Constructor and Copy Assignement Operator
    FOdysseySurface( const FOdysseyBlock& ) = delete;
    FOdysseySurface& operator=( const FOdysseyBlock& ) = delete;

public:
    // Public API
    FOdysseyBlock* Block();
    const FOdysseyBlock* Block() const;
    UTexture2D* Texture();
    const UTexture2D* Texture() const;
    bool IsBorrowedTexture() const;
    void CommitBlockChangesIntoTextureBulk();

public:
    // Public Tampon Methods
    int Width();
    int Height();
    void Invalidate();
    void Invalidate( int iX1, int iY1, int iX2, int iY2 );
    void Invalidate( const ::ULIS::FRect& iRect );

private:
    // Private Data
    const bool mBorrowedTex;
    const bool mBorrowedBlock;

    UPROPERTY()
    UTexture2D* mTexture;

    FOdysseyBlock* mBlock;
};

