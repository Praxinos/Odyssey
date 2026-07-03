// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTextureScanCleaner.h"

#include "SCurveEditor.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"

#define LOCTEXT_NAMESPACE "PainterEditor"

#define CURVEWIDGET_INPUT_MIN -0.1f
#define CURVEWIDGET_INPUT_MAX 1.1f
#define CURVEWIDGET_OUTPUT_MIN -0.05f
#define CURVEWIDGET_OUTPUT_MAX 1.20f //let's some space to display Time and Value widgets

// Construction / Destruction
SOdysseyImportTextureScanCleaner::~SOdysseyImportTextureScanCleaner()
{

}

void
SOdysseyImportTextureScanCleaner::Construct(const FArguments& InArgs)
{
    mViewMinInput = CURVEWIDGET_INPUT_MIN;
    mViewMaxInput = CURVEWIDGET_INPUT_MAX;
    mViewMinOutput = CURVEWIDGET_OUTPUT_MIN;
    mViewMaxOutput = CURVEWIDGET_OUTPUT_MAX;

    mData = InArgs._Data;
    mOnChanged = InArgs._OnChanged;

    mData->GetScanCleanerCurve()->OnUpdateCurve.AddSP(SharedThis(this), &SOdysseyImportTextureScanCleaner::OnUpdateCurve);

    FMargin alignmentButtonPadding(4.0f);

    ChildSlot
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Top)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("import-texture-dialog.scan-cleaner.activate", "Activate Scan Cleaner"))
            ]

            + SHorizontalBox::Slot()
            [
                SNew(SCheckBox)
                //.Padding(alignmentButtonPadding)
                //.HAlign( HAlign_Center )
                .OnCheckStateChanged( this, &SOdysseyImportTextureScanCleaner::OnActivateCheckBoxStateChanged)
                .IsChecked( this, &SOdysseyImportTextureScanCleaner::IsActivateChecked )
            ]
        ]

        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("import-texture-dialog.scan-cleaner.color-saturation", "Color Saturation"))
            ]

            + SHorizontalBox::Slot()
            [
                SNew(SNumericEntryBox<float>)
                .IsEnabled_Lambda([this]() { return mData->GetIsScanCleanerActivated();})
                .Value_Lambda(
                    [this]()
                    {
                        return mData->GetScanCleanerColorSaturation() * 100.f;
                    }
                )
                .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<float>( EUnit::Percentage ) ))
                .AllowSpin(true)
                .LinearDeltaSensitivity(5)
                .Delta(1)
                .MinValue(0)
                .MinSliderValue(0)
                .MaxValue(TOptional<float>())
                .MaxSliderValue(TOptional<float>())
                .MinFractionalDigits(0)
                .MaxFractionalDigits(0)
                .OnValueChanged_Lambda(
                    [this](float iValue)
                    {
                        mData->SetScanCleanerColorSaturation(iValue / 100.f);
                        mOnChanged.ExecuteIfBound();
                    }
                )
            ]
        ]

        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            [
                SNew(STextBlock)
                .Text(LOCTEXT("import-texture-dialog.scan-cleaner.color-value", "Color Value"))
            ]

            + SHorizontalBox::Slot()
            [
                SNew(SNumericEntryBox<float>)
                .IsEnabled_Lambda([this]() { return mData->GetIsScanCleanerActivated();})
                .Value_Lambda(
                    [this]()
                    {
                        return mData->GetScanCleanerColorValue() * 100.f;
                    }
                )
                .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<float>( EUnit::Percentage ) ))
                .AllowSpin(true)
                .Delta(1)
                .LinearDeltaSensitivity(5)
                .MinValue(0)
                .MinSliderValue(0)
                .MaxValue(TOptional<float>())
                .MaxSliderValue(TOptional<float>())
                .MinFractionalDigits(0)
                .MaxFractionalDigits(0)
                .OnValueChanged_Lambda(
                    [this](float iValue)
                    {
                        mData->SetScanCleanerColorValue(iValue / 100.f);
                        mOnChanged.ExecuteIfBound();
                    }
                )
            ]
        ]

        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        [
            /*
                "Input" is X axis
                "Output" is Y axis
                "TimelineLength" is the length of the highlighted part of the editor (We don't want it to be highlighted so it's 0.f)
            */

            SAssignNew(mCurveEditor, SCurveEditor)
            .IsEnabled_Lambda([this]() { return mData->GetIsScanCleanerActivated();})
            .ViewMinInput_Lambda( [this]() { return mViewMinInput; })
            .ViewMaxInput_Lambda( [this]() { return mViewMaxInput; })
            .ViewMinOutput_Lambda( [this]() { return mViewMinOutput; })
            .ViewMaxOutput_Lambda( [this]() { return mViewMaxOutput; })
            .DataMinInput(CURVEWIDGET_INPUT_MIN) //can't scroll before 0.f
            .DataMaxInput(CURVEWIDGET_INPUT_MAX) //can't scroll past 1.f
            .TimelineLength(0.f)
            //.InputSnap(0.5f) //Snap value on X axis
            //.OutputSnap(1.0f)  //Snap value on Y axis
            //.InputSnappingEnabled(true) //Activate Snap on X axis
            //.OutputSnappingEnabled(true) //Activate Snap on Y axis
            //.AreCurvesVisible(false) //doew not seem to change anything
            //.DrawCurve(false) //if false, draw only keys and not the curve itself
            .DesiredSize(FVector2D(300, 300))
            .HideUI(false) //if true, hides the overlay UI when mouse is out of the widget
            .AllowZoomOutput(false) //if false, force the Y axis zoom to be fixed
            .AlwaysDisplayColorCurves(true)
            .AlwaysHideGradientEditor(true)
            .ZoomToFitVertical(false) //Simulates a click on the Zoom To Fit Vertically button when creating the widget
            .ZoomToFitHorizontal(false) //Simulates a click on the Zoom To Fit Vertically button when creating the widget
            .OnSetInputViewRange(this, &SOdysseyImportTextureScanCleaner::OnSetInputViewRange)
            .OnSetOutputViewRange(this, &SOdysseyImportTextureScanCleaner::OnSetOutputViewRange)
            .ShowZoomButtons(false)
        ]

        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 0.f, 0.f, 4.f))
        .AutoHeight()
        .HAlign( HAlign_Center )
        [
            SNew( SButton )
            .Text( LOCTEXT( "import-texture-dialog.scan-cleaner.reset.label", "Reset" ) )
            .OnClicked_Lambda( [this]() -> FReply
                                {
                                    mData->ResetScanCleaner();
                                    mOnChanged.ExecuteIfBound();

                                    return FReply::Handled();
                                } )
        ]
    ];

    mCurveEditor->SetCurveOwner(mData->GetScanCleanerCurve());
}

