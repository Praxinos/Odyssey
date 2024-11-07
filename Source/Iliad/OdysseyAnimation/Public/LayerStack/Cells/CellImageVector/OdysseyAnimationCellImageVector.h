// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/Cells/OdysseyAnimationCell.h"

#include "OdysseyAnimationCellImageVector.generated.h"

class FOdysseyVectorBlock;
class FOdysseyMediaVector;
class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class UOdysseyAnimationLayerImageVector;

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationCellImageVector
    : public UOdysseyAnimationCell
{
    GENERATED_BODY()

public:
    virtual ~UOdysseyAnimationCellImageVector();

public:
    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void Serialize(FArchive& Ar) override;
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey

    FOdysseyVectorEngine* GetEngine() const;
    TSharedPtr<FOdysseyVectorBlock> GetVectorBlock() const;
    FGuid GetVectorBlockId();
    void SetVectorBlockId( FGuid iVectorBlockID );

public:
    // Event Listeners
    void OnVectorSceneSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer);
    void OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer);

public:
    //FOdysseyImageRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex) const override;
    virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const override;
    FCriticalSection* GetImageRenderingMutex() const;
    bool IsImageRenderingGameThreadOnly() const;

private:
    void OnVectorBlockInvalidated(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageVectorExport;
    friend class FOdysseyAnimationCellImageVectorImport;

private:
    FOdysseyVectorEngine* mEngine = nullptr;
    FGuid mVectorBlockId;
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaVector> mMediaVector;
};
