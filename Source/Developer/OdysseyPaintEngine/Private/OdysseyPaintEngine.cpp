// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
    delete mSmoothingParameters;
    delete mInterpolator;
    delete mPreviewBlock;
    delete mStrokeBlock;
    DeallocInvalidTileMap( mTmpInvalidTileMap );
    DeallocInvalidTileMap( mStrokeInvalidTileMap );

    if( mBrushCursorPreviewSurface )
        delete  mBrushCursorPreviewSurface;
}

FOdysseyPaintEngine::FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryPtr )
    : mIsLocked(false)
    , mEditedBlock( NULL )
    , mStrokeBlock(NULL)
	, mPreviewBlock(NULL)
    , mBrushInstance( NULL )
    
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
    , mSmoothingParameters( NULL )
    , mSmoother( NULL )

    , mIsSmoothingEnabled( true )
    , mIsRealTime( true )
    // , mIsCatchUp( true )
    , mIsAdaptativeStep( true )
    , mIsPaintOnTick( false )
    , mIsPendingEndStroke( false )
    , mDelayQueue()

    , mBrushCursorPreviewSurface( nullptr )
    , mBrushCursorPreviewShift( FVector2D(0,0) )
    , mLastBrushCursorComputationTime( 0 )
    , mBrushCursorInvalid( true )
{
    mSmoothingParameters = new FOdysseySmoothingParameters();
	mSmoother = new FOdysseySmoothingAverage(mSmoothingParameters);
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
FOdysseyPaintEngine::SmoothingCatchUpTick()
{
    if (mIsSmoothingEnabled && mSmoothingParameters->GetCatchUp() && mSmoother->CanCatchUp() && mRawStroke.Num() > 0)
    {
		long long max_time = 1000 / 60;
		auto end_time = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - mLastStrokeTimePoint).count();
        if( delta > max_time )
        {
			FOdysseyStrokePoint newPoint = mRawStroke[mRawStroke.Num() - 1];
            PushStroke(newPoint);
        }
    }
}

void
FOdysseyPaintEngine::ExecuteDelayQueue()
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
}

TArray<::ul3::FRect>
FOdysseyPaintEngine::GetTmpInvalidTiles()
{
    TArray<::ul3::FRect> tiles;
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            if( mTmpInvalidTileMap[k][l] )
            {
                ::ul3::FRect rect = MakeTileRect(l, k);
                ::ul3::FVec2F pos(rect.x, rect.y);
                ::ul3::FVec2I posi(pos.x, pos.y);
                tiles.Add(rect);
            }
        }
    }
    return tiles;
}

TArray<::ul3::FRect>
FOdysseyPaintEngine::GetStrokeInvalidTiles()
{
    TArray<::ul3::FRect> tiles;
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            if (!mStrokeInvalidTileMap[k][l])
                continue;
			tiles.Add(MakeTileRect(l, k));
        }
    }
    return tiles;
}

void
FOdysseyPaintEngine::ClearStrokeBlock()
{
    if (!mStrokeBlock)
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mStrokeBlock->GetBlock(), mStrokeBlock->GetBlock()->Rect() );
}

void
FOdysseyPaintEngine::ResetStroke()
{
    mInterpolator->Reset();
    mSmoother->Reset();
    mRawStroke.Empty();
    mResultStroke.Empty();
    mIsPendingEndStroke = false;
}

void
FOdysseyPaintEngine::UpdateInvalidMaps()
{
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
}

void
FOdysseyPaintEngine::EndStrokeTick()
{
    SmoothingEndStroke();

    mBrushInstance->ExecuteStrokeEnd();
    mOnStrokeEndDelegate.Broadcast();
    
    // UpdateInvalidMaps();
    // mBrushInstance->ClearInvalidRects();

    //Refresh Preview Block
    UpdatePreviewBlockTiles();

    //Refresh Edited Block
    Flush();
}

