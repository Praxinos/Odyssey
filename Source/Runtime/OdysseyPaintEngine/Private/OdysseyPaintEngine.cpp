// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyPaintEngine.h"
#include "OdysseyMathUtils.h"
#include "OdysseyInterpolationTypes.h"
#include "OdysseyBrushAssetBase.h"
#include <ULIS3>
#include "ULISLoaderModule.h"
#include <chrono>

#define TILE_SIZE 64

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
    delete mSmoother;
    delete mInterpolator;
    DeallocInvalidTileMap( mTmpInvalidTileMap );
    DeallocInvalidTileMap( mStrokeInvalidTileMap );

    if( mBrushCursorPreviewSurface )
        delete  mBrushCursorPreviewSurface;
}

FOdysseyPaintEngine::FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryPtr )
    : mBlock( NULL )
    , mTempBuffer(NULL)
    , mBrushInstance( NULL )

    , mWidth( 0 )
    , mHeight( 0 )
    , mCountTileX( 0 )
    , mCountTileY( 0 )

    , mTmpInvalidTileMap( NULL )
    , mStrokeInvalidTileMap( NULL )

    , mColor( ::ul3::FPixelValue::FromRGBA8( 0, 0, 0, 255 ) )

    , mSizeModifier( 20.f )
    , mOpacityModifier( 1.f )
    , mFlowModifier( 1.f )
    , mBlendingModeModifier( ::ul3::BM_NORMAL )
    , mAlphaModeModifier( ::ul3::AM_NORMAL )
    , mStepValue( 20.f )

    , mInterpolator( NULL )
    , mSmoother( NULL )

    , mIsSmoothingEnabled( true )
    , mIsRealTime( true )
    , mIsCatchUp( true )
    , mIsAdaptativeStep( true )
    , mIsPaintOnTick( false )
    , mIsPendingEndStroke( false )
    , mDelayQueue()

    , mBrushCursorPreviewSurface( nullptr )
    , mBrushCursorPreviewShift( FVector2D() )
    , mLastBrushCursorComputationTime( 0 )
    , mBrushCursorInvalid( true )
{
    mSmoother = new FOdysseySmoothingAverage();
    mInterpolator = new FOdysseyInterpolationCatmullRom();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
void
FOdysseyPaintEngine::InterruptDelay()
{
    while( !mDelayQueue.empty() )
        mDelayQueue.pop();
}

void
FOdysseyPaintEngine::Tick()
{
    if( !mBrushInstance || !mTempBuffer )
        return;

    mBrushInstance->ExecuteTick();

    auto start_time = std::chrono::steady_clock::now();
    long long max_time = 1000 / 60;
    while( !mDelayQueue.empty() )
    {
        std::function<void() >& f = mDelayQueue.front();
        f();
        mDelayQueue.pop();
        auto end_time = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count();
        if( delta > max_time )
            break;
    }


    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    TArray<::ul3::FRect> changedTiles;
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            if( mTmpInvalidTileMap[k][l] )
            {
                ::ul3::FRect rect = MakeTileRect(l, k);
                ::ul3::FVec2F pos(rect.x, rect.y);
                ::ul3::FVec2I posi(pos.x, pos.y);
                changedTiles.Add(rect);
            }
        }
    }

    if (changedTiles.Num() > 0) {
        mOnStrokeChangedDelegate.Broadcast(changedTiles);
    }
    changedTiles.Reset();

    ClearInvalidTileMap( mTmpInvalidTileMap );

    if( mIsPendingEndStroke && mDelayQueue.empty() )
    {
        for( int k = 0; k < mCountTileY; ++k )
        {
            for( int l = 0; l < mCountTileX; ++l )
            {
                if (!mStrokeInvalidTileMap[k][l])
                    continue;
                changedTiles.Add(MakeTileRect(l, k));
            }
        }

        if (changedTiles.Num() > 0) 
        {
            mOnStrokeWillEndDelegate.Broadcast(changedTiles);
            mOnStrokeEndDelegate.Broadcast(changedTiles);
        }
        changedTiles.Reset();

        ClearInvalidTileMap( mStrokeInvalidTileMap );

        ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mTempBuffer->GetBlock(), mTempBuffer->GetBlock()->Rect() );

        mBrushInstance->ExecuteStrokeEnd();

        if( mBrushInstance )
            mBrushInstance->CleansePool( ECacheLevel::kStroke );

        mInterpolator->Reset();
        mSmoother->Reset();
        mRawStroke.Empty();
        mResultStroke.Empty();
        mIsPendingEndStroke = false;
    }
}

