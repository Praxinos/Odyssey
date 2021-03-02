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
    for (int i = 0; i < mDrawingStates.Num(); i++)
    {
        delete mDrawingStates[i];
    }

    delete mSmoother;
    delete mSmoothingParameters;
    delete mInterpolator;
    delete mOriginalBlock;
    delete mPaintBlock;
    DeallocInvalidMap( mPaintBlockInvalidMap );
    DeallocInvalidMap( mEditedBlockInvalidMap );

    if( mBrushCursorPreviewSurface )
        delete  mBrushCursorPreviewSurface;
}

FOdysseyPaintEngine::FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryPtr )
    : mPaintState(kIDLE)
    , mBrush(nullptr)
    , mIsLocked(false)
    , mEditedBlock( NULL )
    , mPaintBlock(NULL)
	, mOriginalBlock(NULL)
    , mBrushInstance( NULL )
    
    , mCountTileX( 0 )
    , mCountTileY( 0 )

    , mPaintBlockInvalidMap( NULL )
    , mEditedBlockInvalidMap( NULL )

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
    , mIsAdaptativeStep( true )
    , mIsPaintOnTick( false )
    , mDrawingQueue()

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
//--------------------------------------------------------------------- Paint Engine API

bool
FOdysseyPaintEngine::PaintInitialize(ePaintState iPaintState)
{
    if (mPaintState != iPaintState)
        Flush();

    if (iPaintState == kIDLE || !mBrushInstance || !mEditedBlock || mIsLocked.Get())
        return false;

    mPaintState = iPaintState;
    UpdateOriginalBlock(true);

    ClearPaintBlock();
    ClearInvalidMap(mPaintBlockInvalidMap);
    ClearInvalidMap(mEditedBlockInvalidMap);

    mOnPaintBeginDelegate.Broadcast();

    return true;
}

bool
FOdysseyPaintEngine::PaintCheck()
{
    if (mPaintState == kIDLE || !mBrushInstance || !mEditedBlock || mIsLocked.Get())
    {
        Flush();
        return false;
    }
        
    return true;
}

void
FOdysseyPaintEngine::PaintStep()
{
    //Call paint step broadcast
    UpdateInvalidMaps();
    UpdateEditedBlock();

    TArray<::ul3::FRect> rects = GetPaintBlockInvalidTiles();
    ClearInvalidMap(mPaintBlockInvalidMap);
    mOnPaintStepDelegate.Broadcast(rects);
}

void
FOdysseyPaintEngine::PaintFinalize()
{
    if (mPaintState == kIDLE)
        return;

    PaintStep();
    TArray<::ul3::FRect> rects = GetEditedBlockInvalidTiles();
    mPaintState = kIDLE;
    mOnPaintEndDelegate.Broadcast(rects);
}

void
FOdysseyPaintEngine::PaintAbort()
{
    //Abort
    ClearPaintBlock();
    CopyInvalidMap(mEditedBlockInvalidMap, mPaintBlockInvalidMap); //Invalidate every tiles that changed
    UpdateEditedBlock();
    ClearInvalidMap(mPaintBlockInvalidMap);

    //End
    TArray<::ul3::FRect> rects = GetEditedBlockInvalidTiles();
    mPaintState = kIDLE;
    mOnPaintAbortDelegate.Broadcast(rects);
}

void
FOdysseyPaintEngine::Flush()
{
    if (mPaintState == kIDLE)
        return;
        
    if (mPaintState == kDrawingStroke)
        EndStroke();

    if (mPaintState == kDrawingTick || mPaintState == kDrawingAction)
        PaintFinalize();
}

