// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "Widgets/Animation/SOdysseyAnimationExportImageSequenceDialog.h"

#include "Dialog/SCustomDialog.h"
#include "LayerStack/Layers/OdysseyAnimationLayer.h"
#include "OdysseyAnimation.h"
#include "OdysseyExportImage.h"
#include "SEnumCombo.h"
#include "Widgets/Input/SNumericEntryBox.h"
#include "Widgets/Layout/SGridPanel.h"
#include "DesktopPlatformModule.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#include "Misc/ScopedSlowTask.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

bool
SOdysseyAnimationExportImageSequenceDialog::Open(UOdysseyAnimation* iAnimation)
{
    TSharedPtr<SCustomDialog> customDialog;

    FText dialogTitle = LOCTEXT("export-image-sequence-dialog.title", "Export Image Sequence" );
    FText exportText = LOCTEXT("export-image-sequence-dialog.export", "Export" );
    FText cancelText = LOCTEXT("export-image-sequence-dialog.cancel", "Cancel");


    TSharedRef<SOdysseyAnimationExportImageSequenceDialog> exportImageSequenceWidget = SNew(SOdysseyAnimationExportImageSequenceDialog, iAnimation);
    customDialog = SNew( SCustomDialog )
        .Title( dialogTitle )
        .Buttons( { SCustomDialog::FButton( exportText ), SCustomDialog::FButton( cancelText ) } )
        .Content()
        [
            exportImageSequenceWidget
        ];

    if (customDialog->ShowModal() != 0)
        return false;

    exportImageSequenceWidget->Export();
    return true;
}

void
SOdysseyAnimationExportImageSequenceDialog::Construct(const FArguments& InArgs, UOdysseyAnimation* iAnimation)
{
    mExporter.mAnimation = iAnimation;
    mExporter.mCustomRange = iAnimation->GetFrameRange();

    ChildSlot
    [
        SNew(SGridPanel)
        + SGridPanel::Slot(0, 0)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-image-sequence-dialog.format.name", "Format"))
        ]
        + SGridPanel::Slot(0, 1)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-image-sequence-dialog.source.name", "Source"))
        ]
        + SGridPanel::Slot(0, 2)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-image-sequence-dialog.range.name", "Range"))
        ]
        + SGridPanel::Slot(0, 4)
        [
            SNew(STextBlock)
            .Text(LOCTEXT("export-image-sequence-dialog.unique-frames-only.name", "Unique Frames Only"))
        ]
        + SGridPanel::Slot(1, 0)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyExportImageFormat>())
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mFormat;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mFormat = (EOdysseyExportImageFormat)iValue;})
        ]
        + SGridPanel::Slot(1, 1)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationExportImageSequenceSource>())
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mSource;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mSource = (EOdysseyAnimationExportImageSequenceSource)iValue;})
        ]
        + SGridPanel::Slot(1, 2)
        [
            SNew(SEnumComboBox, StaticEnum<EOdysseyAnimationExportImageSequenceRange>())
            .ContentPadding(FMargin(0))
            .CurrentValue_Lambda([this](){ return (int32)mExporter.mRange;})
            .OnEnumSelectionChanged_Lambda([this](int32 iValue, ESelectInfo::Type iSelectInfo){ mExporter.mRange = (EOdysseyAnimationExportImageSequenceRange)iValue;})
        ]
        + SGridPanel::Slot(1, 3)
        [
            SNew(SHorizontalBox)

            + SHorizontalBox::Slot()
            [
                SNew(SNumericEntryBox<int>)
                .Visibility_Lambda([this](){ return mExporter.mRange == EOdysseyAnimationExportImageSequenceRange::Custom ? EVisibility::Visible : EVisibility::Collapsed; })
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
                .Visibility_Lambda([this](){ return mExporter.mRange == EOdysseyAnimationExportImageSequenceRange::Custom ? EVisibility::Visible : EVisibility::Collapsed; })
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
        + SGridPanel::Slot(1, 4)
        [
            SNew( SCheckBox )
            .IsChecked_Lambda( [this](){ return mExporter.mUniqueFramesOnly ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;} )
            .OnCheckStateChanged_Lambda( [this](ECheckBoxState iNewCheckedState){ mExporter.mUniqueFramesOnly = iNewCheckedState == ECheckBoxState::Checked; } )
            .ToolTipText( LOCTEXT( "export-image-sequence-dialog.unique-frames-only.tooltip", "Export only unique frames instead of every single frames" ) )
        ]
    ];
}