void
SOdysseyImportTextureScanCleaner::OnActivateCheckBoxStateChanged(ECheckBoxState InCheckState)
{
    mData->SetIsScanCleanerActivated(InCheckState == ECheckBoxState::Checked);
    mOnChanged.ExecuteIfBound();
}

ECheckBoxState
SOdysseyImportTextureScanCleaner::IsActivateChecked() const
{
    return mData->GetIsScanCleanerActivated() ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void
SOdysseyImportTextureScanCleaner::OnSetInputViewRange(float Min, float Max)
{
    mViewMinInput = CURVEWIDGET_INPUT_MIN;
    mViewMaxInput = CURVEWIDGET_INPUT_MAX;
}

void
SOdysseyImportTextureScanCleaner::OnSetOutputViewRange(float Min, float Max)
{
    float range = Max - Min;
    mViewMinOutput = FMath::Max(Min, CURVEWIDGET_OUTPUT_MIN);
    mViewMaxOutput = mViewMinOutput + range;
}

void
SOdysseyImportTextureScanCleaner::OnUpdateCurve( UCurveBase* Curve, EPropertyChangeType::Type ChangeType)
{
    //PATCH: ?
    // Need to add this here to recreate the models inside the curve editor
    // But it's only useful during an undo
    // Because when undoing, models are emptied (that's why nothing is displayed after an undo)
    // and this delegate is called during an undo, so add it here
    // Otherwise, all the widget (mCurveEditor or even SOdysseyImportTextureScanCleaner) should be recreated (?) in a tick (?) in undo delegate ?
    // (in the curve editor in timeline in actor blueprint, during an undo, all the widgets are recreated)
    mCurveEditor->SetCurveOwner( mData->GetScanCleanerCurve() );

    FRichCurve& curve = mData->GetScanCleanerCurve()->FloatCurve;
    for (auto it = curve.GetKeyHandleIterator(); it; it++)
    {
        FKeyHandle keyHandle = *it;
        float keyTime = curve.GetKeyTime(keyHandle);
        float keyValue = curve.GetKeyValue(keyHandle);

        curve.SetKeyTime(keyHandle, FMath::Clamp(keyTime, 0.f, 1.f));
        curve.SetKeyValue(keyHandle, FMath::Clamp(keyValue, 0.f, 1.f));
    }

    mOnChanged.ExecuteIfBound(); //Allows to refresh viewport
}

#undef LOCTEXT_NAMESPACE
