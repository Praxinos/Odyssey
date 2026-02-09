// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTextureScanCleaner.h"

#include "SCurveEditor.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

// Construction / Destruction
SOdysseyImportTextureScanCleaner::~SOdysseyImportTextureScanCleaner()
{

}

void
SOdysseyImportTextureScanCleaner::Construct(const FArguments& InArgs)
{
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
            SAssignNew(mCurveEditor, SCurveEditor)

            /*
                "Input" is X axis
                "Output" is Y axis
                "TimelineLength" is the length of the highlighted part of the editor (Here we work in the range [0, 1] so the timeline length is 1.0f)
            */

            .ViewMinInput_Lambda( [this]() { return mViewMinInput; })
            .ViewMaxInput_Lambda( [this]() { return mViewMaxInput; })
            .ViewMinOutput_Lambda( [this]() { return mViewMinOutput; })
            .ViewMaxOutput_Lambda( [this]() { return mViewMaxOutput; })
            .DataMinInput(-0.05f) //can't scroll before 0.f
            .DataMaxInput(1.05f) //can't scroll past 1.f
            .TimelineLength(0.f)
            //.InputSnap(0.5f) //Snap value on X axis
            //.OutputSnap(1.0f)  //Snap value on Y axis
            //.InputSnappingEnabled(true) //Activate Snap on X axis
            //.OutputSnappingEnabled(true) //Activate Snap on Y axis
            //.AreCurvesVisible(false) //doew not seem to change anything
            //.DrawCurve(false) //if false, draw only keys and not the curve itself
            .DesiredSize(FVector2D(300, 300))
            .HideUI(false) //if true, hides the overlay UI when mouse is out of the widget
            //.AllowZoomOutput(false) //if false, force the Y axis zoom to be fixed
            .AlwaysDisplayColorCurves(true)
            .AlwaysHideGradientEditor(true)
            .ZoomToFitVertical(false) //Simulates a click on the Zoom To Fit Vertically button when creating the widget
            .ZoomToFitHorizontal(false) //Simulates a click on the Zoom To Fit Vertically button when creating the widget
            .OnSetInputViewRange(this, &SOdysseyImportTextureScanCleaner::OnSetInputViewRange)
            .OnSetOutputViewRange(this, &SOdysseyImportTextureScanCleaner::OnSetOutputViewRange)
            .ShowZoomButtons(false)
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
    mViewMinInput = -0.05f;
    mViewMaxInput = 1.05f;
}

void
SOdysseyImportTextureScanCleaner::OnSetOutputViewRange(float Min, float Max)
{
    float range = Max - Min;
    mViewMinOutput = FMath::Max(Min, -0.05f);
    mViewMaxOutput = mViewMinOutput + range;
}

void
SOdysseyImportTextureScanCleaner::OnUpdateCurve( UCurveBase* Curve, EPropertyChangeType::Type ChangeType)
{
    mOnChanged.ExecuteIfBound(); //Allows to refresh viewport
}

#undef LOCTEXT_NAMESPACE