FString
SOdysseyAnimationExportImageSequenceDialog::GetSaveFileDialogExtension()
{
    switch(mExporter.mFormat)
    {
        case EOdysseyExportImageFormat::PNG: return FString::Format(TEXT("{0} (.png)|*.png"), { LOCTEXT("export-image-sequence-dialog.format-extension.png", "PNG Image").ToString() } );
        case EOdysseyExportImageFormat::BMP: return FString::Format(TEXT("{0} (.bmp)|*.bmp"), { LOCTEXT("export-image-sequence-dialog.format-extension.bmp", "BMP Image").ToString() } );
        case EOdysseyExportImageFormat::TGA: return FString::Format(TEXT("{0} (.tga)|*.tga"), { LOCTEXT("export-image-sequence-dialog.format-extension.tga", "TGA Image").ToString() } );
        case EOdysseyExportImageFormat::Jpeg: return FString::Format(TEXT("{0} (.jpg)|*.jpg"), { LOCTEXT("export-image-sequence-dialog.format-extension.jpeg", "Jpeg Image").ToString() } );
    }

    return TEXT("");
}

FOdysseyAnimationImageSequenceExporter::FOdysseyAnimationImageSequenceExporter()
    : mAnimation(nullptr)
{
}

FOdysseyAnimationImageSequenceExporter::FOdysseyAnimationImageSequenceExporter(UOdysseyAnimation* iAnimation)
    : mAnimation(iAnimation)
    , mCustomRange(iAnimation->GetFrameRange())
{
}

FInt32Range
FOdysseyAnimationImageSequenceExporter::GetSourceRange(const FSource& iSource)
{
    switch(mRange)
    {
        case EOdysseyAnimationExportImageSequenceRange::AllFrames:
        {
            FInt32Range range = iSource.mRange;
            range.SetLowerBoundValue(0);
            return range;
        }
        case EOdysseyAnimationExportImageSequenceRange::AllCells: return iSource.mRange;
        case EOdysseyAnimationExportImageSequenceRange::Custom: return mCustomRange;
    }

    return FInt32Range::Empty();
}

TArray<FOdysseyAnimationImageSequenceExporter::FSource>
FOdysseyAnimationImageSequenceExporter::GetSources()
{
    switch(mSource)
    {
        case EOdysseyAnimationExportImageSequenceSource::Animation:
        {
            return
            {
                {
                    nullptr,
                    mAnimation,
                    TEXT(""),
                    mAnimation->GetFrameRange()
                }
            };
        }
        case EOdysseyAnimationExportImageSequenceSource::AllLayers:
        {
            TArray<UOdysseyLayer*> layers = mAnimation->GetLayerStack()->GetLayers();
            TArray<FSource> animationLayers;

            for (UOdysseyLayer* layer : layers)
            {
                if (layer->CanHaveChildren) //do not export folders here
                    continue;

                UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(layer);
                animationLayers.Add(
                    {
                        animationLayer,
                        nullptr,
                        animationLayer->Name.ToString().Replace(TEXT(" "), TEXT("_")),
                        animationLayer->GetFrameRange()
                    }
                );
            }

            return animationLayers;
        }
        case EOdysseyAnimationExportImageSequenceSource::CurrentLayer:
        {
            UOdysseyAnimationLayer* animationLayer = Cast<UOdysseyAnimationLayer>(mAnimation->GetLayerStack()->CurrentLayer.Get());
            return
            {
                {
                    animationLayer,
                    nullptr,
                    animationLayer->Name.ToString().Replace(TEXT(" "), TEXT("_")),
                    animationLayer->GetFrameRange()
                }
            };
        }
    }

    return {{nullptr, nullptr, FString()}};
}

void
SOdysseyAnimationExportImageSequenceDialog::Export()
{
    IDesktopPlatform* desktopPlatformHandle = FDesktopPlatformModule::Get();
    TArray< FString > filenames;
    bool saveSuccess = desktopPlatformHandle->SaveFileDialog(
        FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr)
        , LOCTEXT("animation.export-image-sequence.save-dialog.title", "Select Export Path & Name").ToString()
        , FPaths::ProjectDir()
        , mExporter.mAnimation->GetName()
        , GetSaveFileDialogExtension()
        , EFileDialogFlags::None
        , filenames
    );

    if( !saveSuccess || filenames.Num() <= 0 )
        return;

    mExporter.Export(filenames[0]);
}

