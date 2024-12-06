// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "LayerStack/OdysseyTextureLayer.h"

#include <ULIS>

#include "OdysseyVectorObject.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyVectorRoot.h"

#include "OdysseyTextureLayerImageVector.generated.h"

class FOdysseyVectorBlock;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorEngine;

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerImageVector
    : public UOdysseyTextureLayer
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
    FOdysseyVectorRoot* mRoot;
    FOdysseyVectorSharedEnv mSharedEnv;
    TSharedPtr<FOdysseyVectorBlock> mVectorBlock; //A automatically cached block containing the render of mEngine

    UPROPERTY()
    FGuid mVectorBlockId;

public:
    //UOdysseyLayer overrides
    virtual void PostInitProperties() override;
    virtual void PostLoad() override;
    virtual void PostDuplicate(bool bDuplicateForPIE) override;
    virtual void Merge(const TArray<UOdysseyLayer*>& Layers) override;

    FOdysseyVectorEngine* GetEngine();

    void IsWireframeChanged();
    void IsColoredChanged();
    void Serialize(FArchive& Ar);
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive) override;
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive) override;

    virtual FOdysseyMediaProvider GetMediaProvider(uint32 iFrameIndex) const override;

public:
    //FOdysseyImageRenderingAbility overrides
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0, FImageRendererFilter iFilter = FImageRendererFilter()) const override;
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0) const override;

private:
    void OnVectorBlockInvalidated(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive);

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
};
