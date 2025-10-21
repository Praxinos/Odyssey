// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationCell.h"

#if WITH_EDITOR
#include "OdysseyVectorCell.h"
#include "Import/v2/OdysseyVectorImport.h"
#endif

#include "OdysseyAnimationCellImageVector.generated.h"

#if WITH_EDITOR
class FOdysseyVectorBlock;
class FOdysseyMediaVector;
class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class UOdysseyAnimationLayerImageVector;
class FOdysseyVectorCell;
class UOdysseyPalette;
#endif

#if !WITH_EDITOR
class IOdysseyVectorCell {};
#endif

UCLASS(BlueprintType)
class ODYSSEYANIMATION_API UOdysseyAnimationCellImageVector
    : public UOdysseyAnimationCell
    , public IOdysseyVectorCell
{
    GENERATED_BODY()

public:
    virtual ~UOdysseyAnimationCellImageVector();
    UOdysseyAnimationCellImageVector();

public:
    //IOdysseyRenderingAbility overrides
    virtual bool BuildRenderPipelineInternal(
        FFrameNumber iFrame,
        uint64 iType,
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents
    ) const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;

public:
#if WITH_EDITORONLY_DATA
    virtual void Serialize(FArchive& Ar) override;
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey
#endif

#if WITH_EDITOR
public:
    virtual FOdysseyMediaProvider GetMediaProvider() const override;
    void OnRefreshReferencedPalette(UOdysseyPalette* iPalette);

public:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual TSharedPtr<::ULIS::FBlock> GetBlock() const override;

    FOdysseyVectorCell* GetVectorCell() const;
    FOdysseyVectorImportV2* GetImporterV2();
    TSharedPtr<FOdysseyVectorBlock> GetVectorBlock() const;
    FGuid GetVectorBlockId();
    void SetVectorBlockId( FGuid iVectorBlockID );

public:
    // Event Listeners
    void OnIsColoredChanged(UOdysseyAnimationLayerImageVector* iLayer);
    void OnIsWireframeChanged(UOdysseyAnimationLayerImageVector* iLayer);
    FCriticalSection* GetImageRenderingMutex() const;

public:
    // Implements Interface IOdysseyVectorCell
    virtual int32 GetIndex() override;
    virtual uint32 GetLength() override;
    virtual uint32 GetFrame() override;

private:
    void OnVectorBlockInvalidated(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);
    bool UpdateDrawingFlags() const;
    void InitTexture();

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

    mutable uint64 mDrawingFlags = 0;
#endif
};
