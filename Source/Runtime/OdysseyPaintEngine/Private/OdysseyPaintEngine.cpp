// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyPaintEngine.h"
#include "OdysseyMathUtils.h"
#include "OdysseyInterpolationTypes.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyLayerStack.h"
#include <ULIS_CORE>
#include <chrono>

#define TILE_SIZE 64

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
    delete mSmoother;
    delete mInterpolator;
    delete mTempBuffer;
    DeallocInvalidTileMap( mTmpInvalidTileMap );
    DeallocInvalidTileMap( mStrokeInvalidTileMap );

    mTileThreadPool->WaitForCompletion();
    delete mTileThreadPool;

    if( mBrushCursorPreviewSurface )
        delete  mBrushCursorPreviewSurface;
}

FOdysseyPaintEngine::FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryPtr )
    : mBrushInstance( NULL )

    , mTextureSourceFormat( ETextureSourceFormat::TSF_BGRA8 )
    , mLayerStack( NULL )
    , mWidth( 0 )
    , mHeight( 0 )
    , mCountTileX( 0 )
    , mCountTileY( 0 )

    , mTempBuffer( NULL )
    , mTmpInvalidTileMap( NULL )
    , mStrokeInvalidTileMap( NULL )

    , mColor( ::ULIS::CColor() )

    , mSizeModifier( 20.f )
    , mOpacityModifier( 1.f )
    , mFlowModifier( 1.f )
    , mBlendingModeModifier( ::ULIS::eBlendingMode::kNormal )
    , mAlphaModeModifier( ::ULIS::eAlphaMode::kNormal )
    , mStepValue( 20.f )

    , mInterpolator( NULL )
    , mSmoother( NULL )

    , mIsSmoothingEnabled( true )
    , mIsRealTime( true )
    , mIsCatchUp( true )
    , mIsAdaptativeStep( true )
    , mIsPaintOnTick( false )
    , mIsPendingEndStroke( false )
    , mTileThreadPool( nullptr )
    , mDelayQueue()

    , mBrushCursorPreviewSurface( nullptr )
    , mBrushCursorPreviewShift( FVector2D() )
    , mLastBrushCursorComputationTime( 0 )
    , mBrushCursorInvalid( true )
{
    mSmoother = new FOdysseySmoothingAverage();
    mInterpolator = new FOdysseyInterpolationBezier();
    mTileThreadPool = new ::ULIS::FThreadPool();
    int maxThreads = mTileThreadPool->GetMaxWorkers();
    mTileThreadPool->SetNumWorkers( maxThreads - 1 );
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
    if( !mBrushInstance )
        return;

    //mBrushInstance->ExecuteTick();

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

    /*
    ::ULIS::ParallelForPool( (*mTileThreadPool), nTileY
                       , [&]( int iLine ) {
                            for( int x = 0; x < nTileX; ++x ) {
                                if( tmpInvalidTileMap[iLine][x] ) {
                                    ::ULIS::FRect  tileRect = MAKE_TILE_RECT( x, iLine );
                                    layer_stack->ComputeResultBlockWithTempBuffer( tileRect, temp_buffer, opacity_modifier );
                                }
                            }
                       } );
    */

    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            if( mTmpInvalidTileMap[k][l] )
            {
                mTileThreadPool->ScheduleJob( [this, l, k]()
                {
                    ::ULIS::FRect tileRect = MakeTileRect( l, k );
                    mLayerStack->ComputeResultBlockWithTempBuffer( tileRect, mTempBuffer, mOpacityModifier, mBlendingModeModifier, mAlphaModeModifier );
                } );
            }
        }
    }
    mTileThreadPool->WaitForCompletion();

    ClearInvalidTileMap( mTmpInvalidTileMap );

    if( mIsPendingEndStroke && mDelayQueue.empty() )
    {
        bool IsRecordStarted = false;

        for( int k = 0; k < mCountTileY; ++k )
        {
            for( int l = 0; l < mCountTileX; ++l )
            {
                if( mStrokeInvalidTileMap[k][l] )
                {
                    if( !IsRecordStarted )
                    {
                        mLayerStack->mDrawingUndo->StartRecord();
                        IsRecordStarted = true;
                    }

                    ::ULIS::FRect tileRect = MakeTileRect( l, k );
                    mLayerStack->mDrawingUndo->SaveData( l, k, tileRect.w, tileRect.h );
                    mTileThreadPool->ScheduleJob( [this, l, k]()
                    {
                        ::ULIS::FRect tileRect = MakeTileRect( l, k );
                        mLayerStack->BlendTempBufferOnCurrentBlock( tileRect, mTempBuffer, mOpacityModifier, mBlendingModeModifier, mAlphaModeModifier );
                    } );
                }
            }
        }

        if( IsRecordStarted )
            mLayerStack->mDrawingUndo->EndRecord();

        mTileThreadPool->WaitForCompletion();

        ClearInvalidTileMap( mStrokeInvalidTileMap );
        ::ULIS::FClearFillContext::Clear( mTempBuffer->GetIBlock() );

        //mBrushInstance->ExecuteStrokeEnd();

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
FOdysseyPaintEngine::SetTextureSourceFormat( ETextureSourceFormat iTextureSourceFormat )
{
    mTextureSourceFormat = iTextureSourceFormat;
}


void
FOdysseyPaintEngine::SetLayerStack( FOdysseyLayerStack* iLayerStack )
{
    mLayerStack = iLayerStack;

    CheckReallocTempBuffer();

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetBrushInstance( UOdysseyBrushAssetBase* iBrushInstance )
{
    //InterruptStrokeAndStampInPlace();

    /*
    if( iBrushInstance != nullptr && mBrushInstance != iBrushInstance )
        iBrushInstance->ExecuteSelected();
    */

    mBrushInstance = iBrushInstance;
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetColor( const ::ULIS::CColor& iColor )
{
    InterruptStrokeAndStampInPlace();
    mColor = iColor;
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
FOdysseyPaintEngine::SetBlendingModeModifier( ::ULIS::eBlendingMode iValue )
{
    InterruptStrokeAndStampInPlace();

    mBlendingModeModifier = iValue;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetAlphaModeModifier( ::ULIS::eAlphaMode iValue )
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
            // Nothing ATM
            break;
        }

        case EOdysseyInterpolationType::kLine:
        {
            // Nothing ATM
            break;
        }
    }
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

void
FOdysseyPaintEngine::PushStroke( const FOdysseyStrokePoint& iPoint, bool iFirst )
{
    if( !mBrushInstance ||
        !mLayerStack ||
        !mTempBuffer ||
        mIsPendingEndStroke )
        return;

    //If the layer is locked, we don't draw
    if( mLayerStack->GetCurrentLayer()->IsLocked() )
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

    /*
    mDelayQueue.emplace( [&](){

    }
    */
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
                const ::ULIS::FRect& rect = invalid_rects[j];
                float xf = FMath::Max( 0.f, float( rect.x ) / TILE_SIZE );
                float yf = FMath::Max( 0.f, float( rect.y ) / TILE_SIZE );
                float wf = float( rect.w ) / TILE_SIZE;
                float hf = float( rect.h ) / TILE_SIZE;
                int x = xf;
                int y = yf;
                int w = FMath::Min( mCountTileX, int( ceil( xf + wf ) ) ) - x;
                int h = FMath::Min( mCountTileY, int( ceil( yf + hf ) ) ) - y;
                ::ULIS::FRect tileRect = { x, y, w, h };
                SetMapWithRect( mTmpInvalidTileMap, tileRect, true );
                SetMapWithRect( mStrokeInvalidTileMap, tileRect, true );
            }
            mBrushInstance->ClearInvalidRects();
        } );
    }

    /*
    for( int i = currentIndexBasis; i < result_stroke.Num(); i++ )
    {
        FOdysseyBrushState& state = brush_instance->GetState();
        state.point = result_stroke[ i ];
        state.currentPointIndex = i;
        brush_instance->ExecuteStep();
        brush_instance->CleansePool( ECacheLevel::kStep );

        auto invalid_rects = brush_instance->GetInvalidRects();
        for( int j = 0; j < invalid_rects.Num(); ++j )
        {
            const ::ULIS::FRect& rect = invalid_rects[j];
            float xf = FMath::Max( 0.f, float( rect.x ) / TILE_SIZE );
            float yf = FMath::Max( 0.f, float( rect.y ) / TILE_SIZE );
            float wf = float( rect.w  ) / TILE_SIZE;
            float hf = float( rect.h ) / TILE_SIZE;
            int x = xf;
            int y = yf;
            int w = FMath::Min( nTileX, int( ceil( xf + wf ) ) ) - x;
            int h = FMath::Min( nTileY, int( ceil( yf + hf ) ) ) - y;
            ::ULIS::FRect tileRect = { x, y, w, h };
            SET_MAP_WITH_RECT( tmpInvalidTileMap,       tileRect, true );
            SET_MAP_WITH_RECT( strokeInvalidTileMap,    tileRect, true );
        }
        brush_instance->ClearInvalidRects();
    }
    */

    /*
    static ::ULIS::FThreadPool tilePool;
    auto num_workers = tilePool.GetNumWorkers();
    auto max_workers = tilePool.GetMaxWorkers() -1;
    if( num_workers!= max_workers )
        tilePool.SetNumWorkers( max_workers );

    ::ULIS::ParallelForPool( tilePool, nTileY
                       , [&]( int iLine ) {
                            for( int x = 0; x < nTileX; ++x ) {
                                if( tmpInvalidTileMap[iLine][x] ) {
                                    ::ULIS::FRect  tileRect = MAKE_TILE_RECT( x, iLine );
                                    layer_stack->ComputeResultBlockWithTempBuffer( tileRect, temp_buffer, opacity_modifier );
                                }
                            }
                       } );
    */

    /*
    for( int k = 0; k < nTileY; ++k ) {
        for( int l = 0; l < nTileX; ++l ) {
            if( tmpInvalidTileMap[k][l] ) {
                ::ULIS::FRect  tileRect = MAKE_TILE_RECT( l, k );
                layer_stack->ComputeResultBlockWithTempBuffer( tileRect, temp_buffer, opacity_modifier );
    } } }
    */

    /*
    CLEAR_MAP( tmpInvalidTileMap );
    brush_instance->ClearInvalidRects();
    brush_instance->CleansePool( ECacheLevel::kSubstroke );
    */
}

