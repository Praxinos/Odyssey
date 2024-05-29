// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "FreehandShape/OdysseyFreehandShape.h"

#include "FreehandShape/Smoothing/OdysseySmoothingAverage.h"
#include "FreehandShape/Smoothing/OdysseySmoothingPull.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"
#include "OdysseyHUDPolygon.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyFreehandShape::~UOdysseyFreehandShape()
{
}

UOdysseyFreehandShape::UOdysseyFreehandShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Properties
    , SmoothingEnabled(false)
    , SmoothingOptions()

    //Internal
    , mRawStroke()
    , mSmoothedStroke()
    , mInterpolatedStroke()
    , mSmoother(nullptr)
    , mHasStrokeBegun(false)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyFreehandShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return BeginStroke(iPointInTexture);
}

bool
UOdysseyFreehandShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return EndStroke();
}

void
UOdysseyFreehandShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyFreehandShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    StrokeTo(iPointInTexture);
    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyFreehandShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
        return AbortShape();

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyFreehandShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Stroke API

bool
UOdysseyFreehandShape::BeginStroke( const FOdysseyPoint& iPoint )
{
    if (mHasStrokeBegun)
        return false;

    mHasStrokeBegun = true;

    //Prepare Raw Stroke Array
    mRawStroke.Empty();

    //Store the first raw point
    mRawStroke.Add( iPoint );

    InitSmoothing();
    InitInterpolation();

    BeginSmoothing();
    BeginInterpolation();
    
    if (mDisplayHUD)
    {
        mPathHUD = MakeShared<FOdysseyHUDPolygon>();
        mPathHUD->GetPoints().Add(iPoint);
        mHUD->AddElement(mPathHUD);
    }

    //Force first point to be drawn
    mOnPathBeginDelegate.Broadcast(iPoint);
    
    return true;
}

bool
UOdysseyFreehandShape::StrokeTo( const FOdysseyPoint& iPoint )
{
    if (!mHasStrokeBegun)
        return false;

    //Add the raw point to the array
    //Use a temporary variable to ensure we are not adding a point that is already contained (see Add())
    FOdysseyPoint point = iPoint;
    mRawStroke.Add(point);

    //Apply smoothing if smoothing is enabled and realtime
    if(SmoothingEnabled && SmoothingOptions.SmoothingRealTime)
    {
        if (!SmoothTo(iPoint)) //false means SmoothTo has just not produced any point but it is not an error
            return true;

        point = mSmoothedStroke.Last();
    }

    //Apply interpolation
    TArray<FOdysseyPoint> interpolatedPoints = InterpolateTo(point);

    if (interpolatedPoints.Num() > 0)
    {
        if (mPathHUD)
            mPathHUD->GetPoints().Append(interpolatedPoints);
            
        mOnPathToDelegate.Broadcast(interpolatedPoints);
    }
        
    return true;
}

bool
UOdysseyFreehandShape::EndStroke()
{
    if (!mHasStrokeBegun)
        return false;

    EndSmoothing();
    EndInterpolation();

    mHUD->EmptyElements();
    mPathHUD = nullptr;
    mOnPathEndDelegate.Broadcast({ mInterpolatedStroke.Last() });
    mHasStrokeBegun = false;

    return true;
}

