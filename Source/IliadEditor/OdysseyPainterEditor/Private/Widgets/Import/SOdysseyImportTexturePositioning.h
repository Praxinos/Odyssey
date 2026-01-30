// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAntiAliasing.h"

UENUM()
enum class EOdysseyImportTextureScaling : uint8
{
    None UMETA(ToolTip="Don't scale"),
    Scale UMETA(ToolTip="Scales without preserving ratio"),
    ScaleAndFit UMETA(ToolTip="Scales and preserves ratio")
};

class SOdysseyImportTexturePositioning : public SCompoundWidget
{
public:
    enum class EAlignment
    {
        TopLeft,
        Top,
        TopRight,
        Left,
        Center,
        Right,
        BottomLeft,
        Bottom,
        BottomRight
    };

public:
    struct FData
    {
        EAlignment mAlignment = EAlignment::Center;
        EOdysseyImportTextureScaling mScaling = EOdysseyImportTextureScaling::None;
        EOdysseyAntiAliasing mResamplingMethod = EOdysseyAntiAliasing::Bilinear;
    };

public:
    DECLARE_DELEGATE_OneParam(FOnChanged, FData);

public:
    SLATE_BEGIN_ARGS(SOdysseyImportTexturePositioning)
        {}
        /** Called when the object value changes */
        SLATE_ATTRIBUTE(FData, Data)
        SLATE_EVENT(FOnChanged, OnChanged)
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyImportTexturePositioning();
    void Construct(const FArguments& InArgs);

private:
    void OnAlignmentCheckBoxStateChanged(ECheckBoxState InCheckState, EAlignment iAlignment);
    ECheckBoxState IsAlignmentChecked(EAlignment iAlignment) const;

    int32 GetScaling() const;
    void OnScalingEnumSelectionChanged(int32, ESelectInfo::Type);

    int32 GetResamplingMethod() const;
    void OnResamplingMethodEnumSelectionChanged(int32, ESelectInfo::Type);

private:
    TAttribute<FData> mData;
    FOnChanged mOnChanged;
};
