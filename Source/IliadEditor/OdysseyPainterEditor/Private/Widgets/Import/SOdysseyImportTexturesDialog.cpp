// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTexturesDialog.h"
#include "SOdysseyImportTexturePositioning.h"
#include "SOdysseyImportTextureScanCleaner.h"
#include "Dialog/SCustomDialog.h"
#include "Widgets/SViewport.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SWidgetSwitcher.h"
#include "OdysseyImportTexturesViewportClient.h"
#include "Slate/SceneViewport.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "OdysseyHUDRectangle.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

bool
SOdysseyImportTexturesDialog::Open(FText iTitle, FOdysseyImportTexturesParameters& ioData)
{
    FText exportText = LOCTEXT("import-textures-dialog.export", "Import" );
    FText cancelText = LOCTEXT("import-textures-dialog.cancel", "Cancel");

    SWindow::FArguments windowArgs;
    windowArgs.MinWidth(500);
    windowArgs.MinHeight(500);

    TSharedRef<SOdysseyImportTexturesDialog> importWidget = SNew(SOdysseyImportTexturesDialog, ioData);

    TSharedPtr<SCustomDialog> customDialog = SNew( SCustomDialog )
        .Title( iTitle )
        .UseScrollBox(false)
        .WindowArguments(windowArgs)
        .ClientSize(FVector2D(1000, 500)) //this line also activates Window Resizing
        .Buttons( { SCustomDialog::FButton( exportText ), SCustomDialog::FButton( cancelText ) } )
        .HAlignContent(HAlign_Fill)
        .VAlignContent(VAlign_Fill)
        .Content()
        [
            importWidget
        ];

    if (customDialog->ShowModal() != 0)
        return false;

    ioData = importWidget->GetImportData();

    return true;
}

// Construction / Destruction
SOdysseyImportTexturesDialog::~SOdysseyImportTexturesDialog()
{

}

void
SOdysseyImportTexturesDialog::Construct(const FArguments& InArgs, const FOdysseyImportTexturesParameters& ioData)
{
    mImportData = ioData;

    mPreviewRenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(mImportData.CreateRT());

    ChildSlot
    .HAlign(HAlign_Fill)
    .VAlign(VAlign_Fill)
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .HAlign(HAlign_Left)
        [
            SNew(SSegmentedControl<ETabs>)
            .Value(this, &SOdysseyImportTexturesDialog::GetActiveTab)
            .OnValueChecked(this, &SOdysseyImportTexturesDialog::OnTabChecked)

            //Selection Tool
            + SSegmentedControl<ETabs>::Slot(ETabs::Positioning)
            .Text(LOCTEXT("import-textures-dialog.tab.positioning.name", "Positioning"))
            .ToolTip(LOCTEXT("import-textures-dialog.tab.positioning.tooltip", "Positioning"))

            //Move Tool
            + SSegmentedControl<ETabs>::Slot(ETabs::ScanCleaner)
            .Text(LOCTEXT("import-textures-dialog.tab.scan-cleaner.name", "Scan Cleaner"))
            .ToolTip(LOCTEXT("import-textures-dialog.tab.scan-cleaner.tooltip", "Scan Cleaner"))

            //Cut Tool
            /*+ SSegmentedControl<ETabs>::Slot(ETabs::PegsStabilization)
            .Text(LOCTEXT("import-textures-dialog.tab.pegs-stabilization.name", "Pegs Stabilization"))
            .ToolTip(LOCTEXT("import-textures-dialog.tab.pegs-stabilization.tooltip", "Pegs Stabilization")) */
        ]
        + SVerticalBox::Slot()
        .Padding(FMargin(0.f, 4.f, 0.f, 0.f))
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot()
            .AutoWidth()
            .MinWidth(300)
            .MaxWidth(300)
            [
                SNew(SWidgetSwitcher)
                .WidgetIndex(this, &SOdysseyImportTexturesDialog::GetActiveTabIndex)
                + SWidgetSwitcher::Slot()
                [
                    SNew(SOdysseyImportTexturePositioning)
                    .Data(this, &SOdysseyImportTexturesDialog::GetImportData)
                    .OnChanged(this, &SOdysseyImportTexturesDialog::OnPositioningChanged)
                ]
                + SWidgetSwitcher::Slot()
                [
                    SNew(SOdysseyImportTextureScanCleaner)
                    .Data(this, &SOdysseyImportTexturesDialog::GetImportData)
                    .OnChanged(this, &SOdysseyImportTexturesDialog::OnScanCleanerChanged)
                ]
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                [
                    SAssignNew(mViewportWidget, SViewport)
                    .EnableGammaCorrection(false)
                    .IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
                    .ShowEffectWhenDisabled(false)
                    .EnableBlending(true)
                ]
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SSlider)
                    .Visibility(this, &SOdysseyImportTexturesDialog::GetCurrentTextureSliderVisibility)
                    .Orientation(Orient_Horizontal)
                    .StepSize(this, &SOdysseyImportTexturesDialog::GetCurrentTextureSliderStepSize)
                    .Value(this, &SOdysseyImportTexturesDialog::GetCurrentTextureSliderValue)
                    .OnValueChanged(this, &SOdysseyImportTexturesDialog::OnCurrentTextureSliderValueChanged)
                    .PreventThrottling(true)
                    .MouseUsesStep(true)
                    .IndentHandle(true)
                ]
            ]
        ]
    ];

    //Setup Viewport
    mViewportClient = MakeShared<FOdysseyImportTexturesViewportClient>(mImportData.GetDestinationWidth(), mImportData.GetDestinationHeight());
    mSceneViewport = MakeShared<FSceneViewport>(mViewportClient.Get(), mViewportWidget);
    mViewportWidget->SetViewportInterface(mSceneViewport.ToSharedRef());
    mViewportClient->SetTexture(mPreviewRenderTarget.Get());

    //Setup HUDs
    mTextureOutlineHUD = CreateTextureOutlineHUD();
    mViewportClient->GetHUD()->AddElement(mTextureOutlineHUD);

    //Update Viewport Preview
    UpdatePreview();
}