void
FOdysseyPaintEngine::BeginStroke( const FOdysseyStrokePoint& iPoint, const FOdysseyStrokePoint& iPreviousPoint )
{
    if (!PaintInitialize(kDrawingStroke))
        return;

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
    if (!PaintCheck())
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
FOdysseyPaintEngine::EndStroke()
{
    //Ending the stroke first
    ExecuteDrawingQueue(0);
    SmoothingEndStroke();
    mBrushInstance->ExecuteStrokeEnd();

    //Ending the painting
    PaintFinalize();
    ResetStroke();
    mOnStrokeEndDelegate.Broadcast();
}

void
FOdysseyPaintEngine::AbortStroke()
{
    //Abort the stroke first
    // ClearStrokeBlock();
    // UpdateEditedBlock();

    //Abort the painting
    PaintAbort();
    ResetStroke();
    mOnStrokeAbortDelegate.Broadcast();
}



void
FOdysseyPaintEngine::Clear()
{
    ::ul3::eBlendingMode blend = mBlendingModeModifier;
    ::ul3::eAlphaMode alpha = mAlphaModeModifier;
    ::ul3::FPixelValue color = mColor;

    mBlendingModeModifier = ::ul3::BM_TOP;
    mAlphaModeModifier = ::ul3::AM_TOP;
    mColor = ::ul3::FPixelValue::FromRGBA8(0, 0, 0, 0);

    Fill();

    mBlendingModeModifier = blend;
    mAlphaModeModifier = alpha;
    mColor = color;
}

void
FOdysseyPaintEngine::Fill()
{
    // InterruptStrokeAndStampInPlace();

    if (!mEditedBlock)
        return;

    PaintInitialize(kDrawingAction);

    //Clear Edited Block
    
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::FRect rect = mPaintBlock->GetBlock()->Rect();
    ::ul3::Fill( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mPaintBlock->GetBlock(), mColor, mPaintBlock->GetBlock()->Rect() );

    UpdateInvalidMaps(rect);

    PaintStep();
    PaintFinalize();
}

void
FOdysseyPaintEngine::UpdateStrokeOptions()
{
    Flush(); //Maybe useless one day

    UpdateStrokeStep();
    UpdateStrokeAdaptative();
    UpdateStrokePaintOnTick();
    UpdateInterpolationType();
    UpdateSmoothingMethod();
    UpdateSmoothingStrength();
    UpdateSmoothingEnabled();
    UpdateSmoothingRealTime();
    UpdateSmoothingCatchUp();

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetCurrentStrokePoint(const FOdysseyStrokePoint& iPoint)
{
    if (!mBrushInstance)
        return;

    mBrushInstance->GetState().point = iPoint;
}

void
FOdysseyPaintEngine::TriggerStateChanged()
{
    Flush(); //useless one day, like the whole methode itself ?
    UpdateBrushInstance();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyPaintEngine::Block(FOdysseyBlock* iBlock)
{
    if (mEditedBlock == iBlock)
        return;

    Flush();

    mEditedBlock = iBlock;
    if (!mEditedBlock) {
		delete mPaintBlock;
        delete mOriginalBlock;
        mPaintBlock = nullptr;
        mOriginalBlock = nullptr;
		UpdateBrushInstance();
        return;
    }

    if (    !mPaintBlock   || mPaintBlock->Size()   != mEditedBlock->Size() || mPaintBlock->Format()   != mEditedBlock->Format()
         || !mOriginalBlock || mOriginalBlock->Size() != mEditedBlock->Size() || mOriginalBlock->Format() != mEditedBlock->Format() )
    {
        delete mPaintBlock;
        delete mOriginalBlock;

        mPaintBlock = new FOdysseyBlock(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());
        mOriginalBlock = new FOdysseyBlock(mEditedBlock->Width(), mEditedBlock->Height(), mEditedBlock->Format());

        ReallocInvalidMaps();
    }
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::Brush(UOdysseyBrush* iBrush)
{
    Flush();
    
	mBrushCursorInvalid = true;

    if (!iBrush)
    {
        mBrushInstance = nullptr;
        mBrush = nullptr;
        return;
    }

    mBrush = iBrush;
    UOdysseyBrushAssetBase* brushInstance = NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(), mBrush->GeneratedClass);

    BrushInstance(brushInstance, true);

    if (mBrush)
        mBrush->OnCompiled().AddRaw(this, &FOdysseyPaintEngine::OnBrushCompiled);
}

void
FOdysseyPaintEngine::IsLocked(TAttribute<bool> iIsLocked)
{
    mIsLocked = iIsLocked;
}

void
FOdysseyPaintEngine::SetColor( const ::ul3::FPixelValue& iColor )
{
    // TODO: Convert to TAttribute
    mColor = iColor; //No need for any conversion here
    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetSizeModifier( float iValue )
{
    if( iValue == 0 )
        iValue = 1;
    mSizeModifier = iValue;

    //TODO: Have Interpolator have a TAttribute for Step

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
FOdysseyPaintEngine::SetBlendingModeModifier( ::ul3::eBlendingMode iValue )
{
    mBlendingModeModifier = iValue;

    UpdateBrushInstance();
}

void
FOdysseyPaintEngine::SetAlphaModeModifier( ::ul3::eAlphaMode iValue )
{
    mAlphaModeModifier = iValue;

    UpdateBrushInstance();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyBlock*
FOdysseyPaintEngine::PaintBlock()
{
    return mPaintBlock;
}

FOdysseyBlock*
FOdysseyPaintEngine::EditedBlock()
{
    return mEditedBlock;
}

FOdysseyBlock*
FOdysseyPaintEngine::OriginalBlock()
{
    return mOriginalBlock;
}

UOdysseyBrush*
FOdysseyPaintEngine::Brush() const
{
    return mBrush;
}

UOdysseyBrushAssetBase*
FOdysseyPaintEngine::BrushInstance() const
{
    return mBrushInstance;
}

bool
FOdysseyPaintEngine::IsLocked() const
{
    return mIsLocked.Get();
}

const ::ul3::FPixelValue&
FOdysseyPaintEngine::GetColor() const
{
    return mColor;
}

float
FOdysseyPaintEngine::GetSizeModifier() const
{
    return mSizeModifier;
}

float
FOdysseyPaintEngine::GetOpacityModifier() const
{
    return mOpacityModifier;
}

float
FOdysseyPaintEngine::GetFlowModifier() const
{
    return mFlowModifier;
}

::ul3::eBlendingMode
FOdysseyPaintEngine::GetBlendingModeModifier() const
{
    return mBlendingModeModifier;
}

::ul3::eAlphaMode
FOdysseyPaintEngine::GetAlphaModeModifier() const
{
    return mAlphaModeModifier;
}

FOdysseyStrokeOptions*
FOdysseyPaintEngine::StrokeOptions()
{
    return &mStrokeOptions;
}

bool
FOdysseyPaintEngine::GetSmoothingCatchUp() const
{
    return mSmoothingParameters->GetCatchUp();
}

void
FOdysseyPaintEngine::ClearDrawingQueue()
{
    while( !mDrawingQueue.empty() )
        mDrawingQueue.pop();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- PaintEnginge Ticks

void
FOdysseyPaintEngine::Tick()
{
    if( !mBrushInstance || !mPaintBlock || mIsLocked.Get() )
        return;

    //Execute Drawing Queue
    ExecuteDrawingQueue(1000/60);

    //Execute CatchUp if needed
    SmoothingCatchUpTick();

    //Execute Tick and Update invalid maps in case of drawing in the tick event
    if (mPaintState == kIDLE)
    {
        PaintInitialize(kDrawingTick);
    }
    mBrushInstance->ExecuteTick();

    //Refresh the tiles
    PaintStep();
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
FOdysseyPaintEngine::SmoothingEndStroke()
{
    if (!mIsRealTime && mIsSmoothingEnabled)
    {
        //Cancel the tempbuffer
        ClearDrawingQueue();

        IULISLoaderModule& hULIS = IULISLoaderModule::Get();
        ::ul3::uint32 perfIntent = ULIS3_PERF_MT | ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
        ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mPaintBlock->GetBlock(), mPaintBlock->GetBlock()->Rect());

        ClearInvalidMap(mPaintBlockInvalidMap );
        CopyInvalidMap(mEditedBlockInvalidMap, mPaintBlockInvalidMap); //Invalidate every tiles that changed before applying smoothing
        ClearInvalidMap( mEditedBlockInvalidMap );

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
        ExecuteDrawingQueue(0);
    }
}

void
FOdysseyPaintEngine::ResetStroke()
{
    //Prepare blocks for the next stroke
    ClearDrawingQueue();
    // ClearStrokeBlock();
    // ClearInvalidMap( mEditedBlockInvalidMap );
    // ClearInvalidMap( mPaintBlockInvalidMap );
    
    //Reset everything else
    mInterpolator->Reset();
    mSmoother->Reset();
    mRawStroke.Empty();
    mResultStroke.Empty();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------ Drawing Queue

void
FOdysseyPaintEngine::ExecuteDrawingQueue(long long iMaxTimeMs)
{
    auto start_time = std::chrono::steady_clock::now();
    while( !mDrawingQueue.empty() )
    {
        std::function<void() >& f = mDrawingQueue.front();
        f();
        mDrawingQueue.pop();
        auto end_time = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count();
        if( iMaxTimeMs > 0 && delta > iMaxTimeMs )
            break;
    }
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Block Management

void
FOdysseyPaintEngine::ClearPaintBlock()
{
    if (!mPaintBlock)
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = /*ULIS3_PERF_MT |*/ ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    ::ul3::Clear( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, mPaintBlock->GetBlock(), mPaintBlock->GetBlock()->Rect() );
}

void
FOdysseyPaintEngine::UpdateOriginalBlock(bool iForceRefresh)
{
    if (!mEditedBlock || !mOriginalBlock)
        return;

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent = ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;

    if (iForceRefresh)
    {
        ClearInvalidMap(mEditedBlockInvalidMap);
        ::ul3::FVec2F pos( 0, 0 );

        ::ul3::Copy( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mEditedBlock->GetBlock()
                    , mOriginalBlock->GetBlock()
                    , mEditedBlock->GetBlock()->Rect()
                    , pos);
        return;
    }

    TArray<::ul3::FRect> changedTiles = GetEditedBlockInvalidTiles();
    ClearInvalidMap(mEditedBlockInvalidMap);
    if (changedTiles.Num() <= 0)
        return;

    for (int i = 0; i < changedTiles.Num(); i++)
	{    
        ::ul3::FVec2F pos( changedTiles[i].x, changedTiles[i].y );
        ::ul3::Copy( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mEditedBlock->GetBlock()
                    , mOriginalBlock->GetBlock()
                    , changedTiles[i]
                    , pos);
    }
}

void
FOdysseyPaintEngine::UpdateEditedBlock()
{
    if (!mPaintBlock || !mEditedBlock || !mOriginalBlock)
        return;

    TArray<::ul3::FRect> changedTiles = GetPaintBlockInvalidTiles();
    if (changedTiles.Num() <= 0)
        return;
    
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    uint32 perfIntent =  ULIS3_PERF_SSE42 | ULIS3_PERF_AVX2;
    for (int i = 0; i < changedTiles.Num(); i++)
	{   
        ::ul3::FVec2F pos( changedTiles[i].x, changedTiles[i].y );

        ::ul3::Copy( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mOriginalBlock->GetBlock()
                    , mEditedBlock->GetBlock()
                    , changedTiles[i]
                    , pos);     
                     
        ::ul3::Blend( hULIS.ThreadPool()
                    , ULIS3_BLOCKING
                    , perfIntent
                    , hULIS.HostDeviceInfo()
                    , ULIS3_NOCB
                    , mPaintBlock->GetBlock()
                    , mEditedBlock->GetBlock()
                    , changedTiles[i]
                    , pos
					, ULIS3_NOAA
                    , mBlendingModeModifier
                    , mAlphaModeModifier
                    , mOpacityModifier
                    );
    
        mEditedBlock->GetBlock()->Invalidate(changedTiles[i]); //can be optimized if we can invalidate an array of rect
    }
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Listeners

void
FOdysseyPaintEngine::OnBrushCompiled( UBlueprint* iBrush )
{
    UOdysseyBrush* brush = dynamic_cast<UOdysseyBrush*>( iBrush );
    if( !brush )
        return;

    BrushInstance(NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(), mBrush->GeneratedClass), false);
}

void
FOdysseyPaintEngine::UpdateBrushInstance()
{
    if( !mBrushInstance )
        return;

    mBrushCursorInvalid = true;

    FOdysseyBrushState& state = mBrushInstance->GetState();
    state.target_temp_buffer = mPaintBlock;
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

    //TODO: Call On Tick, so we can have a dynamic state and attributes everywhere
    mBrushInstance->ExecuteStateChanged();
}

void
FOdysseyPaintEngine::BrushInstance(UOdysseyBrushAssetBase* iBrushInstance, bool iApplyOverrides)
{
    mBrushInstance = iBrushInstance;

    //Set all the overrides
    if (iApplyOverrides)
    {
        FOdysseyBrushPreferencesOverrides& overrides = mBrushInstance->Preferences;
        if (overrides.bOverride_Step)          mStrokeOptions.Step = overrides.Step;
        if (overrides.bOverride_Adaptative)    mStrokeOptions.SizeAdaptative = overrides.SizeAdaptative;
        if (overrides.bOverride_PaintOnTick)   mStrokeOptions.PaintOnTick = overrides.PaintOnTick;
        if (overrides.bOverride_Type)          mStrokeOptions.Type = overrides.Type;
        if (overrides.bOverride_Method)        mStrokeOptions.Method = overrides.Method;
        if (overrides.bOverride_Strength)      mStrokeOptions.Strength = overrides.Strength;
        if (overrides.bOverride_Enabled)       mStrokeOptions.Enabled = overrides.Enabled;
        if (overrides.bOverride_RealTime)      mStrokeOptions.RealTime = overrides.RealTime;
        if (overrides.bOverride_CatchUp)       mStrokeOptions.CatchUp = overrides.CatchUp;
        if (overrides.bOverride_Size)          mSizeModifier = overrides.Size;
        if (overrides.bOverride_Opacity)       mOpacityModifier = overrides.Opacity;
        if (overrides.bOverride_Flow)          mFlowModifier = overrides.Flow;
        if (overrides.bOverride_BlendingMode)  mBlendingModeModifier = (::ul3::eBlendingMode)overrides.BlendingMode;
        if (overrides.bOverride_AlphaMode)     mAlphaModeModifier = (::ul3::eAlphaMode)overrides.AlphaMode;
    }

    //we need to do this before iBrushInstance->ExecuteSelected();
	//and we cannot use UpdateBrushInstance() as it sends a StateChanged, and we only want this after iBrushInstance->ExecuteSelected();
	FOdysseyBrushState& state = mBrushInstance->GetState();
	state.target_temp_buffer = mPaintBlock;
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

    mBrushInstance->ExecuteSelected();

    if (iApplyOverrides)
    {
        UpdateStrokeOptions(); //calls UpdateBrushInstance
    }
    else
    {
        UpdateBrushInstance();
    }

    for (int i = 0; i < mDrawingStates.Num(); i++)
    {
        mBrushInstance->AddOrReplaceState(mDrawingStates[i]->Id(), mDrawingStates[i]);
    }
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------- Tiles Management

void
FOdysseyPaintEngine::ReallocInvalidMaps()
{
    DeallocInvalidMap(mPaintBlockInvalidMap );
    DeallocInvalidMap( mEditedBlockInvalidMap );

    mCountTileX = mEditedBlock ? ceil( (float)mEditedBlock->Width() / TILE_SIZE ) : 0;
    mCountTileY = mEditedBlock ? ceil( (float)mEditedBlock->Height() / TILE_SIZE ) : 0;

    AllocInvalidMap(mPaintBlockInvalidMap );
    AllocInvalidMap( mEditedBlockInvalidMap );
}

void
FOdysseyPaintEngine::UpdateInvalidMaps()
{
    auto invalid_rects = mBrushInstance->GetInvalidRects();
    for( int j = 0; j < invalid_rects.Num(); ++j )
    {
        UpdateInvalidMaps(invalid_rects[j]);
    }
    mBrushInstance->ClearInvalidRects();
}

void
FOdysseyPaintEngine::UpdateInvalidMaps(::ul3::FRect iRect)
{
    float xf = FMath::Max(0.f, float(iRect.x) / TILE_SIZE);
    float yf = FMath::Max(0.f, float(iRect.y) / TILE_SIZE);
    float wf = float(iRect.w) / TILE_SIZE;
    float hf = float(iRect.h) / TILE_SIZE;
    int x = xf;
    int y = yf;
    int w = FMath::Min(mCountTileX, int(ceil(xf + wf))) - x;
    int h = FMath::Min(mCountTileY, int(ceil(yf + hf))) - y;
    ::ul3::FRect tileRect = { x, y, w, h };
    SetMapWithRect(mPaintBlockInvalidMap, tileRect, true);
    SetMapWithRect(mEditedBlockInvalidMap, tileRect, true);
}

TArray<::ul3::FRect>
FOdysseyPaintEngine::GetPaintBlockInvalidTiles()
{
    TArray<::ul3::FRect> tiles;
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            if(mPaintBlockInvalidMap[k][l] )
            {
                ::ul3::FRect rect = MakeTileRect(l, k);
                tiles.Add(rect);
            }
        }
    }
    return tiles;
}

TArray<::ul3::FRect>
FOdysseyPaintEngine::GetEditedBlockInvalidTiles()
{
    TArray<::ul3::FRect> tiles;
    for( int k = 0; k < mCountTileY; ++k )
    {
        for( int l = 0; l < mCountTileX; ++l )
        {
            if (!mEditedBlockInvalidMap[k][l])
                continue;
			tiles.Add(MakeTileRect(l, k));
        }
    }
    return tiles;
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

void
FOdysseyPaintEngine::DeallocInvalidMap( InvalidTileMap& ioMap )
{
    if( !ioMap )
        return;

    for( int i = 0; i < mCountTileY; ++i )
        delete[] ioMap[i];
    delete[] ioMap;
    ioMap = 0;
}

void
FOdysseyPaintEngine::AllocInvalidMap( InvalidTileMap& ioMap )
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
FOdysseyPaintEngine::ClearInvalidMap( InvalidTileMap ioMap )
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
FOdysseyPaintEngine::CopyInvalidMap( InvalidTileMap iSrcMap, InvalidTileMap ioDstMap )
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


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Stroke Helpers

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
        mDrawingQueue.emplace( [this, i, point = mResultStroke[i], currentIndexBasis]()
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
        } );
    }

    if (currentIndexBasis < mResultStroke.Num())
    {
        mDrawingQueue.emplace( [this, i = mResultStroke.Num() - 1, point = mResultStroke.Last()]()
        {
            FOdysseyBrushState& state = mBrushInstance->GetState();
            state.point = point;
            state.currentPointIndex = i;
            mBrushInstance->ExecuteSubStrokeEnd();
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------ Stroke Options Management

void
FOdysseyPaintEngine::UpdateStrokeStep()
{
    mStepValue = mStrokeOptions.Step;
    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );
}

void
FOdysseyPaintEngine::UpdateStrokeAdaptative()
{
    mIsAdaptativeStep = mStrokeOptions.SizeAdaptative;
    float val = FMath::Max( 1.f, mIsAdaptativeStep ? ( mStepValue / 100.f ) * mSizeModifier : (float)mStepValue );
    mInterpolator->SetStep( val );
}

void
FOdysseyPaintEngine::UpdateStrokePaintOnTick()
{
    mIsPaintOnTick = mStrokeOptions.PaintOnTick;
}

void
FOdysseyPaintEngine::UpdateInterpolationType()
{
    switch( mStrokeOptions.Type )
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
}

void
FOdysseyPaintEngine::UpdateSmoothingMethod()
{
    switch(mStrokeOptions.Method)
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
FOdysseyPaintEngine::UpdateSmoothingStrength()
{
    mSmoothingParameters->SetStrength(mStrokeOptions.Strength);
}

void
FOdysseyPaintEngine::UpdateSmoothingEnabled()
{
    mIsSmoothingEnabled = mStrokeOptions.Enabled;
}

void
FOdysseyPaintEngine::UpdateSmoothingRealTime()
{
    mIsRealTime = mStrokeOptions.RealTime;
}

void
FOdysseyPaintEngine::UpdateSmoothingCatchUp()
{
    mSmoothingParameters->SetCatchUp(mStrokeOptions.CatchUp);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Private API


/* void
FOdysseyPaintEngine::UpdateBrushCursorPreview()
{
    // CURRENTLY DISABLED !
    return; */

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

    // Reset brush state target to mPaintBlock
    state.target_temp_buffer = mPaintBlock;

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
//}

void
FOdysseyPaintEngine::AddDrawingState(FOdysseyDrawingState* iDrawingState)
{
    mDrawingStates.Add(iDrawingState);
    UpdateBrushInstance();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ FGCObject interface

void
FOdysseyPaintEngine::AddReferencedObjects(FReferenceCollector& Collector)
{
    if (mBrush)
        Collector.AddReferencedObject(mBrush);

    if (mBrushInstance)
        Collector.AddReferencedObject(mBrushInstance);
}

FString
FOdysseyPaintEngine::GetReferencerName() const
{
	return TEXT("FOdysseyPaintEngine");
}
