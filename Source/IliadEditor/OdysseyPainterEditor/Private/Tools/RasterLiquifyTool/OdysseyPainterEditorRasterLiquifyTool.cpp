// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyTool.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyToolHUD.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyToolUndo.h"
#include "OdysseyMediaRaster.h"
#include "Toolkits/BaseToolkit.h"

#include "ISinglePropertyView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaProvider.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyPainterEditorCommands.h"
#include "ScopedTransaction.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "ULISLoaderModule.h"
#include "ULISUtils.h"

#include "OdysseyVector.h"

#include "OdysseyHUDElement.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

#define M_PI 3.14159265358979323846L
//#define ROTATIONSPEEDINRADIANS 0.0872665f // five degrees
#define ROTATIONSPEEDINRADIANS 0.0349066f // 2 degrees

UOdysseyPainterEditorRasterLiquifyTool::FAlteredImage::~FAlteredImage()
{
    paintEngine.RasterBlock( nullptr );
    paintEngine.SetMaskBlock( nullptr );
}

UOdysseyPainterEditorRasterLiquifyTool::FAlteredImage::FAlteredImage( TSharedPtr<FOdysseyRasterBlock> iSourceRasterBlock
                                                                    , TSharedPtr<::ULIS::FBlock> iMaskBlock )
    : context( IULISLoaderModule::StaticFindOrAddContext( ::ULIS::eFormat::Format_RGBA8 ) )
{
    TSharedPtr<::ULIS::FBlock> sourceBlock = iSourceRasterBlock->GetBlock();

    copiedSourceBlock =  MakeShared<::ULIS::FBlock>( sourceBlock->Width()
                                                   , sourceBlock->Height()
                                                   , ::ULIS::eFormat::Format_RGBA8 );

    context.ConvertFormat( *sourceBlock.Get(), *copiedSourceBlock.Get() );
    context.Finish();

    destinationBlock =  MakeShared<::ULIS::FBlock>( sourceBlock->Width()
                                                  , sourceBlock->Height()
                                                  , ::ULIS::eFormat::Format_RGBA8 );

    paintEngine.RasterBlock( iSourceRasterBlock );
    paintEngine.SetMaskBlock( iMaskBlock );
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterLiquifyTool::~UOdysseyPainterEditorRasterLiquifyTool()
{
}

UOdysseyPainterEditorRasterLiquifyTool::UOdysseyPainterEditorRasterLiquifyTool()
    : mLiquifyHUD( MakeShared<FOdysseyPainterEditorRasterLiquifyToolHUD>( this ) )
    , Radius ( 40 )
    , Mode ()
    , mHiddenModeAsEnum ( Mode.Get() )
    , Strength ( 100 )
    , Hardness ( 0 )
    , OnlyReferToEditngArea ( true )
    , AdjustmentStrength( 100 )
    , bIsMouseLeftButtonDown ( false )
    , mPressure ( 1.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Liquify64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorRasterLiquifyTool::Activate()
{
    Super::Activate();
}

void
UOdysseyPainterEditorRasterLiquifyTool::Load()
{
    UOdysseyPainterEditorTool::Load();

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterLiquifyTool::OnRasterSelectionChanged);

    mHUD->AddElement( rasterSelection->GetHUD() );
    mHUD->AddElement( mLiquifyHUD );

    FetchSourceImages();
    MakeDistortionMap();
    MakeFlowMap();

    mLiquifyHUD->Reset();
}

void
UOdysseyPainterEditorRasterLiquifyTool::Unload()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

     // TODO: what to do when the tool is unloaded ? commit changes or not ?

    mAlteredImageArray.Empty();

    rasterSelection->OnChanged().RemoveAll(this);

    mHUD->RemoveElement( mLiquifyHUD );
    mHUD->RemoveElement( rasterSelection->GetHUD() );

    UOdysseyPainterEditorTool::Unload();
}

bool
UOdysseyPainterEditorRasterLiquifyTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

