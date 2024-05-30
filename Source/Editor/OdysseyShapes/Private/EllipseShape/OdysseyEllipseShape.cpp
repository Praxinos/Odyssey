// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EllipseShape/OdysseyEllipseShape.h"
#include "OdysseyHUDEllipse.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyEllipseShape::~UOdysseyEllipseShape()
{
}

UOdysseyEllipseShape::UOdysseyEllipseShape(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse Events

bool
UOdysseyEllipseShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    mIsDrawing = true;

    //Ellipse Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    FOdysseyPoint point(iPointInTexture.x, iPointInTexture.y);
    mCenterPoint = point;
    mBorderPoint = point;

    CreateHUD();

    return true;
}

void
UOdysseyEllipseShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsDrawing)
        return;

    //Ellipse Shape does not manage stylus params, so we create a new OdysseyPoint from scratch
    FOdysseyPoint point(iPointInTexture.x, iPointInTexture.y);
    mBorderPoint = point;
    if (mUniform)
    {
        int shiftX = FMath::Abs(mBorderPoint.x - mCenterPoint.x);
        int shiftY = FMath::Abs(mBorderPoint.y - mCenterPoint.y);

        int maxShift = FMath::Max( shiftX, shiftY );

        mBorderPoint.x = mCenterPoint.x + maxShift;
        mBorderPoint.y = mCenterPoint.y + maxShift;
    }

    RefreshHUD();

    mOnInteractive.Broadcast( GeneratePoints(), true );

    UOdysseyShape::OnMouseDrag(iPointInTexture);
}

bool
UOdysseyEllipseShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!mIsDrawing)
        return UOdysseyShape::OnMouseUp(iPointInTexture, iKey);

    RemoveHUD();

    mOnCommit.Broadcast( GeneratePoints(), true);
    return true;
}

bool
UOdysseyEllipseShape::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return true;
    }

    if( iKey == EKeys::LeftShift || iKey == EKeys::RightShift )
	{
		mUniform = true;
		return true;
	}

    return UOdysseyShape::OnKeyDown(iKey);
}

bool
UOdysseyEllipseShape::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        mUniform = false;
		return true;
    }

    return UOdysseyShape::OnKeyUp(iKey);
}

void
UOdysseyEllipseShape::Abort()
{
    mIsDrawing = false;

    RemoveHUD();
    
    mOnAbort.Broadcast();
}

TArray<FOdysseyPoint>
UOdysseyEllipseShape::GeneratePoints() const
{
    /* float xRadius = FMath::Abs(mCenterPoint.X - mBorderPoint.x);
    float yRadius = FMath::Abs(mCenterPoint.Y - mBorderPoint.y);

    int numLines = FMath::Max(4, );*/

    return TArray<FOdysseyPoint>();
}

void
UOdysseyEllipseShape::CreateHUD()
{
    int xRadius = FMath::Abs((int)(mCenterPoint.x - mBorderPoint.x));
    int yRadius = FMath::Abs((int)(mCenterPoint.y - mBorderPoint.y));

    mEllipseHUD = MakeShared<FOdysseyHUDEllipse>(mCenterPoint, xRadius, yRadius);
    mHUD->AddElement(mEllipseHUD);
}

void
UOdysseyEllipseShape::RefreshHUD()
{    
    int xRadius = FMath::Abs((int)(mCenterPoint.x - mBorderPoint.x));
    int yRadius = FMath::Abs((int)(mCenterPoint.y - mBorderPoint.y));

    mEllipseHUD->SetXRadius(xRadius);
    mEllipseHUD->SetYRadius(yRadius);
}

void
UOdysseyEllipseShape::RemoveHUD()
{
    mHUD->RemoveElement(mEllipseHUD);
    mEllipseHUD = nullptr;
}