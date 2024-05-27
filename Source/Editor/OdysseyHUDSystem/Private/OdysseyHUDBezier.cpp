// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDBezier.h"
#include "CanvasTypes.h"

#include "ULISLoaderModule.h"

FOdysseyHUDBezier::~FOdysseyHUDBezier()
{

}

FOdysseyHUDBezier::FOdysseyHUDBezier(FName iName, FVector2D iStartPoint, FVector2D iEndPoint, FVector2D iControlPoint) :
    FOdysseyHUDElement(iName)
{
    mStartPoint = iStartPoint;
    mEndPoint = iEndPoint;
    mControlPoint = iControlPoint;
}

void
FOdysseyHUDBezier::DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams)
{
    const FLinearColor bezierLineColor(0.f, 1.f, 0.f, 1.f);
    const FLinearColor controlLineColor(0.f, 1.f, 0.f, 0.4f);
    
    FVector2D startPoint = iParams.mTextureToHUD.Execute(mStartPoint);
    FVector2D controlPoint = iParams.mTextureToHUD.Execute(mControlPoint);
    FVector2D endPoint = iParams.mTextureToHUD.Execute(mEndPoint);

    FBatchedElements* batchedElements = iParams.mCanvas->GetBatchedElements(FCanvas::ET_Line);

    ::ULIS::TArray<::ULIS::FVec2I> pointsArray;
    ::ULIS::GenerateQuadraticBezierPoints(
        ::ULIS::FVec2I(mStartPoint.X, mStartPoint.Y),
        ::ULIS::FVec2I(mControlPoint.X, mControlPoint.Y),
        ::ULIS::FVec2I(mEndPoint.X, mEndPoint.Y),
        1.0f,
        pointsArray
    );

    if (pointsArray.Size() < 2)
        return;
        
    for (int i = 1; i < pointsArray.Size(); i++)
    {
        
        FVector2D startBezierPoint = iParams.mTextureToHUD.Execute(FVector2D(pointsArray[i - 1].x, pointsArray[i - 1].y));
        FVector2D endBezierPoint = iParams.mTextureToHUD.Execute(FVector2D(pointsArray[i].x, pointsArray[i].y));
        
        batchedElements->AddTranslucentLine(FVector(startBezierPoint, 0.f), FVector(endBezierPoint, 0.f), bezierLineColor, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);
    }

    batchedElements->AddTranslucentLine(FVector(startPoint, 0.f), FVector(controlPoint, 0.f), controlLineColor, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);
    batchedElements->AddTranslucentLine(FVector(controlPoint, 0.f), FVector(endPoint, 0.f), controlLineColor, iParams.mCanvas->GetHitProxyId(), 1.f, 0.f, true);

    FOdysseyHUDElement::DrawHUD(iParams); 
}
