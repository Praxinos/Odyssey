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
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

    FOdysseyVectorCell* GetVectorCell();
    FOdysseyVectorImportV2* GetImporterV2();

    void Serialize(FArchive& Ar);

    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    //IOdysseyRenderingAbility overrides
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
