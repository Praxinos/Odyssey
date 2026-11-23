// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyTextureLayer.h"

#include <ULIS>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Import/v2/OdysseyVectorImport.h"
#include "OdysseyVectorContent.h"

#include "OdysseyTextureLayerImageVector.generated.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorEngine;
class UOdysseyTiledImage;

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerImageVector
    : public UOdysseyTextureLayer
    , public IOdysseyVectorLayer
    , public IOdysseyVectorCell
{
    GENERATED_BODY()

private:
    void Init( uint32 iWidth, uint32 iHeight );

public:
    ~UOdysseyTextureLayerImageVector();
    UOdysseyTextureLayerImageVector();

protected:
    UPROPERTY()
    uint32 Width;

    UPROPERTY()
    uint32 Height;

private:
    // handle to a callback to refresh the layer when a property of an object's details view is changed
    TSharedPtr<FOdysseyVectorCell> mVectorCell;
    TSharedPtr<FOdysseyVectorLayer> mVectorLayer;

    /** The render of the scene, blended by the layer stack. Rebuilt from the scene: never saved. */
    UPROPERTY(Transient, NonTransactional)
    TObjectPtr<UOdysseyTiledImage> TiledImage;

    mutable TSharedPtr<FOdysseyVectorContent> VectorContent;

public:
    //UOdysseyLayer overrides
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void BeginDestroy() override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

    TSharedPtr<FOdysseyVectorCell> GetVectorCell();
    TSharedPtr<FOdysseyVectorLayer> GetVectorLayer();
    FOdysseyVectorImportV2* GetImporterV2();

    void Serialize(FArchive& Ar);

    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

    /** Renders the scene again when the sRGB setting of the texture changed. */
    virtual void ColorSettingsChanged() override;

    /**
     * Returns the render of the scene, see FOdysseyVectorContent.
     * Created when needed, following the current vector cell.
     */
    FOdysseyVectorContent& GetVectorContent() const;

    /** Returns the tiled image holding the render of the scene, without creating it. */
    const UOdysseyTiledImage* GetTiledImage() const;

public:
    //IOdysseyRenderingAbility overrides
    virtual bool BuildRenderPipelineInternal(IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction, const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext) const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrame = 0) const override;

private:
    /** Returns the flags the scene is drawn with. */
    uint64 GetDrawingFlags() const;

public:
    // Implements Interface IOdysseyVectorLayer
    virtual FOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) override;
    virtual FOdysseyVectorCell* GetFirstCell() override;
    virtual FOdysseyVectorCell* GetLastCell() override;
    virtual bool Contains( FOdysseyVectorCell* iCandidateCell )override;
    virtual uint32 GetWidth() override;
    virtual uint32 GetHeight() override;
    virtual FOdysseyVectorCell* GetMaxCellFrom( uint32 iIndex ) override;
    virtual FOdysseyVectorCell* GetMinCellFrom( uint32 iIndex ) override;
    // Implements Interface IOdysseyVectorLayer::GetMinCellFrom
    virtual FGuid GetPaletteSetID( UOdysseyPalette* iPalette ) override;
    const TArray<UOdysseyPaletteSet*> GetPaletteSets() const;

    // Implements Interface IOdysseyVectorCell
    virtual int32 GetIndex() override;
    virtual uint32 GetLength() override;
    virtual uint32 GetFrame() override;

public:
    void OnRefreshReferencedPalette(UOdysseyPalette* iPalette);

private:
    //Import/Export
    friend class FOdysseyTextureLayerImageVectorExport;
    friend class FOdysseyTextureLayerImageVectorImport;

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsWireframe(bool Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    void SetIsColored(bool Value);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool IsWireframe() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    bool IsColored() const;

protected:
    UPROPERTY(NonTransactional)
    bool bIsWireframe = false;

    UPROPERTY(NonTransactional)
    bool bIsColored = true;

private:
    FOdysseyVectorImportV2 mImporterV2;
};
