// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImportTexturesData.h"
class SOdysseyImportTexturePositioning : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnChanged, FOdysseyImportTexturesData);

public:
    SLATE_BEGIN_ARGS(SOdysseyImportTexturePositioning)
        {}
        /** Called when the object value changes */
        SLATE_ATTRIBUTE(FOdysseyImportTexturesData, Data)
        SLATE_EVENT(FOnChanged, OnChanged)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTexturePositioning();
    void Construct(const FArguments& InArgs);

private:
    void OnAlignmentCheckBoxStateChanged(ECheckBoxState InCheckState, FOdysseyImportTexturesData::EAlignment iAlignment);
    ECheckBoxState IsAlignmentChecked(FOdysseyImportTexturesData::EAlignment iAlignment) const;

    int32 GetScaling() const;
    void OnScalingEnumSelectionChanged(int32, ESelectInfo::Type);

    int32 GetResamplingMethod() const;
    void OnResamplingMethodEnumSelectionChanged(int32, ESelectInfo::Type);

private:
    TAttribute<FOdysseyImportTexturesData> mData;
    FOnChanged mOnChanged;
};
