// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "OdysseyTextureLayer.h"

#include <ULIS>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyVectorCell.h"
#include "Import/v2/OdysseyVectorImport.h"

#include "OdysseyTextureLayerImageVector.generated.h"

class FOdysseyVectorBlock;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorEngine;

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
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine

    UPROPERTY()
    FGuid mVectorBlockId;

public:
    //UOdysseyLayer overrides
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(EDuplicateMode::Type iDuplicateMode) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

    TSharedPtr<FOdysseyVectorCell> GetVectorCell();
    TSharedPtr<FOdysseyVectorLayer> GetVectorLayer();
    FOdysseyVectorImportV2* GetImporterV2();

    void Serialize(FArchive& Ar);

    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    //IOdysseyRenderingAbility overrides
    virtual bool BuildRenderPipelineInternal(FFrameNumber iFrame, uint64 iType, IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction, const IOdysseyTextureRenderingAbility::FCanRenderFunction& iCanRenderFunction, const TArray<const IOdysseyTextureRenderingAbility*>& iParents) const override;
    virtual TArray<FGuid> GetRenderingComposition(uint64 iRenderType, int iFrame = 0) const override;

private:
    bool UpdateDrawingFlags() const;
    void InitTexture();
    void OnVectorBlockInvalidated(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

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
    virtual FString GetPaletteSetID( UOdysseyPalette* iPalette ) override;

    // Implements Interface IOdysseyVectorCell
    virtual int32 GetIndex() override;
    virtual uint32 GetLength() override;
    virtual uint32 GetFrame() override;

    virtual TSharedPtr<::ULIS::FBlock> GetBlock() const override;

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
    mutable uint64 mDrawingFlags = 0;
    FOdysseyVectorImportV2 mImporterV2;
};
