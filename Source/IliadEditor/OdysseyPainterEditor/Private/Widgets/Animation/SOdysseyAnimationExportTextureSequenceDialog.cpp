// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyAnimationExportTextureSequenceDialog.h"

#include "ContentBrowserModule.h"
#include "Dialog/SCustomDialog.h"
#include "Engine/Texture2D.h"
#include "IContentBrowserSingleton.h"
#include "Misc/ScopedSlowTask.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SGridPanel.h"

#include "OdysseyAnimationLayer.h"
#include "OdysseyAnimation.h"
#include "OdysseyExportImage.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

bool
SOdysseyAnimationExportTextureSequenceDialog::Open(UOdysseyAnimation* iAnimation)
{
    TSharedPtr<SCustomDialog> customDialog;

    FText dialogTitle = LOCTEXT("animation.export-texture-sequence-dialog.title", "Export Texture Sequence" );
    FText exportText = LOCTEXT("animation.export-texture-sequence-dialog.export", "Export" );
    FText cancelText = LOCTEXT("animation.export-texture-sequence-dialog.cancel", "Cancel");


    TSharedRef<SOdysseyAnimationExportTextureSequenceDialog> exportTextureSequenceWidget = SNew(SOdysseyAnimationExportTextureSequenceDialog, iAnimation);
    customDialog = SNew( SCustomDialog )
        .Title( dialogTitle )
        .Buttons( { SCustomDialog::FButton( exportText ), SCustomDialog::FButton( cancelText ) } )
        .Content()
        [
            exportTextureSequenceWidget
        ];

    if (customDialog->ShowModal() != 0)
        return false;

    exportTextureSequenceWidget->Export();
    return true;
}

void
SOdysseyAnimationExportTextureSequenceDialog::Construct(const FArguments& InArgs, UOdysseyAnimation* iAnimation)
{
    mExporter.mAnimation = iAnimation;
    mExporter.mCustomRange = iAnimation->GetFrameRange();

    ChildSlot
    [
        SNew(SGridPanel)
        + SGridPanel::Slot(0, 0)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-texture-sequence-dialog.source.name", "Source"))
        ]
        + SGridPanel::Slot(0, 1)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-texture-sequence-dialog.range.name", "Range"))
        ]
        + SGridPanel::Slot(0, 3)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-texture-sequence-dialog.unique-frames-only.name", "Unique Frames Only"))
        ]
        + SGridPanel::Slot(1, 0)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationExportTextureSequenceSource>())
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mSource;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mSource = (EOdysseyAnimationExportTextureSequenceSource)iValue;})
        ]
        + SGridPanel::Slot(1, 1)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationExportTextureSequenceRange>())
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mRange;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mRange = (EOdysseyAnimationExportTextureSequenceRange)iValue;})
        ]
        + SGridPanel::Slot(1, 2)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            [
                SNew(SNumericEntryBox<int>)
                .Visibility_Lambda([this](){ return mExporter.mRange == EOdysseyAnimationExportTextureSequenceRange::Custom ? EVisibility::Visible : EVisibility::Collapsed; })
                .Value_Lambda([this](){ return mExporter.mCustomRange.GetLowerBoundValue(); })
                .AllowSpin(true)
                .Delta(1)
                .LinearDeltaSensitivity(10)
                .MinValue(0)
                .MaxValue(TOptional<int>())
                .MinSliderValue(0)
                .MaxSliderValue(TOptional<int>())
                .OnValueChanged_Lambda([this](int iValue) { mExporter.mCustomRange.SetLowerBoundValue(FMath::Clamp(iValue, 0, mExporter.mCustomRange.GetUpperBoundValue())); })
                .OnValueCommitted_Lambda([this](int iValue, ETextCommit::Type iType) { mExporter.mCustomRange.SetLowerBoundValue(FMath::Clamp(iValue, 0, mExporter.mCustomRange.GetUpperBoundValue())); })
            ]

            + SHorizontalBox::Slot()
            [
                SNew(SNumericEntryBox<int>)
                .Visibility_Lambda([this](){ return mExporter.mRange == EOdysseyAnimationExportTextureSequenceRange::Custom ? EVisibility::Visible : EVisibility::Collapsed; })
                .Value_Lambda([this](){ return mExporter.mCustomRange.GetUpperBoundValue(); })
                .AllowSpin(true)
                .Delta(1)
                .LinearDeltaSensitivity(10)
                .MinValue(0)
                .MaxValue(TOptional<int>())
                .MinSliderValue(0)
                .MaxSliderValue(TOptional<int>())
                .OnValueChanged_Lambda([this](int iValue) { mExporter.mCustomRange.SetUpperBoundValue(FMath::Max(iValue, mExporter.mCustomRange.GetLowerBoundValue())); })
                .OnValueCommitted_Lambda([this](int iValue, ETextCommit::Type iType) { mExporter.mCustomRange.SetUpperBoundValue(FMath::Max(iValue, mExporter.mCustomRange.GetLowerBoundValue())); })
            ]
        ]
        + SGridPanel::Slot(1, 3)
        [
            SNew( SCheckBox )
            .IsChecked_Lambda( [this](){ return mExporter.mUniqueFramesOnly ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;} )
            .OnCheckStateChanged_Lambda( [this](ECheckBoxState iNewCheckedState){ mExporter.mUniqueFramesOnly = iNewCheckedState == ECheckBoxState::Checked; } )
            .ToolTipText( LOCTEXT( "export-texture-sequence-dialog.unique-frames-only.tooltip", "Export only unique frames instead of every single frames" ) )
        ]
    ];
}

