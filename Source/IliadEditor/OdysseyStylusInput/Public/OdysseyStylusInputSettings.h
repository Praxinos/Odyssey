// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "InputCoreTypes.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "IDetailCustomization.h"
#include "Engine/DeveloperSettings.h"

#include "OdysseyStylusInputSettings.generated.h"

class ODYSSEYSTYLUSINPUT_API FOdysseyStylusInputSettingsCustomization : public IDetailCustomization
{
public:
    static TSharedRef<IDetailCustomization> MakeInstance()
    {
        return MakeShareable(new FOdysseyStylusInputSettingsCustomization);
    }

    virtual void CustomizeDetails(IDetailLayoutBuilder& iDetailBuilder) override;

private:
    TArray<TSharedPtr<FName>> mStylusAPIs;
    TMap<FName, FText> mStylusAPIsLabels;

    TSharedPtr<class IPropertyHandle> mStylusAPIProperty;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnStylusInputDriverChanged, FName);

/**
 * Implements the StylusInput settings.
 */
UCLASS(config = Odyssey, meta = (DisplayName = "Odyssey - Stylus Input"))
class ODYSSEYSTYLUSINPUT_API UOdysseyStylusInputSettings
    : public UDeveloperSettings
{
    GENERATED_UCLASS_BODY()

public:
    virtual void PostInitProperties() override;
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent& iPropertyChangedEvent) override;

public:
    FName GetStylusDriver() const;
    static FText GetFormatText(FName iStylusInputDriver );

    virtual FName GetContainerName() const override;
    virtual FName GetCategoryName() const override;

private:
    FName GetDefaultStylusDriver() const;

public:
    /** Driver to use to interpret Stylus inputs. */
    UPROPERTY(config, EditAnywhere, Category = "StylusDriver")
    FName StylusInputDriver;

    static FOnStylusInputDriverChanged OnStylusInputDriverChanged;
};
