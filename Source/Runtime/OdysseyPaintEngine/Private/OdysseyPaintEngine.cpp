// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
#include "OdysseyPaintEngine.h"
#include "OdysseyMathUtils.h"
#include "OdysseyInterpolationTypes.h"
#include "OdysseyBrushAssetBase.h"
#include "OdysseyLayerStack.h"
#include <ULIS_CORE>
#include <chrono>

#define TILE_SIZE 64

#define DEALLOC_TILE_MAP( iName )               \
    if( iName ) {                               \
        for( int i = 0; i < nTileY; ++i )       \
                delete []  iName[i];            \
        delete [] iName;                        \
        iName = 0;                              \
    }

#define REALLOC_TILE_MAP( iName )               \
    if( !iName ) {                              \
        iName = new  bool*[ nTileY ];           \
        for( int i = 0; i < nTileY; ++i ) {     \
            iName[i] = new  bool[ nTileX ];     \
            for( int j = 0; j < nTileX; ++j ) { \
                iName[i][j] = false;            \
    } } }

#define TO_TILE_RECT( iRect )                                   \
    { FMath::Max( 0, iRect.x / TILE_SIZE )                      \
    , FMath::Max( 0, iRect.y / TILE_SIZE )                      \
    , FMath::Min( nTileX - 1, int( ceil( float( iRect.x + iRect.width  ) / TILE_SIZE ) ) - ( iRect.x / TILE_SIZE ) ) \
    , FMath::Min( nTileY - 1, int( ceil( float( iRect.y + iRect.height ) / TILE_SIZE ) ) - ( iRect.y / TILE_SIZE ) ) }

#define SET_MAP_WITH_RECT( iMap, iRect, iValue )                \
    for( int k = 0; k < iRect.h; ++k ) {                        \
        for( int l = 0; l < iRect.w; ++l ) {                    \
            iMap[k + iRect.y ][l + iRect.x ] = iValue; }}

#define CLEAR_MAP( iMap )                                       \
    for( int k = 0; k < nTileY; ++k ) {                         \
        for( int l = 0; l < nTileX; ++l ) {                     \
            iMap[k][l] = false; }}

#define MAKE_TILE_RECT( x, y )  \
    { x * TILE_SIZE, y * TILE_SIZE, FMath::Min( x * TILE_SIZE + TILE_SIZE, width ) - x * TILE_SIZE, FMath::Min( y * TILE_SIZE + TILE_SIZE, height ) - y * TILE_SIZE }


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPaintEngine::~FOdysseyPaintEngine()
{
    delete  smoother;
    delete  interpolator;
    delete  temp_buffer;
    DEALLOC_TILE_MAP( tmpInvalidTileMap )
    DEALLOC_TILE_MAP( strokeInvalidTileMap )

    mTileThreadPool->WaitForCompletion();
    delete  mTileThreadPool;
}

