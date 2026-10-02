// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "OdysseyImportTexturesParameters.h"

class SOdysseyImportTexturePositioning : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyImportTexturePositioning)
        {}
        /** Called when the object value changes */
        SLATE_ARGUMENT(FOdysseyImportTexturesParameters*, Data)
        SLATE_EVENT(FSimpleDelegate, OnChanged)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTexturePositioning();
    void Construct(const FArguments& InArgs);

private:
    void OnAlignmentCheckBoxStateChanged(ECheckBoxState InCheckState, EOdysseyImportTextureAlignment iAlignment);
    ECheckBoxState IsAlignmentChecked(EOdysseyImportTextureAlignment iAlignment) const;

    int32 GetScaling() const;
    void OnScalingEnumSelectionChanged(int32, ESelectInfo::Type);

    int32 GetScalingOptionsWidgetIndex() const;
    EVisibility GetScalingOptionsVisibility() const;

    ECheckBoxState IsScalingFullFitChecked() const;
    void OnScalingFullFitChanged( ECheckBoxState iNewState );

    ECheckBoxState IsScalingCustomLockChecked() const;
    void OnScalingCustomLockChanged( ECheckBoxState iNewState );

    float GetScalingCustomX() const;
    void SetScalingCustomX( float iNewValue );

    float GetScalingCustomY() const;
    void SetScalingCustomY( float iNewValue );

    int32 GetResamplingMethod() const;
    void OnResamplingMethodEnumSelectionChanged(int32, ESelectInfo::Type);

private:
    FOdysseyImportTexturesParameters* mData;
    FSimpleDelegate mOnChanged;

    bool ScalingCustomLock = true;
};
