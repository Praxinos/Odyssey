// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyTool.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyToolCustomization.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyToolHUD.h"
#include "Tools/RasterLiquifyTool/OdysseyPainterEditorRasterLiquifyToolUndo.h"
#include "OdysseyMediaRaster.h"
#include "Toolkits/BaseToolkit.h"
#include "Async/ParallelFor.h"
#include "Editor.h"

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
#include "Editor/Transactor.h"
#include "Editor/TransBuffer.h"
#include "Framework/Notifications/NotificationManager.h"
#include <chrono>

#include "OdysseyHUDElement.h"
#include "SOdysseySinglePropertyView.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

#ifndef M_PI
    #define M_PI 3.14159265358979323846L
#endif

//#define ROTATIONSPEEDINRADIANS 0.0872665f // five degrees
#define ROTATIONSPEEDINRADIANS 0.0349066f // 2 degrees

#define STAMP_DISTANCE 1.0f // stamp every "n" pixels

UOdysseyPainterEditorRasterLiquifyTool::FAlteredImage::~FAlteredImage()
{
}

UOdysseyPainterEditorRasterLiquifyTool::FAlteredImage::FAlteredImage( TSharedPtr<FOdysseyRasterBlock> iSourceRasterBlock
                                                                    , TSharedPtr<::ULIS::FBlock> iMaskBlock )
    : sourceRasterBlock ( iSourceRasterBlock )
    , sourceBlock ( iSourceRasterBlock->GetBlock() )
    , maskBlock( iMaskBlock )
    , mutator( iSourceRasterBlock, true )
    , context( IULISLoaderModule::StaticFindOrAddContext( ::ULIS::eFormat::Format_RGBA8 ) )
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
    : Mode ( EOdysseyLiquifyMode::Push )
    , mPreviousMode ( EOdysseyLiquifyMode::Push )
    , Size ( 200 )
    , Strength ( 20 )
    , Hardness ( 0 )
    , AdjustmentStrength( 100 )
    , PushDirection ( EOdysseyLiquifyPushDirection::Front )
    , TwirlDirection( EOdysseyLiquifyTwirlDirection::Clockwise )
    , BorderPolicy ( EOdysseyLiquifyBorderPolicy::Clamp )
    , mLiquifyHUD( MakeShared<FOdysseyPainterEditorRasterLiquifyToolHUD>( this ) )
    , bIsMouseLeftButtonDown ( false )
    , mPressure ( 1.0f )
{
    mIconStyleSet = FName(TEXT("PainterEditor.ToolsTab.Liquify64"));

    mHUD->AddElement( mLiquifyHUD );
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

    Init();
}

void
UOdysseyPainterEditorRasterLiquifyTool::Unload()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();

     // TODO: what to do when the tool is unloaded ? commit changes or not ?

    mAlteredImageBuffer.Empty();

    rasterSelection->OnChanged().RemoveAll(this);

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
    UOdysseyPainterEditorTool::OnMouseDown(iPointInTexture, iKey);

    float radius = GetRadius();

    mMouseAtDown = FVector2D( iPointInTexture.x, iPointInTexture.y );
    mPreviousPointInTexture = mMouseAtDown;

    // we get too many events when using the stylus, so we will filter some event based on time between 2 events
    mPreviousTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    if ( iKey == EKeys::LeftMouseButton )
    {
        if (mAlteredImageBuffer.IsEmpty())
            return false;

        uint32 assetWidth = mAlteredImageBuffer[0].sourceBlockCopy->Width();
        uint32 assetHeight = mAlteredImageBuffer[0].sourceBlockCopy->Height();
        ::ULIS::FRectI screen = ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );

        bIsMouseLeftButtonDown = true;

        mFlowMapBackup = mFlowMap;

        mActionArea = ::ULIS::FRectI::FromXYWH( iPointInTexture.x - radius
                                              , iPointInTexture.y - radius
                                              , (radius*2) + 1
                                              , (radius*2) + 1 ) & screen;

        mEditingArea = ( mEditingArea.Area() == 0.0f ) ? mActionArea
                                                       : ( mEditingArea | mActionArea );

        // Apply non-bilinear filtered flow on image on mouse down for consistency, or else during the drag, some part of the image
        // image will be bilinear filtered and some other parts not.
