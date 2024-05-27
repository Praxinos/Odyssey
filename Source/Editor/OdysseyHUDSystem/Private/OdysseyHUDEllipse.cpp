// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDEllipse.h"
#include "CanvasTypes.h"

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
FOdysseyHUDEllipse::DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams)
{
    const FLinearColor color(0.f, 1.f, 0.f);

    int ellipseAaxis = FMath::Abs( mCenterPoint.X - mBorderPoint.X );
    int ellipseBaxis = FMath::Abs( mCenterPoint.Y - mBorderPoint.Y );

    ::ULIS::TArray<::ULIS::FVec2I> points;
    ::ULIS::GenerateEllipsePoints( ::ULIS::FVec2I(mCenterPoint.X, mCenterPoint.Y), ellipseAaxis, ellipseBaxis, points );

    if (points.Size() < 2)
        return;

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);
        
    for (int i = 1; i < points.Size(); i++)
    {
        FVector2D startPoint = iParams.mTextureToHUD.Execute(FVector2D(points[i - 1].x, points[i - 1].y));
        FVector2D endPoint = iParams.mTextureToHUD.Execute(FVector2D(points[i].x, points[i].y));
        
        batchedElements->AddTranslucentLine(FVector(startPoint, 0.f), FVector(endPoint, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);
    }

    FVector2D startPoint = iParams.mTextureToHUD.Execute(FVector2D(points[points.Size() - 1].x, points[points.Size() - 1].y));
    FVector2D endPoint = iParams.mTextureToHUD.Execute(FVector2D(points[0].x, points[0].y));
    batchedElements->AddTranslucentLine(FVector(startPoint, 0.f), FVector(endPoint, 0.f), color, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);

    FOdysseyHUDElement::DrawHUD(iParams); 
}

int FOdysseyHUDEllipse::GetAAxis()
{
    return abs((int)(mCenterPoint.X - mBorderPoint.X));
}

int FOdysseyHUDEllipse::GetBAxis()
{
    return abs((int)(mCenterPoint.Y - mBorderPoint.Y));
}
