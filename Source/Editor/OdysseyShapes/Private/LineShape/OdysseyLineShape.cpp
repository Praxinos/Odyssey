// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LineShape/OdysseyLineShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDLine.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyLineShape::~UOdysseyLineShape()
{
}

UOdysseyLineShape::UOdysseyLineShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun( false )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyLineShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( !mHasStrokeBegun )
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        mLine = MakeShared<FOdysseyHUDLine>(iPointInTexture, iPointInTexture);
        mHUD->AddElement(mLine);

        mHandleStart = MakeShared<FOdysseyHUDHandle>(iPointInTexture);
        mHandleEnd = MakeShared<FOdysseyHUDHandle>(iPointInTexture);

        mHandleStart->IsInteractable(false);
        mHandleEnd->IsInteractable(false);

        mLine->AddElement(mHandleStart);
        mLine->AddElement(mHandleEnd);
        return true;
    }

    return false;
}

bool
UOdysseyLineShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mHasStrokeBegun )
    {
        CommitLine();
        return true;
    }
    return false;
}

void
UOdysseyLineShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyLineShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mHasStrokeBegun )
    {
        FOdysseyPoint point = iPointInTexture;
        if( Uniform )
        {
            int shiftX = FMath::Abs( iPointInTexture.x - mLine->GetStartPoint().X);
            int shiftY = FMath::Abs( iPointInTexture.y - mLine->GetStartPoint().Y);

            if( shiftX > shiftY )
                point.y = mLine->GetStartPoint().Y;
            else
                point.x = mLine->GetStartPoint().X;
        }
        mLine->SetEndPoint(point);
        mHandleEnd->SetPosition(point);

        UOdysseyShape::OnMouseDrag(iPointInTexture);
    }
}

bool
UOdysseyLineShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyLineShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}


/*void UOdysseyLineShape::Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions)
{
    if (!iBlock)
        return;


    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());

    if (iOptions.mPrecision == EOdysseyDrawingPrecision::kRaw)
        ctx.DrawLine(*(iBlock), ::ULIS::FVec2I(mLine->GetStartPoint().X, mLine->GetStartPoint().Y), ::ULIS::FVec2I(mLine->GetEndPoint().X, mLine->GetEndPoint().Y), iOptions.mColor);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kAA)
        ctx.DrawLineAA(*(iBlock), ::ULIS::FVec2I(mLine->GetStartPoint().X, mLine->GetStartPoint().Y), ::ULIS::FVec2I(mLine->GetEndPoint().X, mLine->GetEndPoint().Y), iOptions.mColor);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kSP)
        ctx.DrawLineSP(*(iBlock), ::ULIS::FVec2I(mLine->GetStartPoint().X, mLine->GetStartPoint().Y), ::ULIS::FVec2I(mLine->GetEndPoint().X, mLine->GetEndPoint().Y), iOptions.mColor);

    ctx.Finish();
}*/

void UOdysseyLineShape::CommitLine()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateLinePoints(::ULIS::FVec2I(mLine->GetStartPoint().X, mLine->GetStartPoint().Y), ::ULIS::FVec2I(mLine->GetEndPoint().X, mLine->GetEndPoint().Y), pointsArray);

        //Todo ? Compute relative parameters

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

bool UOdysseyLineShape::AbortShape()
{
    if( mHasStrokeBegun )
    {
        mHasStrokeBegun = false;
        return UOdysseyShape::AbortShape();
    }
    return false;
}
