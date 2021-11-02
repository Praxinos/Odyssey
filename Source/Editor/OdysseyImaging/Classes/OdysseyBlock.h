// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include <ULIS>

//
ODYSSEYIMAGING_API int UE4TextureSourceFormatBytesPerPixel(ETextureSourceFormat iFormat);
ODYSSEYIMAGING_API bool UE4TextureSourceFormatNeedsConversionToULISFormat( ETextureSourceFormat iFormat );
ODYSSEYIMAGING_API void ConvertUE4TextureSourceFormatToULISFormat( const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat);
ODYSSEYIMAGING_API void ConvertULISFormatToUE4TextureSourceFormat(const uint8* iSrc, uint8* oDst, int iWidth, int iHeight, ETextureSourceFormat iFormat);

ODYSSEYIMAGING_API ::ULIS::eFormat ULISFormatForUE4TextureSourceFormat( ETextureSourceFormat iFormat );
ODYSSEYIMAGING_API ::ULIS::eFormat ULISFormatForUE4PixelFormat( EPixelFormat iFormat );
ODYSSEYIMAGING_API ETextureSourceFormat UE4TextureSourceFormatForULISFormat(::ULIS::eFormat iFormat);
ODYSSEYIMAGING_API EPixelFormat UE4PixelFormatForULISFormat( ::ULIS::eFormat iFormat );

/////////////////////////////////////////////////////
// FOdysseyBlock
// Block Wrapper for Odyssey
class ODYSSEYIMAGING_API FOdysseyBlock// : public TSharedFromThis<FOdysseyBlock>
{
public:
    // Construction / Destruction
    ~FOdysseyBlock();
    FOdysseyBlock(
          int iWidth
        , int iHeight
        , ::ULIS::eFormat iFormat
        , const ::ULIS::FOnInvalidBlock& iInvFunc = ::ULIS::FOnInvalidBlock()
        , bool iInitializeData = false
    );

private:
    // Forbid Copy / Move / Default
    FOdysseyBlock()                                  = delete; // Default Constructor
    FOdysseyBlock( const FOdysseyBlock& )            = delete; // Copy Constructor
    FOdysseyBlock( const FOdysseyBlock&& )           = delete; // Move Constructor
    FOdysseyBlock& operator=( const FOdysseyBlock& ) = delete; // Copy Assignement Operator

public:
    // Public API
    void Reallocate(
          int  iWidth
        , int  iHeight
        , ::ULIS::eFormat iFormat
        , const ::ULIS::FOnInvalidBlock& iInvFunc = ::ULIS::FOnInvalidBlock()
        , bool iInitializeData = false
    );

    TArray64< uint8 >&          GetArray();
    const TArray64< uint8 >&    GetArray() const;
    ::ULIS::FBlock*             GetBlock();
    const ::ULIS::FBlock*       GetBlock() const;
    int                         Width() const;
    int                         Height() const;
    FVector2D                   Size() const;
    ::ULIS::eFormat             Format() const; 
    void                        ResyncData();

private:
    // Private Data Members
    ::ULIS::FBlock* mBlock;
    TArray64<uint8> mArray;
};