void
FOdysseyPaintEngine::SmoothingEndStroke()
{
    if (!mIsRealTime && mIsSmoothingEnabled)
    {
        //Cancel the tempbuffer   
        if( !mStrokeBlock || !mPreviewBlock )
            return;

        InterruptDelay();

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mStrokeBlock->GetBlock(), mStrokeBlock->GetBlock()->Rect());

        ClearInvalidTileMap( mTmpInvalidTileMap );
        CopyInvalidTileMap(mStrokeInvalidTileMap, mTmpInvalidTileMap);
        ClearInvalidTileMap( mStrokeInvalidTileMap );

        mInterpolator->Reset();
        mSmoother->Reset();
        mResultStroke.Empty();

        if (mRawStroke.Num() <= 0)
            return;

        //BeginStroke
        mSmoother->AddPoint(mRawStroke[0]);
        
        for (int i = 0; i < mInterpolator->MinimumRequiredPoints(); i++)
        {
            mInterpolator->AddPoint( mRawStroke[i] );
        }

        TArray< FOdysseyStrokePoint > firstPoints;
        firstPoints.Add(mRawStroke[0]);
        AddResultPoints(firstPoints);

        //Steps
        for (int i = 1; i < mRawStroke.Num(); i++)
        {
            mSmoother->AddPoint(mRawStroke[i]);

            if( !mSmoother->IsReady() )
                continue;

            mInterpolator->AddPoint( mSmoother->ComputePoint() );

			if (!mInterpolator->IsReady())
				continue;

            TArray< FOdysseyStrokePoint > points = ComputeInterpolation();
            AddResultPoints(points);
        }

        //Execute everything in the queue at once to avoid glitches
        while( !mDelayQueue.empty() )
        {
            std::function<void() >& f = mDelayQueue.front();
            f();
            mDelayQueue.pop();
        }
    }
}

void
FOdysseyPaintEngine::Flush()
{
    UpdateEditedBlockTiles();
    ClearStrokeBlock();
    ResetStroke();
}

void
FOdysseyPaintEngine::UpdatePreviewBlockTiles()
{
    UpdateInvalidMaps();
    mBrushInstance->ClearInvalidRects();

    TArray<::ul3::FRect> changedTiles = GetTmpInvalidTiles();
    if (changedTiles.Num() > 0)
    {
        CopyEditedBlockInPreviewBlock(changedTiles);
        BlendStrokeBlockInPreviewBlock(changedTiles);
        mOnPreviewBlockTilesChangedDelegate.Broadcast(changedTiles);
    }
    ClearInvalidTileMap(mTmpInvalidTileMap);
}

void
FOdysseyPaintEngine::UpdateEditedBlockTiles()
{
    TArray<::ul3::FRect> changedTiles = GetStrokeInvalidTiles();
    if (changedTiles.Num() > 0)
    {
        mOnEditedBlockTilesWillChangeDelegate.Broadcast(changedTiles);
        CopyPreviewBlockInEditedBlock(changedTiles);
        mOnEditedBlockTilesChangedDelegate.Broadcast(changedTiles);
    }
    ClearInvalidTileMap(mStrokeInvalidTileMap);
}

void
FOdysseyPaintEngine::Tick()
{
    if( !mBrushInstance || !mStrokeBlock || !mPreviewBlock || mIsLocked )
        return;

    ExecuteDelayQueue();

    //Execute Tick and Update invalid maps in case of drawing in the tick event
    mBrushInstance->ExecuteTick();
    
    // UpdateInvalidMaps();
    // mBrushInstance->ClearInvalidRects();

    //Refresh the tiles
    UpdatePreviewBlockTiles();
    
    //End the stroke if we need to
    if( mIsPendingEndStroke && mDelayQueue.empty() )
    {
        EndStrokeTick();
    }
}

