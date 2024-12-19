// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"

#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyMediaRaster.h"
#include "OdysseyPainterEditor.h"
#include "Palette/OdysseyPaletteEntryColor.h"
#include "Widgets/SOdysseyPainterEditorRasterPaintBucketToolTopTab.h"
#include "PainterEditor/OdysseyPainterEditorSource.h"
#include "OdysseyPainterEditorViewportTab.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDElement.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterPaintBucketTool::~UOdysseyPainterEditorRasterPaintBucketTool()
{

}

UOdysseyPainterEditorRasterPaintBucketTool::UOdysseyPainterEditorRasterPaintBucketTool()
    : mSourceProvider(nullptr)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorRasterPaintBucketTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

void
UOdysseyPainterEditorRasterPaintBucketTool::Load()
{
    UOdysseyPainterEditorTool::Load();

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterPaintBucketTool::OnRasterSelectionChanged);
    if (!rasterSelection->IsEmpty())
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
    mHUD->AddElement(rasterSelection->GetHUD());

    bool hasRaster = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
    if( hasRaster )
    {
        /* It would be better if this is done in OnMouseDown()

        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaRaster>();
        if( mediaRasters.Num() > 0 )
        {
            mPaintEngine.RasterBlock(mediaRasters[0].GetRasterBlock());
        }
        */
    }
}

void
UOdysseyPainterEditorRasterPaintBucketTool::Unload()
{
    mPaintEngine.RasterBlock(nullptr);
    mPaintEngine.SetMaskBlock(nullptr);

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().RemoveAll(this);
    mHUD->RemoveElement(rasterSelection->GetHUD());

    UOdysseyPainterEditorTool::Unload();
}

bool
UOdysseyPainterEditorRasterPaintBucketTool::OnKeyDown( const FKey& iKey )
{
    return false;
}

bool
UOdysseyPainterEditorRasterPaintBucketTool::OnKeyUp( const FKey& iKey )
{
    return false;
}

bool
UOdysseyPainterEditorRasterPaintBucketTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    return true; //prevent any level viewport actions if we get a left mouse down, because we apply the paint bucket on the following mouse up
}

bool
UOdysseyPainterEditorRasterPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if( !hasRaster )
        return false;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if( mediaRasters.IsEmpty() )
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    mPaintEngine.RasterBlock(rasterBlock);

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::eFormat format = paintBlock->Format();
    if (!paintBlock)
        return false;

    if (iPointInTexture.x < 0 || iPointInTexture.y < 0)
        return false;

    if (iPointInTexture.x >= paintBlock->Width() || iPointInTexture.y >= paintBlock->Height())
        return false;

    //TODO: define SourceBlock from the mSource value
    TSharedPtr<::ULIS::FBlock> sourceBlock = GetSourceBlock();
    if (!sourceBlock)
        return false;

    TSharedPtr<::ULIS::FBlock> block = rasterBlock->GetBlock();
    ::ULIS::FPixel sourceColor = sourceBlock->Pixel(iPointInTexture.x, iPointInTexture.y);
    ::ULIS::FColor dstColor = GetEditor()->PaintColor().GetValue();
    dstColor.SetAlphaF(BlendParameters.Opacity / 100.f);
    TSharedPtr<::ULIS::FBlock> sourceMaskBlock = CreateSourceMaskBlock(sourceBlock, sourceColor);
    TSharedPtr<::ULIS::FBlock> maskBlock = MakeShared<::ULIS::FBlock>(block->Width(), block->Height(), ::ULIS::Format_G8);

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);

    ctx.Clear(*maskBlock);
    ctx.Finish();

    if (GapTolerance > 0)
    {
        ctx.Dilate(
              *sourceMaskBlock
            , GapTolerance
            , ::ULIS::FColor::FromGrey8(255)
            , ::ULIS::FColor::FromGrey8(255)
        );
        ctx.Finish();
    }

    //FloodFill
    ctx.FloodFill(
        *sourceMaskBlock
        , *maskBlock
        , iPointInTexture.x
        , iPointInTexture.y
        , ::ULIS::FColor::FromGrey8(0)
        , ::ULIS::FColor::FromGrey8(255));
    ctx.Finish();

    //Expansion and final step of gap tolerance is managed in a single dilate operation
    if (Expansion != 0 || GapTolerance > 0)
    {
        float dilateAmount = Expansion + GapTolerance;

        ctx.Dilate(
              *maskBlock
            , dilateAmount
            , ::ULIS::FColor::FromGrey8(255)
            , dilateAmount > 0 ? ::ULIS::FColor::FromGrey8(255) : ::ULIS::FColor::FromGrey8(0)
        );
        ctx.Finish();
    }

    if (IncludeColors.Num() > 0)
    {
        TSharedPtr<::ULIS::FBlock> tmpMaskBlock = MakeShared<::ULIS::FBlock>(block->Width(), block->Height(), ::ULIS::Format_G8);
        ctx.Clear(*tmpMaskBlock);
        ctx.Finish();

        //add lines to maskBlock as 255
        IncludeColorsToMaskBlock(sourceBlock, maskBlock);

        //floodfill at same position with src 255 dst 255
        ctx.FloodFill(
            *maskBlock
            , *tmpMaskBlock
            , iPointInTexture.x
            , iPointInTexture.y
            , ::ULIS::FColor::FromGrey8(255)
            , ::ULIS::FColor::FromGrey8(255)
        );
        ctx.Finish();

        //use the resulting mask as the maskblock
        maskBlock = tmpMaskBlock;
    }

    //Apply the color to the paintblock

    GEditor->BeginTransaction(LOCTEXT("raster-paint-bucket-tool.transaction.paint-stroke", "FloodFill"));
    ConvertMaskBlockToColorBlock(maskBlock, paintBlock, dstColor);

    paintBlock->Dirty();
    Commit();

    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();

    GEditor->EndTransaction();

    return true;
}