FOdysseyAnimationTextureSequenceExporter::FOdysseyAnimationTextureSequenceExporter()
    : mAnimation(nullptr)
{
}

FOdysseyAnimationTextureSequenceExporter::FOdysseyAnimationTextureSequenceExporter(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
    , mCustomRange(iAnimation->GetFrameRange())
{
}

FInt32Range
FOdysseyAnimationTextureSequenceExporter::GetSourceRange(const FSource& iSource)
{
    switch(mRange)
    {
        case EOdysseyAnimationExportTextureSequenceRange::AllFrames:
        {
            FInt32Range range = iSource.mRange;
            range.SetLowerBoundValue(0);
            return range;
        }

        case EOdysseyAnimationExportTextureSequenceRange::MarkInMarkOut: return mAnimation->GetFrameRange();
        case EOdysseyAnimationExportTextureSequenceRange::AllCells: return iSource.mRange;
        case EOdysseyAnimationExportTextureSequenceRange::Custom: return mCustomRange;
    }

    return FInt32Range::Empty();
}

TArray<FOdysseyAnimationTextureSequenceExporter::FSource>
FOdysseyAnimationTextureSequenceExporter::GetSources()
{
    UOdysseyAnimationLayerStack* layerStack = Cast<UOdysseyAnimationLayerStack>(mAnimation->GetLayerStack());
    switch(mSource)
    {
        case EOdysseyAnimationExportTextureSequenceSource::Animation:
        {
            return
            {
                {
                    mAnimation,
                    TEXT(""),
                    mAnimation->GetFrameRange()
                }
            };
        }
        case EOdysseyAnimationExportTextureSequenceSource::AllLayers:
        {
            TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
            TArray<FSource> animationLayers;

            for (UOdysseyLayer* layer : layers)
            {
                if (layer->CanHaveChildren()) //do not export folders here
                    continue;

                UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
                animationLayers.Add(
                    {
                        animationLayer,
                        animationLayer->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_")),
                        animationLayer->GetFrameRange()
                    }
                );
            }

            return animationLayers;
        }
        case EOdysseyAnimationExportTextureSequenceSource::CurrentLayer:
        {
            UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());
            return
            {
                {
                    animationLayer,
                    animationLayer->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_")),
                    animationLayer->GetFrameRange()
                }
            };
        }

        case EOdysseyAnimationExportTextureSequenceSource::SelectedLayers:
        {
            TArray<UOdysseyLayer*> layers = layerStack->GetLayers();
            TArray<UOdysseyLayer*> selectedLayers;
            TArray<FSource> animationLayers;

            selectedLayers.Reserve( layers.Num() );

            for (UOdysseyLayer* layer : layers)
            {
                if ( layer->IsSelected() )
                {
                    UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);

                    selectedLayers.Add( animationLayer );
                }
            }

            // add the current layer
            if ( selectedLayers.Find( layerStack->GetCurrentLayer() ) == INDEX_NONE )
            {
                UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(layerStack->GetCurrentLayer());

                selectedLayers.Add( currentLayer );
            }

            for( UOdysseyLayer* selectedLayer : selectedLayers )
            {
                if ( !selectedLayer->CanHaveChildren() ) //do not export folders here
                {
                    UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(selectedLayer);

                    animationLayers.Add(
                        {
                            animationLayer,
                            animationLayer->GetLayerName().ToString().Replace(TEXT(" "), TEXT("_")),
                            animationLayer->GetFrameRange()
                        } );
                }
            }

            return animationLayers;
        }
    }

    return {{nullptr, FString()}};
}