bool
UOdysseyFreehandShape::AbortShape()
{
    if (!mHasStrokeBegun)
        return false;

    AbortSmoothing();
    AbortInterpolation();

    mHUD->EmptyElements();
    mPathHUD = nullptr;

    mHasStrokeBegun = false;
    
    UOdysseyShape::AbortShape();

    return true;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Tick API
    
void
UOdysseyFreehandShape::Tick(float iDeltaTime)
{
    if (mHasStrokeBegun)
        CatchUp();
}

void
UOdysseyFreehandShape::ApplyOverrides(const TMap<TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides)
{
    const UOdysseyFreehandShapeOverrides* freehandShapeOverrides = Cast<const UOdysseyFreehandShapeOverrides>(iOverrides[UOdysseyFreehandShapeOverrides::StaticClass()]);
    if (freehandShapeOverrides)
        freehandShapeOverrides->Override(this);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseySmoothingOptions&
UOdysseyFreehandShape::GetSmoothingOptions()
{
    return SmoothingOptions;
}

void
UOdysseyFreehandShape::DisplayHUD(bool iDisplayHUD)
{
    mDisplayHUD = iDisplayHUD;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Internal - Smoothing

void
UOdysseyFreehandShape::InitSmoothing()
{
    switch(SmoothingOptions.SmoothingMethod)
    {
        case EOdysseySmoothingMethod::kAverage : mSmoother = MakeShared<FOdysseySmoothingAverage>(&SmoothingOptions); break;
        case EOdysseySmoothingMethod::kPull : mSmoother = MakeShared<FOdysseySmoothingPull>(&SmoothingOptions) ; break;
        default: break;
    }
}

void
UOdysseyFreehandShape::BeginSmoothing()
{
    //Add the first point to the smoothing system
    if(SmoothingEnabled && SmoothingOptions.SmoothingRealTime)
        mSmoother->AddPoint( mRawStroke[0] );
}

bool
UOdysseyFreehandShape::SmoothTo(const FOdysseyPoint& iPoint)
{
    mSmoother->AddPoint( iPoint );
    if( !mSmoother->IsReady() )
        return false;

    mSmoothedStroke.Add(mSmoother->ComputePoint());
    return true;
}

void
UOdysseyFreehandShape::EndSmoothing()
{
    //Apply Smoothing if it is not realtime
    //Reapplies the smoothing if it is enabled but not realtime
    if (SmoothingEnabled && !SmoothingOptions.SmoothingRealTime && mRawStroke.Num() > 0 )
        ReapplySmoothing();
}

void
UOdysseyFreehandShape::AbortSmoothing()
{
}

void
UOdysseyFreehandShape::ResetSmoothing()
{
    mSmoother->Reset();
    mSmoothedStroke.Empty();
}

void
UOdysseyFreehandShape::ReapplySmoothing()
{
    // Reset the stroke (except the raw stroke) to reapply the smoothing and interpolation
    ResetSmoothing();
    ResetInterpolation();
    mOnPathResetDelegate.Broadcast();
    
    // Reapply the smoothing

    //Begin the smoothing and interpolation process
    BeginSmoothing();
    BeginInterpolation();

    //Force first point to be drawn
    mOnPathBeginDelegate.Broadcast(mRawStroke[0]);

    //Continue the Stroke
    for (int i = 1; i < mRawStroke.Num(); i++)
    {
        if (!SmoothTo(mRawStroke[i])) //false means SmoothTo has just not produced any point but it is not an error
            continue;

        //Apply interpolation
        TArray<FOdysseyPoint> interpolatedPoints = InterpolateTo(mSmoothedStroke.Last());

        if (interpolatedPoints.Num() > 0)
            mOnPathToDelegate.Broadcast(interpolatedPoints);
    }
}

void
UOdysseyFreehandShape::CatchUp()
{
    if (!mHasStrokeBegun || !SmoothingEnabled || !SmoothingOptions.SmoothingCatchUp || !mSmoother->CanCatchUp() || mRawStroke.Num() <= 0)
        return;

    StrokeTo(mRawStroke[mRawStroke.Num() - 1]);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - Interpolation

void
UOdysseyFreehandShape::InitInterpolation()
{
    switch(InterpolationType)
    {
        case EOdysseyInterpolationType::kCatmullRom: mInterpolator = MakeShared<FOdysseyInterpolationCatmullRom>(); break;
        case EOdysseyInterpolationType::kBezier: mInterpolator = MakeShared<FOdysseyInterpolationBezier>(); break;
        case EOdysseyInterpolationType::kLine: mInterpolator = MakeShared<FOdysseyInterpolationCatmullRom>(); break;

        default: break;
    }

    if (AdaptativeStep && mAdaptStepDelegate.IsBound())
    {
        mInterpolator->SetStep( FMath::Max( 1.f, mAdaptStepDelegate.Execute(Step) ) );
    }
    else
    {
        mInterpolator->SetStep(Step);
    }
}

void
UOdysseyFreehandShape::BeginInterpolation()
{
    //Add Point to Interpolator
    mInterpolator->AddPoint( mRawStroke[0] );
    mInterpolatedStroke.Add( mRawStroke[0] );
}

TArray<FOdysseyPoint>
UOdysseyFreehandShape::InterpolateTo(const FOdysseyPoint& iPoint)
{

    //If the Interpolator is ready to produce points do it, otherwise.... don't (Thanks Captain Obvious)
    while( !mInterpolator->IsReady() )
    {
        //Add Point to Interpolator
        mInterpolator->AddPoint( iPoint );
    }

    TArray<FOdysseyPoint> newPoints = mInterpolator->ComputePoints();

    for( int i = 0; i < newPoints.Num(); ++i )
    {
        newPoints[i].ComputeRelativeParameters(mInterpolatedStroke.Last(), true);
        mInterpolatedStroke.Add(newPoints[i]);
    }

    return newPoints;
}

void
UOdysseyFreehandShape::EndInterpolation()
{
}

void
UOdysseyFreehandShape::AbortInterpolation()
{
}

void
UOdysseyFreehandShape::ResetInterpolation()
{
    mInterpolator->Reset();
    mInterpolatedStroke.Empty();
}