void UOdysseyPainterEditorRasterPaintBucketTool::ExtendContextMenu(FMenuBuilder& iBuilder, const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    Super::ExtendContextMenu(iBuilder, iPointInTexture, iKey);

    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    if( !mediaProvider.HasMedia<FOdysseyMediaRaster>() )
        return;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if( mediaRasters.IsEmpty() )
        return;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    ::ULIS::FColor color = rasterBlock->GetBlock()->Color(iPointInTexture.x, iPointInTexture.y);

    iBuilder.BeginSection("PaintBucket", LOCTEXT("raster-paint-bucket-tool.context-menu.paint-bucket-section.name", "Paint Bucket"));

        iBuilder.AddMenuEntry(
            LOCTEXT("raster-paint-bucket-tool.context-menu.include-color.name", "Include Color")
            , LOCTEXT("raster-paint-bucket-tool.context-menu.include-color.tooltip", "Adds this color in the paint bucket include color list.")
            , FSlateIcon()
            , FUIAction(FExecuteAction::CreateUObject(this, &UOdysseyPainterEditorRasterPaintBucketTool::IncludeColor, color )));
    iBuilder.EndSection();
}

void
UOdysseyPainterEditorRasterPaintBucketTool::IncludeColor( ::ULIS::FColor iColor )
{
    ::ULIS::FColor color = iColor.ToFormat(::ULIS::Format_RGBAF);
    color.SetAlphaF(1.f);
    FOdysseyObjectEditorUtils::PreChangePropertyValue(this, "IncludeColors");
    IncludeColors.AddUnique(FLinearColor(iColor.RedF(), iColor.GreenF(), iColor.BlueF(), iColor.AlphaF()));
    FOdysseyObjectEditorUtils::PostChangePropertyValue(this, "IncludeColors", EPropertyChangeType::ArrayAdd);
}

void
UOdysseyPainterEditorRasterPaintBucketTool::Commit()
{
    mPaintEngine.Commit(BlendParameters);
}

void
UOdysseyPainterEditorRasterPaintBucketTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyPainterEditorRasterPaintBucketTool::CreateSourceMaskBlock(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const
{
    switch(ColorToleranceSource)
    {
        case EOdysseyRasterPaintBucketToolColorToleranceSource::Color: return CreateSourceMaskBlockFromColor(iBlock, iColor, true, false);
        case EOdysseyRasterPaintBucketToolColorToleranceSource::Transparency: return CreateSourceMaskBlockFromColor(iBlock, iColor, false, true);
        case EOdysseyRasterPaintBucketToolColorToleranceSource::ColorAndTransparency: return CreateSourceMaskBlockFromColor(iBlock, iColor, true, true);
        case EOdysseyRasterPaintBucketToolColorToleranceSource::Luminosity: return CreateSourceMaskBlockFromLuminosity(iBlock, iColor);
    }
    return nullptr;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyPainterEditorRasterPaintBucketTool::CreateSourceMaskBlockFromColor(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor, bool iUseColor, bool iUseTransparency) const
{
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iBlock->Width(), iBlock->Height(), ::ULIS::Format_G8);

    ::ULIS::FColor color = iColor.ToFormat(iBlock->Format());
    TArray<float> channelsMin;
    TArray<float> channelsMax;

    for (int i = 0; i < color.SamplesPerPixel(); i++)
    {
        channelsMin.Add(color.ChannelF(i) - ColorTolerance / 100.f);
        channelsMax.Add(color.ChannelF(i) + ColorTolerance / 100.f);
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);
    ctx.FilterInto(
        [&, this]( const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels )
        {
            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
            {
                iDstPixel.SetGrey8( 0 );
                for (int j = 0; j < color.SamplesPerPixel(); j++)
                {
                    if (!iUseTransparency && color.HasAlpha() && color.AlphaIndex() == j)
                        continue;

                    if (!iUseColor && (!color.HasAlpha() || color.AlphaIndex() != j))
                        continue;

                    if( iSrcPixel.ChannelF(j) < channelsMin[j] || iSrcPixel.ChannelF(j) > channelsMax[j] )
                    {
                        iDstPixel.SetGrey8( 255 );
                        break;
                    }
                }
            }
        }
        , *iBlock
        , *block
    );
    ctx.Finish();

    return block;
}

TSharedPtr<::ULIS::FBlock>
UOdysseyPainterEditorRasterPaintBucketTool::CreateSourceMaskBlockFromLuminosity(TSharedPtr<::ULIS::FBlock> iBlock, const ::ULIS::ISample& iColor) const
{
    TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>(iBlock->Width(), iBlock->Height(), ::ULIS::Format_G8);

    float minLuma = iColor.LumaF() - ColorTolerance / 100.f;
    float maxLuma = iColor.LumaF() + ColorTolerance / 100.f;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);
    ctx.FilterInto(
        [this, minLuma, maxLuma]( const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels )
        {
            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
            {
                if( iSrcPixel.LumaF() < minLuma || iSrcPixel.LumaF() > maxLuma )
                    iDstPixel.SetGrey8( 255 );
                else
                    iDstPixel.SetGrey8( 0 );
            }
        }
        , *iBlock
        , *block
    );
    ctx.Finish();

    return block;
}

