// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "PolygonShape/OdysseyPolygonShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDPolygon.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPolygonShape::~UOdysseyPolygonShape()
{
}

UOdysseyPolygonShape::UOdysseyPolygonShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun( false )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

void
UOdysseyPolygonShape::RebuildHandles()
{
    mHandles.Empty();
    mPolygon->EmptyElements();

    TArray<FVector2D>& points = mPolygon->GetPoints();
    if (points.IsEmpty())
        return;

    TSharedPtr<FOdysseyHUDHandle> handle = MakeShared<FOdysseyHUDHandle>(points[0]);
    handle->IsPositionLocked(true);
    handle->OnDragEnd().AddUObject(this, &UOdysseyPolygonShape::OnFirstHandleDragEnd);
    mHandles.Add(handle);
    mPolygon->AddElement(handle);

    for (int i = 1; i < points.Num(); i++)
    {
        handle = MakeShared<FOdysseyHUDHandle>(points[i]);
        handle->IsInteractable(false);
        mHandles.Add(handle);
        mPolygon->AddElement(handle);
    }
}

bool
UOdysseyPolygonShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mHasStrokeBegun)
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        mPolygon = MakeShared<FOdysseyHUDPolygon>();
        mPolygon->ClosePolygon(false);
        mPolygon->GetPoints().Add( iPointInTexture );

        mHUD->AddElement(mPolygon);

        RebuildHandles();
        return true;
    }

    return true;
}

void
UOdysseyPolygonShape::OnFirstHandleDragEnd()
{
    if (mHandles.Num() <= 2)
    {
        AbortShape();
        return;
    }

    CommitPolygon();
}

bool
UOdysseyPolygonShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mHasStrokeBegun)
    {
        mPolygon->GetPoints().Add( iPointInTexture);
        RebuildHandles();
        return true;
    }

    return false;
}

void
UOdysseyPolygonShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (mHasStrokeBegun)
    {
        FOdysseyPoint point = iPointInTexture;
        if (Uniform)
        {
            int num = mPolygon->GetPoints().Num() - 2; 
            int shiftX = FMath::Abs(iPointInTexture.x - mPolygon->GetPoints()[num].X);
            int shiftY = FMath::Abs(iPointInTexture.y - mPolygon->GetPoints()[num].Y);

            if (shiftX > shiftY)
                point.y = mPolygon->GetPoints()[num].Y;
            else
                point.x = mPolygon->GetPoints()[num].X;
        }
        mPolygon->GetPoints().Last() = point;
        mHandles.Last()->SetPosition(point);
    }
        
    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyPolygonShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mHasStrokeBegun )
    {
        FOdysseyPoint point = iPointInTexture;
        if (Uniform)
        {
            int num = mPolygon->GetPoints().Num() - 2; 
            int shiftX = FMath::Abs(iPointInTexture.x - mPolygon->GetPoints()[num].X);
            int shiftY = FMath::Abs(iPointInTexture.y - mPolygon->GetPoints()[num].Y);

            if (shiftX > shiftY)
                point.y = mPolygon->GetPoints()[num].Y;
            else
                point.x = mPolygon->GetPoints()[num].X;
        }
        mPolygon->GetPoints().Last() = point;
        mHandles.Last()->SetPosition(point);

        UOdysseyShape::OnMouseDrag(iPointInTexture);
    }
}

bool
UOdysseyPolygonShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }
    else if( iKey == EKeys::Enter )
    {
        CommitPolygon();
        return true;
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyPolygonShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

void UOdysseyPolygonShape::Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions)
{
    if (!iBlock)
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());

    if (iOptions.mPrecision == EOdysseyDrawingPrecision::kRaw)
    {
        std::vector< ::ULIS::FVec2I > points;
        for (int i = 0; i < mPolygon->GetPoints().Num(); i++)
        {
            points.push_back(::ULIS::FVec2I(mPolygon->GetPoints()[i].X, mPolygon->GetPoints()[i].Y));
        }
        
        ctx.DrawPolygon(*(iBlock), points, iOptions.mColor, iOptions.mFilled );
    }
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kAA)
    {
        std::vector< ::ULIS::FVec2I > points;
        for (int i = 0; i < mPolygon->GetPoints().Num(); i++)
        {
            points.push_back(::ULIS::FVec2I(mPolygon->GetPoints()[i].X, mPolygon->GetPoints()[i].Y));
        }

        ctx.DrawPolygonAA(*(iBlock), points, iOptions.mColor, iOptions.mFilled);
    }
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kSP)
    {
        std::vector< ::ULIS::FVec2F > points;
        for (int i = 0; i < mPolygon->GetPoints().Num(); i++)
        {
            points.push_back(::ULIS::FVec2F(mPolygon->GetPoints()[i].X, mPolygon->GetPoints()[i].Y));
        }

        ctx.DrawPolygonSP(*(iBlock), points, iOptions.mColor, iOptions.mFilled);
    }

    ctx.Finish();
}

void UOdysseyPolygonShape::CommitPolygon()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        std::vector<::ULIS::FVec2I> polygonPoints;

        for (const FVector2D& point : mPolygon->GetPoints())
            polygonPoints.push_back(::ULIS::FVec2I(point.X, point.Y));

        ::ULIS::GeneratePolygonPoints(polygonPoints, pointsArray);

        FOdysseyPoint pointToAdd = FOdysseyPoint::DefaultPoint();
        for (float i = 0.f; i < pointsArray.Size(); i+=Step)
        {
            pointToAdd.x = pointsArray[i].x;
            pointToAdd.y = pointsArray[i].y;
            mRawStroke.Add(pointToAdd);
        }
        if (mRawStroke.Num() != 0)
        {
            mOnPathBeginDelegate.Broadcast(mRawStroke[0]);
            mOnPathToDelegate.Broadcast(mRawStroke);
            mOnPathEndDelegate.Broadcast({ mRawStroke.Last() });
        }
        else
        {
            AbortShape();
        }
        mHasStrokeBegun = false;
        mHandles.Empty();
        mPolygon = nullptr;
    }
}

bool UOdysseyPolygonShape::AbortShape()
{
    if( mHasStrokeBegun )
    {
        mHasStrokeBegun = false;
        mHandles.Empty();
        mPolygon = nullptr;
        return UOdysseyShape::AbortShape();
    }
    return false;
}