TSharedRef<FOdysseyHUDRectangle>
SOdysseyImportTexturesDialog::CreateTextureOutlineHUD() const
{
    TSharedRef<FOdysseyHUDRectangle> hud = MakeShared<FOdysseyHUDRectangle>(FVector2D(0, 0), FVector2D(0, 0));

    FOdysseyHUDElement::FHUDCustomization customization;
    customization.mColors.Add(FLinearColor::Black);
    customization.mColors.Add(FLinearColor::White);
    customization.mGapLength = 0.f;
    customization.mSegmentLength = 10.f;
    customization.mSpeed = 10.f;
    hud->SetCustomization(customization);

    return hud;
}

FOdysseyImportTexturesParameters
SOdysseyImportTexturesDialog::GetImportData() const
{
    return mImportData;
}

SOdysseyImportTexturesDialog::ETabs
SOdysseyImportTexturesDialog::GetActiveTab() const
{
    return mActiveTab;
}

int32
SOdysseyImportTexturesDialog::GetActiveTabIndex() const
{
    return (int32)mActiveTab;
}

void
SOdysseyImportTexturesDialog::OnTabChecked(ETabs iTab, ECheckBoxState iState)
{
    if (iState != ECheckBoxState::Checked)
        return;

    mActiveTab = iTab;
}

void
SOdysseyImportTexturesDialog::OnPositioningChanged(FOdysseyImportTexturesParameters iData)
{
    mImportData = iData;
    UpdatePreview();
}

void
SOdysseyImportTexturesDialog::OnScanCleanerChanged(FOdysseyImportTexturesParameters iData)
{
    mImportData = iData;
    UpdatePreview();
}

EVisibility
SOdysseyImportTexturesDialog::GetCurrentTextureSliderVisibility() const
{
    return mImportData.GetSourceTextures().Num() - 1 <= 0 ? EVisibility::Collapsed : EVisibility::Visible;
}

float
SOdysseyImportTexturesDialog::GetCurrentTextureSliderStepSize() const
{
    if (mImportData.GetSourceTextures().Num() - 1 <= 0)
            return 0;
    return 1.0f / (mImportData.GetSourceTextures().Num() - 1);
}

float
SOdysseyImportTexturesDialog::GetCurrentTextureSliderValue() const
{
    if (mImportData.GetSourceTextures().Num() - 1 <= 0)
        return 0;

    return float(mCurrentTextureIndex) / (mImportData.GetSourceTextures().Num() - 1);
}

void
SOdysseyImportTexturesDialog::OnCurrentTextureSliderValueChanged(float iValue)
{
    mCurrentTextureIndex = (uint32)FMath::RoundToInt(iValue * (mImportData.GetSourceTextures().Num() - 1));
    mViewportClient->SetTexture(mImportData.GetSourceTextures()[mCurrentTextureIndex]);
    UpdatePreview();
}

void
SOdysseyImportTexturesDialog::UpdatePreview()
{
    mImportData.Render(mPreviewRenderTarget.Get(), mCurrentTextureIndex);
    FVector2D textureSize = mImportData.GetTextureScaledSize(mCurrentTextureIndex);
    FVector2D texturePosition = mImportData.GetTexturePosition(textureSize);
    mTextureOutlineHUD->SetTopLeftPoint(texturePosition);
    mTextureOutlineHUD->SetBottomRightPoint(texturePosition + textureSize);
}

void
SOdysseyImportTexturesDialog::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime )
{
    mSceneViewport->Invalidate(); //Redraws the viewport each tick to display HUD animations
}

#undef LOCTEXT_NAMESPACE
