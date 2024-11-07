// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyAnimationPlayer.h"
#include "Engine/Texture.h"

#include "OdysseyAnimationTexture.generated.h"

UCLASS(hidecategories=(Adjustments, Compositing, LevelOfDetail, ImportSettings, Object))
class ODYSSEYANIMATIONTEXTURE_API UOdysseyAnimationTexture
    : public UTexture
{
    GENERATED_BODY()

public:
    void SetPlayer(UOdysseyAnimationPlayer* iPlayer);
    UOdysseyAnimationPlayer* GetPlayer() const;

public:
    //~ UTexture interface.
    virtual FTextureResource* CreateResource() override;
    virtual EMaterialValueType GetMaterialType() const override;
    virtual float GetSurfaceWidth() const override;
    virtual float GetSurfaceHeight() const override;
    virtual float GetSurfaceDepth() const override { return 0; }
    virtual uint32 GetSurfaceArraySize() const override { return 0; }
    virtual ETextureClass GetTextureClass() const { return ETextureClass::Other2DNoSource; }

protected:
    //Property changed methods
    virtual void PlayerChanged();
    virtual void PropertyChanged(const FName& iPropertyName);

public:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;

public:
    //~ UObject interface.
    //virtual void PostLoad() override;
    //virtual bool IsPostLoadThreadSafe() const override;

//#if WITH_EDITOR
    //virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
//#endif

private:
    UPROPERTY()
    UOdysseyAnimationPlayer*     Player = nullptr;

private:
    /** Current width and height of the resource (in pixels). */
    FIntPoint mDimensions;
};