void
FOdysseyPaintEngine::Block(FOdysseyBlock* iBlock)
{
    if (mEditedBlock == iBlock)
        return;

    Flush();

    mEditedBlock = iBlock;
    if (!mEditedBlock) {
		delete mStrokeBlock;
        delete mPreviewBlock;
        mStrokeBlock = nullptr;
        mPreviewBlock = nullptr;
		UpdateBrushInstance();
        return;
    }

    if (    !mStrokeBlock
         || !mPreviewBlock
         || mStrokeBlock->Size() != mEditedBlock->Size()
         || mPreviewBlock->Size() != mEditedBlock->Size()
         || mStrokeBlock->Format() != mEditedBlock->Format()
         || mPreviewBlock->Format() != mEditedBlock->Format() )
    {
        delete mStrokeBlock;
        delete mPreviewBlock;
        mStrokeBlock = new FOdysseyBlock(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        mPreviewBlock = new FOdysseyBlock(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

        ::ul3::FVec2F pos( 0, 0 );
        ::ul3::Clear(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mStrokeBlock->GetBlock(), mStrokeBlock->GetBlock()->Rect());
        ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mEditedBlock->GetBlock(), mPreviewBlock->GetBlock(), mPreviewBlock->GetBlock()->Rect(), pos);

        ReallocInvalidMaps();
    }
    UpdateBrushInstance();
}

FOdysseyBlock*
FOdysseyPaintEngine::StrokeBlock()
{
    return mStrokeBlock;
}

FOdysseyBlock*
FOdysseyPaintEngine::PreviewBlock()
{
    return mPreviewBlock;
}

void
FOdysseyPaintEngine::SetLock(bool iValue)
{
    mIsLocked = iValue;
}

void
FOdysseyPaintEngine::SetBrushInstance( UOdysseyBrushAssetBase* iBrushInstance )
{
    if (iBrushInstance == mBrushInstance)
        return;

    Flush();

	mBrushInstance = iBrushInstance;
	mBrushCursorInvalid = true;

    if (!mBrushInstance)
        return;

	//we need to do this before iBrushInstance->ExecuteSelected();
	//and we cannot use UpdateBrushInstance() as it sends a StateChanged, and we only want this after iBrushInstance->ExecuteSelected();
	FOdysseyBrushState& state = mBrushInstance->GetState();
	state.target_temp_buffer = mStrokeBlock;
	state.point = FOdysseyStrokePoint();
	state.color = mColor;
	state.size_modifier = mSizeModifier;
	state.opacity_modifier = mOpacityModifier;
	state.flow_modifier = mFlowModifier;
	state.blendingMode_modifier = mBlendingModeModifier;
	state.alphaMode_modifier = mAlphaModeModifier;
	state.step = mInterpolator->GetStep();
	state.smoothing_strength = mSmoothingParameters->GetStrength();
	state.currentPointIndex = 0;
	state.currentStroke = &mResultStroke;

    iBrushInstance->ExecuteSelected();
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetColor( const ::ul3::FPixelValue& iColor )
{
    InterruptStrokeAndStampInPlace();
    mColor = iColor; //No need for any conversion here
    //::ul3::Conv( iColor, mColor );
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
            if (mSmoother) delete mSmoother;
            mSmoother = new FOdysseySmoothingAverage(mSmoothingParameters);
            break;
        }

        /* case EOdysseySmoothingMethod::kGravity:
        {
            if (mSmoother) delete mSmoother;
            mSmoother = new FOdysseySmoothingAverage(mSmoothingParameters);
            break;
        } */

        case EOdysseySmoothingMethod::kPull:
        {
            if (mSmoother) delete mSmoother;
            mSmoother = new FOdysseySmoothingPull(mSmoothingParameters);
            break;
        }
    }
}

void
FOdysseyPaintEngine::SetSmoothingStrength( int32 iValue )
{
    InterruptStrokeAndStampInPlace();

    mSmoothingParameters->SetStrength( iValue );

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

    mSmoothingParameters->SetCatchUp( iValue );

    //mIsCatchUp = iValue;
}

bool
FOdysseyPaintEngine::GetStokePaintOnTick() const
{
    return mIsPaintOnTick;
}

