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
}

UOdysseyPainterEditorRasterLiquifyTool::FAlteredImage::FAlteredImage( TSharedPtr<FOdysseyRasterBlock> iSourceRasterBlock
                                                                    , TSharedPtr<::ULIS::FBlock> iMaskBlock )
    : context( IULISLoaderModule::StaticFindOrAddContext( ::ULIS::eFormat::Format_RGBA8 ) )
    // I don't know why but it does not work when I use constructor member variable initialization above.
    // I guess it has something to to with TSharedPtr constructors
    , sourceRasterBlock ( iSourceRasterBlock )
    , sourceBlock ( iSourceRasterBlock->GetBlock() )
    , maskBlock( iMaskBlock )

{
    //sourceRasterBlock = iSourceRasterBlock;
    //maskBlock = iMaskBlock;

    uint32 blockWidth = sourceRasterBlock->GetWidth();
    uint32 blockHeight = sourceRasterBlock->GetHeight();

    // we convert to RGBA8 and will convert back before rendering
    sourceBlockCopy =  MakeShared<::ULIS::FBlock>( blockWidth
                                                 , blockHeight
                                                 , ::ULIS::eFormat::Format_RGBA8 );

    context.ConvertFormat( *sourceRasterBlock->GetBlock(), *sourceBlockCopy.Get() );
    context.Finish();

    destinationBlock =  MakeShared<::ULIS::FBlock>( blockWidth
                                                  , blockHeight
                                                  , ::ULIS::eFormat::Format_RGBA8 );

    context.ConvertFormat( *sourceBlockCopy.Get(), *destinationBlock.Get() );
    context.Finish();
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterLiquifyTool::~UOdysseyPainterEditorRasterLiquifyTool()
{
}

UOdysseyPainterEditorRasterLiquifyTool::UOdysseyPainterEditorRasterLiquifyTool()
    : mLiquifyHUD( MakeShared<FOdysseyPainterEditorRasterLiquifyToolHUD>( this ) )
    , Size ( 100 )
    , Mode ()
    , mHiddenModeAsEnum ( Mode.Get() )
    , Strength ( 100 )
    , Hardness ( 0 )
    , AdjustmentStrength( 100 )
    , PushDirection ( EOdysseyLiquifyPushDirection::Front )
    , TwirlDirection( EOdysseyLiquifyTwirlDirection::Clockwise )
    , OnlyReferToEditngArea ( true )
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

    AdjustmentStrength = 100;

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
    uint32 radius = GetRadius();

    mMouseAtDown = FVector2D( iPointInTexture.x, iPointInTexture.y );
    mPreviousPointInTexture = mMouseAtDown;

    if ( iKey == EKeys::LeftMouseButton )
    {
        bIsMouseLeftButtonDown = true;

        mFlowMapBackup = mFlowMap;

        mEditingArea = ::ULIS::FRectI::FromXYWH( iPointInTexture.x - radius
                                               , iPointInTexture.y - radius
                                               , (radius*2) + 1
                                               , (radius*2) + 1 );

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
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();

    mDistortionMap.Empty();
    mDistortionMap.SetSize( srcBlock->Width(), srcBlock->Height() );
}

void
UOdysseyPainterEditorRasterLiquifyTool::Flow( const FVector2D& iPrevCenter
                                            , const FVector2D& iCurrCenter
                                            , double iStrength
                                            , double iHardness )
{
    uint32 threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    FFlow motion = FFlow( iPrevCenter.X - iCurrCenter.X
                        , iPrevCenter.Y - iCurrCenter.Y );
    uint32 assetWidth = mAlteredImageArray[0].sourceBlockCopy->Width();
    uint32 assetHeight = mAlteredImageArray[0].sourceBlockCopy->Height();
    double twirlDirection = ( TwirlDirection == EOdysseyLiquifyTwirlDirection::Clockwise ) ? -1.0f :  1.0f;
    uint32 influenceRadius = GetRadius();
    uint32 influenceRadiusSquared =  (influenceRadius * influenceRadius );
    uint32 influenceSize = ((influenceRadius*2)+1);
    uint32 xmin = assetWidth - 1
         , ymin = assetHeight - 1
         , xmax = 0
         , ymax = 0;

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( int32 j = coreID, y = - (int32) influenceRadius + coreID; j < (int32) influenceSize; j += threadCount, y += threadCount )
        {
            for( int32 i = 0, x = - (int32) influenceRadius; i < (int32) influenceSize; i++, x++ )
            {
                ::ULIS::FVec2I absoluteCoords = ::ULIS::FVec2I( ((int32)iCurrCenter.X) + x
                                                              , ((int32)iCurrCenter.Y) + y );
                uint32 distanceSquared = ( y * y ) + ( x * x );

                if( ( absoluteCoords.x >= 0 ) && ( absoluteCoords.x < (int32) assetWidth )
                 && ( absoluteCoords.y >= 0 ) && ( absoluteCoords.y < (int32) assetHeight ) )
                {
                    uint32 distortionOffset = ( absoluteCoords.y * assetWidth ) + absoluteCoords.x;
                    FFlow& distortion = mDistortionMap.currToPrevBuffer[distortionOffset];

                    distortion = FFlow::Zero();

                    if ( distanceSquared <= influenceRadiusSquared )
                    {
                        FFlow localPosition = FFlow( x, y );
                        double vdistSquared = ::ULIS::FMath::Max( (double) 0.0f, distanceSquared - (iHardness * influenceRadiusSquared ) );
                        double factor = 1.0f - ( vdistSquared / influenceRadiusSquared );
                        FFlow newPosition = localPosition; // having the same values will result in no difference, so no effect

                        switch( Mode.Get() )
                        {
                            case EOdysseyLiquifyMode::Push :
                            {
                                FFlow motionDirection;

                                switch ( PushDirection )
                                {
                                    case EOdysseyLiquifyPushDirection::Right :
                                        motionDirection = FFlow( -motion.Y, motion.X );
                                    break;

                                    case EOdysseyLiquifyPushDirection::Left :
                                        motionDirection = FFlow( motion.Y, -motion.X );
                                    break;

                                    default :
                                        motionDirection = motion;
                                    break;
                                }

                                newPosition = localPosition + ( motionDirection * factor * iStrength );
                            }
                            break;

                            case EOdysseyLiquifyMode::Expand :
                            {
                                FFlow minPosition = FFlow( 0.0f, 0.0f );

                                newPosition = localPosition + ( minPosition - localPosition ) * factor * iStrength * 0.005f;
                            }
                            break;

                            case EOdysseyLiquifyMode::Pinch :
                            {
                                double angle = atan2( y, x );
                                double cosAngle = cos( angle );
                                double sinAngle = sin( angle );
                                FFlow maxPosition = FFlow( influenceRadius * cosAngle
                                                         , influenceRadius * sinAngle );

                                newPosition = localPosition + ( maxPosition - localPosition ) * factor * iStrength * 0.005f;
                            }
                            break;

                            case EOdysseyLiquifyMode::Crystals :
                            {
                                double angle = ( ( atan2( y, x ) + M_PI ) * 180.0f / M_PI );

                                uint32 intAngle = angle * 0.1f; // reduce the number of possible angles by dividing by 10

                                newPosition = localPosition + ( localPosition * factor * iStrength * 0.1f * ( ( intAngle % 2 ) ? 1.0f : -1.0f )  );
                            }
                            break;

                            case EOdysseyLiquifyMode::Reconstruct :
                            {
                                newPosition = localPosition;

                                distortion = mFlowMap.currToPrevBuffer[distortionOffset] * factor * iStrength;
                            }
                            break;

                            case EOdysseyLiquifyMode::Twirl :
                            {
                                double cosAngle = cos( ROTATIONSPEEDINRADIANS * factor * iStrength * twirlDirection );
                                double sinAngle = sin( ROTATIONSPEEDINRADIANS * factor * iStrength * twirlDirection );

                                newPosition = FFlow( ( localPosition.X * cosAngle ) - ( localPosition.Y * sinAngle )
                                                   , ( localPosition.X * sinAngle ) + ( localPosition.Y * cosAngle ) );
                            }
                            break;

                            default :
                            break;
                        }

                        if( newPosition != localPosition )
                        {
                            bool Repeat = true;
                            // WARNING : At this step, only iCurrCenter must be cast to int32. DO NOT cast
                            // newPosition or else in case of negative value the result will not be correct
                            FIntVector2 intSrcCoords = FIntVector2( ( newPosition.X + (int32)iCurrCenter.X )
                                                                  , ( newPosition.Y + (int32)iCurrCenter.Y ) );

                            if( Repeat )
                            {
                                intSrcCoords.X = intSrcCoords.X % assetWidth;
                                intSrcCoords.Y = intSrcCoords.Y % assetHeight;
                            }

                            uint32 srcOffset = ((intSrcCoords.Y) * assetWidth) + intSrcCoords.X;
                            FFlow vf = FFlow::Zero();

                            if( ( intSrcCoords.X >= 0 ) &&
                                ( intSrcCoords.X < ((int32) assetWidth  - 1 ) ) &&
                                ( intSrcCoords.Y >= 0 ) &&
                                ( intSrcCoords.Y < ((int32) assetHeight - 1 ) ) )
                            {
                                // retrieve deltas for bilinear filtering of the vectors
                                double deltaX = newPosition.X - floorf(newPosition.X);
                                double deltaY = newPosition.Y - floorf(newPosition.Y);

                                uint32 SRCOFFSETTOPLEFT     = srcOffset
                                     , SRCOFFSETTOPRIGHT    = SRCOFFSETTOPLEFT + 1
                                     , SRCOFFSETBOTTOMRIGHT = SRCOFFSETTOPLEFT + 1 + (assetWidth)
                                     , SRCOFFSETBOTTOMLEFT  = SRCOFFSETTOPLEFT + (assetWidth);

                                FFlow v0 = (mFlowMap.currToPrevBuffer[SRCOFFSETTOPRIGHT   ] - mFlowMap.currToPrevBuffer[SRCOFFSETTOPLEFT   ]) * deltaX + mFlowMap.currToPrevBuffer[SRCOFFSETTOPLEFT   ];
                                FFlow v1 = (mFlowMap.currToPrevBuffer[SRCOFFSETBOTTOMRIGHT] - mFlowMap.currToPrevBuffer[SRCOFFSETBOTTOMLEFT]) * deltaX + mFlowMap.currToPrevBuffer[SRCOFFSETBOTTOMLEFT];

                                vf = ( v1 - v0 ) * deltaY + v0;
                            }

                            distortion = FFlow( vf.X, vf.Y ) + FFlow( newPosition.X - localPosition.X
                                                                    , newPosition.Y - localPosition.Y );
                        }

                        if( (uint32) absoluteCoords.x < xmin ) xmin = (uint32) absoluteCoords.x;
                        if( (uint32) absoluteCoords.y < ymin ) ymin = (uint32) absoluteCoords.y;
                        if( (uint32) absoluteCoords.x > xmax ) xmax = (uint32) absoluteCoords.x;
                        if( (uint32) absoluteCoords.y > ymax ) ymax = (uint32) absoluteCoords.y;
                    }
                }
            }
        }
    } );

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( uint32 y = ymin + coreID; y <= ymax; y += threadCount )
        {
            for( uint32 x = xmin; x <= xmax; x++ )
            {
                uint32 flowIndex = ( y * assetWidth ) + x;
                FFlow& flow = mDistortionMap.currToPrevBuffer[flowIndex];

                if( flow != FFlow::Zero() )
                {
                    ::ULIS::FVec2I absoluteCoords = ::ULIS::FVec2I( x, y );
                    uint32 offset = ( absoluteCoords.y * assetWidth ) + absoluteCoords.x;

                    if( OnlyReferToEditngArea )
                    {
                        ::ULIS::FVec2I absoluteExpectedCoords = ::ULIS::FVec2I( absoluteCoords.x + flow.X
                                                                              , absoluteCoords.y + flow.Y );

                        if( mEditingArea.HitTest( absoluteExpectedCoords ) == true )
                        {
                            mFlowMap.currToPrevBuffer[offset] = flow;
                        }
                    }
                    else
                    {
                        mFlowMap.currToPrevBuffer[offset] = flow;
                    }
                }
            }
        }
    });
}

