// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "RectangleShape/OdysseyRectangleShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDRectangle.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyRectangleShape::~UOdysseyRectangleShape()
{
}

UOdysseyRectangleShape::UOdysseyRectangleShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun( false )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyRectangleShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( !mHasStrokeBegun )
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        mRectangle = MakeShared<FOdysseyHUDRectangle>(FName("Rectangle"), FVector2D( iPointInTexture.x, iPointInTexture.y), FVector2D( iPointInTexture.x, iPointInTexture.y) );
        mHUD->AddElement(mRectangle);

        TSharedPtr<FOdysseyHUDHandle> handleTopLeft = MakeShared<FOdysseyHUDHandle>(FName("handleTopLeft"), &(mRectangle->mTopLeftPoint));
        TSharedPtr<FOdysseyHUDHandle> handleBottomRight = MakeShared<FOdysseyHUDHandle>(FName("handleBottomRight"), &(mRectangle->mBottomRightPoint));

        handleTopLeft->IsInteractable(false);
        handleBottomRight->IsInteractable(false);

        mRectangle->AddElement(handleBottomRight);
        mRectangle->AddElement(handleTopLeft);
        return true;
    }

    return false;
}

bool
UOdysseyRectangleShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mHasStrokeBegun )
    {
        CommitRectangle();
        return true;
    }
    return false;
}

void
UOdysseyRectangleShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyRectangleShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mHasStrokeBegun )
    {
        FOdysseyPoint point = iPointInTexture;
        if (Uniform)
        {
            int shiftX = iPointInTexture.x - mRectangle->mTopLeftPoint.X;
            int shiftY = iPointInTexture.y - mRectangle->mTopLeftPoint.Y;

            int signX = shiftX < 0 ? -1 : 1;
            int signY = shiftY < 0 ? -1 : 1;

            int mult = signX == signY ? 1 : -1;

            if( FMath::Abs(shiftX) > FMath::Abs(shiftY) )
            {
                point.x = mRectangle->mTopLeftPoint.X + shiftX;
                point.y = mRectangle->mTopLeftPoint.Y + shiftX * mult;
            }
            else
            {
                point.x = mRectangle->mTopLeftPoint.X + shiftY * mult;
                point.y = mRectangle->mTopLeftPoint.Y + shiftY;
            }
        }
        mRectangle->mBottomRightPoint = FVector2D(point.x, point.y);
        UOdysseyShape::OnMouseDrag(iPointInTexture);
    }
}

bool
UOdysseyRectangleShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyRectangleShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

void UOdysseyRectangleShape::Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions)
{
    if (!iBlock)
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());

    ctx.DrawRectangle(*(iBlock), ::ULIS::FVec2I(mRectangle->mTopLeftPoint.X, mRectangle->mTopLeftPoint.Y), ::ULIS::FVec2I(mRectangle->mBottomRightPoint.X, mRectangle->mBottomRightPoint.Y), iOptions.mColor, iOptions.mFilled);

    ctx.Finish();
}

void UOdysseyRectangleShape::CommitRectangle()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateRectanglePoints(::ULIS::FVec2I(mRectangle->mTopLeftPoint.X, mRectangle->mTopLeftPoint.Y), ::ULIS::FVec2I(mRectangle->mBottomRightPoint.X, mRectangle->mBottomRightPoint.Y), pointsArray);

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
    }
}

bool UOdysseyRectangleShape::AbortShape()
{
    if( mHasStrokeBegun )
    {
        mHasStrokeBegun = false;
        return UOdysseyShape::AbortShape();
    }
    return false;
}