void
SOdysseyAnimationExportTextureSequenceDialog::Export()
{
    FSaveAssetDialogConfig saveAssetDialogConfig;
    saveAssetDialogConfig.DialogTitleOverride = LOCTEXT( "animation.export-texture-sequence.save-asset-dialog.title", "Select Export Path & Name" );
    saveAssetDialogConfig.DefaultPath = FPaths::GetPath(mExporter.mAnimation->GetPathName() );
    saveAssetDialogConfig.DefaultAssetName = mExporter.mAnimation->GetName();
    saveAssetDialogConfig.AssetClassNames.Add( UTexture2D::StaticClass()->GetClassPathName() );
    saveAssetDialogConfig.ExistingAssetPolicy = ESaveAssetDialogExistingAssetPolicy::AllowButWarn;

    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( "ContentBrowser" );
    FString saveObjectPath = contentBrowserModule.Get().CreateModalSaveAssetDialog( saveAssetDialogConfig );

    if ( saveObjectPath == "" )
        return;

    mExporter.Export(saveObjectPath);
}

void
FOdysseyAnimationTextureSequenceExporter::Export(const FString& iFilename)
{
    TArray<FSource> sources = GetSources();

    FScopedSlowTask progressBar(sources.Num(), LOCTEXT("timeline-tab.export-texture-sequence.progress-bar.title", "Exporting Texture Sequence"));
    progressBar.MakeDialog();

    TArray<FInt32Range> ranges;
    for (const FSource& source : sources)
    {
        ranges.Add(GetSourceRange(source));
    }
    FInt32Range fullRange = FInt32Range::Hull(ranges);
    FString endFrameStr = FString::FromInt(fullRange.GetUpperBoundValue());

    for (const FSource& source : sources)
    {
        progressBar.EnterProgressFrame();
        ExportSource(source, iFilename, endFrameStr.Len());
    }
}

void
FOdysseyAnimationTextureSequenceExporter::ExportSource(const FSource& iSource, const FString& iPath, int iNumZero)
{
    FString folder = FPaths::GetPath(iPath) + "/";
    FString filename = FPaths::GetBaseFilename(iPath);
    if (!iSource.mFilename.IsEmpty())
        filename += TEXT("_") + iSource.mFilename;

    FInt32Range frameRange = GetSourceRange(iSource);
    int startFrame = frameRange.GetLowerBoundValue();
    int endFrame = frameRange.GetUpperBoundValue();

    FScopedSlowTask progressBar(endFrame - startFrame + 1);
    TArray<FGuid> lastRenderingComposition;

    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();
        if (mUniqueFramesOnly)
        {
            IOdysseyTextureRenderingAbility* textureRenderingAbility = Cast<IOdysseyTextureRenderingAbility>(iSource.mTextureRenderingAbility);
            if (!textureRenderingAbility)
                continue;

            TArray<FGuid> renderingComposition = textureRenderingAbility->GetRenderingComposition(EOdysseyRenderingType::Render, i);

            if (renderingComposition == lastRenderingComposition)
                continue;

            lastRenderingComposition = renderingComposition;
        }

        FString frameStr = FString::FromInt(i);
        FString imageName = filename + TEXT("_");
         for (int j = 0; j < iNumZero - frameStr.Len(); j++)
        {
            imageName += TEXT("0");
        }
        imageName += FString::Printf(TEXT("%d"), i);
        Odyssey::ExportAsTexture(iSource.mTextureRenderingAbility, i, imageName, folder);
    }
}

#undef LOCTEXT_NAMESPACE
