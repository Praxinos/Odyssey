// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "OdysseyAnimation.h"

#include "OdysseyAnimationFactory.generated.h"

UCLASS(hidecategories=Object, MinimalAPI)
class UOdysseyAnimationFactory
    : public UFactory
{
    GENERATED_UCLASS_BODY()

public:
    /** The default name of the new animation asset. */
    UPROPERTY()
    FString DefaultName = TEXT( "Animation" );

    /** The width of the new animation asset. */
    UPROPERTY()
    uint32 Width = 1920;

    /** The height of the new animation asset. */
    UPROPERTY()
    uint32 Height = 1080;

    /** The height of the new animation asset. */
    UPROPERTY()
    EOdysseyAnimationFormat Format = EOdysseyAnimationFormat::BGRA8;

    /** The framerate of the new animation asset. */
    UPROPERTY()
    FFrameRate FrameRate = FFrameRate( 24.f, 1.f );

    /** The default layer type to use with this animation */
    UPROPERTY()
    TSubclassOf<class UOdysseyAnimationLayer> DefaultLayerClass;

    /** A background color of the new animation asset. */
    UPROPERTY()
    TOptional<FLinearColor> LayerBackgroundColor;

protected:
    virtual UObject* FactoryCreateNew( UClass* iClass, UObject* iParent, FName iName, EObjectFlags iFlags, UObject* iContext, FFeedbackContext* iWarn ) override;

    virtual bool ConfigureProperties() override;

    virtual FString GetDefaultNewAssetName() const;
};
