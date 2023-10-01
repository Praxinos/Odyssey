// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LineShape/OdysseyLineShape.h"
#include "HUDViewportElement/Elements/OdysseyHUDHandle.h"
#include "HUDViewportElement/Elements/OdysseyHUDLine.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyLineShape::~UOdysseyLineShape()
{
}

UOdysseyLineShape::UOdysseyLineShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun(false)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyLineShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mLine = new FOdysseyHUDLine(FName("Line"), FVector2D(iPointInTexture.x, iPointInTexture.y), FVector2D(iPointInTexture.x, iPointInTexture.y));
    mHUD->AddElement(mLine);

    FOdysseyHUDHandle* handleStart = new FOdysseyHUDHandle(FName("handleStart"), mLine, &(mLine->mStartPoint));
    FOdysseyHUDHandle* handleFinish = new FOdysseyHUDHandle(FName("handleFinish"), mLine, &(mLine->mFinishPoint));

    mLine->AddElement(handleStart);
    mLine->AddElement(handleFinish);

    mHandles.Add(handleStart);
    mHandles.Add(handleFinish);

    mHUD->OnKeyDown(iPointInTexture, iKey);

    return BeginStroke(iPointInTexture);
}

bool
UOdysseyLineShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return EndStroke();
}

void
UOdysseyLineShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    mHUD->MouseMove(iPointInTexture);

    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyLineShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mHUD->CapturedMouseMove(iPointInTexture);

    StrokeTo(iPointInTexture);
    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyLineShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
        return AbortStroke();

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyLineShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Stroke API

bool
UOdysseyLineShape::BeginStroke( const FOdysseyPoint& iPoint )
{
    if (mHasStrokeBegun)
        return false;

    mHasStrokeBegun = true;

    //Prepare Raw Stroke Array
    mRawStroke.Empty();

    //Store the first raw point
    mRawStroke.Add( iPoint );

    //Force first point to be drawn
    mOnPathBeginDelegate.Broadcast(iPoint);
    
    return true;
}

bool
UOdysseyLineShape::StrokeTo( const FOdysseyPoint& iPoint )
{
    if (!mHasStrokeBegun)
        return false;

    //Add the raw point to the array
    //Use a temporary variable to ensure we are not adding a point that is already contained (see Add())
    FOdysseyPoint point = iPoint;
    mRawStroke.Add(point);

    mOnPathToDelegate.Broadcast(mRawStroke);
        
    return true;
}

bool
UOdysseyLineShape::EndStroke()
{
    if (!mHasStrokeBegun)
        return false;

    mOnPathEndDelegate.Broadcast({ mRawStroke.Last() });
    mHasStrokeBegun = false;

    return true;
}

bool
UOdysseyLineShape::AbortStroke()
{
    if (!mHasStrokeBegun)
        return false;

    mHasStrokeBegun = false;
    
    mOnPathResetDelegate.Broadcast();
    mOnPathAbortDelegate.Broadcast();
    return true;
}
