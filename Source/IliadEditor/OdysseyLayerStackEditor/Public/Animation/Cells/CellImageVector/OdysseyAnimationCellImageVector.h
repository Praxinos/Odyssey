// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationCell.h"
#include "OdysseyVectorCell.h"
#include "Import/v2/OdysseyVectorImport.h"

#include "OdysseyAnimationCellImageVector.generated.h"

class FOdysseyVectorBlock;
class FOdysseyMediaVector;
class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class UOdysseyAnimationLayerImageVector;
class FOdysseyVectorCell;

UCLASS(BlueprintType)
class ODYSSEYLAYERSTACKEDITOR_API UOdysseyAnimationCellImageVector
    : public UOdysseyAnimationCell, public IOdysseyVectorCell
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

    FOdysseyVectorCell* GetVectorCell() const;
    FOdysseyVectorImportV2* GetImporterV2();
    TSharedPtr<FOdysseyVectorBlock> GetVectorBlock() const;
    FGuid GetVectorBlockId();
    void SetVectorBlockId( FGuid iVectorBlockID );

public:
    // Event Listeners
    void OnVectorSceneSignal( FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags );
    void OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer);
    void OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer);

public:
    //IOdysseyRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(EOdysseyRenderingType iRenderType, int iFrame, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex) const override;
    virtual TArray<FIntRect> GetRenderingRects() const override;
    FCriticalSection* GetImageRenderingMutex() const;
    bool IsImageRenderingGameThreadOnly() const;

public:
    // Implements Interface IOdysseyVectorCell
    virtual int32 GetIndex() override;
    virtual uint32 GetLength() override;
    virtual uint32 GetFrame() override;

private:
    void OnVectorBlockInvalidated(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);
    void OnVectorEngineNotify(FOdysseyVectorGroupPaint* iScene, uint64 iSignalFlags);

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageVectorExport;
    friend class FOdysseyAnimationCellImageVectorImport;

private:
    TSharedPtr<FOdysseyVectorCell> mVectorCell = nullptr;
    FGuid mVectorBlockId;
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine
    mutable FCriticalSection mImageRenderingMutex;
    mutable TWeakPtr<FOdysseyMediaVector> mMediaVector;
    FOdysseyVectorImportV2 mImporterV2;
};