/* Commented-out.
        for( FAlteredImage& alteredImage : mAlteredImageBuffer )
        {
            ApplyFlow( alteredImage
                     , mEditingArea
                     , false );

            UpdateAlteredImage( alteredImage
                              , mEditingArea
                              , false );
        }
*/
        return true;
    }

    return false;
}

UOdysseyPainterEditorRasterLiquifyTool::FFlowMap&
UOdysseyPainterEditorRasterLiquifyTool::GetFlowMap()
{
    return mFlowMap;
}

void
UOdysseyPainterEditorRasterLiquifyTool::FetchSourceImages()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    if (!srcRasterBlock)
        return;

    TSharedPtr<::ULIS::FBlock> maskBlock = ( rasterSelection->IsEmpty() == false ) ? rasterSelection->GetBlock() : nullptr;

    mAlteredImageBuffer.Empty();
    mAlteredImageBuffer.Emplace( srcRasterBlock, maskBlock );
}

void
UOdysseyPainterEditorRasterLiquifyTool::MakeFlowMap()
{
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    if (!srcRasterBlock)
        return;

    TSharedPtr<::ULIS::FBlock> srcBlock = srcRasterBlock->GetBlock();

    mFlowMap.Empty();
    mFlowMap.SetSize( srcBlock->Width(), srcBlock->Height() );
}