uint32
UOdysseyPainterEditorRasterLiquifyTool::GetRadius()
{
    return Size * 0.5f;
}

double
UOdysseyPainterEditorRasterLiquifyTool::GetStrength()
{
    return UseStylusPressure && StylusPressureOptions.UseStrength ? mPressure : ( double ) Strength / 100;
}

double
UOdysseyPainterEditorRasterLiquifyTool::GetHardness()
{
    return UseStylusPressure && StylusPressureOptions.UseHardness ? mPressure : ( double ) Hardness / 100;
}

void UOdysseyPainterEditorRasterLiquifyTool::Tick(float iDeltaTime)
{

    if( bIsMouseLeftButtonDown )
    {
        uint32 assetWidth = mAlteredImageArray[0].sourceBlockCopy->Width();
        uint32 assetHeight = mAlteredImageArray[0].sourceBlockCopy->Height();
        ::ULIS::FRectI screen = ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );
        uint32 radius = GetRadius();
        ::ULIS::FRectI roi = ::ULIS::FRectI::FromXYWH( mMousePosition.X - radius
                                                     , mMousePosition.Y - radius
                                                     , (radius*2) + 1
                                                     , (radius*2) + 1 ) ;
        double strength = GetStrength();
        double hardness = GetHardness();

        roi = roi & screen; // sanitize

        switch( Mode.Get() )
        {
            case EOdysseyLiquifyMode::Expand :
            case EOdysseyLiquifyMode::Pinch  :
            case EOdysseyLiquifyMode::Twirl  :
            case EOdysseyLiquifyMode::Reconstruct :
                Flow( mMousePosition
                    , mMousePosition
                    , strength
                    , hardness );

                for( FAlteredImage& alteredImage : mAlteredImageArray )
                {
                    ApplyFlow( alteredImage
                             , roi
                             , false );

                    CommitAlteredImage( alteredImage
                                      , roi
                                      , true );
                }
            break;

            default:
            break;
        }
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::ApplyFlow( FAlteredImage& iAlteredImage
                                                 , const ::ULIS::FRectI& iSanitizedRegionOfInterest
                                                 , bool iBilinearFiltered )
{
    const ULIS::uint8 *srcBlockPixels = iAlteredImage.sourceBlockCopy->PixelBits(0,0);
    const ULIS::uint8 *dstBlockPixels = iAlteredImage.destinationBlock->PixelBits(0,0);
    const ULIS::uint8 *mskBlockPixels = iAlteredImage.maskBlock->PixelBits(0,0);
    const float* mskBlockPixelsGF = (float*)mskBlockPixels;
    uint32 assetWidth = mAlteredImageArray[0].sourceBlockCopy->Width();
    uint32 assetHeight = mAlteredImageArray[0].sourceBlockCopy->Height();
    ::ULIS::FRectI sanitizedROI = iSanitizedRegionOfInterest;
    uint32 threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    double adjustment = ( double ) AdjustmentStrength / 100;

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( int32 y = sanitizedROI.y + coreID; y < ( sanitizedROI.y + sanitizedROI.h ); y += threadCount )
        {
            for( int32 x = sanitizedROI.x; x < ( sanitizedROI.x + sanitizedROI.w ); x++ )
            {
                uint32 dstOffset = ( y * assetWidth ) + x;
                FFlow& currToPrev = mFlowMap.currToPrevBuffer[dstOffset];

                if( ( currToPrev.IsZero() == false )
                // we handle masking by ourselves because we don't use a paintengine
                 && ( ( mskBlockPixelsGF == nullptr ) || ( mskBlockPixelsGF[dstOffset] ) ) )
                {
                    ::ULIS::FVec2D srcCoords = ::ULIS::FVec2D( x + ( currToPrev.X * adjustment )
                                                             , y + ( currToPrev.Y * adjustment ) );
                    double deltaX = srcCoords.x - ((int32)srcCoords.x);
                    double deltaY = srcCoords.y - ((int32)srcCoords.y);

                    srcCoords.x = ((int32)srcCoords.x) % assetWidth;
                    srcCoords.y = ((int32)srcCoords.y) % assetHeight;

                    uint32 srcOffset = ( ((int32)srcCoords.y) * assetWidth ) + ((int32)srcCoords.x);

                    if( ( srcCoords.x >= 0 ) &&
                        // substract 1 pixel from width for bilinear filtering
                        ( srcCoords.x < ( assetWidth - 1 ) ) &&
                        ( srcCoords.y >= 0 ) &&
                        // substract 1 pixel from height for bilinear filtering
                        ( srcCoords.y < ( assetHeight - 1 ) ) )
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
                }
            }
        }
    });
}