TSharedPtr<FOdysseyRasterBlock>
UOdysseyPainterEditorRasterLiquifyTool::GetRasterBlockFromEditor(bool iCreate) const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return nullptr;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters;
    if (iCreate)
        mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    else
        mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();

    if( mediaRasters.Num() <= 0 )
        return nullptr;

    return mediaRasters[0]->GetRasterBlock();
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);

    mMousePosition = FVector2D( iPointInTexture.x, iPointInTexture.y );

    mLiquifyHUD->SetCursorPositionInTexture( mMousePosition );

    if (!rasterBlock)
        return;
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mMouseAtDown = FVector2D( iPointInTexture.x, iPointInTexture.y );
    mPreviousPointInTexture = mMouseAtDown;

    if ( iKey == EKeys::LeftMouseButton )
    {
        bIsMouseLeftButtonDown = true;

        mFlowMapBackup = mFlowMap;

        mEditingArea = ::ULIS::FRectI::FromXYWH( iPointInTexture.x - Radius
                                               , iPointInTexture.y - Radius
                                               , (Radius*2) + 1
                                               , (Radius*2) + 1 );

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorRasterLiquifyTool::FetchSourceImages()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);

    TSharedPtr<::ULIS::FBlock> maskBlock = ( rasterSelection->IsEmpty() == false ) ? rasterSelection->GetBlock() : nullptr;

    mAlteredImageArray.Empty();
    mAlteredImageArray.Emplace( srcRasterBlock, maskBlock );
}

void
UOdysseyPainterEditorRasterLiquifyTool::MakeFlowMap()
{
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();

    mFlowMap.Empty();
    mFlowMap.SetSize( srcBlock->Width(), srcBlock->Height() );
}