void
UOdysseyPainterEditorRasterLiquifyTool::MakeDistortionMap()
{
    TSharedPtr<FOdysseyRasterBlock> srcRasterBlock = GetRasterBlockFromEditor(false);
    if (!srcRasterBlock)
        return;

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
    if (mAlteredImageBuffer.IsEmpty())
        return;

    uint32 threadCount = GetThreadCount();
    FFlow motion = FFlow( iCurrCenter.X - iPrevCenter.X
                        , iCurrCenter.Y - iPrevCenter.Y );
    uint32 assetWidth = mAlteredImageBuffer[0].sourceBlockCopy->Width();
    uint32 assetHeight = mAlteredImageBuffer[0].sourceBlockCopy->Height();
    double twirlDirection = ( TwirlDirection == EOdysseyLiquifyTwirlDirection::Clockwise ) ? -1.0f :  1.0f;
    uint32 influenceRadius = GetRadius();
    uint32 influenceRadiusSquared =  (influenceRadius * influenceRadius );
    uint32 xmin = assetWidth - 1
         , ymin = assetHeight - 1
         , xmax = 0
         , ymax = 0;
    double motionLength = motion.Length();

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( int32 j = coreID, y = - (int32) influenceRadius + coreID; y < (int32) influenceRadius; j += threadCount, y += threadCount )
        {
            for( int32 i = 0, x = - (int32) influenceRadius; x < (int32) influenceRadius; i++, x++ )
            {
                ::ULIS::FVec2I absoluteCoords = ::ULIS::FVec2I( ((int32)iCurrCenter.X) + x
                                                              , ((int32)iCurrCenter.Y) + y );
                uint32 distanceSquared = ( y * y ) + ( x * x );

                if( ( absoluteCoords.x >= 0 ) && ( absoluteCoords.x < (int32) assetWidth )
                 && ( absoluteCoords.y >= 0 ) && ( absoluteCoords.y < (int32) assetHeight ) )
                {
                    uint32 distortionOffset = ( absoluteCoords.y * assetWidth ) + absoluteCoords.x;
                    FFlow& distortion = mDistortionMap.toTargetBuffer[distortionOffset];

                    distortion = FFlow::Zero();

                    if ( distanceSquared <= influenceRadiusSquared )
                    {
                        FFlow localPosition = FFlow( x, y );
                        // compute a "virtual distance" determined by the hardness. That will impact the factor.
                        // When the hardness is 100%, the distance is 0, mimicking a factor of 1.0f.
                        // Well, or almost 1.0f because we don't use square roots to compute the factor
                        // but squared values. It's good enough and saves some CPU cycles.
                        double vdistSquared = ::ULIS::FMath::Max( (double) 0.0f, distanceSquared - (iHardness * influenceRadiusSquared ) );
                        double factor = 1.0f - ( vdistSquared / influenceRadiusSquared );
                        FFlow targetPosition = localPosition; // having the same values will result in no difference, so no effect

                        switch( Mode )
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

                                targetPosition = localPosition - ( motionDirection * factor * iStrength );
                            }
                            break;

                            case EOdysseyLiquifyMode::Edge :
                            {
                                // https://en.wikipedia.org/wiki/Vector_projection
                                float dot =  ( motion.X * x ) + ( motion.Y * y );
                                double t = dot / ( motionLength * motionLength );
                                // compute the position of the point projected on the motion vector
                                FFlow projected = t * motion;
                                // compute a new vector that will go towards the motion direction (perpendicular to it, then)
                                FFlow motionDirection = FFlow( x - projected.X
                                                             , y - projected.Y );

                                targetPosition = localPosition + ( motionDirection * factor * iStrength * 0.01f );
                            }
                            break;

                            case EOdysseyLiquifyMode::Expand :
                            {
                                FFlow minPosition = FFlow( 0.0f, 0.0f );

                                targetPosition = localPosition + ( minPosition - localPosition ) * factor * iStrength * 0.005f;
                            }
                            break;

                            case EOdysseyLiquifyMode::Pinch :
                            {
                                double angle = atan2( y, x );
                                double cosAngle = cos( angle );
                                double sinAngle = sin( angle );
                                FFlow maxPosition = FFlow( influenceRadius * cosAngle
                                                         , influenceRadius * sinAngle );

                                targetPosition = localPosition + ( maxPosition - localPosition ) * factor * iStrength * 0.005f;
                            }
                            break;

                            case EOdysseyLiquifyMode::Crystals :
                            {
                                double angle = ( ( atan2( y, x ) + M_PI ) * 180.0f / M_PI );

                                uint32 intAngle = angle * 0.1f; // reduce the number of possible angles by dividing by 10

                                targetPosition = localPosition + ( localPosition * factor * iStrength * 0.1f * ( ( intAngle % 2 ) ? 1.0f : -1.0f )  );
                            }
                            break;

                            case EOdysseyLiquifyMode::Reconstruct :
                            {
                                targetPosition = localPosition;

                                distortion = mFlowMap.toTargetBuffer[distortionOffset] * ( 1.0f - factor * iStrength * 0.01f );
                            }
                            break;

                            case EOdysseyLiquifyMode::Twirl :
                            {
                                double cosAngle = cos( ROTATIONSPEEDINRADIANS * factor * iStrength * twirlDirection );
                                double sinAngle = sin( ROTATIONSPEEDINRADIANS * factor * iStrength * twirlDirection );

                                targetPosition = FFlow( ( localPosition.X * cosAngle ) - ( localPosition.Y * sinAngle )
                                                      , ( localPosition.X * sinAngle ) + ( localPosition.Y * cosAngle ) );
                            }
                            break;

                            default :
                            break;
                        }

                        if( targetPosition != localPosition )
                        {
                            bool Repeat = true;
                            FVector2D floatSrcCoords = FVector2D( ( targetPosition.X + (int32)iCurrCenter.X )
                                                                , ( targetPosition.Y + (int32)iCurrCenter.Y ) );
                            // WARNING : At this step, only iCurrCenter must be cast to int32. DO NOT cast
                            // targetPosition or else in case of negative value the result will not be correct
                            FIntVector2 intSrcCoords = FIntVector2( ( targetPosition.X + (int32)iCurrCenter.X )
                                                                  , ( targetPosition.Y + (int32)iCurrCenter.Y ) );
                            FFlow vf = FFlow::Zero();

                            if( Repeat )
                            {
                                intSrcCoords.X = ( intSrcCoords.X % assetWidth  );
                                intSrcCoords.Y = ( intSrcCoords.Y % assetHeight );
                            }

                            if( ( intSrcCoords.X >= 0 ) &&
                                ( intSrcCoords.X < ((int32) assetWidth  ) ) &&
                                ( intSrcCoords.Y >= 0 ) &&
                                ( intSrcCoords.Y < ((int32) assetHeight ) ) )
                            {
                                uint32 srcOffset = ((intSrcCoords.Y) * assetWidth) + intSrcCoords.X;
                                // retrieve deltas for bilinear filtering of the vectors. Use absolute coords only
                                // as they are always positive. there won't be any miscalculation then.
                                double deltaX = floatSrcCoords.X - floorf(floatSrcCoords.X);
                                double deltaY = floatSrcCoords.Y - floorf(floatSrcCoords.Y);
                                int32 nextSrcCoordsX = intSrcCoords.X + 1;
                                int32 nextSrcCoordsY = intSrcCoords.Y + 1;

                                uint32 SRCOFFSETTOPLEFT     = srcOffset
                                     , SRCOFFSETTOPRIGHT    = ( nextSrcCoordsX == ((int32)assetWidth)  ) ? srcOffset
                                                                                                         : SRCOFFSETTOPLEFT + 1
                                     , SRCOFFSETBOTTOMRIGHT = ( nextSrcCoordsX == ((int32)assetWidth)  )
                                                           || ( nextSrcCoordsY == ((int32)assetHeight) ) ? srcOffset
                                                                                                         : SRCOFFSETTOPLEFT + 1 + (assetWidth)
                                     , SRCOFFSETBOTTOMLEFT  = ( nextSrcCoordsY == ((int32)assetHeight) ) ? srcOffset
                                                                                                         : SRCOFFSETTOPLEFT + (assetWidth);
                                // bilinear filtering
                                FFlow v0 = (mFlowMap.toTargetBuffer[SRCOFFSETTOPRIGHT   ] - mFlowMap.toTargetBuffer[SRCOFFSETTOPLEFT   ]) * deltaX + mFlowMap.toTargetBuffer[SRCOFFSETTOPLEFT   ];
                                FFlow v1 = (mFlowMap.toTargetBuffer[SRCOFFSETBOTTOMRIGHT] - mFlowMap.toTargetBuffer[SRCOFFSETBOTTOMLEFT]) * deltaX + mFlowMap.toTargetBuffer[SRCOFFSETBOTTOMLEFT];

                                vf = ( v1 - v0 ) * deltaY + v0;
                            }

                            distortion = FFlow( vf.X, vf.Y ) + FFlow( targetPosition.X - localPosition.X
                                                                    , targetPosition.Y - localPosition.Y );
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
                FFlow& flow = mDistortionMap.toTargetBuffer[flowIndex];

                if( flow != FFlow::Zero() )
                {
                    ::ULIS::FVec2I absoluteCoords = ::ULIS::FVec2I( x, y );
                    uint32 offset = ( absoluteCoords.y * assetWidth ) + absoluteCoords.x;

                    mFlowMap.toTargetBuffer[offset] = flow;

                    flow = FFlow::Zero();
                }
            }
        }
    });
}

