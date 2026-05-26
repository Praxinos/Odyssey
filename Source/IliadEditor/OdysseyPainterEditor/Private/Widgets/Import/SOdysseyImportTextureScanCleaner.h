// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"

#include "OdysseyImportTexturesParameters.h"
#include "Curves/CurveFloat.h"
class SOdysseyImportTextureScanCleaner
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyImportTextureScanCleaner)
        {}
        /** Called when the object value changes */
        SLATE_ARGUMENT(FOdysseyImportTexturesParameters*, Data)
        SLATE_EVENT(FSimpleDelegate, OnChanged)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTextureScanCleaner();
    void Construct(const FArguments& InArgs);

public:
    bool IsActivated() const;

private:
    void OnActivateCheckBoxStateChanged(ECheckBoxState InCheckState);
    ECheckBoxState IsActivateChecked() const;

    void OnSetInputViewRange(float Min, float Max);
    void OnSetOutputViewRange(float Min, float Max);

    void OnUpdateCurve( UCurveBase* Curve, EPropertyChangeType::Type ChangeType);

private:
    FOdysseyImportTexturesParameters* mData;
    FSimpleDelegate mOnChanged;

    TSharedPtr<class SCurveEditor> mCurveEditor;

    float mViewMinInput;
    float mViewMaxInput;
    float mViewMinOutput;
    float mViewMaxOutput;
};
