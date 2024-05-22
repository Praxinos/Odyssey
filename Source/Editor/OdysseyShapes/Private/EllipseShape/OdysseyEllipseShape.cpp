// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EllipseShape/OdysseyEllipseShape.h"
#include "HUDViewportElement/Elements/OdysseyHUDHandle.h"
#include "HUDViewportElement/Elements/OdysseyHUDEllipse.h"
#include <ULIS>

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyEllipseShape::~UOdysseyEllipseShape()
{
}

UOdysseyEllipseShape::UOdysseyEllipseShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Internal
    , mRawStroke()
    , mHasStrokeBegun( false )
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyEllipseShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( !mHasStrokeBegun )
    {
        mHasStrokeBegun = true;
        mRawStroke.Empty();

        mEllipse = MakeShared<FOdysseyHUDEllipse>(FName("Ellipse"), FVector2D(iPointInTexture.x, iPointInTexture.y), FVector2D(iPointInTexture.x, iPointInTexture.y));
        mHUD->AddElement(mEllipse);
        return true;
    }

    return false;
}

bool
UOdysseyEllipseShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mHasStrokeBegun )
    {
        CommitEllipse();
        return true;
    }
    return false;
}

void
UOdysseyEllipseShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    //mHUD->MouseMove(iPointInTexture);

    UOdysseyShape::OnMouseHover(iPointInTexture);
}

void
UOdysseyEllipseShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if( mHasStrokeBegun )
    {
        FOdysseyPoint point = iPointInTexture;
        if (Uniform)
        {
            int shiftX = FMath::Abs(iPointInTexture.x - mEllipse->mCenterPoint.X);
            int shiftY = FMath::Abs(iPointInTexture.y - mEllipse->mCenterPoint.Y);

            int maxShift = FMath::Max( shiftX, shiftY );

            point.x = mEllipse->mCenterPoint.X + maxShift;
            point.y = mEllipse->mCenterPoint.Y + maxShift;
        }
        //mHUD->CapturedMouseMove(point);
        mEllipse->mBorderPoint = FVector2D(point.x, point.y);

        UOdysseyShape::OnMouseDrag(iPointInTexture);
    }
}

bool
UOdysseyEllipseShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        return AbortShape();
    }

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyEllipseShape::OnKeyUp(const FKey& iKey)
{
    return UOdysseyShape::OnKeyUp(iKey);
}

void UOdysseyEllipseShape::Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions)
{
    if (!iBlock)
        return;

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(iBlock->Format());
    
    if (iOptions.mPrecision == EOdysseyDrawingPrecision::kRaw)
        ctx.DrawEllipse(*(iBlock), ::ULIS::FVec2I(mEllipse->mCenterPoint.X, mEllipse->mCenterPoint.Y), mEllipse->GetAAxis(), mEllipse->GetBAxis(), iOptions.mColor, iOptions.mFilled);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kAA)
        ctx.DrawEllipseAA(*(iBlock), ::ULIS::FVec2I(mEllipse->mCenterPoint.X, mEllipse->mCenterPoint.Y), mEllipse->GetAAxis(), mEllipse->GetBAxis(), iOptions.mColor, iOptions.mFilled);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kSP)
        ctx.DrawEllipseSP(*(iBlock), ::ULIS::FVec2I(mEllipse->mCenterPoint.X, mEllipse->mCenterPoint.Y), mEllipse->GetAAxis(), mEllipse->GetBAxis(), iOptions.mColor, iOptions.mFilled);

    ctx.Finish();
}

void UOdysseyEllipseShape::CommitEllipse()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateEllipsePoints(::ULIS::FVec2I(mEllipse->mCenterPoint.X, mEllipse->mCenterPoint.Y), mEllipse->GetAAxis(), mEllipse->GetBAxis(), pointsArray);

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

bool UOdysseyEllipseShape::AbortShape()
{
    if( mHasStrokeBegun )
    {
        mHasStrokeBegun = false;
        return UOdysseyShape::AbortShape();
    }
    return false;
}