bool
UOdysseyPainterEditorRasterLiquifyTool::HasRadius() const
{
    return Mode != EOdysseyLiquifyMode::Adjust;
}

void
UOdysseyPainterEditorRasterLiquifyTool::SetRadius(float iRadius)
{
    Size = iRadius * 2.f;
}

float
UOdysseyPainterEditorRasterLiquifyTool::GetRadius() const
{
    return Size / 2.f;
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

::ULIS::FRectI
UOdysseyPainterEditorRasterLiquifyTool::GetEditingArea()
{
    return mEditingArea;
}

void UOdysseyPainterEditorRasterLiquifyTool::Tick(float iDeltaTime)
{
    if (mAlteredImageBuffer.IsEmpty())
        return;

    if( bIsMouseLeftButtonDown )
    {
        uint32 assetWidth = mAlteredImageBuffer[0].sourceBlockCopy->Width();
        uint32 assetHeight = mAlteredImageBuffer[0].sourceBlockCopy->Height();
        ::ULIS::FRectI screen = ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );
        float radius = GetRadius();
        ::ULIS::FRectI roi = ::ULIS::FRectI::FromXYWH( mMousePosition.X - radius
                                                     , mMousePosition.Y - radius
                                                     , (radius*2) + 1
                                                     , (radius*2) + 1 ) ;
        double strength = GetStrength();
        double hardness = GetHardness();

        roi = roi & screen; // sanitize

        switch( Mode )
        {
            case EOdysseyLiquifyMode::Expand :
            case EOdysseyLiquifyMode::Pinch  :
            case EOdysseyLiquifyMode::Twirl  :
            //case EOdysseyLiquifyMode::Edge  :
            case EOdysseyLiquifyMode::Reconstruct :
                Flow( mMousePosition
                    , mMousePosition
                    , strength
                    , hardness );

                for( FAlteredImage& alteredImage : mAlteredImageBuffer )
                {
                    ApplyFlow( alteredImage
                             , roi
                             , false );

                    UpdateAlteredImage( alteredImage
                                      , roi
                                      , false );
                }
            break;

            default:
            break;
        }
    }
}

uint32
UOdysseyPainterEditorRasterLiquifyTool::GetThreadCount()
{
    return FPlatformMisc::NumberOfCoresIncludingHyperthreads();
}