void
FOdysseyPaintEngine::EndStroke()
{
    mIsPendingEndStroke = true;
}

void
FOdysseyPaintEngine::AbortStroke()
{
    mIsPendingEndStroke = false;
    InterruptDelay();
    mLayerStack->ComputeResultBlock();
    ::ULIS::FClearFillContext::Clear( mTempBuffer->GetIBlock() );
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
    EndStroke();
    InterruptDelay();
    Tick();
}


const ::ULIS::CColor&
FOdysseyPaintEngine::GetColor() const
{
    return mColor;
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API
void
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
        ::ULIS::FClearFillContext::Clear( mTempBuffer->GetIBlock() );
        ReallocInvalidMaps();
    }
}

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
        const ::ULIS::FRect& rect = invalid_rects[j];
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
    ::ULIS::FKernel edge_kernel( ::ULIS::FSize( 3, 3 )
                          , {  255,   255,  255
                            ,  255, -4080,  255
                            ,  255,   255,  255 } );
    ::ULIS::FKernel gaussian_kernel( ::ULIS::FSize( 3, 3 )
                               , {  8, 16,  8
                               ,   16, 32, 16
                               ,    8, 16,  8 } );
    gaussian_kernel.Normalize();
    /*
    ::ULIS::FKernel kernel( ::ULIS::FSize( 3, 3 )
                          , {  0,   0,  0
                            ,  0,   1,  0
                            ,  0,   0,  0 } );
    */

    // Dealloc Cursor Preview Surface
    if( mBrushCursorPreviewSurface )
        delete  mBrushCursorPreviewSurface;

    // Realloc
    mBrushCursorPreviewSurface = new FOdysseySurface( preview_w, preview_h, mTextureSourceFormat );

    // Compute Outline in surface
    FOdysseyBlock* preview_outline = new FOdysseyBlock( preview_w, preview_h, mTextureSourceFormat, nullptr, nullptr, false );
    FOdysseyBlock* preview_shadow = new FOdysseyBlock( preview_w, preview_h, mTextureSourceFormat, nullptr, nullptr, false );
    ::ULIS::FFXContext::Convolution( preview_color->GetIBlock(), preview_outline->GetIBlock(), edge_kernel, true );
    ::ULIS::FClearFillContext::FillPreserveAlpha( preview_outline->GetIBlock(), ::ULIS::CColor( 0, 0, 0 ) );
    ::ULIS::FFXContext::Convolution( preview_outline->GetIBlock(), preview_shadow->GetIBlock(), gaussian_kernel, true );
    ::ULIS::FMakeContext::CopyBlockInto( preview_shadow->GetIBlock(), mBrushCursorPreviewSurface->Block()->GetIBlock() );

    ::ULIS::FClearFillContext::FillPreserveAlpha( preview_outline->GetIBlock(), ::ULIS::CColor( 220, 220, 220 ) );
    ::ULIS::FBlendingContext::Blend( preview_outline->GetIBlock(), mBrushCursorPreviewSurface->Block()->GetIBlock(), 0, 0, ::ULIS::eBlendingMode::kNormal, ::ULIS::eAlphaMode::kNormal, 1.f );

    mBrushCursorPreviewSurface->Block()->GetIBlock()->Invalidate();
    mLastBrushCursorComputationTime = current_millis;

    delete preview_color;
    delete preview_outline;
    delete preview_shadow;
    mBrushCursorInvalid = false;
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

::ULIS::FRect
FOdysseyPaintEngine::MakeTileRect( int iTileX, int iTileY )
{
    return { iTileX * TILE_SIZE,
             iTileY * TILE_SIZE,
             FMath::Min( iTileX * TILE_SIZE + TILE_SIZE, mWidth ) - iTileX * TILE_SIZE,
             FMath::Min( iTileY * TILE_SIZE + TILE_SIZE, mHeight ) - iTileY * TILE_SIZE };
}

void
FOdysseyPaintEngine::SetMapWithRect( InvalidTileMap ioMap, const ::ULIS::FRect& iRect, bool iValue )
{
    for( int k = 0; k < iRect.h; ++k )
    {
        for( int l = 0; l < iRect.w; ++l )
        {
            ioMap[k + iRect.y][l + iRect.x] = iValue;
        }
    }
}
