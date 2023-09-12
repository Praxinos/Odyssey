// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "OdysseyAnimationImageRenderingAbility.h"

class UOdysseyAnimationLayerImageVector;

class ODYSSEYANIMATION_API FOdysseyAnimationCellImageVector
    : public FOdysseyAnimationCell
{    
public:
    static TSharedRef<FOdysseyAnimationCellImageVector> Create(UOdysseyAnimationLayerImageVector* iLayer, int iWidth, int iHeight);
    static const FName& StaticType();

public:
    virtual ~FOdysseyAnimationCellImageVector();
    FOdysseyAnimationCellImageVector(UOdysseyAnimationLayerImageVector* iLayer);

    void Init(int iWidth, int iHeight);
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;
    virtual TSharedPtr<FOdysseyAnimationCell> CreateCellFromFrame(uint32 iFrameIndex) const override;

public:
    virtual const FName& GetType() const override;
    virtual void Serialize(FArchive& Ar);
        
    FOdysseyVectorEngine* GetEngine() const;

public:
    // Event Listeners
    void OnVectorSceneSignal( FOdysseyVectorScene* iScene, uint64 iSignalFlags );
    void OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer);

public:
	//FOdysseyImageRenderingAbility overrides
	virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame) const override;
	virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
	virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;

private:
    UOdysseyAnimationLayerImageVector* mLayer;
    FOdysseyVectorEngine* mEngine;
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> mBlock;
    int mWidth;
    int mHeight;
};