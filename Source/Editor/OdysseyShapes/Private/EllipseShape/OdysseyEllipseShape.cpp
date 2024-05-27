// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EllipseShape/OdysseyEllipseShape.h"
#include "OdysseyHUDHandle.h"
#include "OdysseyHUDEllipse.h"
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

        int xRadius = 0;
        int yRadius = 0;
        mEllipse = MakeShared<FOdysseyHUDEllipse>(iPointInTexture, xRadius, yRadius);
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
            int shiftX = FMath::Abs(iPointInTexture.x - mEllipse->GetCenter().X);
            int shiftY = FMath::Abs(iPointInTexture.y - mEllipse->GetCenter().Y);

            int maxShift = FMath::Max( shiftX, shiftY );

            point.x = mEllipse->GetCenter().X + maxShift;
            point.y = mEllipse->GetCenter().Y + maxShift;
        }
        
        int xRadius = FMath::Abs((int)(mEllipse->GetCenter().X - point.x));
        int yRadius = FMath::Abs((int)(mEllipse->GetCenter().Y - point.y));
        mEllipse->SetXRadius(xRadius);
        mEllipse->SetYRadius(yRadius);

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
        ctx.DrawEllipse(*(iBlock), ::ULIS::FVec2I(mEllipse->GetCenter().X, mEllipse->GetCenter().Y), mEllipse->GetXRadius(), mEllipse->GetYRadius(), iOptions.mColor, iOptions.mFilled);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kAA)
        ctx.DrawEllipseAA(*(iBlock), ::ULIS::FVec2I(mEllipse->GetCenter().X, mEllipse->GetCenter().Y), mEllipse->GetXRadius(), mEllipse->GetYRadius(), iOptions.mColor, iOptions.mFilled);
    else if (iOptions.mPrecision == EOdysseyDrawingPrecision::kSP)
        ctx.DrawEllipseSP(*(iBlock), ::ULIS::FVec2I(mEllipse->GetCenter().X, mEllipse->GetCenter().Y), mEllipse->GetXRadius(), mEllipse->GetYRadius(), iOptions.mColor, iOptions.mFilled);

    ctx.Finish();
}

void UOdysseyEllipseShape::CommitEllipse()
{
    if( mHasStrokeBegun )
    {
        ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
        ::ULIS::GenerateEllipsePoints(::ULIS::FVec2I(mEllipse->GetCenter().X, mEllipse->GetCenter().Y), mEllipse->GetXRadius(), mEllipse->GetYRadius(), pointsArray);

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