void
FOdysseyPaintEngine::Block(FOdysseyBlock* iBlock)
{
    mBlock = iBlock;
    if (!mBlock) {
        mWidth = -1;
        mHeight = -1;

		delete mTempBuffer;
        mTempBuffer = nullptr;

        return;
    }
    mWidth = iBlock->Width();
    mHeight = iBlock->Height();

    if (!mTempBuffer || mTempBuffer->Size() != mBlock->Size())
    {
        delete mTempBuffer;
        mTempBuffer = new FOdysseyBlock(mWidth, mHeight, mBlock->GetUE4TextureSourceFormat());

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mTempBuffer->GetBlock(), mTempBuffer->GetBlock()->Rect());

        ReallocInvalidMaps();
    }
    UpdateBrushInstance();
}

FOdysseyBlock*
FOdysseyPaintEngine::TempBuffer()
{
    return mTempBuffer;
}

void
FOdysseyPaintEngine::SetBrushInstance( UOdysseyBrushAssetBase* iBrushInstance )
{
    if( iBrushInstance != nullptr && mBrushInstance != iBrushInstance )
        iBrushInstance->ExecuteSelected();

    mBrushInstance = iBrushInstance;
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetColor( const ::ul3::FPixelValue& iColor )
{
    InterruptStrokeAndStampInPlace();
    ::ul3::Conv( iColor, mColor );
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetSizeModifier( float iValue )
{
    InterruptStrokeAndStampInPlace();

    if( iValue == 0 )
        iValue = 1;
    mSizeModifier = iValue;

    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetOpacityModifier( float iValue )
{
    InterruptStrokeAndStampInPlace();

    mOpacityModifier = iValue / 100.f;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetFlowModifier( float iValue )
{
    InterruptStrokeAndStampInPlace();

    mFlowModifier = iValue / 100.f;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetBlendingModeModifier( ::ul3::eBlendingMode iValue )
{
    InterruptStrokeAndStampInPlace();

    mBlendingModeModifier = iValue;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetAlphaModeModifier( ::ul3::eAlphaMode iValue )
{
    InterruptStrokeAndStampInPlace();

    mAlphaModeModifier = iValue;

    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::SetStrokeStep( int32 iValue )
{
    InterruptStrokeAndStampInPlace();

    mStepValue = iValue;
    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetStrokeAdaptative( bool iValue )
{
    InterruptStrokeAndStampInPlace();

    mIsAdaptativeStep = iValue;
    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetStrokePaintOnTick( bool iValue )
{
    InterruptStrokeAndStampInPlace();

    mIsPaintOnTick = iValue;
}

void
FOdysseyPaintEngine::SetInterpolationType( EOdysseyInterpolationType iValue )
{
    InterruptStrokeAndStampInPlace();

    switch( iValue )
    {
        case EOdysseyInterpolationType::kBezier:
        {
            if( mInterpolator ) delete mInterpolator;
            mInterpolator = new FOdysseyInterpolationBezier();
            break;
        }

        case EOdysseyInterpolationType::kLine:
        {
            if( mInterpolator ) delete mInterpolator;
            mInterpolator = new FOdysseyInterpolationCatmullRom();
            break;
        }

        case EOdysseyInterpolationType::kCatmullRom:
        {
            if( mInterpolator ) delete mInterpolator;
            mInterpolator = new FOdysseyInterpolationCatmullRom();
            break;
        }
    }

    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetSmoothingMethod( EOdysseySmoothingMethod iValue )
{
    InterruptStrokeAndStampInPlace();

    switch( iValue )
    {
        case EOdysseySmoothingMethod::kAverage:
        {
            // Nothing ATM
            break;
        }

        case EOdysseySmoothingMethod::kGravity:
        {
            // Nothing ATM
            break;
        }

        case EOdysseySmoothingMethod::kPull:
        {
            // Nothing ATM
            break;
        }
    }
}

void
FOdysseyPaintEngine::SetSmoothingStrength( int32 iValue )
{
    InterruptStrokeAndStampInPlace();

    mSmoother->SetStrength( iValue );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetSmoothingEnabled( bool iValue )
{
    InterruptStrokeAndStampInPlace();

    mIsSmoothingEnabled = iValue;
}

void
FOdysseyPaintEngine::SetSmoothingRealTime( bool iValue )
{
    InterruptStrokeAndStampInPlace();

    mIsRealTime = iValue;
}

void
FOdysseyPaintEngine::SetSmoothingCatchUp( bool iValue )
{
    InterruptStrokeAndStampInPlace();

    mIsCatchUp = iValue;
}

bool
FOdysseyPaintEngine::GetStokePaintOnTick() const
{
    return mIsPaintOnTick;
}

bool
FOdysseyPaintEngine::GetSmoothingCatchUp() const
{
    return mIsCatchUp;
}

float
FOdysseyPaintEngine::GetOpacity() const
{
    return mOpacityModifier;
}

::ul3::eBlendingMode
FOdysseyPaintEngine::GetBlendingMode() const
{
    return mBlendingModeModifier;
}

::ul3::eAlphaMode 
FOdysseyPaintEngine::GetAlphaMode() const
{
    return mAlphaModeModifier;
}

void
FOdysseyPaintEngine::PushStroke( const FOdysseyStrokePoint& iPoint, bool iFirst )
{
    if( !mBrushInstance ||
        !mTempBuffer ||
        mIsPendingEndStroke )
        return;

    bool firstPoint = ( mRawStroke.Num() == 0 ) && ( iFirst == false );

    if( firstPoint )
    {
        int duplicate_number = mInterpolator->MinimumRequiredPoints();
        if( mIsRealTime && mIsSmoothingEnabled )
            duplicate_number += mSmoother->MinimumRequiredPoints();

        for( int i = 0; i < duplicate_number; ++i )
            PushStroke( iPoint, true );

        return;
    }

    mRawStroke.Add( iPoint );

    if( mIsSmoothingEnabled )
    {
        mSmoother->AddPoint( iPoint );

        if( !mSmoother->IsReady() )
            return;

        mInterpolator->AddPoint( mSmoother->ComputePoint() );
    }
    else
    {
        mInterpolator->AddPoint( iPoint );
    }

    if( !mInterpolator->IsReady() )
        return;

    const TArray< FOdysseyStrokePoint >& tmp = mInterpolator->ComputePoints();
    int currentIndexBasis = mResultStroke.Num();
    mResultStroke.Append( tmp );

    if( !tmp.Num() )
        return;

    for( int i = currentIndexBasis; i < mResultStroke.Num(); ++i )
    {
        if( i == 0 )
            continue;

        FOdysseyStrokePoint& previous_point = mResultStroke[i - 1];
        FOdysseyStrokePoint& current_point = mResultStroke[i];
        current_point.speed = FVector2D( current_point.x - previous_point.x, current_point.y - previous_point.y );
        current_point.acceleration = current_point.speed - previous_point.speed;
        current_point.jolt = current_point.acceleration - previous_point.acceleration;
        current_point.direction_angle_deg_tangent = atan2( current_point.y - previous_point.y, current_point.x - previous_point.x ) * 180.f / 3.14159265359f;
        current_point.direction_angle_deg_normal = current_point.direction_angle_deg_tangent + 90;
        current_point.direction_vector_tangent = current_point.speed.GetSafeNormal();
        current_point.direction_vector_normal = FVector2D( -current_point.direction_vector_tangent.Y, current_point.direction_vector_tangent.X );
        current_point.distance_travelled = previous_point.distance_travelled + current_point.speed.Size();
    }

    for( int i = currentIndexBasis; i < mResultStroke.Num(); i++ )
    {
        auto point = mResultStroke[i];
        mDelayQueue.emplace( [this, i, point]()
        {
            FOdysseyBrushState& state = mBrushInstance->GetState();
            state.point = point;
            state.currentPointIndex = i;
            if( i == 0 )
                mBrushInstance->ExecuteStrokeBegin();

            mBrushInstance->ExecuteStep();
            mBrushInstance->CleansePool( ECacheLevel::kStep );

            auto invalid_rects = mBrushInstance->GetInvalidRects();
            for( int j = 0; j < invalid_rects.Num(); ++j )
            {
                const ::ul3::FRect& rect = invalid_rects[j];
                float xf = FMath::Max( 0.f, float( rect.x ) / TILE_SIZE );
                float yf = FMath::Max( 0.f, float( rect.y ) / TILE_SIZE );
                float wf = float( rect.w ) / TILE_SIZE;
                float hf = float( rect.h ) / TILE_SIZE;
                int x = xf;
                int y = yf;
                int w = FMath::Min( mCountTileX, int( ceil( xf + wf ) ) ) - x;
                int h = FMath::Min( mCountTileY, int( ceil( yf + hf ) ) ) - y;
                ::ul3::FRect tileRect = { x, y, w, h };
                SetMapWithRect( mTmpInvalidTileMap, tileRect, true );
                SetMapWithRect( mStrokeInvalidTileMap, tileRect, true );
            }
            mBrushInstance->ClearInvalidRects();
        } );
    }
}

void
FOdysseyPaintEngine::EndStroke()
{
    mIsPendingEndStroke = true;
}

void
FOdysseyPaintEngine::AbortStroke()
{
    if( !mTempBuffer )
        return;

    mIsPendingEndStroke = false;
    InterruptDelay();
    mOnStrokeAbortDelegate.Broadcast();

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mTempBuffer->GetBlock(), mTempBuffer->GetBlock()->Rect());

    ClearInvalidTileMap( mTmpInvalidTileMap );
    ClearInvalidTileMap( mStrokeInvalidTileMap );

    if( mBrushInstance )
    {
        mBrushInstance->CleansePool( ECacheLevel::kStep );
        mBrushInstance->CleansePool( ECacheLevel::kSubstroke );
        mBrushInstance->CleansePool( ECacheLevel::kStroke );
    }

    mInterpolator->Reset();
    mSmoother->Reset();
    mRawStroke.Empty();
    mResultStroke.Empty();
}

void
FOdysseyPaintEngine::TriggerStateChanged()
{
    InterruptStrokeAndStampInPlace();
    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::InterruptStrokeAndStampInPlace()
{
    if( mDelayQueue.empty() )
        return;

    EndStroke();
    InterruptDelay();
    Tick();
}


const ::ul3::FPixelValue&
FOdysseyPaintEngine::GetColor() const
{
    return mColor;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API
/* void
FOdysseyPaintEngine::CheckReallocTempBuffer()
{
    if( !mLayerStack )
        return;

    mWidth = mLayerStack->Width();
    mHeight = mLayerStack->Height();

    bool realloc = !mTempBuffer || ( mTempBuffer && mTempBuffer->Size() != mLayerStack->Size() );

    if( realloc )
    {
        delete mTempBuffer;
        mTempBuffer = new FOdysseyBlock( mWidth, mHeight, mTextureSourceFormat );

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mTempBuffer->GetBlock(), mTempBuffer->GetBlock()->Rect() );

        ReallocInvalidMaps();
    }
} */

void
FOdysseyPaintEngine::ReallocInvalidMaps()
{
    DeallocInvalidTileMap( mTmpInvalidTileMap );
    DeallocInvalidTileMap( mStrokeInvalidTileMap );

    mCountTileX = ceil( (float)mWidth / TILE_SIZE );
    mCountTileY = ceil( (float)mHeight / TILE_SIZE );

    ReallocInvalidTileMap( mTmpInvalidTileMap );
    ReallocInvalidTileMap( mStrokeInvalidTileMap );
}

void
FOdysseyPaintEngine::UpdateBrushInstance()
{
    if( !mBrushInstance )
        return;

    mBrushCursorInvalid = true;

    FOdysseyBrushState& state = mBrushInstance->GetState();
    state.target_temp_buffer = mTempBuffer;
    state.point = FOdysseyStrokePoint();
    state.color = mColor;
    state.size_modifier = mSizeModifier;
    state.opacity_modifier = mOpacityModifier;
    state.flow_modifier = mFlowModifier;
    state.blendingMode_modifier = mBlendingModeModifier;
    state.alphaMode_modifier = mAlphaModeModifier;
    state.step = mInterpolator->GetStep();
    state.smoothing_strength = mSmoother->GetStrength();
    state.currentPointIndex = 0;
    state.currentStroke = &mResultStroke;
    mBrushInstance->CleansePool( ECacheLevel::kState );

    mBrushInstance->ExecuteStateChanged();
}


void
FOdysseyPaintEngine::UpdateBrushCursorPreview()
{
    // CURRENTLY DISABLED !
    return;

    /*
    auto current_time = std::chrono::system_clock::now();
    auto duration = current_time.time_since_epoch();
    auto current_millis = std::chrono::duration_cast< std::chrono::milliseconds >( duration ).count();

    // TOPO:
    // 1. Draw One Step in a dummy 1px brush: nothing is actually drawn but the invalid zone feedback is collected
    // 2. Allocate a block big enough to hold everything, max size computed from previous invalid zone
    // 3. Draw One Step in the big block.
    // 4. Use a kernel to detect edge.
    // 5. Make black version of the edge, gaussian blurred with radius 1px
    // 6. Make light version of the edge
    // 7. Blend Black shadow and light outline together in the display surface to make cursor

    if( !mBrushCursorInvalid )
        return;

    if( current_millis - mLastBrushCursorComputationTime < 1000 )
        return;

    if( !mBrushInstance )
        return;

    // Collect Brush State
    FOdysseyBrushState& state = mBrushInstance->GetState();
    state.point.x = 0;
    state.point.y = 0;

    // Create a dummy 1px block to gather size information.
    FOdysseyBlock* dummy1px = new FOdysseyBlock( 1, 1, mTextureSourceFormat );

    // Set the dummy 1px block as target for brush
    state.target_temp_buffer = dummy1px;

    // Execute Brush into dummy 1px target
    mBrushInstance->ExecuteStep();

    // Gather size and invalid information
    auto invalid_rects = mBrushInstance->GetInvalidRects();
    int xmin = INT_MAX;
    int ymin = INT_MAX;
    int xmax = INT_MIN;
    int ymax = INT_MIN;

    // Compute max invalid geometry
    for( int j = 0; j < invalid_rects.Num(); ++j )
    {
        const ::ul3::FRect& rect = invalid_rects[j];
        int x1 = rect.x;
        int y1 = rect.y;
        int x2 = rect.x + rect.w;
        int y2 = rect.y + rect.h;
        xmin = x1 < xmin ? x1 : xmin;
        ymin = y1 < ymin ? y1 : ymin;
        xmax = x2 > xmax ? x2 : xmax;
        ymax = y2 > ymax ? y2 : ymax;
    }

    // Clear invalid rects in brush instance
    mBrushInstance->ClearInvalidRects();
    // Get rid of the dummy 1px block
    delete  dummy1px;

    // Compute preview width / height geometry
    int preview_w = FMath::Max( 1, xmax - xmin );
    int preview_h = FMath::Max( 1, ymax - ymin );

    // Allocate preview_color & preview_outline to draw on step in
    FOdysseyBlock* preview_color = new FOdysseyBlock( preview_w, preview_h, mTextureSourceFormat, nullptr, nullptr, true );

    // Set the preview_color block as target for brush
    state.target_temp_buffer = preview_color;
    state.point.x = -xmin;
    state.point.y = -ymin;

    // Execute Brush into preview_color target
    mBrushInstance->ExecuteStep();

    // Clear invalid rects in brush instance
    mBrushInstance->ClearInvalidRects();

    // Reset brush state target to mTempBuffer
    state.target_temp_buffer = mTempBuffer;

    // Compute Brush Shift
    int shiftx = xmin;
    int shifty = ymin;
    mBrushCursorPreviewShift = FVector2D( shiftx, shifty );

    // Create Outline kernel for convolution
    ::ul3::FKernel edge_kernel( ::ul3::FSize( 3, 3 )
                          , {  255,   255,  255
                            ,  255, -4080,  255
                            ,  255,   255,  255 } );
    ::ul3::FKernel gaussian_kernel( ::ul3::FSize( 3, 3 )
                               , {  8, 16,  8
                               ,   16, 32, 16
                               ,    8, 16,  8 } );
    gaussian_kernel.Normalize();
    // Dealloc Cursor Preview Surface
    if( mBrushCursorPreviewSurface )
        delete  mBrushCursorPreviewSurface;

    // Realloc
    mBrushCursorPreviewSurface = new FOdysseySurface( preview_w, preview_h, mTextureSourceFormat );

    // Compute Outline in surface
    FOdysseyBlock* preview_outline = new FOdysseyBlock( preview_w, preview_h, mTextureSourceFormat, nullptr, nullptr, false );
    FOdysseyBlock* preview_shadow = new FOdysseyBlock( preview_w, preview_h, mTextureSourceFormat, nullptr, nullptr, false );
    ::ul3::FFXContext::Convolution( preview_color->GetBlock(), preview_outline->GetBlock(), edge_kernel, true );
    ::ul3::FClearFillContext::FillPreserveAlpha( preview_outline->GetBlock(), ::ul3::FPixelValue::FromRGBA8( 0, 0, 0 ) );
    ::ul3::FFXContext::Convolution( preview_outline->GetBlock(), preview_shadow->GetBlock(), gaussian_kernel, true );
    ::ul3::FMakeContext::CopyBlockInto( preview_shadow->GetBlock(), mBrushCursorPreviewSurface->Block()->GetBlock() );
    ::ul3::FClearFillContext::FillPreserveAlpha( preview_outline->GetBlock(), ::ul3::FPixelValue::FromRGBA8( 220, 220, 220 ) );
    ::ul3::FBlendingContext::Blend( preview_outline->GetBlock(), mBrushCursorPreviewSurface->Block()->GetBlock(), 0, 0, ::ul3::BM_NORMAL, ::ul3::AM_NORMAL, 1.f );

    mBrushCursorPreviewSurface->Block()->GetBlock()->Invalidate();
    mLastBrushCursorComputationTime = current_millis;

    delete preview_color;
    delete preview_outline;
    delete preview_shadow;
    mBrushCursorInvalid = false;
    */
}


void
FOdysseyPaintEngine::DeallocInvalidTileMap( InvalidTileMap& ioMap )
{
    if( !ioMap )
        return;

    for( int i = 0; i < mCountTileY; ++i )
        delete[] ioMap[i];
    delete[] ioMap;
    ioMap = 0;
}

void
FOdysseyPaintEngine::ReallocInvalidTileMap( InvalidTileMap& ioMap )
{
    if( ioMap )
        return;

    ioMap = new bool*[mCountTileY];
    for( int i = 0; i < mCountTileY; ++i )
    {
        ioMap[i] = new bool[mCountTileX];
        for( int j = 0; j < mCountTileX; ++j )
        {
            ioMap[i][j] = false;
        }
    }
}

void
FOdysseyPaintEngine::ClearInvalidTileMap( InvalidTileMap ioMap )
{
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            ioMap[k][l] = false;
        }
    }
}

::ul3::FRect
FOdysseyPaintEngine::MakeTileRect( int iTileX, int iTileY )
{
    return { iTileX * TILE_SIZE,
             iTileY * TILE_SIZE,
             FMath::Min( iTileX * TILE_SIZE + TILE_SIZE, mWidth ) - iTileX * TILE_SIZE,
             FMath::Min( iTileY * TILE_SIZE + TILE_SIZE, mHeight ) - iTileY * TILE_SIZE };
}

void
FOdysseyPaintEngine::SetMapWithRect( InvalidTileMap ioMap, const ::ul3::FRect& iRect, bool iValue )
{
    for( int k = 0; k < iRect.h; ++k )
    {
        for( int l = 0; l < iRect.w; ++l )
        {
            ioMap[k + iRect.y][l + iRect.x] = iValue;
        }
    }
}
