// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImportTexturesParameters.h"
class SOdysseyImportTextureScanCleaner : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam(FOnChanged, FOdysseyImportTexturesParameters);

public:
    SLATE_BEGIN_ARGS(SOdysseyImportTextureScanCleaner)
        {}
        /** Called when the object value changes */
        SLATE_ATTRIBUTE(FOdysseyImportTexturesParameters, Data)
        SLATE_EVENT(FOnChanged, OnChanged)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTextureScanCleaner();
    void Construct(const FArguments& InArgs);

private:
    void OnActivateCheckBoxStateChanged(ECheckBoxState InCheckState);
    ECheckBoxState IsActivateChecked() const;

private:
    TAttribute<FOdysseyImportTexturesParameters> mData;
    FOnChanged mOnChanged;
};
