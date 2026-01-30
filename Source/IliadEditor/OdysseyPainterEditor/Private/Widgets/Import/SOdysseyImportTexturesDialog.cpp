// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyImportTexturesDialog.h"
#include "SOdysseyImportTexturePositioning.h"
#include "Dialog/SCustomDialog.h"
#include "Widgets/SViewport.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "Widgets/Input/SSlider.h"
#include "OdysseyImportTexturesViewportClient.h"
#include "Slate/SceneViewport.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "RenderGraphBuilder.h"
#include "RenderGraphUtils.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

bool
SOdysseyImportTexturesDialog::Open(const FInputParams& iInputParams, FOutputParams& oOutputParams)
{
    FText exportText = LOCTEXT("import-textures-dialog.export", "Import" );
    FText cancelText = LOCTEXT("import-textures-dialog.cancel", "Cancel");

    SWindow::FArguments windowArgs;
    windowArgs.MinWidth(500);
    windowArgs.MinHeight(500);

    TSharedRef<SOdysseyImportTexturesDialog> importWidget = SNew(SOdysseyImportTexturesDialog, iInputParams, oOutputParams);

    TSharedPtr<SCustomDialog> customDialog = SNew( SCustomDialog )
        .Title( iInputParams.Title )
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
    return true;
}

// Construction / Destruction
SOdysseyImportTexturesDialog::~SOdysseyImportTexturesDialog()
{

}

