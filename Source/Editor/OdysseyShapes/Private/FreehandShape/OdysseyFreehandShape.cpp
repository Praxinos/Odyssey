// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "FreehandShape/OdysseyFreehandShape.h"

#include "FreehandShape/Smoothing/OdysseySmoothingAverage.h"
#include "FreehandShape/Smoothing/OdysseySmoothingPull.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyFreehandShape::~UOdysseyFreehandShape()
{
}

UOdysseyFreehandShape::UOdysseyFreehandShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Properties
    , SmoothingOptions()

    //Internal
    , mRawStroke()
    , mSmoother(nullptr)
    , mIsPainting(false)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

void
UOdysseyFreehandShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    Begin(iPointInTexture);
    UOdysseyShape::OnMouseDown(iPointInTexture, iKey);
}

void
UOdysseyFreehandShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    End();
    UOdysseyShape::OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyFreehandShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyFreehandShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    To(iPointInTexture);
    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

void
UOdysseyFreehandShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return;
    }
    UOdysseyShape::OnKeyDown(iKey);
}

void
UOdysseyFreehandShape::OnKeyUp(const FKey& iKey)
{
    UOdysseyShape::OnKeyUp(iKey);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Stroke API

bool
UOdysseyFreehandShape::Begin( const FOdysseyPoint& iPoint )
{
    if (mIsPainting)
        return false;

    mIsPainting = true;

    //Reset the smoother to use the one selected in the SmoothingOptions
    ResetSmoother();

    //Prepare Raw Stroke Array
    mRawStroke.Empty();

    //Store the first raw point
    mRawStroke.Add( iPoint );

    InternalStrokeBegin(iPoint);
    return true;
}

bool
UOdysseyFreehandShape::To( const FOdysseyPoint& iPoint )
{
    if (!mIsPainting)
        return false;

    //Add the raw point to the array
    //Use a temporary variable to ensure we are not adding a point that is already contained (see Add())
    FOdysseyPoint point = iPoint;
    mRawStroke.Add(point);

    //Call the Internal Stroke To
    return InternalStrokeTo(iPoint, false);
}

bool
UOdysseyFreehandShape::End()
{
    if (!mIsPainting)
        return false;

    //Apply Smoothing if it is not realtime
    ApplySmoothing();

    mOnPathEndDelegate.Broadcast({ mRawStroke.Last() });

    mIsPainting = false;
    return true;
}

bool
UOdysseyFreehandShape::Abort()
{
    if (!mIsPainting)
        return false;

    mIsPainting = false;
    
    mOnPathResetDelegate.Broadcast(); //just to be sure
    mOnPathAbortDelegate.Broadcast();
    return true;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Tick API
    
void
UOdysseyFreehandShape::Tick(float iDeltaTime)
{
    if (mIsPainting)
    {
        CatchUp();
    }
}

void
UOdysseyFreehandShape::ApplyOverrides(const TMap<FName, UObject*>& iOverrides)
{
    const UOdysseyFreehandShapeOverrides* freehandShapeOverrides = Cast<const UOdysseyFreehandShapeOverrides>(iOverrides["OdysseyFreehandShapeOverrides"]);
    if (freehandShapeOverrides)
        freehandShapeOverrides->Override(this);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Internal Stroke API

bool
UOdysseyFreehandShape::InternalStrokeBegin(const FOdysseyPoint& iPoint)
{
    //Add the first point to the smoothing system
    if(SmoothingOptions.SmoothingEnabled && SmoothingOptions.SmoothingRealTime)
        mSmoother->AddPoint( iPoint );

    mOnPathBeginDelegate.Broadcast(iPoint);
    return true;
}

bool
UOdysseyFreehandShape::InternalStrokeTo(const FOdysseyPoint& iPoint, bool iIsStrokeEnd)
{
    //Apply smoothing if needed, otherwise skip it
    FOdysseyPoint point = iPoint;
    if(SmoothingOptions.SmoothingEnabled && (SmoothingOptions.SmoothingRealTime || iIsStrokeEnd ) )
    {
        mSmoother->AddPoint( iPoint );

        if( !mSmoother->IsReady() )
            return true;

        point = mSmoother->ComputePoint();
    }

    mOnPathToDelegate.Broadcast({ point });

    return true;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseySmoothingOptions&
UOdysseyFreehandShape::GetSmoothingOptions()
{
    return SmoothingOptions;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Internal - Smoothing

void
UOdysseyFreehandShape::ApplySmoothing()
{
    //Ensure the smoothing is enabled, is not realtime and we have some raw inputs to work with
    if (!SmoothingOptions.SmoothingEnabled || SmoothingOptions.SmoothingRealTime || mRawStroke.Num() <= 0 )
        return;

    //
    // Clean up before apply the smoothing again
    //

    mOnPathResetDelegate.Broadcast();

    // Reset Smoother
    mSmoother->Reset();
    
    //
    // Apply the smoothing again
    //

    //Begin the Stroke
    InternalStrokeBegin(mRawStroke[0]);

    //Continue the Stroke
    for (int i = 1; i < mRawStroke.Num(); i++)
    {
        InternalStrokeTo(mRawStroke[i], true);
    }
}

void
UOdysseyFreehandShape::CatchUp()
{
    if (!mIsPainting || !SmoothingOptions.SmoothingEnabled || !SmoothingOptions.SmoothingCatchUp || !mSmoother->CanCatchUp() || mRawStroke.Num() <= 0)
        return;

    To(mRawStroke[mRawStroke.Num() - 1]);
}

void
UOdysseyFreehandShape::ResetSmoother()
{
    switch(SmoothingOptions.SmoothingMethod)
    {
        case EOdysseySmoothingMethod::kAverage : mSmoother = MakeShared<FOdysseySmoothingAverage>(&SmoothingOptions); break;
        case EOdysseySmoothingMethod::kPull : mSmoother = MakeShared<FOdysseySmoothingPull>(&SmoothingOptions) ; break;
        default: break;
    }
    mSmoother->Reset();
}
