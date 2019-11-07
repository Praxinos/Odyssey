// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

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
}

FOdysseyPaintEngine::FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryPtr )
    : FOdysseyTransactionnable( iUndoHistoryPtr )
    , mBrushInstance( NULL )

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
                    mLayerStack->ComputeResultBlockWithTempBuffer( tileRect, mTempBuffer, mOpacityModifier, mBlendingModeModifier );
                } );
            }
        }
    }
    mTileThreadPool->WaitForCompletion();

    ClearInvalidTileMap( mTmpInvalidTileMap );

    if( mIsPendingEndStroke && mDelayQueue.empty() )
    {
        for( int k = 0; k < mCountTileY; ++k )
        {
            for( int l = 0; l < mCountTileX; ++l )
            {
                if( mStrokeInvalidTileMap[k][l] )
                {
                    mTileThreadPool->ScheduleJob( [this, l, k]()
                    {
                        ::ULIS::FRect tileRect = MakeTileRect( l, k );
                        mLayerStack->BlendTempBufferOnCurrentBlock( tileRect, mTempBuffer, mOpacityModifier, mBlendingModeModifier );
                    } );
                }
            }
        }
        mTileThreadPool->WaitForCompletion();

        ClearInvalidTileMap( mStrokeInvalidTileMap );
        ::ULIS::FClearFillContext::Clear( mTempBuffer->GetIBlock() );

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
FOdysseyPaintEngine::SetLayerStack( FOdysseyLayerStack* iLayerStack )
{
    mLayerStack = iLayerStack;

    CheckReallocTempBuffer();

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetBrushInstance( UOdysseyBrushAssetBase* iBrushInstance )
{
    mBrushInstance = iBrushInstance;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetColor( const ::ULIS::CColor& iColor )
{
    mColor = iColor;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetSizeModifier( float iValue )
{
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
    mOpacityModifier = iValue / 100.f;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetFlowModifier( float iValue )
{
    mFlowModifier = iValue / 100.f;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetBlendingModeModifier( ::ULIS::eBlendingMode iValue )
{
    mBlendingModeModifier = iValue;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetStrokeStep( int32 iValue )
{
    mStepValue = iValue;
    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetStrokeAdaptative( bool iValue )
{
    mIsAdaptativeStep = iValue;
    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetStrokePaintOnTick( bool iValue )
{
    mIsPaintOnTick = iValue;
}

void
FOdysseyPaintEngine::SetInterpolationType( EOdysseyInterpolationType iValue )
{
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
    mSmoother->SetStrength( iValue );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetSmoothingEnabled( bool iValue )
{
    mIsSmoothingEnabled = iValue;
}

void
FOdysseyPaintEngine::SetSmoothingRealTime( bool iValue )
{
    mIsRealTime = iValue;
}

void
FOdysseyPaintEngine::SetSmoothingCatchUp( bool iValue )
{
    Record( FName( TEXT( "CatchUp" ) ) );
    ModifyAsState( mIsCatchUp, &mIsCatchUp );
    mIsCatchUp = iValue;
    EndRecord();
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
    UpdateBrushInstance();
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
        mTempBuffer = new FOdysseyBlock( mWidth, mHeight );
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

    FOdysseyBrushState& state = mBrushInstance->GetState();
    state.target_temp_buffer = mTempBuffer;
    state.point = FOdysseyStrokePoint();
    state.color = mColor;
    state.size_modifier = mSizeModifier;
    state.opacity_modifier = mOpacityModifier;
    state.flow_modifier = mFlowModifier;
    state.blendingMode_modifier = mBlendingModeModifier;
    state.step = mInterpolator->GetStep();
    state.smoothing_strength = mSmoother->GetStrength();
    state.currentPointIndex = 0;
    state.currentStroke = &mResultStroke;
    mBrushInstance->CleansePool( ECacheLevel::kState );
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