bool
FOdysseyPaintEngine::GetSmoothingCatchUp() const
{
    return mSmoothingParameters->GetCatchUp();
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
FOdysseyPaintEngine::BeginStroke( const FOdysseyStrokePoint& iPoint, const FOdysseyStrokePoint& iPreviousPoint )
{
    if( !mBrushInstance ||
        !mStrokeBlock ||
        !mPreviewBlock ||
        mIsPendingEndStroke ||
        mIsLocked )
        return;

	//TODO: Maybe find another way to keep the preview block uptodate, because this is potentially heavy
    // But for now it's only in the begin stroke, so it should be ok
    Flush();
    CopyEditedBlockInPreviewBlock();

    mLastStrokeTimePoint = std::chrono::steady_clock::now();

    mRawStroke.Add( iPoint );

    if( mIsSmoothingEnabled && mIsRealTime )
    {
        mSmoother->AddPoint( iPoint );
    }
    
    for (int i = 0; i < mInterpolator->MinimumRequiredPoints(); i++)
    {
        mInterpolator->AddPoint( iPoint );
    }

    FOdysseyStrokePoint point = iPoint;
    ComputePointRelativeParameters(point, iPreviousPoint);

    TArray<FOdysseyStrokePoint> points;
    points.Add(point);

    AddResultPoints(points);
}

void
FOdysseyPaintEngine::PushStroke( const FOdysseyStrokePoint& iPoint )
{
    if( !mBrushInstance ||
        !mStrokeBlock ||
        !mPreviewBlock ||
        mIsPendingEndStroke ||
        mIsLocked )
        return;

    mLastStrokeTimePoint = std::chrono::steady_clock::now();
    mRawStroke.Add( iPoint );

    if( mIsSmoothingEnabled && mIsRealTime )
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

    TArray< FOdysseyStrokePoint > points = ComputeInterpolation();
    AddResultPoints(points);
}

void
FOdysseyPaintEngine::SetCurrentStrokePoint(const FOdysseyStrokePoint& iPoint)
{
    if (!mBrushInstance)
        return;
    mBrushInstance->GetState().point = iPoint;
}

void
FOdysseyPaintEngine::ComputePointRelativeParameters(FOdysseyStrokePoint& ioPoint, const FOdysseyStrokePoint& iPreviousPoint)
{
    ioPoint.deltaPosition = FVector2D( ioPoint.x - iPreviousPoint.x, ioPoint.y - iPreviousPoint.y );
    ioPoint.deltaTime = FMath::Max(ioPoint.time, iPreviousPoint.time) - FMath::Min(ioPoint.time, iPreviousPoint.time);
    if( ioPoint.deltaTime == 0) ioPoint.deltaTime++;

    ioPoint.speed = ioPoint.deltaTime != 0 ? ioPoint.deltaPosition / ioPoint.deltaTime : FVector2D(0,0);
    ioPoint.acceleration = ioPoint.speed - iPreviousPoint.speed;
    ioPoint.jolt = ioPoint.acceleration - iPreviousPoint.acceleration;

    ioPoint.direction_angle_deg_tangent = atan2( ioPoint.y - iPreviousPoint.y, ioPoint.x - iPreviousPoint.x ) * 180.f / 3.14159265359f;
    ioPoint.direction_angle_deg_normal = ioPoint.direction_angle_deg_tangent + 90;
    ioPoint.direction_vector_tangent = ioPoint.deltaPosition.GetSafeNormal();
    ioPoint.direction_vector_normal = FVector2D( -ioPoint.direction_vector_tangent.Y, ioPoint.direction_vector_tangent.X );
    ioPoint.distance_travelled = iPreviousPoint.distance_travelled + ioPoint.deltaPosition.Size();
}

void
FOdysseyPaintEngine::AddResultPoints(const TArray< FOdysseyStrokePoint >& iPoints)
{
    int currentIndexBasis = mResultStroke.Num();
    mResultStroke.Append( iPoints );
    for( int i = currentIndexBasis; i < mResultStroke.Num(); i++ )
    {
        mDelayQueue.emplace( [this, i, point = mResultStroke[i], currentIndexBasis]()
        {
            FOdysseyBrushState& state = mBrushInstance->GetState();
            state.point = point;
            state.currentPointIndex = i;
            if( i == 0 )
            {
                mBrushInstance->ExecuteStrokeBegin();
                mOnStrokeBeginDelegate.Broadcast();
            }

            if( i == currentIndexBasis )
                mBrushInstance->ExecuteSubStrokeBegin();

            mBrushInstance->ExecuteStep();
            mOnStrokeStepDelegate.Broadcast();

            // UpdateInvalidMaps();
            // mBrushInstance->ClearInvalidRects();
        } );
    }

    if (currentIndexBasis < mResultStroke.Num())
    {
        mDelayQueue.emplace( [this, i = mResultStroke.Num() - 1, point = mResultStroke.Last()]()
        {
            FOdysseyBrushState& state = mBrushInstance->GetState();
            state.point = point;
            state.currentPointIndex = i;
            mBrushInstance->ExecuteSubStrokeEnd();
            // UpdateInvalidMaps();
            // mBrushInstance->ClearInvalidRects();
        } );
    }
}

TArray< FOdysseyStrokePoint >
FOdysseyPaintEngine::ComputeInterpolation()
{
    TArray< FOdysseyStrokePoint > tmp = mInterpolator->ComputePoints();

	if (tmp.Num() > 0 && mResultStroke.Num() > 0)
	{
		FOdysseyStrokePoint& previous_point = mResultStroke[mResultStroke.Num() - 1];
		FOdysseyStrokePoint& current_point = tmp[0];

		ComputePointRelativeParameters(current_point, previous_point);
	}

    for( int i =  1; i < tmp.Num(); ++i )
    {
        FOdysseyStrokePoint& previous_point = tmp[i - 1];
        FOdysseyStrokePoint& current_point = tmp[i];
        
        ComputePointRelativeParameters(current_point, previous_point);
    }

    return tmp;
}

void
FOdysseyPaintEngine::EndStroke()
{
    if( !mBrushInstance ||
        !mStrokeBlock ||
        !mPreviewBlock ||
        mIsPendingEndStroke ||
        mIsLocked )
        return;
        
    mIsPendingEndStroke = true;
}

void
FOdysseyPaintEngine::AbortStroke()
{
    if( !mStrokeBlock || !mPreviewBlock || mIsLocked )
        return;

    mIsPendingEndStroke = false;
    InterruptDelay();
    mOnStrokeAbortDelegate.Broadcast();

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mStrokeBlock->GetBlock(), mStrokeBlock->GetBlock()->Rect());

    ClearInvalidTileMap( mTmpInvalidTileMap );
    ClearInvalidTileMap( mStrokeInvalidTileMap );

    mInterpolator->Reset();
    mSmoother->Reset();
    mRawStroke.Empty();
    mResultStroke.Empty();
}