FOdysseyPaintEngine::FOdysseyPaintEngine( FOdysseyUndoHistory* InUndoHistoryPtr )
    : FOdysseyTransactionnable    (   InUndoHistoryPtr )
    , brush_instance              (   NULL             )

    , layer_stack                 (   NULL             )
    , width                       (   0                )
    , height                      (   0                )
    , nTileX                      (   0                )
    , nTileY                      (   0                )

    , temp_buffer                 (   NULL             )
    , tmpInvalidTileMap           (   NULL             )
    , strokeInvalidTileMap        (   NULL             )

    , color                       (   ::ULIS::CColor()  )

    , size_modifier               (   20.f             )
    , opacity_modifier            (   1.f              )
    , flow_modifier               (   1.f              )
    , mBlendingModeModifier       (   ::ULIS::eBlendingMode::kNormal )
    , mStepValue                  (   20.f )

    , interpolator                (   NULL             )
    , smoother                    (   NULL             )

    , bSmoothingEnabled           (   true             )
    , bRealTime                   (   true             )
    , bCatchUp                    (   true             )
    , bAdaptativeStep             (   true             )
    , bPaintOnTick                (   false            )
    , bPendingEndStroke           (   false            )
    , mTileThreadPool             (   nullptr          )
    , mDelayQueue                 (                    )
{
    smoother        = new FOdysseySmoothingAverage();
    interpolator    = new FOdysseyInterpolationBezier();
    mTileThreadPool = new ::ULIS::FThreadPool();
    int maxThreads = mTileThreadPool->GetMaxWorkers();
    mTileThreadPool->SetNumWorkers( maxThreads - 1 );
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API
void
FOdysseyPaintEngine::InterruptDelay()
{
    while(!mDelayQueue.empty()) mDelayQueue.pop();
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
        auto end_time   = std::chrono::steady_clock::now();
        auto delta      = std::chrono::duration_cast< std::chrono::milliseconds>(end_time - start_time ).count();
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

    for( int k = 0; k < nTileY; ++k ) {
        for( int l = 0; l < nTileX; ++l ) {
            if( tmpInvalidTileMap[k][l] ) {
                mTileThreadPool->ScheduleJob( [this,l,k]() {
                    ::ULIS::FRect  tileRect = MAKE_TILE_RECT( l, k );
                    layer_stack->ComputeResultBlockWithTempBuffer( tileRect, temp_buffer, opacity_modifier, mBlendingModeModifier );
                } );
    } } }
    mTileThreadPool->WaitForCompletion();

    CLEAR_MAP( tmpInvalidTileMap );

    if( bPendingEndStroke && mDelayQueue.empty() )
    {
        for( int k = 0; k < nTileY; ++k ) {
            for( int l = 0; l < nTileX; ++l ) {
                if( strokeInvalidTileMap[k][l] ) {
                    mTileThreadPool->ScheduleJob( [this,l,k]() {
                        ::ULIS::FRect  tileRect = MAKE_TILE_RECT( l, k );
                        layer_stack->BlendTempBufferOnCurrentBlock( tileRect, temp_buffer, opacity_modifier, mBlendingModeModifier );
                    } );
        } } }
        mTileThreadPool->WaitForCompletion();

        CLEAR_MAP( strokeInvalidTileMap );
        ::ULIS::FClearFillContext::Clear( temp_buffer->GetIBlock() );

        if( brush_instance )
            brush_instance->CleansePool( ECacheLevel::kStroke );

        interpolator->Reset();
        smoother->Reset();
        raw_stroke.Empty();
        result_stroke.Empty();
        bPendingEndStroke = false;
    }
}

void
FOdysseyPaintEngine::SetLayerStack( FOdysseyLayerStack* iLayerStack )
{
    layer_stack = iLayerStack;

    CheckReallocTempBuffer();
    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::SetBrushInstance( UOdysseyBrushAssetBase* iBrushInstance )
{
    brush_instance = iBrushInstance;
    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::SetColor( const ::ULIS::CColor& iColor )
{
    color = iColor;
    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::SetSizeModifier( float iValue )
{
    if( iValue == 0 ) iValue = 1;
    size_modifier = iValue;

    float val = FMath::Max( 1.f, bAdaptativeStep ? ( mStepValue / 100.f ) * size_modifier : (float)mStepValue );
    interpolator->SetStep( val );

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetOpacityModifier( float iValue )
{
    opacity_modifier = iValue / 100.f;
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetFlowModifier( float iValue )
{
    flow_modifier = iValue / 100.f;
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
    float val = FMath::Max( 1.f, bAdaptativeStep ? ( mStepValue / 100.f ) * size_modifier : (float)mStepValue );
    interpolator->SetStep( val );
    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::SetStrokeAdaptative( bool  iValue )
{
    bAdaptativeStep = iValue;
    float val = FMath::Max( 1.f, bAdaptativeStep ? ( mStepValue / 100.f ) * size_modifier : (float)mStepValue );
    interpolator->SetStep( val );
    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::SetStrokePaintOnTick( bool  iValue )
{
    bPaintOnTick = iValue;
}


void
FOdysseyPaintEngine::SetInterpolationType( EOdysseyInterpolationType iValue )
{
    switch( iValue )
    {
        case  EOdysseyInterpolationType::kBezier:
        {
            // Nothing ATM
            break;
        }

        case  EOdysseyInterpolationType::kLine:
        {
            // Nothing ATM
            break;
        }
    }
}


void
FOdysseyPaintEngine::SetSmoothingMethod( EOdysseySmoothingMethod   iValue )
{
    switch( iValue )
    {
        case  EOdysseySmoothingMethod::kAverage:
        {
            // Nothing ATM
            break;
        }

        case  EOdysseySmoothingMethod::kGravity:
        {
            // Nothing ATM
            break;
        }

        case  EOdysseySmoothingMethod::kPull:
        {
            // Nothing ATM
            break;
        }
    }
}


void
FOdysseyPaintEngine::SetSmoothingStrength( int32 iValue )
{
    smoother->SetStrength( iValue );
    UpdateBrushInstance();
}


void
FOdysseyPaintEngine::SetSmoothingEnabled( bool  iValue )
{
    bSmoothingEnabled = iValue;
}


void
FOdysseyPaintEngine::SetSmoothingRealTime( bool  iValue )
{
    bRealTime = iValue;
}


void
FOdysseyPaintEngine::SetSmoothingCatchUp( bool  iValue )
{
    Record( FName( TEXT( "CatchUp") ) );
    ModifyAsState( bCatchUp, &bCatchUp );
    bCatchUp = iValue;
    EndRecord();
}


bool
FOdysseyPaintEngine::GetStokePaintOnTick() const
{
    return  bPaintOnTick;
}


bool
FOdysseyPaintEngine::GetSmoothingCatchUp() const
{
    return  bCatchUp;
}


void
FOdysseyPaintEngine::PushStroke( const  FOdysseyStrokePoint&  iPoint, bool first )
{
    if( !brush_instance ||
        !layer_stack    ||
        !temp_buffer    ||
        bPendingEndStroke )
        return;

    //If the layer is locked, we don't draw
    if( layer_stack->GetCurrentLayer()->IsLocked() )
        return;


    bool firstPoint = ( raw_stroke.Num() == 0 ) && ( first == false );

    if( firstPoint )
    {
        int duplicate_number = interpolator->MinimumRequiredPoints();
        if( bRealTime && bSmoothingEnabled )
            duplicate_number += smoother->MinimumRequiredPoints();

        for( int i = 0; i < duplicate_number; ++i )
            PushStroke( iPoint, true );

        return;
    }

    raw_stroke.Add( iPoint );

    if( bSmoothingEnabled )
    {
        smoother->AddPoint( iPoint );

        if( !smoother->IsReady() )
            return;

        interpolator->AddPoint( smoother->ComputePoint() );
    }
    else
    {
        interpolator->AddPoint( iPoint );
    }

    if( !interpolator->IsReady() )
        return;

    const  TArray< FOdysseyStrokePoint >&  tmp = interpolator->ComputePoints();
    int currentIndexBasis = result_stroke.Num();
    result_stroke.Append( tmp );

    if( !tmp.Num() )
        return;

    for( int i = currentIndexBasis; i < result_stroke.Num(); ++i )
    {
        if( i == 0 ) continue;
        FOdysseyStrokePoint& previous_point = result_stroke[i - 1];
        FOdysseyStrokePoint& current_point = result_stroke[i];
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
    for( int i = currentIndexBasis; i < result_stroke.Num(); i++ )
    {
        auto point = result_stroke[ i ];
        mDelayQueue.emplace( [this, i, point](){
            FOdysseyBrushState& state = brush_instance->GetState();
            state.point = point;
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
    bPendingEndStroke = true;
}


void
FOdysseyPaintEngine::AbortStroke()
{
    bPendingEndStroke = false;
    InterruptDelay();
    layer_stack->ComputeResultBlock();
    ::ULIS::FClearFillContext::Clear( temp_buffer->GetIBlock() );
    CLEAR_MAP( tmpInvalidTileMap );
    CLEAR_MAP( strokeInvalidTileMap );
    if( brush_instance )
    {
        brush_instance->CleansePool( ECacheLevel::kStep );
        brush_instance->CleansePool( ECacheLevel::kSubstroke );
        brush_instance->CleansePool( ECacheLevel::kStroke );
    }

    interpolator->Reset();
    smoother->Reset();
    raw_stroke.Empty();
    result_stroke.Empty();
}


void
FOdysseyPaintEngine::TriggerStateChanged()
{
    UpdateBrushInstance();
}


const  ::ULIS::CColor&
FOdysseyPaintEngine::GetColor() const
{
    return  color;
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API
void
FOdysseyPaintEngine::CheckReallocTempBuffer()
{
    if( !layer_stack )
        return;

    width = layer_stack->Width();
    height = layer_stack->Height();

    bool realloc = !temp_buffer || ( temp_buffer && temp_buffer->Size() != layer_stack->Size() );

    if( realloc )
    {
        delete  temp_buffer;
        temp_buffer = new FOdysseyBlock( width, height );
        ::ULIS::FClearFillContext::Clear( temp_buffer->GetIBlock() );
        ReallocInvalidMaps();
    }
}


void
FOdysseyPaintEngine::ReallocInvalidMaps()
{
    DEALLOC_TILE_MAP( tmpInvalidTileMap )
    DEALLOC_TILE_MAP( strokeInvalidTileMap )

    nTileX = ceil( (float)width / TILE_SIZE );
    nTileY = ceil( (float)height / TILE_SIZE );

    REALLOC_TILE_MAP( tmpInvalidTileMap )
    REALLOC_TILE_MAP( strokeInvalidTileMap )
}


void
FOdysseyPaintEngine::UpdateBrushInstance()
{
    if( !brush_instance )
        return;

    FOdysseyBrushState& state   = brush_instance->GetState();
    state.target_temp_buffer    = temp_buffer;
    state.point                 = FOdysseyStrokePoint();
    state.color                 = color;
    state.size_modifier         = size_modifier;
    state.opacity_modifier      = opacity_modifier;
    state.flow_modifier         = flow_modifier;
    state.blendingMode_modifier = mBlendingModeModifier;
    state.step                  = interpolator->GetStep();
    state.smoothing_strength    = smoother->GetStrength();
    state.currentPointIndex     = 0;
    state.currentStroke         = &result_stroke;
    brush_instance->CleansePool( ECacheLevel::kState );
}
