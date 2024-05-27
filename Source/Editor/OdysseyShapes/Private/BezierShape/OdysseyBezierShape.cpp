// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "BezierShape/OdysseyBezierShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDBezier.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyBezierShape::~UOdysseyBezierShape()
{
}

UOdysseyBezierShape::UOdysseyBezierShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun( false )
    , mHasControlBegun( false )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyBezierShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( !mHasStrokeBegun && !mHasControlBegun)
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        mBezier = MakeShared<FOdysseyHUDBezier>(iPointInTexture, iPointInTexture, iPointInTexture);
        mHUD->AddElement(mBezier);

        mHandleStart = MakeShared<FOdysseyHUDHandle>(iPointInTexture);
        mHandleControl = MakeShared<FOdysseyHUDHandle>(iPointInTexture);
        mHandleEnd = MakeShared<FOdysseyHUDHandle>(iPointInTexture);

        mHandleStart->IsInteractable(false);
        mHandleControl->IsInteractable(false);
        mHandleEnd->IsInteractable(false);

        mBezier->AddElement(mHandleStart);
        mBezier->AddElement(mHandleControl);
        mBezier->AddElement(mHandleEnd);

        return true;
    }

    return true;
}

bool
UOdysseyBezierShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (mHasControlBegun)
    {
        CommitBezier();
        return true;
    }
    else if( mHasStrokeBegun )
    {
        mHasStrokeBegun = false;
        mHasControlBegun = true;
        return true;
    }

    return false;
}

void
UOdysseyBezierShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if ( mHasControlBegun )
    {
        mBezier->SetControlPoint(iPointInTexture);
        mHandleControl->SetPosition(iPointInTexture);
    }
    
    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyBezierShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if ( mHasStrokeBegun )
    {
        mBezier->SetEndPoint(iPointInTexture);
        mHandleEnd->SetPosition(iPointInTexture);
    }

    if ( mHasControlBegun )
    {
        mBezier->SetControlPoint(iPointInTexture);
        mHandleControl->SetPosition(iPointInTexture);
    }

    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyBezierShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyBezierShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

/*void UOdysseyBezierShape::Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions)
{
    if (!iBlock)
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());

    if (iOptions.mPrecision == EOdysseyDrawingPrecision::kRaw)
        ctx.DrawQuadraticBezier(*(iBlock), ::ULIS::FVec2I(mBezier->GetStartPoint().X, mBezier->GetStartPoint().Y), ::ULIS::FVec2I(mBezier->GetControlPoint().X, mBezier->GetControlPoint().Y), ::ULIS::FVec2I(mBezier->GetEndPoint().X, mBezier->GetEndPoint().Y), 1.f, iOptions.mColor);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kAA)
        ctx.DrawQuadraticBezierAA(*(iBlock), ::ULIS::FVec2I(mBezier->GetStartPoint().X, mBezier->GetStartPoint().Y), ::ULIS::FVec2I(mBezier->GetControlPoint().X, mBezier->GetControlPoint().Y), ::ULIS::FVec2I(mBezier->GetEndPoint().X, mBezier->GetEndPoint().Y), 1.f, iOptions.mColor);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kSP)
        ctx.DrawQuadraticBezierSP(*(iBlock), ::ULIS::FVec2I(mBezier->GetStartPoint().X, mBezier->GetStartPoint().Y), ::ULIS::FVec2I(mBezier->GetControlPoint().X, mBezier->GetControlPoint().Y), ::ULIS::FVec2I(mBezier->GetEndPoint().X, mBezier->GetEndPoint().Y), 1.f, iOptions.mColor);

    ctx.Finish();
}*/

void UOdysseyBezierShape::CommitBezier()
{
    if( mHasControlBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateQuadraticBezierPoints(::ULIS::FVec2I(mBezier->GetStartPoint().X, mBezier->GetStartPoint().Y), ::ULIS::FVec2I(mBezier->GetControlPoint().X, mBezier->GetControlPoint().Y), ::ULIS::FVec2I(mBezier->GetEndPoint().X, mBezier->GetEndPoint().Y), 1.f, pointsArray);

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
        mHasControlBegun = false;
        
        mBezier = nullptr;
        mHandleStart = nullptr;
        mHandleControl = nullptr;
        mHandleEnd = nullptr;
    }
}

bool UOdysseyBezierShape::AbortShape()
{
    if( mHasStrokeBegun || mHasControlBegun )
    {
        mHasStrokeBegun = false;
        mHasControlBegun = false;

        mBezier = nullptr;
        mHandleStart = nullptr;
        mHandleControl = nullptr;
        mHandleEnd = nullptr;
        return UOdysseyShape::AbortShape();
    }
    return false;
}

FVector2D
UOdysseyBezierShape::GetStartPoint() const
{
    if (!mBezier)
        return FVector2D(0, 0);

    return mBezier->GetStartPoint();
}

FVector2D
UOdysseyBezierShape::GetControlPoint() const
{
    if (!mBezier)
        return FVector2D(0, 0);

    return mBezier->GetControlPoint();
}

FVector2D
UOdysseyBezierShape::GetEndPoint() const
{
    if (!mBezier)
        return FVector2D(0, 0);

    return mBezier->GetEndPoint();
}