void
FOdysseyPaintEngine::BlendStrokeBlockInPreviewBlock(TArray<::ul3::FRect>& iRects)
{
    if (!mStrokeBlock || !mPreviewBlock)
        return;

    if (iRects.Num() <= 0)
        return;
    
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent =  ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    for (int i = 0; i < iRects.Num(); i++)
	{    
        ::ul3::FVec2F pos( iRects[i].x, iRects[i].y );
        ::ul3::Blend( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mStrokeBlock->GetBlock()
                    , mPreviewBlock->GetBlock()
                    , iRects[i]
                    , pos
					, ULIS3_NOAA
                    , mBlendingModeModifier
                    , mAlphaModeModifier
                    , mOpacityModifier
                    );
    }
}


void
FOdysseyPaintEngine::CopyPreviewBlockInEditedBlock(TArray<::ul3::FRect>& iRects)
{
    if (!mEditedBlock || !mPreviewBlock)
        return;

    if (iRects.Num() <= 0)
        return;
    
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent =  ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    for (int i = 0; i < iRects.Num(); i++)
	{    
        ::ul3::FVec2F pos( iRects[i].x, iRects[i].y );
        ::ul3::Copy( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mPreviewBlock->GetBlock()
                    , mEditedBlock->GetBlock()
                    , iRects[i]
                    , pos);               
    }
}


void
FOdysseyPaintEngine::CopyEditedBlockInPreviewBlock(TArray<::ul3::FRect>& iRects)
{
    if (!mEditedBlock || !mPreviewBlock)
        return;

    if (iRects.Num() <= 0)
        return;
    
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    for (int i = 0; i < iRects.Num(); i++)
	{    
        ::ul3::FVec2F pos( iRects[i].x, iRects[i].y );
        ::ul3::Copy( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mEditedBlock->GetBlock()
                    , mPreviewBlock->GetBlock()
                    , iRects[i]
                    , pos);               
    }
}