void
UOdysseyPainterEditorRasterPaintBucketTool::IncludeColorsToMaskBlock(TSharedPtr<::ULIS::FBlock> iSrcBlock, TSharedPtr<::ULIS::FBlock> iMaskBlock) const
{
    TArray<::ULIS::FColor> colors;

    for (const FLinearColor& linearColor : IncludeColors)
    {
        ::ULIS::FColor color = ::ULIS::FColor::FromRGBAF(linearColor.R, linearColor.G, linearColor.B, linearColor.A);
        if (color.Format() != iSrcBlock->Format())
            color = color.ToFormat(iSrcBlock->Format());

        colors.Add(color);
    }

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_G8);
    ctx.FilterInto(
        [this, &colors]( const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels )
        {
            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
            {
                if (iSrcPixel.AlphaF() == 0.f)
                    continue;

                for (const ::ULIS::FColor& color : colors)
                {
                    bool includeColor = true;
                    for (int j = 0; j < color.SamplesPerPixel(); j++)
                    {
                        if (iSrcPixel.HasAlpha() && iSrcPixel.AlphaIndex() == j)
                            continue;


                        if (iSrcPixel.ChannelF(j) != color.ChannelF(j))
                        {
                            includeColor = false;
                            break;
                        }
                    }

                    if (includeColor)
                    {
                        iDstPixel.SetGrey8( 255 );
                        break;
                    }
                }
            }
        }
        , *iSrcBlock
        , *iMaskBlock
    );
    ctx.Finish();
}

void
UOdysseyPainterEditorRasterPaintBucketTool::ConvertMaskBlockToColorBlock(TSharedPtr<::ULIS::FBlock> iMask, TSharedPtr<::ULIS::FBlock> iColorBlock, const ::ULIS::FColor& iColor) const
{
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iColorBlock->Format());

    ::ULIS::FColor color = iColor.ToFormat(iColorBlock->Format());
    ::ULIS::FColor emptyColor = ::ULIS::FColor::Transparent;

    ctx.FilterInto(
        [this, color, emptyColor]( const ::ULIS::FPixel& iSrcPixel, ::ULIS::FPixel& iDstPixel, uint64 iNumPixels )
        {
            for (int i = 0; i < iNumPixels; i++, iSrcPixel.Next(), iDstPixel.Next())
            {
                if( iSrcPixel.Grey8() == 255 )
                    ::ULIS::ISample::ConvertFormat(color, iDstPixel);
                else
                    ::ULIS::ISample::ConvertFormat(emptyColor, iDstPixel);
            }
        }
        , *iMask
        , *iColorBlock
    );
    ctx.Finish();
}

TSharedPtr<::ULIS::FBlock>
UOdysseyPainterEditorRasterPaintBucketTool::GetSourceBlock() const
{
    if (!mSourceProvider)
        return nullptr;

    return mSourceProvider->GetSourceBlock(Source);
}

void
UOdysseyPainterEditorRasterPaintBucketTool::SetSourceProvider(TSharedPtr<FOdysseyPainterEditorRasterPaintBucketToolSourceProvider> iProvider)
{
    mSourceProvider = iProvider;
}

TSharedRef<SWidget>
UOdysseyPainterEditorRasterPaintBucketTool::CreateTopTabWidget()
{
    return SNew(SOdysseyPainterEditorRasterPaintBucketToolTopTab, this);
}

// Returns the BlendParameters
FOdysseyBlendParameters
UOdysseyPainterEditorRasterPaintBucketTool::GetBlendParameters() const
{
    return BlendParameters;
}

EMouseCursor::Type
UOdysseyPainterEditorRasterPaintBucketTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

    return UOdysseyPainterEditorTool::GetMouseCursor();
}

FText
UOdysseyPainterEditorRasterPaintBucketTool::GetTooltip() const
{
    return LOCTEXT("raster-paint-bucket-tool.tooltip", "Paint Bucket Tool");
}

void
UOdysseyPainterEditorRasterPaintBucketTool::OnRasterSelectionChanged()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (rasterSelection->IsEmpty())
    {
        mPaintEngine.SetMaskBlock(nullptr);
    }
    else
    {
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
    }
}

#undef LOCTEXT_NAMESPACE