void
UOdysseyPainterEditorRasterLiquifyTool::MakeDistortionMap()
{
    uint32 offset = 0;

    mDistortionMap.SetNum( ((Radius*2)+1) * ((Radius*2)+1) );

    for( int32 j = -Radius, y = 0; j <= Radius; j++, y++ )
    {
        for( int32 i = -Radius, x = 0; i <= Radius; i++, x++ )
        {
            double distanceToCenter = sqrt( ( i * i ) + ( j * j ) );
            FDistortion& distortion = mDistortionMap[offset++];

            if( distanceToCenter <= Radius )
            {
                ::ULIS::FVec2I pointCoords = ::ULIS::FVec2I( i, j );

                distortion.mapped = true;
                distortion.coords = pointCoords;
                distortion.distanceToCenter = distanceToCenter;
                distortion.angle = atan2( j, i );
            }
            else
            {
                distortion.mapped = false;
            }

            //distortion.offset = offset++;
        }
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::Flow( const FVector2D& iPrevCenter
                                            , const FVector2D& iCurrCenter
                                            , double iStrength
                                            , double iHardness )
{
    uint32 threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    FVector2D motion = iPrevCenter - iCurrCenter;
    uint32 assetWidth = mAlteredImageArray[0].copiedSourceBlock->Width();
    uint32 assetHeight = mAlteredImageArray[0].copiedSourceBlock->Height();

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( int32 y = coreID; y < ((Radius*2)+1); y += threadCount )
        {
            for( int32 x = 0; x < ((Radius*2)+1); x++ )
            {
                uint32 distortionIndex = y * ((Radius*2)+1) + x;
                FDistortion& distortion = mDistortionMap[distortionIndex];

                if( distortion.mapped )
                {
                    ::ULIS::FVec2I absoluteCoords = ::ULIS::FVec2I( ((int32)iCurrCenter.X) + distortion.coords.x
                                                                  , ((int32)iCurrCenter.Y) + distortion.coords.y );
                    FVector2D distortionDstCoords = FVector2D( (double) distortion.coords.x
                                                             , (double) distortion.coords.y );
                    double factor = 1.0f - ( distortion.distanceToCenter / Radius ); // <---- can be precomputed
                    FVector2D newPosition = FVector2D( 0.0f, 0.0f );

                    switch( Mode.Get() )
                    {
                        case EOdysseyLiquifyMode::Push :
                        {
                            newPosition = distortionDstCoords + ( motion * factor );
                        }
                        break;

                        case EOdysseyLiquifyMode::Expand :
                        {
                            double cosAngle = cos( distortion.angle );
                            double sinAngle = sin( distortion.angle );
                            FVector2D minPosition = FVector2D( 0.0f
                                                             , 0.0f );

                            newPosition = distortionDstCoords + ( minPosition - distortionDstCoords ) * factor * iStrength * 0.005f;
                        }
                        break;

                        case EOdysseyLiquifyMode::Pinch :
                        {
                            double cosAngle = cos( distortion.angle );
                            double sinAngle = sin( distortion.angle );
                            FVector2D maxPosition = FVector2D( Radius * cosAngle
                                                             , Radius * sinAngle );

                            newPosition = distortionDstCoords + ( maxPosition - distortionDstCoords ) * factor * iStrength * 0.005f;
                        }
                        break;

                        case EOdysseyLiquifyMode::Twirl :
                        {
                            double cosAngle = cos( ROTATIONSPEEDINRADIANS * factor * iStrength );
                            double sinAngle = sin( ROTATIONSPEEDINRADIANS * factor * iStrength );
                            // Note: can be precomputed
                            FVector2D distortionSrcCoords  = FVector2D( ( distortionDstCoords.X * cosAngle ) - ( distortionDstCoords.Y * sinAngle )
                                                                      , ( distortionDstCoords.X * sinAngle ) + ( distortionDstCoords.Y * cosAngle ) );

                            newPosition = ( distortionSrcCoords );
                        }
                        break;

                        default :
                        break;
                    }

                    FIntVector2 intSrcCoords = FIntVector2( newPosition.X + (int32)iCurrCenter.X
                                                          , newPosition.Y + (int32)iCurrCenter.Y );
                    uint32 srcOffset = ((intSrcCoords.Y) * assetWidth) + intSrcCoords.X;

                    // retrieve deltas for bilinear filtering of the vectors
                    double deltaX = newPosition.X - floorf(newPosition.X);
                    double deltaY = newPosition.Y - floorf(newPosition.Y);

                    uint32 SRCOFFSETTOPLEFT     = srcOffset
                         , SRCOFFSETTOPRIGHT    = SRCOFFSETTOPLEFT + 1
                         , SRCOFFSETBOTTOMRIGHT = SRCOFFSETTOPLEFT + 1 + (assetWidth)
                         , SRCOFFSETBOTTOMLEFT  = SRCOFFSETTOPLEFT + (assetWidth);

                    FFlow v0 = (mFlowMap.currToPrevBuffer[SRCOFFSETTOPRIGHT   ] - mFlowMap.currToPrevBuffer[SRCOFFSETTOPLEFT   ]) * deltaX + mFlowMap.currToPrevBuffer[SRCOFFSETTOPLEFT   ];
                    FFlow v1 = (mFlowMap.currToPrevBuffer[SRCOFFSETBOTTOMRIGHT] - mFlowMap.currToPrevBuffer[SRCOFFSETBOTTOMLEFT]) * deltaX + mFlowMap.currToPrevBuffer[SRCOFFSETBOTTOMLEFT];
                    FFlow vf = ( v1 - v0 ) * deltaY + v0;

                    distortion.currToPrev = FFlow( vf.X, vf.Y ) + FFlow( newPosition.X - distortionDstCoords.X
                                                                       , newPosition.Y - distortionDstCoords.Y );
                }
            }
        }
    } );

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( int32 y = coreID; y < ((Radius*2)+1); y += threadCount )
        {
            for( int32 x = 0; x < ((Radius*2)+1); x++ )
            {
                uint32 distortionIndex = y * ((Radius*2)+1) + x;
                FDistortion& distortion = mDistortionMap[distortionIndex];

                if( distortion.mapped )
                {
                    ::ULIS::FVec2I absoluteCoords = ::ULIS::FVec2I( ((int32)iCurrCenter.X) + distortion.coords.x
                                                                  , ((int32)iCurrCenter.Y) + distortion.coords.y );
                    uint32 offset = ( absoluteCoords.y * assetWidth ) + absoluteCoords.x;

                    if( OnlyReferToEditngArea )
                    {
                        ::ULIS::FVec2I absoluteExpectedCoords = ::ULIS::FVec2I( absoluteCoords.x + distortion.currToPrev.X
                                                                              , absoluteCoords.y + distortion.currToPrev.Y );

                        if( mEditingArea.HitTest( absoluteExpectedCoords ) == true )
                        {
                            mFlowMap.currToPrevBuffer[offset] = distortion.currToPrev;
                        }
                    }
                    else
                    {
                        mFlowMap.currToPrevBuffer[offset] = distortion.currToPrev;
                    }
                }
            }
        }
    });
}

