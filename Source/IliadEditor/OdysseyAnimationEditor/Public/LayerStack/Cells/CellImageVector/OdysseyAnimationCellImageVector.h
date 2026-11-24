// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyAnimationCell.h"

#include "OdysseyVectorCell.h"
#include "OdysseyVectorContent.h"
#include "Import/v2/OdysseyVectorImport.h"

#include "OdysseyAnimationCellImageVector.generated.h"

class FOdysseyMediaVector;
class FOdysseyVectorEngine;
class FOdysseyVectorGroupPaint;
class UOdysseyAnimationLayerImageVector;
class FOdysseyVectorCell;
class UOdysseyPalette;

UCLASS(BlueprintType)
class ODYSSEYANIMATIONEDITOR_API UOdysseyAnimationCellImageVector
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
        IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction,
        const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext
    ) const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrameIndex) const override;
    virtual FIntRect GetDefaultRenderRect() const override;

public:
    virtual void Serialize(FArchive& Ar) override;
    virtual void OldSerialize(FArchive& Ar) override; //DEPRECATED: Keep that for compatibility with early versions of Odyssey

public:
    virtual FOdysseyMediaProvider GetMediaProvider() const override;
    virtual void Clear() override;
    void OnRefreshReferencedPalette(UOdysseyPalette* iPalette);

public:
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void BeginDestroy() override;

    FOdysseyVectorCell* GetVectorCell() const;
    FOdysseyVectorImportV2* GetImporterV2();

    /**
     * Returns the render of the scene, see FOdysseyVectorContent.
     * Created when needed, following the current vector cell.
     */
    FOdysseyVectorContent& GetVectorContent() const;

public:
    // Implements Interface IOdysseyVectorCell
    virtual int32 GetIndex() override;
    virtual uint32 GetLength() override;
    virtual uint32 GetFrame() override;

private:
    /** Returns the flags the scene is drawn with, set by the layer. */
    uint64 GetDrawingFlags() const;

private:
    //Import/Export
    friend class FOdysseyAnimationCellImageVectorExport;
    friend class FOdysseyAnimationCellImageVectorImport;

private:
    TSharedPtr<FOdysseyVectorCell> mVectorCell = nullptr;
    mutable TSharedPtr<FOdysseyVectorContent> mVectorContent;
    FOdysseyVectorImportV2 mImporterV2;
};
