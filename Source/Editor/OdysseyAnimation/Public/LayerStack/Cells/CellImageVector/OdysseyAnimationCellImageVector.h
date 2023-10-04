// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationImageRenderingAbility.h"

class UOdysseyAnimationLayerImageVector;
class FOdysseyVectorBlock;
class FOdysseyMediaVector;


class ODYSSEYANIMATION_API FOdysseyAnimationCellImageVector
    : public FOdysseyAnimationCell
{    
public:
    static TSharedRef<FOdysseyAnimationCellImageVector> Create(UOdysseyAnimationLayerImageVector* iLayer, int iLength, int iWidth, int iHeight);
    static const FName& StaticType();

public:
    virtual ~FOdysseyAnimationCellImageVector();
    FOdysseyAnimationCellImageVector(UOdysseyAnimationLayerImageVector* iLayer, int iLength);

    void Init(int iWidth, int iHeight);
    virtual TSharedPtr<FOdysseyAnimationCell> Clone(UOdysseyAnimationLayer* iLayer, int iLength) const override;
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual TSharedPtr<FOdysseyAnimationCell> CreateCellFromFrame(uint32 iFrameIndex) const override;

public:
    virtual const FName& GetType() const override;
    virtual void Serialize(FArchive& Ar);
        
    FOdysseyVectorEngine* GetEngine() const;
    TSharedPtr<FOdysseyVectorBlock> GetVectorBlock() const;
    uint32 GetWidth();
    uint32 GetHeight();
    FGuid GetVectorBlockId();
    void SetWidth( uint32 iWidth );
    void SetHeight( uint32 iHeight );
    void SetVectorBlockId( FGuid iVectorBlockID );

public:
    // Event Listeners
    void OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags );
    void OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer);

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    FCriticalSection* GetImageRenderingMutex() const;
    bool IsImageRenderingGameThreadOnly() const;
    UOdysseyAnimationLayerImageVector* GetLayer() const;

private:
    void OnVectorBlockInvalidated(bool iIsInteractive);

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageVectorExport;
    friend class FOdysseyAnimationCellImageVectorImport;

private:
    UOdysseyAnimationLayerImageVector* mLayer;
    FOdysseyVectorEngine* mEngine;
    FGuid mVectorBlockId;
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine
    uint32 mWidth;
    uint32 mHeight;
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaVector> mMediaVector;
};
