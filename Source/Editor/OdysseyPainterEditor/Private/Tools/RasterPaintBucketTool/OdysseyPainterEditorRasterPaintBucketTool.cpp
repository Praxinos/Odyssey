// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"

#include "PainterEditor/OdysseyPainterEditorColorPaletteTab.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyMediaRaster.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorRasterPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterPaintBucketTool::~UOdysseyPainterEditorRasterPaintBucketTool()
{

}

UOdysseyPainterEditorRasterPaintBucketTool::UOdysseyPainterEditorRasterPaintBucketTool()
    : ColorToleranceSource(EOdysseyRasterPaintBucketToolColorToleranceSource::ColorAndTransparency)
    , ColorTolerance( 0 )
    , Expansion( 0 )
    , GapTolerance( 0 )
    , mSourceProvider(nullptr)
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
UOdysseyPainterEditorRasterPaintBucketTool::Unload()
{
    mPaintEngine.RasterBlock(nullptr);
}

void
UOdysseyPainterEditorRasterPaintBucketTool::Load()
{
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
UOdysseyPainterEditorRasterPaintBucketTool::OnMouseDownRaster( TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock
                                                             , const FOdysseyPoint& iPointInTexture
                                                             , const FKey& iKey )
{
    

    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> paintBlock = mPaintEngine.PaintBlock();
    ::ULIS::eFormat format = paintBlock->Format();
    if (!paintBlock)
        return false;
    
    //TODO: define SourceBlock from the mSource value
    TSharedPtr<::ULIS::FBlock> sourceBlock = GetSourceBlock();
    if (!sourceBlock)
        return false;

    ::ULIS::FPixel sourceColor = sourceBlock->Pixel(iPointInTexture.x, iPointInTexture.y);
    ::ULIS::FColor dstColor = GetEditor()->PaintColor().GetValue();
    TSharedPtr<::ULIS::FBlock> sourceMaskBlock = CreateSourceMaskBlock(sourceBlock, sourceColor);
    TSharedPtr<::ULIS::FBlock> maskBlock = MakeShared<::ULIS::FBlock>(iBlock->Width(), iBlock->Height(), ::ULIS::Format_G8);

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);

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

    //Apply the color to the paintblock
    
    GEditor->BeginTransaction(TEXT("PaintEngine"), LOCTEXT("OnPaintStroke", "FloodFill"), nullptr);
    ConvertMaskBlockToColorBlock(maskBlock, paintBlock, dstColor);
    //ConvertMaskBlockToColorBlock(sourceMaskBlock, paintBlock, dstColor);
    
	paintBlock->Dirty();
    Commit();
    GEditor->EndTransaction();

    return true;
}

bool
UOdysseyPainterEditorRasterPaintBucketTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                       , const FKey& iKey )
{
    bool hasRaster = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
    bool ret = false;

    if( hasRaster )
    {
        TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = GetEditor()->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaRaster>();

        if( mediaRasters.Num() && ( mediaRasters[0]->IsLocked() == false ) )
        {
            TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();

            mPaintEngine.RasterBlock(rasterBlock);

            ret = OnMouseDownRaster( rasterBlock->GetBlock(), iPointInTexture, iKey );
        }
    }

    return ret;
}

void
UOdysseyPainterEditorRasterPaintBucketTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
}

void
UOdysseyPainterEditorRasterPaintBucketTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
}

bool
UOdysseyPainterEditorRasterPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    return false;
}

void
UOdysseyPainterEditorRasterPaintBucketTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
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

#undef LOCTEXT_NAMESPACE
