// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDEllipse.h"

#include "ULISLoaderModule.h"

FOdysseyHUDEllipse::~FOdysseyHUDEllipse()
{

}

FOdysseyHUDEllipse::FOdysseyHUDEllipse(FName iName, FVector2D iCenterPoint, FVector2D iBorderPoint) :
    FOdysseyHUDElement(iName)
{
    mCenterPoint = iCenterPoint;
    mBorderPoint = iBorderPoint;
    mEllipseAaxis = (int)(mCenterPoint.X - mBorderPoint.X);
    mEllipseBaxis = (int)(mCenterPoint.Y - mBorderPoint.Y);
}

FOdysseyHUDEllipse::FOdysseyHUDEllipse(FName iName, FVector2D iCenterPoint, int iEllipseAaxis, int iEllipseBaxis) :
    FOdysseyHUDElement(iName)
{
    mCenterPoint = iCenterPoint;
    mEllipseAaxis = iEllipseAaxis;
    mEllipseBaxis = iEllipseBaxis;
    mBorderPoint = FVector2D(iCenterPoint.X + iEllipseAaxis, iCenterPoint.Y + iEllipseBaxis);
}

void
FOdysseyHUDEllipse::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);
    
    FVector center = iParams.mOrigin
        + mCenterPoint.X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mCenterPoint.Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;

    int ellipseAaxis = FMath::Abs( mCenterPoint.X - mBorderPoint.X );
    int ellipseBaxis = FMath::Abs( mCenterPoint.Y - mBorderPoint.Y );

    float radius = 1.f / iParams.mTextureWidth * iParams.mPlaneWidth;

    UE_LOG(LogTemp, Warning, TEXT("mXAxis: %2f      %2f      %2f"), iParams.mXAxis.X, iParams.mXAxis.Y, iParams.mXAxis.Z);
    UE_LOG(LogTemp, Warning, TEXT("mYAxis: %2f      %2f      %2f"), iParams.mYAxis.X, iParams.mYAxis.Y, iParams.mYAxis.Z);
    UE_LOG(LogTemp, Warning, TEXT("Aaxis: %d"), ellipseAaxis);
    UE_LOG(LogTemp, Warning, TEXT("Baxis: %d"), ellipseBaxis);

    FVector x = iParams.mXAxis * ellipseAaxis;
    FVector y = iParams.mYAxis * ellipseBaxis;

    UE_LOG(LogTemp, Warning, TEXT("x: %2f      %2f      %2f"), x.X, x.Y, x.Z);
    UE_LOG(LogTemp, Warning, TEXT("y: %2f      %2f      %2f"), y.X, y.Y, y.Z);

    DrawCircle(
        iParams.mPDI,
        center,
        x,
        y,
        color,
        radius,
        64,
        SDPG_Foreground,
        1.f,
        0.f,
        true
    );

    FOdysseyHUDElement::Render(iParams);
}

int FOdysseyHUDEllipse::GetAAxis()
{
    return abs((int)(mCenterPoint.X - mBorderPoint.X));
}

int FOdysseyHUDEllipse::GetBAxis()
{
    return abs((int)(mCenterPoint.Y - mBorderPoint.Y));
}
