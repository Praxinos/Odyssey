// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    : public UOdysseyTextureLayer , public IOdysseyVectorLayer, public IOdysseyVectorCell
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
    FOdysseyVectorLayer mVectorLayer;
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine

    UPROPERTY()
    FGuid mVectorBlockId;

public:
    //UOdysseyLayer overrides
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

    FOdysseyVectorCell* GetVectorCell();
    FOdysseyVectorImportV2* GetImporterV2();

    void IsWireframeChanged();
    void IsColoredChanged();
    void Serialize(FArchive& Ar);
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    //IOdysseyRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(EOdysseyRenderingType iRenderType, int iFrame = 0, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrame = 0) const override;

private:
    void OnVectorBlockInvalidated(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

public:
    // Implements Interface IOdysseyVectorLayer
    virtual FOdysseyVectorCell* GetCellByIndex( uint32 iIndex ) override;
    virtual FOdysseyVectorCell* GetFirstCell() override;
    virtual FOdysseyVectorCell* GetLastCell() override;
    virtual bool Contains( FOdysseyVectorCell* iCandidateCell )override;
    virtual uint32 GetWidth() override;
    virtual uint32 GetHeight() override;

    // Implements Interface IOdysseyVectorCell
    virtual int32 GetIndex() override;
    virtual uint32 GetLength() override;
    virtual uint32 GetFrame() override;

private:
    //Import/Export
    friend class FOdysseyTextureLayerImageVectorExport;
    friend class FOdysseyTextureLayerImageVectorImport;

private:
    UFUNCTION(BlueprintSetter)
    void IsWireframeBlueprintSetter(bool Value);

    UFUNCTION(BlueprintSetter)
    void IsColoredBlueprintSetter(bool Value);

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, NonTransactional, Category="Odyssey|Layer")
    bool IsWireframe = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, NonTransactional, Category="Odyssey|Layer")
    bool IsColored = true;

private:
    FOdysseyVectorImportV2 mImporterV2;
};