void
UOdysseyPainterEditorRasterLiquifyTool::ApplyFlow( FAlteredImage& iAlteredImage
                                                 , const ::ULIS::FRectI& iSanitizedRegionOfInterest
                                                 , bool iBilinearFiltered )
{
    if (mAlteredImageBuffer.IsEmpty())
        return;

    const ULIS::uint8 *srcBlockPixels = iAlteredImage.sourceBlockCopy->PixelBits(0,0);
    const ULIS::uint8 *dstBlockPixels = iAlteredImage.destinationBlock->PixelBits(0,0);
    const ULIS::uint8 *mskBlockPixels = iAlteredImage.maskBlock ? iAlteredImage.maskBlock->PixelBits(0,0) : nullptr;
    const float* mskBlockPixelsGF = (float*)mskBlockPixels;
    uint32 assetWidth = mAlteredImageBuffer[0].sourceBlockCopy->Width();
    uint32 assetHeight = mAlteredImageBuffer[0].sourceBlockCopy->Height();
    ::ULIS::FRectI sanitizedROI = iSanitizedRegionOfInterest;
    uint32 threadCount = GetThreadCount();
    double adjustment = ( double ) AdjustmentStrength / 100;
    ::ULIS::FRectI screen = ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );

    ParallelFor( threadCount, [&]( int32 coreID )
    {
        for( int32 y = sanitizedROI.y + coreID; y < ( sanitizedROI.y + sanitizedROI.h ); y += threadCount )
        {
            for( int32 x = sanitizedROI.x; x < ( sanitizedROI.x + sanitizedROI.w ); x++ )
            {
                uint32 dstOffset = ( y * assetWidth ) + x;
                FFlow& toTarget = mFlowMap.toTargetBuffer[dstOffset];

                if( ( toTarget.IsZero() == false )
                // we handle masking by ourselves because we don't use a paintengine
                 && ( ( mskBlockPixelsGF == nullptr ) || ( mskBlockPixelsGF[dstOffset] ) ) )
                {
                    ::ULIS::FVec2D targetCoords = ::ULIS::FVec2D( (double) x + ( toTarget.X * adjustment )
                                                                , (double) y + ( toTarget.Y * adjustment ) );
                    double deltaX = targetCoords.x - floorf(targetCoords.x);
                    double deltaY = targetCoords.y - floorf(targetCoords.y);

                    targetCoords.x = ((int32)targetCoords.x);
                    targetCoords.y = ((int32)targetCoords.y);

                    // truncate vector if not in editing area
                    if( screen.HitTest( targetCoords ) == false )
                    {
                        switch( BorderPolicy )
                        {
                            case EOdysseyLiquifyBorderPolicy::Clamp :
                            {
                                if( targetCoords.x >= screen.w )
                                {
                                    double ratio = ( screen.w - x ) / toTarget.X;
                                    targetCoords = ::ULIS::FVec2D( screen.w - 1, y + targetCoords.y * ratio );
                                }

                                if( targetCoords.x < 0 )
                                {
                                    double ratio = ( - x ) / toTarget.X;
                                    targetCoords = ::ULIS::FVec2D( 0, y + toTarget.Y * ratio );
                                }

                                if( targetCoords.y >= screen.h )
                                {
                                    double ratio = ( screen.h - y ) / toTarget.Y;
                                    targetCoords = ::ULIS::FVec2D( x + toTarget.X * ratio, screen.h - 1 );
                                }

                                if( targetCoords.y < 0 )
                                {
                                    double ratio = ( - y ) / toTarget.Y;
                                    targetCoords = ::ULIS::FVec2D( x + toTarget.X * ratio, 0 );
                                }
                            }
                            break;

                            default:
                            break;
                        }
                    }

                    uint32 srcOffset = ( ((int32)targetCoords.y) * assetWidth ) + ((int32)targetCoords.x);
                    uint8 (*dstBlockPixels32)[4] = (uint8(*)[4]) dstBlockPixels;

                    if( ( targetCoords.x >= 0 )
                        // substract 1 pixel from width for bilinear filtering
                     && ( targetCoords.x < ( assetWidth  ) )
                     && ( targetCoords.y >= 0 )
                        // substract 1 pixel from height for bilinear filtering
                     && ( targetCoords.y < ( assetHeight ) ) )
                    {
                        uint8 (*srcBlockPixels32)[4] = (uint8(*)[4]) srcBlockPixels;

                        // Do the bilinear interpolation thing for every component of the pixel
                        if( iBilinearFiltered
                         && ( targetCoords.x < ( assetWidth  - 1 ) )
                         && ( targetCoords.y < ( assetHeight - 1 ) ) )
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
                    else
                    {
                        dstBlockPixels32[dstOffset][0] = 0;
                        dstBlockPixels32[dstOffset][1] = 0;
                        dstBlockPixels32[dstOffset][2] = 0;
                        dstBlockPixels32[dstOffset][3] = 0;
                    }
                }
            }
        }
    });
}

