// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "PolygonShape/OdysseyPolygonShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyHUDElement.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPolygonShape::~UOdysseyPolygonShape()
{
}

UOdysseyPolygonShape::UOdysseyPolygonShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyPolygonShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsDrawing)
    {
        mIsDrawing = true;

        if (mPoints.IsEmpty())
            mOnBegin.Broadcast();

        //Polygon Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
        FOdysseyPoint point(iPointInTexture.x, iPointInTexture.y);

        mPoints.Add(point);

        CreateHUD();
    }
    return true;
}

void
UOdysseyPolygonShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (mIsDrawing)
    {
        //Polygon Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
        mPoints.Last() = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);

        bool snapAngles = SnapAngles ^ mInvertSnapAngles;
        if (snapAngles)
        {
            int num = mPoints.Num() - 2;
            int shiftX = FMath::Abs(iPointInTexture.x - mPoints[num].x);
            int shiftY = FMath::Abs(iPointInTexture.y - mPoints[num].y);

            if (shiftX > shiftY)
                mPoints.Last().y = mPoints[num].y;
            else
                mPoints.Last().x = mPoints[num].x;
        }

        SetLastHUDPoint(mPoints.Last());

        mOnInteractive.Broadcast( mPoints );
    }

    UOdysseyShape::OnMouseHover(iPointInTexture);
}

bool
UOdysseyPolygonShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsDrawing)
        return UOdysseyShape::OnMouseUp(iPointInTexture, iKey);

    FOdysseyPoint point(iPointInTexture.x, iPointInTexture.y);

    mPoints.Add( point );
    AddPointToHUD(point);

    mOnInteractive.Broadcast( mPoints );
    return true;
}

void
UOdysseyPolygonShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsDrawing)
        return UOdysseyShape::OnMouseDrag(iPointInTexture);

    //Polygon Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    mPoints.Last() = FOdysseyPoint(iPointInTexture.x, iPointInTexture.y);

    bool snapAngles = SnapAngles ^ mInvertSnapAngles;
    if (snapAngles && mPoints.Num() > 1)
    {
        int num = mPoints.Num() - 2;
        int shiftX = FMath::Abs(iPointInTexture.x - mPoints[num].x);
        int shiftY = FMath::Abs(iPointInTexture.y - mPoints[num].y);

        if (shiftX > shiftY)
            mPoints.Last().y = mPoints[num].y;
        else
            mPoints.Last().x = mPoints[num].x;
    }

    SetLastHUDPoint(mPoints.Last());

    mOnInteractive.Broadcast( mPoints );

    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyPolygonShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return true;
    }

    if( iKey == EKeys::Enter )
    {
        CommitPolygon();
        return true;
    }

    if( iKey == EKeys::LeftShift || iKey == EKeys::RightShift )
    {
        mInvertSnapAngles = true;
        return true;
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyPolygonShape::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        mInvertSnapAngles = false;
        return true;
    }

    return UOdysseyShape::OnKeyUp(iKey);
}

void
UOdysseyPolygonShape::CommitPolygon()
{
    mIsDrawing = false;

    if ( mPoints.Num() <= 2 )
    {
        Abort();
        return;
    }

    //The lastPoint is always a temporary point, so we remove it before commiting
    mPoints.RemoveAt(mPoints.Num() - 1);

    //We close the polygon by adding the first point again
    mPoints.Emplace(mPoints[0]);

    mOnCommit.Broadcast( mPoints, true);

    mPoints.Empty();

    //HUD
    RemoveHUD();
}

void
UOdysseyPolygonShape::Abort()
{
    mIsDrawing = false;
    mOnAbort.Broadcast();

    mPoints.Empty();

    //HUD
    RemoveHUD();
}

void
UOdysseyPolygonShape::OnFirstHandleDragEnd()
{
    CommitPolygon();
}

void
UOdysseyPolygonShape::CreateHUD()
{
    mPolygonHUD = MakeShared<FOdysseyHUDPolygon>();
    mPolygonHUD->ClosePolygon(false);
    mPolygonHUD->GetPoints().Append(mPoints);

    mHUD->AddElement(mPolygonHUD);
    RebuildHandleHUDs();
}

void
UOdysseyPolygonShape::SetLastHUDPoint(const FOdysseyPoint& iPoint)
{
    mPolygonHUD->GetPoints().Last() = iPoint;
    mHandleHUDs.Last()->SetPosition(iPoint);
}

void
UOdysseyPolygonShape::AddPointToHUD(const FOdysseyPoint& iPoint)
{
    mPolygonHUD->GetPoints().Add(iPoint);
    RebuildHandleHUDs();
}

void
UOdysseyPolygonShape::RemoveHUD()
{
    mHUD->RemoveElement(mPolygonHUD);
    mPolygonHUD = nullptr;
    mHandleHUDs.Empty();
}

void
UOdysseyPolygonShape::RebuildHandleHUDs()
{
    mHandleHUDs.Empty();
    mPolygonHUD->EmptyElements();

    TArray<FVector2D>& points = mPolygonHUD->GetPoints();
    if (points.IsEmpty())
        return;

    TSharedPtr<FOdysseyHUDHandle> handle = MakeShared<FOdysseyHUDHandle>(points[0]);
    handle->IsPositionLocked(true);
    handle->OnDragEnd().AddUObject(this, &UOdysseyPolygonShape::OnFirstHandleDragEnd);
    mHandleHUDs.Add(handle);
    mPolygonHUD->AddElement(handle);

    for (int i = 1; i < points.Num(); i++)
    {
        handle = MakeShared<FOdysseyHUDHandle>(points[i]);
        handle->IsInteractable(false);
        mHandleHUDs.Add(handle);
        mPolygonHUD->AddElement(handle);
    }
}
