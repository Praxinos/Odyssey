// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "FreehandShape/OdysseyFreehandShape.h"

#include "FreehandShape/Smoothing/OdysseySmoothingAverage.h"
#include "FreehandShape/Smoothing/OdysseySmoothingPull.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyHUDPolygon.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyFreehandShape::~UOdysseyFreehandShape()
{
}

UOdysseyFreehandShape::UOdysseyFreehandShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
    mIsProgressive = true;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyFreehandShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mIsDrawing)
        return false;

    mIsDrawing = true;

    //Store the first raw point
    mRawStroke.Add( iPointInTexture );

    switch(SmoothingOptions.SmoothingMethod)
    {
        case EOdysseySmoothingMethod::kAverage : mSmoother = MakeShared<FOdysseySmoothingAverage>(&SmoothingOptions); break;
        case EOdysseySmoothingMethod::kPull : mSmoother = MakeShared<FOdysseySmoothingPull>(&SmoothingOptions) ; break;
        default: break;
    }

    CreateHUD();

    //Force first point to be drawn
    if(SmoothingEnabled && SmoothingOptions.SmoothingRealTime)
    {
        BeginSmoothing();

        mOnInteractive.Broadcast( mSmoothedStroke );

        if ( mPathHUD )
            mPathHUD->GetPoints().Append(mSmoothedStroke);
    }
    else
    {
        mOnInteractive.Broadcast( mRawStroke );
        if ( mPathHUD )
            mPathHUD->GetPoints().Append(mRawStroke);
    }

    return true;
}

void
UOdysseyFreehandShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsDrawing)
        return;

    StrokeTo(iPointInTexture);
}

bool
UOdysseyFreehandShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsDrawing)
        return false;

    mIsDrawing = false;

    //Apply Smoothing if it is not realtime
    //Reapplies the smoothing if it is enabled but not realtime
    if (SmoothingEnabled && !SmoothingOptions.SmoothingRealTime && mRawStroke.Num() > 0 )
    {
        ReapplySmoothing();
        mOnCommit.Broadcast( mSmoothedStroke, true);
    }
    else
    {
        mOnCommit.Broadcast( mRawStroke, false);
    }

    mRawStroke.Empty();
    mSmoothedStroke.Empty();
    mSmoother = nullptr;

    RemoveHUD();

    return true;
}

bool
UOdysseyFreehandShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return true;
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Stroke API

bool
UOdysseyFreehandShape::StrokeTo( const FOdysseyPoint& iPoint )
{
    if (!mIsDrawing)
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

        mOnInteractive.Broadcast( { mSmoothedStroke.Last() } );
        if ( mPathHUD )
            mPathHUD->GetPoints().Add(mSmoothedStroke.Last());
    }
    else
    {
        mOnInteractive.Broadcast( { mRawStroke.Last() } );
        if ( mPathHUD )
            mPathHUD->GetPoints().Add(mRawStroke.Last());
    }

    return true;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Tick API

void
UOdysseyFreehandShape::Tick(float iDeltaTime)
{
    if (mIsDrawing)
        CatchUp();
}

void
UOdysseyFreehandShape::ApplyOverrides(const TMap<TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides)
{
    const UOdysseyFreehandShapeOverrides* freehandShapeOverrides = Cast<const UOdysseyFreehandShapeOverrides>(iOverrides[UOdysseyFreehandShapeOverrides::StaticClass()]);
    if (freehandShapeOverrides)
    {
        FOdysseySmoothingOptions smoothingOptions = SmoothingOptions;
        if(freehandShapeOverrides->bOverride_SmoothingMethod)
            smoothingOptions.SmoothingMethod = freehandShapeOverrides->SmoothingMethod;
        if(freehandShapeOverrides->bOverride_SmoothingStrength)
            smoothingOptions.SmoothingStrength = freehandShapeOverrides->SmoothingStrength;
        if(freehandShapeOverrides->bOverride_SmoothingRealTime)
            smoothingOptions.SmoothingRealTime = freehandShapeOverrides->SmoothingRealTime;
        if(freehandShapeOverrides->bOverride_SmoothingCatchUp)
            smoothingOptions.SmoothingCatchUp = freehandShapeOverrides->SmoothingCatchUp;

        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyFreehandShape, SmoothingOptions), smoothingOptions);

        if(freehandShapeOverrides->bOverride_SmoothingEnabled)
            FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyFreehandShape, SmoothingEnabled), freehandShapeOverrides->SmoothingEnabled);
    }
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
UOdysseyFreehandShape::BeginSmoothing()
{
    //Add the first point to the smoothing system
    if(SmoothingEnabled && SmoothingOptions.SmoothingRealTime)
        mSmoother->AddPoint( mRawStroke[0] );

    mSmoothedStroke.Add(mRawStroke[0]);
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
UOdysseyFreehandShape::ReapplySmoothing()
{
    // Reset the stroke (except the raw stroke) to reapply the smoothing and interpolation
    mSmoothedStroke.Empty();

    //Begin the smoothing and interpolation process
    BeginSmoothing();

    //Continue the Stroke
    for (int i = 1; i < mRawStroke.Num(); i++)
        SmoothTo(mRawStroke[i]);
}

void
UOdysseyFreehandShape::CatchUp()
{
    if (!mIsDrawing || !SmoothingEnabled || !SmoothingOptions.SmoothingCatchUp || !mSmoother->CanCatchUp() || mRawStroke.Num() <= 0)
        return;

    StrokeTo(mRawStroke[mRawStroke.Num() - 1]);
}

void
UOdysseyFreehandShape::Abort()
{
    mIsDrawing = false;

    mRawStroke.Empty();
    mSmoothedStroke.Empty();
    mSmoother = nullptr;

    RemoveHUD();

    mOnAbort.Broadcast();
}

void
UOdysseyFreehandShape::CreateHUD()
{
    if (!mDisplayHUD)
        return;

    mPathHUD = MakeShared<FOdysseyHUDPolygon>();
    mHUD->AddElement(mPathHUD);
}

void
UOdysseyFreehandShape::RefreshHUD()
{
}

void
UOdysseyFreehandShape::RemoveHUD()
{
    mHUD->RemoveElement(mPathHUD);
    mPathHUD = nullptr;
}
