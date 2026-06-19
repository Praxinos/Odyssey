// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "Proxies/OdysseyBrushColor.h"

#include <ULIS>

#include "OdysseyBrushOptions.generated.h"

UCLASS()
class ODYSSEYBRUSH_API UOdysseyBrushOptions : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    DECLARE_MULTICAST_DELEGATE(FOnPropertyChanged);


    static const FName GetSizePropertyName()
    {
        return GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Size);
    }

    static const FName GetFlowPropertyName()
    {
        return GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Flow);
    }

    static const FName GetColorPropertyName()
    {
        return GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Color);
    }

public:
    /**
     * Starts an interactive operation
     *
     * Every call between BeginInteractiveMode() and EndInteractiveMode()
     * is considered as interactive, meaning the value which is set is not guaranteed
     * to the definitive value the user wants (example : we are dragging a slider)
     *
     * Also, it is recommended to not do any heavy computation and limit callback calls while in Interactive Mode
     * to avoid lag while dragging sliders for example.
     */
    void BeginInteractiveMode();

    /**
     * End the Interactive Mode
     *
     * If you want callbacks to be called please call your setters again after this function
     * as InteractiveMode does not track function calls or modified properties.
     *
     * example: when releaseing an opacity slider call
     *   tool->EndInteractiveMode();
     *   tool->SetOpacity(tool->GetOpacity());
     */
    void EndInteractiveMode();

    /**
     * Returns wether Interactive Mode is active or not
     */
    bool IsInInteractiveMode() const;

    //UObject overrides
    void SetSize(float Size);
    float GetSize() const;

    void SetFlow(float Size);
    float GetFlow() const;

    void SetColor(FOdysseyBrushColor Color);
    FOdysseyBrushColor GetColor() const;

    //Called when a simple property changes
    virtual void PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent);

public:
    // Delegates
    FOnPropertyChanged& OnPropertyChangedDelegate() { return mOnPropertyChangedDelegate; }

protected:
    /** The size. */
    UPROPERTY(
        EditInstanceOnly,
        BlueprintReadOnly,
        Category="Common",
        meta = (
            Tooltip="The size of the brush (if the brush uses it)",
            ClampMin = "1",
            UIMin = "1",
            LinearDeltaSensitivity = "15",
            Delta = "1",
            DisplayPriority="1"
        ) )
    float   Size = 20.f;

    /** The flow. */
    UPROPERTY(
        EditInstanceOnly,
        BlueprintReadOnly,
        Category="Common",
        meta = (
            Tooltip="The flow of the brush (if the brush uses it)",
            ClampMin = "0",
            ClampMax = "100",
            UIMin = "0",
            UIMax = "100",
            Delta = "1",
            Units="Percent",
            DisplayPriority = "1"
        ) )
    float   Flow = 100.f;

    /** The color. */
    UPROPERTY( BlueprintReadOnly, Category="Common")
    FOdysseyBrushColor Color;

private:
    // Delegates
    FOnPropertyChanged mOnPropertyChangedDelegate;
    bool mIsInInteractiveMode = false;
};