void
UOdysseyPainterEditorRasterLiquifyTool::ApplyAdjustment()
{
    for( FFlow& flow : mFlowMap.toTargetBuffer )
    {
        flow = flow * ( double ) AdjustmentStrength / 100.0f;
    }

    AdjustmentStrength = 100;
}

void
UOdysseyPainterEditorRasterLiquifyTool::Init()
{
    FetchSourceImages();
    MakeDistortionMap();
    MakeFlowMap();

    AdjustmentStrength = 100;

    mEditingArea = ::ULIS::FRectD::FromXYWH( 0, 0, 0, 0 );

    mLiquifyHUD->Reset();
}

void
UOdysseyPainterEditorRasterLiquifyTool::Reset()
{
    // needed by the FOdysseyRasterBlockMutator for recording an undo
    GEditor->BeginTransaction( LOCTEXT("raster-liquify-tool.transaction.reset","Reset Liquify"));

    // restore source images as it was
    for( FAlteredImage& alteredImage : mAlteredImageBuffer )
    {
        alteredImage.mutator.Copy( alteredImage.sourceBlockCopy, { alteredImage.sourceBlockCopy->Rect() } );
        alteredImage.mutator.Commit();
    }

    Init();

    // the tool's reference images must be reset after a call to undo or redo
    if( GUndo )
    {
        FCommandChange* undo = new FOdysseyPainterEditorRasterLiquifyToolUndo( this );

        GUndo->StoreUndo( this, TUniquePtr<FCommandChange>(undo) );

        TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
        if ( source )
            source->RecordCurrentFrameUndo();
    }

    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorRasterLiquifyTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FVector2D deltaPoint = FVector2D( iPointInTexture.x - mPreviousPointInTexture.X
                                    , iPointInTexture.y - mPreviousPointInTexture.Y );
    double deltaPointDistance = deltaPoint.Length();
    uint64 currentTime = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    mMousePosition = FVector2D( iPointInTexture.x, iPointInTexture.y );

    if( bIsMouseLeftButtonDown )
    {
        if (mAlteredImageBuffer.IsEmpty())
            return;

        mLiquifyHUD->SetCursorPositionInTexture( mMousePosition );

        // Note: deltaPointDistance stores the delta between two Flow operation
        // not between to calls of Mouse Drag ...
        // we also don't want to many calls to this function especially when using the stylus so we
        // want at least 20 milliseconds between 2 calls
        if( ( deltaPointDistance >= STAMP_DISTANCE ) && ( ( currentTime - mPreviousTime ) >= 40 ) )
        {
            float radius = GetRadius();
            uint32 assetWidth = mAlteredImageBuffer[0].sourceBlockCopy->Width();
            uint32 assetHeight = mAlteredImageBuffer[0].sourceBlockCopy->Height();
            ::ULIS::FRectI screen = ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );

//UE_LOG( LogTemp, Warning, TEXT("event delta time : %d ms"), ( currentTime - mPreviousTime ) );

            FVector2D stampPointInTexture = mPreviousPointInTexture;
            double stepX = deltaPoint.X  / (uint32) deltaPointDistance;
            double stepY = deltaPoint.Y  / (uint32) deltaPointDistance;
            double strength = (double) Strength / 100;
            double hardness = (double) Hardness / 100;
            // ... however, iPointInTexture.deltaPosition is the delta between 2 calls to Mouse Drag. So,
            // if the mouse motion is very fast, we call Flow every 4 pixels.
            //uint32 flow_distance = 4;
            uint32 flow_distance = 1;

            mActionArea = ( mActionArea | ::ULIS::FRectI::FromXYWH( iPointInTexture.x - radius
                                                                  , iPointInTexture.y - radius
                                                                  , (radius*2) + 1
                                                                  , (radius*2) + 1 ) ) & screen;

            mEditingArea = mEditingArea | mActionArea;

            for( uint32 i = 0; i < (uint32) deltaPointDistance; i += flow_distance )
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

            for( FAlteredImage& alteredImage : mAlteredImageBuffer )
            {
                ApplyFlow( alteredImage
                         , roi
                         , false );

                UpdateAlteredImage( alteredImage
                                  , roi
                                  , false );
            }

            mPreviousPointInTexture = FVector2D( iPointInTexture.x, iPointInTexture.y );
            mPreviousTime = currentTime;
        }
    }
}