void
FOdysseyPaintEngine::CopyEditedBlockInPreviewBlock()
{
	if (!mEditedBlock || !mPreviewBlock)
		return;

	IULISLoaderModule& hULIS = IULISLoaderModule::Get();
	uint32 perfIntent = ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
	::ul3::FRect rect(0, 0, mEditedBlock->Width(), mEditedBlock->Height());

	::ul3::FVec2F pos(0, 0);
	::ul3::Copy(hULIS.ThreadPool()
		, ULIS3_BLOCKING
		, perfIntent
		, hULIS.HostDeviceInfo()
		, ULIS3_NOCB
		, mEditedBlock->GetBlock()
		, mPreviewBlock->GetBlock()
		, rect
		, pos);
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

    Flush();
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

void
FOdysseyPaintEngine::ReallocInvalidMaps()
{
    DeallocInvalidTileMap( mTmpInvalidTileMap );
    DeallocInvalidTileMap( mStrokeInvalidTileMap );

    mCountTileX = mEditedBlock ? ceil( (float)mEditedBlock->Width() / TILE_SIZE ) : 0;
    mCountTileY = mEditedBlock ? ceil( (float)mEditedBlock->Height() / TILE_SIZE ) : 0;

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
    state.target_temp_buffer = mStrokeBlock;
    state.point = FOdysseyStrokePoint();
    state.color = mColor;
    state.size_modifier = mSizeModifier;
    state.opacity_modifier = mOpacityModifier;
    state.flow_modifier = mFlowModifier;
    state.blendingMode_modifier = mBlendingModeModifier;
    state.alphaMode_modifier = mAlphaModeModifier;
    state.step = mInterpolator->GetStep();
    state.smoothing_strength = mSmoothingParameters->GetStrength();
    state.currentPointIndex = 0;
    state.currentStroke = &mResultStroke;

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
    FOdysseyBlock* preview_color = new FOdysseyBlock( preview_w, preview_h, ULISFormatForUE4TextureSourceFormat(mTextureSourceFormat), nullptr, nullptr, true );

    // Set the preview_color block as target for brush
    state.target_temp_buffer = preview_color;
    state.point.x = -xmin;
    state.point.y = -ymin;

    // Execute Brush into preview_color target
    mBrushInstance->ExecuteStep();

    // Clear invalid rects in brush instance
    mBrushInstance->ClearInvalidRects();

    // Reset brush state target to mStrokeBlock
    state.target_temp_buffer = mStrokeBlock;

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
    mBrushCursorPreviewSurface = new FOdysseySurface( preview_w, preview_h, ULISFormatForUE4TextureSourceFormat(mTextureSourceFormat) );

    // Compute Outline in surface
    FOdysseyBlock* preview_outline = new FOdysseyBlock( preview_w, preview_h, ULISFormatForUE4TextureSourceFormat(mTextureSourceFormat) );
    FOdysseyBlock* preview_shadow = new FOdysseyBlock( preview_w, preview_h, ULISFormatForUE4TextureSourceFormat(mTextureSourceFormat) );
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

void
FOdysseyPaintEngine::CopyInvalidTileMap( InvalidTileMap iSrcMap, InvalidTileMap ioDstMap )
{
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            ioDstMap[k][l] = iSrcMap[k][l];
        }
    }
}

::ul3::FRect
FOdysseyPaintEngine::MakeTileRect( int iTileX, int iTileY )
{
    return { iTileX * TILE_SIZE,
             iTileY * TILE_SIZE,
             mEditedBlock ? FMath::Min( iTileX * TILE_SIZE + TILE_SIZE, mEditedBlock->Width() ) - iTileX * TILE_SIZE : 0,
             mEditedBlock ? FMath::Min( iTileY * TILE_SIZE + TILE_SIZE, mEditedBlock->Height() ) - iTileY * TILE_SIZE : 0 } ;
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