double
UOdysseyPainterEditorRasterLiquifyTool::GetStrength()
{
    return UseStylusPressure && StylusPressureOptions.UseStrength ? mPressure : Strength / 100;
}

double
UOdysseyPainterEditorRasterLiquifyTool::GetHardness()
{
    return UseStylusPressure && StylusPressureOptions.UseHardness ? mPressure : Hardness / 100;
}

void UOdysseyPainterEditorRasterLiquifyTool::Tick(float iDeltaTime)
{
    if( bIsMouseLeftButtonDown )
    {
        ::ULIS::FRectI roi = ::ULIS::FRectI::FromXYWH( mMousePosition.X - Radius
                                                     , mMousePosition.Y - Radius
                                                     , (Radius*2) + 1
                                                     , (Radius*2) + 1 ) ;
        double strength = GetStrength();
        double hardness = GetHardness();

        switch( Mode.Get() )
        {
            case EOdysseyLiquifyMode::Expand :
            case EOdysseyLiquifyMode::Pinch  :
            case EOdysseyLiquifyMode::Twirl  :
                Flow( mMousePosition
                    , mMousePosition
                    , strength
                    , hardness );

                for( FAlteredImage& alteredImage : mAlteredImageArray )
                {
                    ApplyFlow( alteredImage
                             , roi
                             , false );

                    alteredImage.paintEngine.Update( FOdysseyBlendParameters() );
                }
            break;

            default:
            break;
        }
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::ApplyFlow( FAlteredImage& iAlteredImage
                                                 , const ::ULIS::FRectI& iRegionOfInterest
                                                 , bool iBilinearFiltered )
{
    const ULIS::uint8 *srcBlockPixels = iAlteredImage.copiedSourceBlock->PixelBits(0,0);
    const ULIS::uint8 *dstBlockPixels = iAlteredImage.destinationBlock->PixelBits(0,0);
    uint32 assetWidth = mAlteredImageArray[0].copiedSourceBlock->Width();
    uint32 assetHeight = mAlteredImageArray[0].copiedSourceBlock->Height();
    ::ULIS::FRectI roi = iRegionOfInterest;
    uint32 threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    double adjustment = ( double ) AdjustmentStrength / 100;

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( int32 y = roi.y + coreID; y < ( roi.y + roi.h ); y += threadCount )
        {
            for( int32 x = roi.x; x < ( roi.x + roi.w ); x++ )
            {
                uint32 dstOffset = ( y * assetWidth ) + x;
                FFlow& currToPrev = mFlowMap.currToPrevBuffer[dstOffset];

                if( currToPrev.IsZero() == false )
                {
                    ::ULIS::FVec2D srcCoords = ::ULIS::FVec2D( x + ( currToPrev.X * adjustment )
                                                             , y + ( currToPrev.Y * adjustment ) );
                    double deltaX = srcCoords.x - ((int32)srcCoords.x);
                    double deltaY = srcCoords.y - ((int32)srcCoords.y);

                    srcCoords.x = ((int32)srcCoords.x) % assetWidth;
                    srcCoords.y = ((int32)srcCoords.y) % assetHeight;

                    uint32 srcOffset = ( ((int32)srcCoords.y) * assetWidth ) + ((int32)srcCoords.x);

                    // we only work at 4-bytes per pixel
                    switch( 4 /*iSrcBlock->BytesPerPixel()*/ )
                    {
                        case 4 :
                        {
                            uint8 (*srcBlockPixels32)[4] = (uint8(*)[4]) srcBlockPixels;
                            uint8 (*dstBlockPixels32)[4] = (uint8(*)[4]) dstBlockPixels;

                            // Do the bilinear interpolation thing for every component of the pixel
                            if( iBilinearFiltered )
                            {
                                uint32 SRCOFFSETTOPLEFT     = srcOffset
                                     , SRCOFFSETTOPRIGHT    = SRCOFFSETTOPLEFT + 1
                                     , SRCOFFSETBOTTOMRIGHT = SRCOFFSETTOPLEFT + 1 + assetWidth
                                     , SRCOFFSETBOTTOMLEFT  = SRCOFFSETTOPLEFT + assetWidth;

                                for ( uint32 k = 0; k < 4; ++k )
                                {
                                    uint8 C0 = (srcBlockPixels32[SRCOFFSETTOPRIGHT][k]    - srcBlockPixels32[SRCOFFSETTOPLEFT][k])    * deltaX + srcBlockPixels32[SRCOFFSETTOPLEFT][k];
                                    uint8 C1 = (srcBlockPixels32[SRCOFFSETBOTTOMRIGHT][k] - srcBlockPixels32[SRCOFFSETBOTTOMLEFT][k]) * deltaX + srcBlockPixels32[SRCOFFSETBOTTOMLEFT][k];
                                    uint8 CF = (C1 - C0) * deltaY + C0;

                                    dstBlockPixels32[dstOffset][k] = CF;
                                }
                            }
                            else
                            {
                                uint8 R = srcBlockPixels32[srcOffset][0];
                                uint8 G = srcBlockPixels32[srcOffset][1];
                                uint8 B = srcBlockPixels32[srcOffset][2];
                                uint8 A = srcBlockPixels32[srcOffset][3];

                                dstBlockPixels32[dstOffset][0] = R;
                                dstBlockPixels32[dstOffset][1] = G;
                                dstBlockPixels32[dstOffset][2] = B;
                                dstBlockPixels32[dstOffset][3] = A;
                            }
                        }
                        break;

                        default :
                        break;
                    }
                }
            }
        }
    });

    iAlteredImage.context.ConvertFormat( *iAlteredImage.destinationBlock.Get()
                                       , *iAlteredImage.paintEngine.PaintBlock().Get()
                                       , roi
                                       , ::ULIS::FVec2I( roi.x, roi.y ) );
    iAlteredImage.context.Finish();

    iAlteredImage.paintEngine.PaintBlock()->Dirty( roi );
}