void
UOdysseyPainterEditorRasterLiquifyTool::ApplyAdjustment()
{
    for( FFlow& flow : mFlowMap.currToPrevBuffer )
    {
        flow = flow * ( double ) AdjustmentStrength / 100.0f;
    }

    AdjustmentStrength = 100;
}

void
UOdysseyPainterEditorRasterLiquifyTool::Reset()
{
    uint32 threadCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();
    double adjustment = ( double ) AdjustmentStrength / 100;
    uint32 assetWidth  = mAlteredImageArray.Num() ? mAlteredImageArray[0].sourceBlockCopy->Width()  : 0;
    uint32 assetHeight = mAlteredImageArray.Num() ? mAlteredImageArray[0].sourceBlockCopy->Height() : 0;

    if( assetWidth && assetHeight )
    {
        // reset flow map
        MakeFlowMap();

        // then restore source image
        for( FAlteredImage& alteredImage : mAlteredImageArray )
        {
            FOdysseyRasterBlockMutator rasterBlockMutator( alteredImage.sourceRasterBlock, false );

            rasterBlockMutator.Copy( alteredImage.sourceBlockCopy, { alteredImage.sourceBlockCopy->Rect() } );
            rasterBlockMutator.Commit();
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
    uint32 radius = GetRadius();

    mMousePosition = FVector2D( iPointInTexture.x, iPointInTexture.y );

    if( bIsMouseLeftButtonDown )
    {
        TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);

        mEditingArea = mEditingArea | ::ULIS::FRectI::FromXYWH( iPointInTexture.x - radius
                                                              , iPointInTexture.y - radius
                                                              , (radius*2) + 1
                                                              , (radius*2) + 1 );

        mLiquifyHUD->SetCursorPositionInTexture( mMousePosition );

        if( deltaPointDistance >= 1.0f )
        {
            uint32 assetWidth = mAlteredImageArray[0].sourceBlockCopy->Width();
            uint32 assetHeight = mAlteredImageArray[0].sourceBlockCopy->Height();
            ::ULIS::FRectI screen = ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );
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

            ::ULIS::FRectI roi = ::ULIS::FRectI::FromXYWH( mPreviousPointInTexture.X - radius
                                                         , mPreviousPointInTexture.Y - radius
                                                         , (radius*2) + 1
                                                         , (radius*2) + 1 )
                               | ::ULIS::FRectI::FromXYWH( iPointInTexture.x - radius
                                                         , iPointInTexture.y - radius
                                                         , (radius*2) + 1
                                                         , (radius*2) + 1 );

            roi = roi & screen; // ROI MUST be sanitized

            for( FAlteredImage& alteredImage : mAlteredImageArray )
            {
                ApplyFlow( alteredImage
                         , roi
                         , false );

                CommitAlteredImage( alteredImage, roi, true );
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
        uint32 assetWidth  = mAlteredImageArray.Num() ? mAlteredImageArray[0].sourceBlockCopy->Width()  : 0;
        uint32 assetHeight = mAlteredImageArray.Num() ? mAlteredImageArray[0].sourceBlockCopy->Height() : 0;
        FScopedTransaction transaction(LOCTEXT("raster-liquify-tool.transaction.liquify", "Liquify"));
        ::ULIS::FRectI screen =  ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );

        for( FAlteredImage& alteredImage : mAlteredImageArray )
        {
            ApplyFlow( alteredImage
                     , alteredImage.sourceBlockCopy->Rect()
                     , true );

            CommitAlteredImage( alteredImage
                              , alteredImage.sourceBlockCopy->Rect()
                              , false );
        }

        if( GUndo )
        {
            FCommandChange* undo = new FOdysseyPainterEditorRasterLiquifyToolUndo( mFlowMapBackup
                                                                                  // sanitize rect
                                                                                 , mEditingArea & screen
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
            alteredImage.maskBlock = nullptr;
        }
        else
        {
            alteredImage.maskBlock = rasterSelection->GetBlock();
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

void
UOdysseyPainterEditorRasterLiquifyTool::CommitAlteredImage( FAlteredImage& iAlteredImage
                                                          , const ::ULIS::FRectI& iSanitizedRegionOfInterest
                                                          , bool iIsInteractive )
{


    if( iIsInteractive )
    {
/*/
        iAlteredImage.context.ConvertFormat(  *iAlteredImage.destinationBlock.Get()
                                            , *iAlteredImage.sourceBlock.Get()
                                            , iSanitizedRegionOfInterest
                                            , ::ULIS::FVec2I( iSanitizedRegionOfInterest.x
                                                            , iSanitizedRegionOfInterest.y ) );
        iAlteredImage.context.Finish();

        iAlteredImage.sourceBlock->Dirty( iSanitizedRegionOfInterest );
*/

        FOdysseyRasterBlockMutator rasterBlockMutator( iAlteredImage.sourceRasterBlock, false );

        rasterBlockMutator.Copy( iAlteredImage.destinationBlock, { iSanitizedRegionOfInterest } /*iAlteredImage.sourceBlockCopy->Rect()*/ );
        rasterBlockMutator.Commit();
    }
    else
    {
        FOdysseyRasterBlockMutator rasterBlockMutator( iAlteredImage.sourceRasterBlock, false );

        rasterBlockMutator.Copy( iAlteredImage.destinationBlock, { iSanitizedRegionOfInterest } /*iAlteredImage.sourceBlockCopy->Rect()*/ );
        rasterBlockMutator.Commit();
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::PropertyChanged( const FName& iPropertyName
                                                       , const FName& iMemberPropertyName
                                                       , bool iIsInteractive )
{
    Super::PropertyChanged(iPropertyName);

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, AdjustmentStrength ) )
    {
        for( FAlteredImage& alteredImage : mAlteredImageArray )
        {
            ApplyFlow( alteredImage
                     , alteredImage.sourceBlockCopy->Rect()
                     , true );

            CommitAlteredImage( alteredImage
                              , alteredImage.sourceBlockCopy->Rect()
                              , iIsInteractive );
        }
    }

    // Caution: FOdysseyLiquifyMode is a struct in order to have a customization in the details view.
    // so we need to compare with iMemberPropertyName instead of iPropertyName
    if( ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Mode   ) ) )
    {
        // Apply adjust (mHiddenModeAsEnum is in its prvious state as it is not updated yet)
        switch ( mHiddenModeAsEnum )
        {
            case EOdysseyLiquifyMode::Adjust :
                ApplyAdjustment();
            break;

            default :
            break;
        }

        mHiddenModeAsEnum = Mode.Get();

        //switch ( mHiddenModeAsEnum )
        //{
        //    default :
        //    break;
        //}
    }

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Size ) )
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