void
SOdysseyImportTexturesDialog::Construct(const FArguments& InArgs, const FInputParams& iInputParams, FOutputParams& oOutputParams)
{
    mInputParams = iInputParams;
    mOutputParams = oOutputParams;

    mPreviewRenderTarget = TStrongObjectPtr<UTextureRenderTarget2D>(NewObject<UTextureRenderTarget2D>());
    mPreviewRenderTarget->InitAutoFormat(mInputParams.CanvasWidth, mInputParams.CanvasHeight);
    mPreviewRenderTarget->UpdateResourceImmediate();

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
            /* + SSegmentedControl<ETabs>::Slot(ETabs::ScanCleaner)
            .Text(LOCTEXT("import-textures-dialog.tab.scan-cleaner.name", "Scan Cleaner"))
            .ToolTip(LOCTEXT("import-textures-dialog.tab.scan-cleaner.tooltip", "Scan Cleaner"))

            //Cut Tool
            + SSegmentedControl<ETabs>::Slot(ETabs::PegsStabilization)
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
                SNew(SOdysseyImportTexturePositioning)
                .Data(this, &SOdysseyImportTexturesDialog::GetPositioningData)
                .OnChanged(this, &SOdysseyImportTexturesDialog::OnPositioningChanged)
            ]
            + SHorizontalBox::Slot()
            [
                SNew(SVerticalBox)
                + SVerticalBox::Slot()
                [
                    SAssignNew(mViewportWidget, SViewport)
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

    mViewportClient = MakeShared<FOdysseyImportTexturesViewportClient>(mInputParams.CanvasWidth, mInputParams.CanvasHeight);
    mSceneViewport = MakeShared<FSceneViewport>(mViewportClient.Get(), mViewportWidget);
    mViewportWidget->SetViewportInterface(mSceneViewport.ToSharedRef());

    mViewportClient->SetTexture(mPreviewRenderTarget.Get());
    UpdatePreview();
}

SOdysseyImportTexturesDialog::ETabs
SOdysseyImportTexturesDialog::GetActiveTab() const
{
    return mActiveTab;
}

void
SOdysseyImportTexturesDialog::OnTabChecked(ETabs iTab, ECheckBoxState iState)
{
    if (iState != ECheckBoxState::Checked)
        return;

    mActiveTab = iTab;
}

SOdysseyImportTexturePositioning::FData
SOdysseyImportTexturesDialog::GetPositioningData() const
{
    return mPositioningData;
}

void
SOdysseyImportTexturesDialog::OnPositioningChanged(SOdysseyImportTexturePositioning::FData iData)
{
    mPositioningData = iData;
    UpdatePreview();
}

EVisibility
SOdysseyImportTexturesDialog::GetCurrentTextureSliderVisibility() const
{
    return mInputParams.Textures.Num() - 1 <= 0 ? EVisibility::Collapsed : EVisibility::Visible;
}

float
SOdysseyImportTexturesDialog::GetCurrentTextureSliderStepSize() const
{
    if (mInputParams.Textures.Num() - 1 <= 0)
            return 0;
    return 1.0f / (mInputParams.Textures.Num() - 1);
}

float
SOdysseyImportTexturesDialog::GetCurrentTextureSliderValue() const
{
    if (mInputParams.Textures.Num() - 1 <= 0)
        return 0;

    return float(mCurrentTextureIndex) / (mInputParams.Textures.Num() - 1);
}

void
SOdysseyImportTexturesDialog::OnCurrentTextureSliderValueChanged(float iValue)
{
    mCurrentTextureIndex = (uint32)FMath::RoundToInt(iValue * (mInputParams.Textures.Num() - 1));
    //mViewportClient->SetTexture(mInputParams.Textures[mCurrentTextureIndex]);
    UpdatePreview();
}

void
SOdysseyImportTexturesDialog::UpdatePreview()
{
    Render(mPreviewRenderTarget.Get(), mCurrentTextureIndex);
    mSceneViewport->Invalidate(); //Redraws the viewport
}

void
SOdysseyImportTexturesDialog::Render(UTextureRenderTarget2D* oRenderTarget, int iTextureIndex)
{
    UTexture2D* texture = mInputParams.Textures[mCurrentTextureIndex];
    texture->UpdateResource();
    texture->SetForceMipLevelsToBeResident( 1.0f );
    texture->WaitForStreaming();

    FTextureRenderTargetResource* renderTargetResource = oRenderTarget->GameThread_GetRenderTargetResource();
    ENQUEUE_RENDER_COMMAND(SOdysseyImportTexturesDialog_Render)(
        [texture, renderTargetResource, positioningData = mPositioningData](FRHICommandListImmediate& RHICmdList)
        {
            FRDGBuilder graphBuilder(RHICmdList);
            FRDGTextureRef destinationTexture = renderTargetResource->GetRenderTargetTexture( graphBuilder );

            /* FRDGTextureDesc renderTextureDesc = FRDGTextureDesc::Create2D(
                destinationTexture->Desc.Extent,
                destinationTexture->Desc.Format,
                FClearValueBinding::Transparent,
                ETextureCreateFlags::ShaderResource | ETextureCreateFlags::RenderTargetable
            ); */

            AddClearRenderTargetPass(graphBuilder, destinationTexture, FLinearColor::Transparent);

            FCanvas* canvas = FCanvas::Create(graphBuilder, destinationTexture, nullptr, FGameTime(), GMaxRHIFeatureLevel);

            int32 textureWidth = texture->GetSurfaceWidth();
            int32 textureHeight = texture->GetSurfaceHeight();

            switch(positioningData.mScaling)
            {
                case EOdysseyImportTextureScaling::None: break;

                case EOdysseyImportTextureScaling::Scale:
                {
                    textureWidth = destinationTexture->Desc.Extent.X;
                    textureHeight = destinationTexture->Desc.Extent.Y;
                }
                break;

                case EOdysseyImportTextureScaling::ScaleAndFit:
                {
                    float ratio = FMath::Min(float(destinationTexture->Desc.Extent.X) / texture->GetSurfaceWidth(), float(destinationTexture->Desc.Extent.Y) / texture->GetSurfaceHeight());

                    textureWidth = texture->GetSurfaceWidth() * ratio;
                    textureHeight = texture->GetSurfaceHeight() * ratio;
                }
                break;
            }

            int32 destTextureWidth = destinationTexture->Desc.Extent.X;
            int32 destTextureHeight = destinationTexture->Desc.Extent.Y;

            FVector2D texturePosition;

            switch(positioningData.mAlignment)
            {
                case SOdysseyImportTexturePositioning::EAlignment::TopLeft: texturePosition = FVector2D::ZeroVector; break;
                case SOdysseyImportTexturePositioning::EAlignment::Top: texturePosition = FVector2D((destTextureWidth - textureWidth) / 2.f, 0); break;
                case SOdysseyImportTexturePositioning::EAlignment::TopRight: texturePosition = FVector2D(destTextureWidth - textureWidth, 0); break;
                case SOdysseyImportTexturePositioning::EAlignment::Left: texturePosition = FVector2D(0, (destTextureHeight - textureHeight) / 2.f); break;
                case SOdysseyImportTexturePositioning::EAlignment::Center: texturePosition = FVector2D((destTextureWidth - textureWidth) / 2.f, (destTextureHeight - textureHeight) / 2.f); break;
                case SOdysseyImportTexturePositioning::EAlignment::Right: texturePosition = FVector2D(destTextureWidth - textureWidth, (destTextureHeight - textureHeight) / 2.f); break;
                case SOdysseyImportTexturePositioning::EAlignment::BottomLeft: texturePosition = FVector2D(0, destTextureHeight - textureHeight); break;
                case SOdysseyImportTexturePositioning::EAlignment::Bottom: texturePosition = FVector2D((destTextureWidth - textureWidth) / 2.f, destTextureHeight - textureHeight); break;
                case SOdysseyImportTexturePositioning::EAlignment::BottomRight: texturePosition = FVector2D(destTextureWidth - textureWidth, destTextureHeight - textureHeight); break;
            }

            FTexture* tileTexture = graphBuilder.AllocObject<FTexture>();
            tileTexture->TextureRHI = texture->GetResource()->TextureRHI;
            tileTexture->SamplerStateRHI = Odyssey::GetSamplerStateForAntiAliasing(positioningData.mResamplingMethod);

            FCanvasTileItem TileItem(
                texturePosition,
                tileTexture,
                FVector2D(textureWidth, textureHeight),
                FColor::White
            );
            canvas->DrawItem(TileItem);
            canvas->Flush_RenderThread(graphBuilder);

            graphBuilder.Execute();
        }
    );
}

#undef LOCTEXT_NAMESPACE