void
UOdysseyPainterEditorRasterLiquifyTool::Apply()
{
    // reset flow map
    MakeFlowMap();
    // use current version of the images as source images
    FetchSourceImages();
}

void
UOdysseyPainterEditorRasterLiquifyTool::Reset()
{
    uint32 threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    double adjustment = ( double ) AdjustmentStrength / 100;
    uint32 assetWidth  = mAlteredImageArray.Num() ? mAlteredImageArray[0].copiedSourceBlock->Width()  : 0;
    uint32 assetHeight = mAlteredImageArray.Num() ? mAlteredImageArray[0].copiedSourceBlock->Height() : 0;

    if( assetWidth && assetHeight )
    {
        // reset flow map
        MakeFlowMap();

        // then restore source image
        for( FAlteredImage& alteredImage : mAlteredImageArray )
        {
            alteredImage.context.ConvertFormat( *alteredImage.copiedSourceBlock
                                              , *alteredImage.paintEngine.PaintBlock() );

            alteredImage.context.Finish();

            alteredImage.paintEngine.PaintBlock()->Dirty();
            alteredImage.paintEngine.Update( FOdysseyBlendParameters() );
        }

        FetchSourceImages();
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FVector2D deltaPoint = FVector2D( iPointInTexture.x - mPreviousPointInTexture.X
                                    , iPointInTexture.y - mPreviousPointInTexture.Y );
    double deltaPointDistance = deltaPoint.Length();

    mMousePosition = FVector2D( iPointInTexture.x, iPointInTexture.y );

    if( bIsMouseLeftButtonDown )
    {
        TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);

        mEditingArea = mEditingArea | ::ULIS::FRectI::FromXYWH( iPointInTexture.x - Radius
                                                              , iPointInTexture.y - Radius
                                                              , (Radius*2) + 1
                                                              , (Radius*2) + 1 );

        mLiquifyHUD->SetCursorPositionInTexture( mMousePosition );

        if( deltaPointDistance >= 1.0f )
        {
            FVector2D stampPointInTexture = mPreviousPointInTexture;
            double stepX = deltaPoint.X  / (uint32) deltaPointDistance;
            double stepY = deltaPoint.Y  / (uint32) deltaPointDistance;
            double strength = (double) Strength / 100;
            double hardness = (double) Hardness / 100;

            for( uint32 i = 0; i < (uint32) deltaPointDistance; i++ )
            {
                FVector2D stampNextPointInTexture = FVector2D( stampPointInTexture.X + stepX
                                                             , stampPointInTexture.Y + stepY );

                Flow( stampPointInTexture
                    , stampNextPointInTexture
                    , strength
                    , hardness );

                stampPointInTexture = stampNextPointInTexture;
            }

            ::ULIS::FRectI roi = ::ULIS::FRectI::FromXYWH( mPreviousPointInTexture.X - Radius
                                                         , mPreviousPointInTexture.Y - Radius
                                                         , (Radius*2) + 1
                                                         , (Radius*2) + 1 )
                               | ::ULIS::FRectI::FromXYWH( iPointInTexture.x - Radius
                                                         , iPointInTexture.y - Radius
                                                         , (Radius*2) + 1
                                                         , (Radius*2) + 1 );

            for( FAlteredImage& alteredImage : mAlteredImageArray )
            {
                ApplyFlow( alteredImage
                         , roi
                         , false );

                alteredImage.paintEngine.Update( FOdysseyBlendParameters() );
            }

            mPreviousPointInTexture = FVector2D( iPointInTexture.x, iPointInTexture.y );
        }
    }
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if ( iKey == EKeys::LeftMouseButton )
    {
        FScopedTransaction transaction(LOCTEXT("raster-liquify-tool.transaction.liquify", "Liquify"));

        for( FAlteredImage& alteredImage : mAlteredImageArray )
        {
            ApplyFlow( alteredImage
                     , alteredImage.copiedSourceBlock->Rect()
                     , true );

            alteredImage.paintEngine.Commit( FOdysseyBlendParameters() );
        }

        if( GUndo )
        {
            FCommandChange* undo = new FOdysseyPainterEditorRasterLiquifyToolUndo( mFlowMapBackup
                                                                                 , mEditingArea
                                                                                 , mFlowMap );

            GUndo->StoreUndo( GEditor, TUniquePtr<FCommandChange>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if ( source )
                source->RecordCurrentFrameUndo();
        }

        bIsMouseLeftButtonDown = false;

        return true;
    }

    return false;
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnKeyDown(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Super::OnKeyDown( iKey );
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnKeyUp(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Super::OnKeyUp( iKey );
}

void
UOdysseyPainterEditorRasterLiquifyTool::Flush()
{
}

void
UOdysseyPainterEditorRasterLiquifyTool::Commit()
{
/*
    FScopedTransaction transaction(LOCTEXT("raster-liquify-tool.transaction.liquify-area", "Liquify Area"));
    mPaintEngine.Commit(BlendParameters);

    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();
*/
}

void UOdysseyPainterEditorRasterLiquifyTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
    Super::BindShortcuts(iCommandList);
/*
    const FOdysseyPainterEditorCommands& painterEditorToolCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorRasterLiquifyTool::__VA_ARGS__ ), FCanExecuteAction() );

    MAP_ACTION(painterEditorToolCommands.RefreshBrush, RefreshBrushInstance)
    MAP_ACTION(painterEditorToolCommands.IncreaseBrushSize, AddSize, 1)
    MAP_ACTION(painterEditorToolCommands.DecreaseBrushSize, AddSize, -1)
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeNormal, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Normal )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeErase, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Erase )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeTop, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Top )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeBack, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Back )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeSub, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Sub )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeAdd, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Add )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMul, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Mul )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMin, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Min )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMax, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Max )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeNormal, SetBlendMode, ::ULIS::eBlendMode::Blend_Normal )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeTop, SetBlendMode, ::ULIS::eBlendMode::Blend_Top )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBack, SetBlendMode, ::ULIS::eBlendMode::Blend_Back )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBehind, SetBlendMode, ::ULIS::eBlendMode::Blend_Behind )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDissolve, SetBlendMode, ::ULIS::eBlendMode::Blend_Dissolve )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBayerDither8x8, SetBlendMode, ::ULIS::eBlendMode::Blend_BayerDither8x8 )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDarken, SetBlendMode, ::ULIS::eBlendMode::Blend_Darken )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeMultiply, SetBlendMode, ::ULIS::eBlendMode::Blend_Multiply )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColorBurn, SetBlendMode, ::ULIS::eBlendMode::Blend_ColorBurn )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearBurn, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearBurn )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDarkerColor, SetBlendMode, ::ULIS::eBlendMode::Blend_DarkerColor )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLighten, SetBlendMode, ::ULIS::eBlendMode::Blend_Lighten )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeScreen, SetBlendMode, ::ULIS::eBlendMode::Blend_Screen )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColorDodge, SetBlendMode, ::ULIS::eBlendMode::Blend_ColorDodge )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearDodge, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearDodge )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLighterColor, SetBlendMode, ::ULIS::eBlendMode::Blend_LighterColor )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeOverlay, SetBlendMode, ::ULIS::eBlendMode::Blend_Overlay )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSoftLight, SetBlendMode, ::ULIS::eBlendMode::Blend_SoftLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHardLight, SetBlendMode, ::ULIS::eBlendMode::Blend_HardLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeVividLight, SetBlendMode, ::ULIS::eBlendMode::Blend_VividLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearLight, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePinLight, SetBlendMode, ::ULIS::eBlendMode::Blend_PinLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHardMix, SetBlendMode, ::ULIS::eBlendMode::Blend_HardMix )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePhoenix, SetBlendMode, ::ULIS::eBlendMode::Blend_Phoenix )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeReflect, SetBlendMode, ::ULIS::eBlendMode::Blend_Reflect )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeGlow, SetBlendMode, ::ULIS::eBlendMode::Blend_Glow )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDifference, SetBlendMode, ::ULIS::eBlendMode::Blend_Difference )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeExclusion, SetBlendMode, ::ULIS::eBlendMode::Blend_Exclusion )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAdd, SetBlendMode, ::ULIS::eBlendMode::Blend_Add )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSubstract, SetBlendMode, ::ULIS::eBlendMode::Blend_Substract )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDivide, SetBlendMode, ::ULIS::eBlendMode::Blend_Divide )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAverage, SetBlendMode, ::ULIS::eBlendMode::Blend_Average )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeNegation, SetBlendMode, ::ULIS::eBlendMode::Blend_Negation )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHue, SetBlendMode, ::ULIS::eBlendMode::Blend_Hue )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSaturation, SetBlendMode, ::ULIS::eBlendMode::Blend_Saturation )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColor, SetBlendMode, ::ULIS::eBlendMode::Blend_Color )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLuminosity, SetBlendMode, ::ULIS::eBlendMode::Blend_Luminosity )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePartialDerivative, SetBlendMode, ::ULIS::eBlendMode::Blend_PartialDerivative )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeWhiteOut, SetBlendMode, ::ULIS::eBlendMode::Blend_Whiteout )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAngleCorrected, SetBlendMode, ::ULIS::eBlendMode::Blend_AngleCorrected )

    #undef MAP_ACTION
*/
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnRasterSelectionChanged()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

    for( FAlteredImage& alteredImage : mAlteredImageArray )
    {
        if ( rasterSelection->IsEmpty() )
        {
            alteredImage.paintEngine.SetMaskBlock( nullptr );
        }
        else
        {
            alteredImage.paintEngine.SetMaskBlock( rasterSelection->GetBlock() );
        }
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    Super::ExtendMenu(iExtender);
}

void
UOdysseyPainterEditorRasterLiquifyTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);
}

void UOdysseyPainterEditorRasterLiquifyTool::PropertyChanged( const FName& iPropertyName
                                                            , const FName& iMemberPropertyName
                                                            , bool iIsInteractive )
{
    Super::PropertyChanged(iPropertyName);

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, AdjustmentStrength ) )
    {
        for( FAlteredImage& alteredImage : mAlteredImageArray )
        {
            ApplyFlow( alteredImage
                     , alteredImage.copiedSourceBlock->Rect()
                     , true );

            alteredImage.paintEngine.Update( FOdysseyBlendParameters() );
        }
    }

    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Mode   ) ) )
    {
        mHiddenModeAsEnum = Mode.Get();
    }

    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Mode   ) )
     || ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Radius ) ) )
    {
        MakeDistortionMap();
    }

    mLiquifyHUD->Reset();
}

void
UOdysseyPainterEditorRasterLiquifyTool::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
    Super::PostPropertyChanged(iPropertyName, iIsInteractive);
}

EMouseCursor::Type
UOdysseyPainterEditorRasterLiquifyTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

    return UOdysseyPainterEditorTool::GetMouseCursor();
}

FText
UOdysseyPainterEditorRasterLiquifyTool::GetTooltip() const
{
    return LOCTEXT("raster-liquify-tool.tooltip", "Liquify Tool");
}

/*
void
UOdysseyPainterEditorRasterLiquifyTool::OnRasterSelectionChanged()
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
*/

#undef LOCTEXT_NAMESPACE
