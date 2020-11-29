// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once
#include "OdysseySurface.h"
#include <ULIS3>

class FOdysseySurfaceEditable;
class FOdysseyBlock;

/////////////////////////////////////////////////////
// Utlity
ODYSSEYIMAGING_API void CopyUTextureSourceDataIntoBlock(FOdysseyBlock* iBlock,UTexture2D* iTexture);
ODYSSEYIMAGING_API void CopyUTexturePixelDataIntoBlock(FOdysseyBlock* iBlock, UTexture2D* iTexture);
ODYSSEYIMAGING_API void CopyURenderTargetPixelDataIntoBlock(FOdysseyBlock* iBlock,UTextureRenderTarget2D* iRenderTarget);
ODYSSEYIMAGING_API void CopyBlockDataIntoUTexture(const FOdysseyBlock* iBlock,UTexture2D* iTexture);
ODYSSEYIMAGING_API void InitTextureWithBlockData(const FOdysseyBlock* iBlock, UTexture2D* iTexture, ETextureSourceFormat iFormat);
ODYSSEYIMAGING_API FOdysseyBlock* NewOdysseyBlockFromUTextureData(UTexture2D* iTexture, ::ul3::tFormat iFormat);
ODYSSEYIMAGING_API void FillOdysseyBlockFromUTextureData(FOdysseyBlock* ioBlock, UTexture2D* iTexture, ::ul3::tFormat iFormat);
ODYSSEYIMAGING_API void InvalidateSurfaceFromData(const FOdysseyBlock* iData,FOdysseySurfaceEditable* iSurface);
ODYSSEYIMAGING_API void InvalidateSurfaceFromData(const FOdysseyBlock* iData,FOdysseySurfaceEditable* iSurface,int iX1,int iY1,int iX2,int iY2);
ODYSSEYIMAGING_API void InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture);
ODYSSEYIMAGING_API void InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect);
ODYSSEYIMAGING_API void InvalidateTextureFromData(const FOdysseyBlock* iData,UTexture2D* iTexture,int iX1,int iY1,int iX2,int iY2);
ODYSSEYIMAGING_API void InvalidateTextureFromData(const ::ul3::FBlock* iData,UTexture2D* iTexture,const ::ul3::FRect& iRect);
ODYSSEYIMAGING_API void InvalidateSurfaceFromData(const ::ul3::FBlock* iData,FOdysseySurfaceEditable* iSurface,const ::ul3::FRect& iRect);

ODYSSEYIMAGING_API void InvalidateSurfaceCallback(const FOdysseyBlock* iData,void* iInfo,int iX1,int iY1,int iX2,int iY2);
ODYSSEYIMAGING_API void InvalidateSurfaceCallback(const ::ul3::FBlock* iData,void* iInfo,const ::ul3::FRect& iRect);

/////////////////////////////////////////////////////
// FOdysseySurface
// Allows pixel wise manipulation and easy interfacing with UTexture
class ODYSSEYIMAGING_API FOdysseySurfaceEditable :
    public IOdysseySurface
{
public:
    // Construction / Destruction

    // Destruction will delete texture if Transient ( not borrowed )
    virtual ~FOdysseySurfaceEditable();

    // Responsible for its underlyin FOdysseyBlock TArray allocation
    // With transient UTexture
    FOdysseySurfaceEditable(int iWidth,int iHeight, ::ul3::tFormat iFormat = ULIS3_FORMAT_BGRA8);

	// Responsible for nothing
	FOdysseySurfaceEditable(UTexture2D* iTexture, FOdysseyBlock* iBlock);

    // Responsible for its underlyin FOdysseyBlock TArray allocation
    // With borrowed UTexture, not responsible for its Allocation / Deallocation
    FOdysseySurfaceEditable(UTexture2D* iTexture);

    // Responsible for its underlyin UTexture2D
    // With borrowed FOdysseyBlock, not responsible for its Allocation / Deallocation
    FOdysseySurfaceEditable(FOdysseyBlock* iBlock);

private:
    // Forbid Copy Constructor and Copy Assignement Operator
    FOdysseySurfaceEditable(const FOdysseyBlock&)            = delete;
    FOdysseySurfaceEditable& operator=(const FOdysseyBlock&) = delete;

public:
    // Public API
    FOdysseyBlock*          Block();
    const FOdysseyBlock*    Block() const;
    virtual UTexture2D*     Texture();
    virtual const UTexture2D*     Texture() const;

    bool IsBorrowedTexture() const;
    void CommitBlockChangesIntoTextureBulk();

public:
    // Public Tampon Methods
    virtual int  Width();
    virtual int  Height();
    void Invalidate();
    void Invalidate(int iX1,int iY1,int iX2,int iY2);
    void Invalidate(const ::ul3::FRect& iRect);

private:
    // Private Data
    const bool mIsBorrowedTexture;
    const bool mIsBorrowedBlock;

    FOdysseyBlock* mBlock;
    UTexture2D* mTexture;
};
