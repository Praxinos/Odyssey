// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "PolygonShape/OdysseyPolygonShape.h"
#include "HUDViewportElement/Elements/OdysseyHUDHandle.h"
#include "HUDViewportElement/Elements/OdysseyHUDPolygon.h"
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

bool
UOdysseyPolygonShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mHasStrokeBegun)
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        TArray<FVector2D> points;

        mPolygon = new FOdysseyHUDPolygon(FName("Polygon"), TArray<FVector2D>() );
        mPolygon->mPoints.Reserve(100);
        mHUD->AddElement(mPolygon);

        mPolygon->mPoints.Add( FVector2D( iPointInTexture.x, iPointInTexture.y ));
        FOdysseyHUDHandle* handle1 = new FOdysseyHUDHandle(FName("handle" + mPolygon->mPoints.Num()), mPolygon, &(mPolygon->mPoints.Last()));
        mPolygon->mPoints.Add(FVector2D(iPointInTexture.x, iPointInTexture.y));
        FOdysseyHUDHandle* handle2 = new FOdysseyHUDHandle(FName("handle" + mPolygon->mPoints.Num()), mPolygon, &(mPolygon->mPoints.Last()));

        mPolygon->AddElement(handle2);
        mPolygon->AddElement(handle1);

        mHandles.Add(handle1);
        mHandles.Add(handle2);

        mHUD->OnKeyDown(iPointInTexture, iKey);
        return true;
    }

    return false;
}

bool
UOdysseyPolygonShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mHasStrokeBegun)
    {
        mHUD->OnKeyUp(iPointInTexture, iKey);
        if( mHandles.Last()->GetPosition() == mHandles.Last(1)->GetPosition() )
        {
            if( mHandles.Num() == 2 )
            {
                mHandles.Last()->Capture();
                return true;
            }

            CommitPolygon();
            return true;
        }
        else
        {
            mPolygon->mPoints.Add( FVector2D( iPointInTexture.x, iPointInTexture.y ));
            FOdysseyHUDHandle* handle = new FOdysseyHUDHandle(FName("handle" + mPolygon->mPoints.Num()), mPolygon, &(mPolygon->mPoints.Last()));
            mPolygon->AddElement(handle);
            mHandles.Add(handle);
            mHandles.Last()->Capture();
            return true;
        }
    }

    mHUD->OnKeyUp(iPointInTexture, iKey);

    return false;
}

void
UOdysseyPolygonShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (mHasStrokeBegun)
    {
        OnMouseDrag( iPointInTexture );
    }
    else
    {
        mHUD->MouseMove(iPointInTexture);
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

        mHUD->CapturedMouseMove(point);

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

        for (int i = 0; i < mPolygon->mPoints.Num(); i++)
            polygonPoints.push_back(::ULIS::FVec2I(mPolygon->mPoints[i].X, mPolygon->mPoints[i].Y));

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
    }
}

bool UOdysseyPolygonShape::AbortShape()
{
    if( mHasStrokeBegun )
    {
        mHasStrokeBegun = false;
        mHandles.Empty();
        return UOdysseyShape::AbortShape();
    }
    return false;
}
