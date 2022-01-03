// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once
#include "OdysseySurfaceEditable.h"
#include <ULIS>

class FOdysseySurfaceTexture2DEditable;

/////////////////////////////////////////////////////
// Utlity

//TODO: Convert to / Duplicate to UTexture* for each function here
ODYSSEYIMAGING_API void CopyUTextureSourceDataIntoBlock( ::ULIS::FBlock* iBlock,UTexture* iTexture );
ODYSSEYIMAGING_API void CopyUTexturePixelDataIntoBlock( ::ULIS::FBlock* iBlock, UTexture* iTexture );
ODYSSEYIMAGING_API void CopyURenderTargetPixelDataIntoBlock( ::ULIS::FBlock* iBlock,UTextureRenderTarget2D* iRenderTarget );
ODYSSEYIMAGING_API void CopyBlockDataIntoUTexture( const ::ULIS::FBlock* iBlock,UTexture2D* iTexture );
ODYSSEYIMAGING_API void InitTextureWithBlockData( const ::ULIS::FBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat );
ODYSSEYIMAGING_API ::ULIS::FBlock* NewBlockFromUTextureData( UTexture2D* iTexture, ::ULIS::eFormat iFormat );
ODYSSEYIMAGING_API void FillOdysseyBlockFromUTextureData( ::ULIS::FBlock* ioBlock, UTexture2D* iTexture, ::ULIS::eFormat iFormat );
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

    // Responsible for its underlying ::ULIS::FBlock TArray allocation
    // With transient UTexture
    FOdysseySurfaceTexture2DEditable( int iWidth, int iHeight, ::ULIS::eFormat iFormat = ::ULIS::Format_BGRA8 );

    // Responsible for nothing
    FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture, ::ULIS::FBlock* iBlock);

    // Responsible for its underlying ::ULIS::FBlock & TArray allocation
    // With borrowed UTexture, not responsible for its Allocation / Deallocation
    FOdysseySurfaceTexture2DEditable(UTexture2D* iTexture);

    // Responsible for its underlying UTexture2D
    // With borrowed ::ULIS::FBlock, not responsible for its Allocation / Deallocation
    FOdysseySurfaceTexture2DEditable(::ULIS::FBlock* iBlock);

private:
    // Forbid Copy Constructor and Copy Assignement Operator
    FOdysseySurfaceTexture2DEditable( const ::ULIS::FBlock& ) = delete;
    FOdysseySurfaceTexture2DEditable& operator=( const ::ULIS::FBlock& ) = delete;

public:
    // Public API
    ::ULIS::FBlock* Block() override;
    const ::ULIS::FBlock* Block() const override;
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
    ::ULIS::FBlock* mBlock;
    UTexture2D* mTexture;
};
