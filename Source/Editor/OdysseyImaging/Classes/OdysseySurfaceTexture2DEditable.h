// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once
#include "OdysseySurfaceEditable.h"
#include <ULIS>

class FOdysseySurfaceTexture2DEditable;
class FOdysseyBlock;

/////////////////////////////////////////////////////
// Utlity

//TODO: Convert to / Duplicate to UTexture* for each function here
ODYSSEYIMAGING_API void CopyUTextureSourceDataIntoBlock( FOdysseyBlock* iBlock,UTexture* iTexture );
ODYSSEYIMAGING_API void CopyUTexturePixelDataIntoBlock( FOdysseyBlock* iBlock, UTexture* iTexture );
ODYSSEYIMAGING_API void CopyURenderTargetPixelDataIntoBlock( FOdysseyBlock* iBlock,UTextureRenderTarget2D* iRenderTarget );
ODYSSEYIMAGING_API void CopyBlockDataIntoUTexture( const FOdysseyBlock* iBlock,UTexture2D* iTexture );
ODYSSEYIMAGING_API void InitTextureWithBlockData( const FOdysseyBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat );
ODYSSEYIMAGING_API FOdysseyBlock* NewOdysseyBlockFromUTextureData( UTexture2D* iTexture, ::ULIS::eFormat iFormat );
ODYSSEYIMAGING_API void FillOdysseyBlockFromUTextureData( FOdysseyBlock* ioBlock, UTexture2D* iTexture, ::ULIS::eFormat iFormat );
ODYSSEYIMAGING_API void InvalidateTextureFromData( const ::ULIS::FBlock* iData, UTexture2D* iTexture, const ::ULIS::FRectI* iRects, const uint32 iNumRects );
ODYSSEYIMAGING_API void InvalidateSurfaceFromData( const ::ULIS::FBlock* iData, FOdysseySurfaceTexture2DEditable* iSurface, const ::ULIS::FRectI* iRects, const uint32 iNumRects );
ODYSSEYIMAGING_API void InvalidateSurfaceCallback( const ::ULIS::FBlock* iBlock, const ::ULIS::FRectI* iRects, const uint32 iNumRects, void* iInfo );

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API FOdysseySurfaceTexture2DEditable :
    public IOdysseySurfaceEditable
{
public:
    // Construction / Destruction
    // Destruction will delete texture if Transient ( not borrowed )
    virtual ~FOdysseySurfaceTexture2DEditable();

    // Responsible for its underlying FOdysseyBlock TArray allocation
    // With transient UTexture
    FOdysseySurfaceTexture2DEditable( int iWidth, int iHeight, ::ULIS::eFormat iFormat = ::ULIS::Format_BGRA8 );

    // Responsible for nothing
    FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture, FOdysseyBlock* iBlock);

    // Responsible for its underlying FOdysseyBlock & TArray allocation
    // With borrowed UTexture, not responsible for its Allocation / Deallocation
    FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture);

    // Responsible for its underlying UTexture2D
    // With borrowed FOdysseyBlock, not responsible for its Allocation / Deallocation
    FOdysseySurfaceTexture2DEditable(FOdysseyBlock* iBlock);

private:
    // Forbid Copy Constructor and Copy Assignement Operator
    FOdysseySurfaceTexture2DEditable( const FOdysseyBlock& ) = delete;
    FOdysseySurfaceTexture2DEditable& operator=( const FOdysseyBlock& ) = delete;

public:
    // Public API
    FOdysseyBlock* Block() override;
    const FOdysseyBlock* Block() const override;
    virtual UTexture2D* Texture();
    virtual const UTexture2D* Texture() const;

    bool IsBorrowedTexture() const;
    virtual int Width();
    virtual int Height();
    void Invalidate() override;
    //void Invalidate(int iX1,int iY1,int iX2,int iY2);
    //void Invalidate(const ::ULIS::FRectI& iRect);
    void Invalidate( const ::ULIS::FRectI* iRects, const uint32 iNumRects ) override;

private:
    // Private Data
    const bool mIsBorrowedTexture;
    const bool mIsBorrowedBlock;
    FOdysseyBlock* mBlock;
    UTexture2D* mTexture;
};