bool
UOdysseyPainterEditorRasterLiquifyTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if ( iKey == EKeys::LeftMouseButton )
    {
        if (mAlteredImageBuffer.IsEmpty())
            return false;

        uint32 assetWidth  = mAlteredImageBuffer[0].sourceBlockCopy->Width();
        uint32 assetHeight = mAlteredImageBuffer[0].sourceBlockCopy->Height();
        ::ULIS::FRectI screen =  ::ULIS::FRectI::FromXYWH( 0, 0, assetWidth, assetHeight );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction( LOCTEXT("raster-liquify-tool.transaction.liquify","Raster Liquify Tool"));

        for( FAlteredImage& alteredImage : mAlteredImageBuffer )
        {
            ApplyFlow( alteredImage
                     , mActionArea & screen
                     , true );

            UpdateAlteredImage( alteredImage
                              , mActionArea & screen
                              , true );
        }

        if( GUndo )
        {
            FCommandChange* undo = new FOdysseyPainterEditorRasterLiquifyToolUndo( this );

            GUndo->StoreUndo( this, TUniquePtr<FCommandChange>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if ( source )
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

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

    for( FAlteredImage& alteredImage : mAlteredImageBuffer )
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
UOdysseyPainterEditorRasterLiquifyTool::UpdateAlteredImage( FAlteredImage& iAlteredImage
                                                          , const ::ULIS::FRectI& iSanitizedRegionOfInterest
                                                          , bool iCommit )
{
    iAlteredImage.mutator.Copy( iAlteredImage.destinationBlock, { iSanitizedRegionOfInterest } );

    if( iCommit )
    {
        iAlteredImage.mutator.Commit();
    }
}

void
UOdysseyPainterEditorRasterLiquifyTool::PropertyChanged( const FName& iPropertyName
                                                       , const FName& iMemberPropertyName
                                                       , bool iIsInteractive )
{
    Super::PropertyChanged(iPropertyName);

    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, AdjustmentStrength ) )
     || ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, BorderPolicy       ) ) )
    {
        for( FAlteredImage& alteredImage : mAlteredImageBuffer )
        {
            ApplyFlow( alteredImage
                     , alteredImage.sourceBlockCopy->Rect()
                     , true );

            UpdateAlteredImage( alteredImage
                              , alteredImage.sourceBlockCopy->Rect()
                              , true );
        }

        // store an undo to reinit the tool on undo / redo
        if( GUndo )
        {
            FCommandChange* undo = new FOdysseyPainterEditorRasterLiquifyToolUndo( this );

            GUndo->StoreUndo( this, TUniquePtr<FCommandChange>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
            if ( source )
                source->RecordCurrentFrameUndo();
        }
    }

    // Caution: FOdysseyLiquifyMode is a struct in order to have a customization in the details view.
    // so we need to compare with iMemberPropertyName instead of iPropertyName
    if( ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorRasterLiquifyTool, Mode   ) ) )
    {
        // Apply adjust (mPreviousMode is in its prvious state as it is not updated yet)
        switch ( mPreviousMode )
        {
            case EOdysseyLiquifyMode::Adjust :
                ApplyAdjustment();
            break;

            default :
            break;
        }
    }

    mPreviousMode = Mode;

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

void
UOdysseyPainterEditorRasterLiquifyTool::RegisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

    // Custom detail views
    PropertyModule.RegisterCustomClassLayout( UOdysseyPainterEditorRasterLiquifyTool::StaticClass()->GetFName()
                                            , FOnGetDetailCustomizationInstance::CreateLambda(
                                                  []()
                                                  {
                                                      return MakeShareable( new FOdysseyPainterEditorRasterLiquifyToolCustomization() );
                                                  } ) );
}

void
UOdysseyPainterEditorRasterLiquifyTool::UnregisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomPropertyTypeLayout( UOdysseyPainterEditorRasterLiquifyTool::StaticClass()->GetFName() );
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
