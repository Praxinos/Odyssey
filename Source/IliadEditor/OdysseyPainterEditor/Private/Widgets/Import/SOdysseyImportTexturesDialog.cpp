// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTexturesDialog.h"

#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Dialog/SCustomDialog.h"
#include "Engine/Texture2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Input/NumericUnitTypeInterface.inl"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Input/SSlider.h"
#include "Widgets/Layout/SWidgetSwitcher.h"


#include "OdysseyImportTexturesViewportClient.h"
#include "SOdysseyImportTexturePositioning.h"
#include "SOdysseyImportTextureScanCleaner.h"

#include "OdysseyHUDRectangle.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

bool
SOdysseyImportTexturesDialog::Open(FText iTitle, FOdysseyImportTexturesParameters& ioData)
{
    if (ioData.GetSourceTextures().IsEmpty())
        return false;

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

            + SSegmentedControl<ETabs>::Slot(ETabs::Positioning)
            .Text(LOCTEXT("import-textures-dialog.tab.positioning.name", "Positioning"))
            .ToolTip(LOCTEXT("import-textures-dialog.tab.positioning.tooltip", "Positioning"))

            + SSegmentedControl<ETabs>::Slot(ETabs::ScanCleaner)
            .Text(LOCTEXT("import-textures-dialog.tab.scan-cleaner.name", "Scan Cleaner"))
            .ToolTip(LOCTEXT("import-textures-dialog.tab.scan-cleaner.tooltip", "Scan Cleaner"))

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
                    .Data(&mImportData)
                    .OnChanged(this, &SOdysseyImportTexturesDialog::OnPositioningChanged)
                ]
                + SWidgetSwitcher::Slot()
                [
                    SNew(SOdysseyImportTextureScanCleaner)
                    .Data(&mImportData)
                    .OnChanged(this, &SOdysseyImportTexturesDialog::OnScanCleanerChanged)
                ]
            ]
            + SHorizontalBox::Slot()
            .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
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
                + SVerticalBox::Slot()
                .AutoHeight()
                [
                    SNew(SHorizontalBox)
                    + SHorizontalBox::Slot()
                    .AutoWidth()
                    .Padding(FMargin(0.f, 0.f, 4.f, 0.f))
                    .VAlign(VAlign_Center)
                    [
                        SNew(STextBlock)
                        .Text(LOCTEXT("import-texture-dialog.viewport.zoom", "Zoom"))
                    ]

                    + SHorizontalBox::Slot()
                    .MinWidth(70)
                    .MaxWidth(70)
                    [
                        SNew(SNumericEntryBox<float>)
                        .Value_Lambda(
                            [this]()
                            {
                                if (!mViewportClient)
                                    return 1.0f;

                                return mViewportClient->GetZoom() * 100.f;
                            }
                        )
                        .TypeInterface(MakeShareable( new TNumericUnitTypeInterface<float>( EUnit::Percentage ) ))
                        .AllowSpin(true)
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
                                if (!mViewportClient)
                                    return;

                                mViewportClient->SetZoom(iValue / 100.f, mSceneViewport->GetSizeXY() / 2.f );
                            }
                        )
                    ]
                ]
            ]
        ]
    ];

    uint32 maxWidth = 0;
    uint32 maxHeight = 0;
    for (UTexture2D* texture : mImportData.GetSourceTextures())
    {
        maxWidth = FMath::Max(maxWidth, (uint32)texture->Source.GetSizeX());
        maxHeight = FMath::Max(maxHeight, (uint32)texture->Source.GetSizeY());
    }

    //Setup Viewport
    mViewportClient = MakeShared<FOdysseyImportTexturesViewportClient>(mImportData.GetDestinationWidth(), mImportData.GetDestinationHeight(), maxWidth, maxHeight);
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
SOdysseyImportTexturesDialog::OnPositioningChanged()
{
    UpdatePreview();
}

void
SOdysseyImportTexturesDialog::OnScanCleanerChanged()
{
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
