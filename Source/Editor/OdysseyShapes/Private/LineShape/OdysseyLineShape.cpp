// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LineShape/OdysseyLineShape.h"
#include "HUDViewportElement/Elements/OdysseyHUDHandle.h"
#include "HUDViewportElement/Elements/OdysseyHUDLine.h"
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
    Step = 1.0f;
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

        mLine = new FOdysseyHUDLine(FName("Line"), FVector2D(iPointInTexture.x, iPointInTexture.y), FVector2D(iPointInTexture.x, iPointInTexture.y));
        mHUD->AddElement(mLine);

        FOdysseyHUDHandle* handleStart = new FOdysseyHUDHandle(FName("handleStart"), mLine, &(mLine->mStartPoint));
        FOdysseyHUDHandle* handleFinish = new FOdysseyHUDHandle(FName("handleFinish"), mLine, &(mLine->mFinishPoint));

        mLine->AddElement(handleStart);
        mLine->AddElement(handleFinish);

        mHUD->OnKeyDown(iPointInTexture, iKey);
        return true;
    }

    return false;
}

bool
UOdysseyLineShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mHasStrokeBegun )
    {
        mHUD->OnKeyUp(iPointInTexture, iKey);
        CommitLine();
        return true;
    }
    return false;
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
    if( mHasStrokeBegun )
    {
        mHUD->CapturedMouseMove(iPointInTexture);

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

void UOdysseyLineShape::CommitLine()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateLinePoints(::ULIS::FVec2I(mLine->mStartPoint.X, mLine->mStartPoint.Y), ::ULIS::FVec2I(mLine->mFinishPoint.X, mLine->mFinishPoint.Y), pointsArray);

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

void UOdysseyLineShape::Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions)
{
    if( !iBlock )
        return;

    
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
    
    if( iOptions.mPrecision == EOdysseyDrawingPrecision::kRaw )
        ctx.DrawLine(*(iBlock), ::ULIS::FVec2I( mLine->mStartPoint.X, mLine->mStartPoint.Y ), ::ULIS::FVec2I( mLine->mFinishPoint.X, mLine->mFinishPoint.Y ), iOptions.mColor);
    else if( iOptions.mPrecision == EOdysseyDrawingPrecision::kAA )
        ctx.DrawLineAA(*(iBlock), ::ULIS::FVec2I(mLine->mStartPoint.X, mLine->mStartPoint.Y), ::ULIS::FVec2I(mLine->mFinishPoint.X, mLine->mFinishPoint.Y), iOptions.mColor);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kSP)
        ctx.DrawLineSP(*(iBlock), ::ULIS::FVec2I(mLine->mStartPoint.X, mLine->mStartPoint.Y), ::ULIS::FVec2I(mLine->mFinishPoint.X, mLine->mFinishPoint.Y), iOptions.mColor);

    ctx.Finish();
}