void
FOdysseyAnimationImageSequenceExporter::Export(const FString& iFilename)
{
    TArray<FSource> sources = GetSources();

    FScopedSlowTask progressBar(sources.Num(), LOCTEXT("timeline-tab.export-image-sequence.progress-bar.title", "Exporting Image Sequence"));
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
FOdysseyAnimationImageSequenceExporter::ExportSource(const FSource& iSource, const FString& iFilename, int iNumZero)
{
    FString path( FPaths::ConvertRelativePathToFull( iFilename ) );
    FString folder = FPaths::GetPath(path);
    FString filename = FPaths::GetBaseFilename(path);
    if (!iSource.mFilename.IsEmpty())
        filename += TEXT("_") + iSource.mFilename;
    FString extension = FPaths::GetExtension(path, false);

    ::ULIS::eFileFormat exportImageFormat = Odyssey::GetFileExportImageFormat(mFormat);
    FInt32Range frameRange = GetSourceRange(iSource);
    int startFrame = frameRange.GetLowerBoundValue();
    int endFrame = frameRange.GetUpperBoundValue();

    FScopedSlowTask progressBar(endFrame - startFrame + 1);

    ::ULIS::eFormat format = ::ULIS::Format_BGRA8;
    switch(mAnimation->GetFormat())
    {
        case EOdysseyAnimationFormat::BGRA8: format = ::ULIS::Format_BGRA8;
        case EOdysseyAnimationFormat::RGBAF: format = ::ULIS::Format_RGBAF;
    }

    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(mAnimation->GetWidth(), mAnimation->GetHeight(), format);
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );
    TArray<FGuid> lastRenderingComposition;

    for (int i = startFrame; i <= endFrame; i++)
    {
        progressBar.EnterProgressFrame();
        if (mUniqueFramesOnly)
        {
            TArray<FGuid> renderingComposition = iSource.mImageRenderingAbility->GetRenderingComposition(EOdysseyRenderingType::Render, i);
            if (renderingComposition == lastRenderingComposition)
                continue;

            lastRenderingComposition = renderingComposition;
        }

        TSharedPtr<IOdysseyImageRenderer> renderer = iSource.mImageRenderingAbility->BuildImageRenderer(EOdysseyRenderingType::Render, i);
        renderer->Init();
        FOdysseyImageRendererCopyParams params(block, { ::ULISUtils::ToIntRect(block->Rect()) });
        renderer->Copy(params, {});

        ctx.Finish();

        //Path

        FString frameStr = FString::FromInt(i);
        FString imagePath = folder / filename + TEXT("_");
         for (int j = 0; j < iNumZero - frameStr.Len(); j++)
        {
            imagePath += TEXT("0");
        }
        imagePath += FString::Printf(TEXT("%d."), i) + extension;
        std::string str = std::string( TCHAR_TO_UTF8( *imagePath ) );

        bool canSaveDirectly = false;
        ::ULIS::FContext::SaveBlockToDiskMetrics( *block, exportImageFormat, &canSaveDirectly );
        if (canSaveDirectly)
        {
            ctx.SaveBlockToDisk(
                *block
                , str
                , exportImageFormat
                , 100
            );

            ctx.Finish();
        }
        else
        {
            ::ULIS::eFormat newFormat = block->Model() == ::ULIS::ColorModel_GREY ? ::ULIS::Format_GA8 : ::ULIS::Format_RGBA8;
            if (exportImageFormat == ::ULIS::FileFormat_hdr)
            {
                newFormat = ::ULIS::Format_RGBAF;
            }

            ::ULIS::FBlock blockProxy(block->Width(), block->Height(), newFormat);

            ::ULIS::FEvent eventConvert;
            ctx.ConvertFormat(
                *block
                , blockProxy
                , ::ULIS::FRectI::Auto
                , ::ULIS::FVec2I( 0 )
                , ULIS::FSchedulePolicy::CacheEfficient
                , 0
                , nullptr
                , &eventConvert
            );

            ctx.SaveBlockToDisk(
                blockProxy
                , str
                , exportImageFormat
                , 100
            );

            ctx.Finish();
        }
    }
}

#undef LOCTEXT_NAMESPACE
