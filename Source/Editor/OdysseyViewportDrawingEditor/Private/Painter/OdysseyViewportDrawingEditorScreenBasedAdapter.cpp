// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorScreenBasedAdapter.h"
#include "MeshPaintHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "CanvasTypes.h"
#include "CanvasItem.h"
#include "OdysseyViewportDrawingEditorUtils.h"
#include <ULIS>
#include "ULISLoaderModule.h"
#include "ULISUtils.h"
#include "ULISEventBuilder.h"


#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorScreenBasedAdapter"

FOdysseyViewportDrawingEditorScreenBasedAdapter::~FOdysseyViewportDrawingEditorScreenBasedAdapter()
{
    RemoveTextureOverride();

    mStrokeBufferRenderTarget2D->ConditionalBeginDestroy();
    mStrokeBufferRenderTarget2D = nullptr;
}

FOdysseyViewportDrawingEditorScreenBasedAdapter::FOdysseyViewportDrawingEditorScreenBasedAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor) :
    IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(iEditor)
{
    PrepareAdapterForPainting();
}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::PrepareAdapterForPainting()
{    
    if (mEditor->Component() == nullptr || mEditor->Texture() == nullptr)
        return;

    IOdysseyViewportDrawingEditorAdapter::PrepareAdapterForPainting();

    if( mState == eState::kIdle )
    {
        BuildPaintingTexture2DRenderTarget();
        TexturePaintHelpers::GenerateSeamMask(mEditor->Component(), mEditor->GetUVIndexUsedByCurrentTexture(), mSeamRenderTarget2D, mEditor->Texture(), mPaintingTexture2DRenderTarget);
    }

    //We're using true pixel value for this adapter, so we put 0 in meshMaxSize
    mEditor->GetGUI()->GetTopTab()->SetMeshMaxSize(0);

    mState = eState::kIdleReady;
}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::StartPainting()
{
    mEditor->GetSelectedTool()->OnMouseDown(mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton);

    if (mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::Paint()
{
    mEditor->GetSelectedTool()->OnMouseDrag(mCurrentStrokeRay.mPoint);
}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::FinishPainting()
{
    mEditor->GetSelectedTool()->OnMouseUp( mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton );

    if (mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::Tick(float iDelta)
{
    if (mEditor->Texture() && mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);

    mEditor->GetSelectedTool()->Tick(iDelta);
}


void FOdysseyViewportDrawingEditorScreenBasedAdapter::RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = nullptr;
    if (mEditor->GetSelectedTool() && mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());
    else
        return;

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return;

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    TArray<MeshPaintHelpers::FPaintRay> paintRays;
    MeshPaintHelpers::RetrieveViewportPaintRays(iView, iViewport, iPDI, paintRays);

    for (const MeshPaintHelpers::FPaintRay& paintRay : paintRays)
    {
        FHitResult traceHitResult(1.0f);
        const FVector rayEnd(paintRay.CameraLocation + paintRay.RayDirection * HALF_WORLD_MAX);

        meshAdapter->LineTraceComponent(traceHitResult, paintRay.CameraLocation, rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

        const FPlane proj = iView->Project(traceHitResult.Location);
        FVector originDeproj, xDeproj, yDeproj, middleViewport;

        iView->DeprojectFVector2D(FVector2D(0, 0), originDeproj, mZScreenAxis);
        iView->DeprojectFVector2D(FVector2D(1, 0), xDeproj, mZScreenAxis);
        iView->DeprojectFVector2D(FVector2D(0, 1), yDeproj, mZScreenAxis);
        iView->DeprojectFVector2D(FVector2D(iViewport->GetSizeXY().X / 2, iViewport->GetSizeXY().Y / 2), middleViewport, mZScreenAxis);
        iView->DeprojectFVector2D(FVector2D(iViewport->GetMouseX(), iViewport->GetMouseY()), mMousePosInWorld, mZScreenAxis);
        //FPlane plan = iView->Project( traceHitResult.Location );
        //UE_LOG(LogTemp, Display, TEXT("plan: %lf, %lf, %lf, %lf"), plan.X, plan.Y, plan.Z, plan.W);


        FVector projOnSphere = traceHitResult.Location - paintRay.CameraLocation;
        projOnSphere = ( 10.f / projOnSphere.Size() ) * projOnSphere;
        float projOnSpherelongitude = FMath::Atan2(projOnSphere.Y, projOnSphere.X);
        float projOnSpherelatitude = FMath::Acos(projOnSphere.Z / 10.f);

        mXScreenAxis = xDeproj - originDeproj;
        mYScreenAxis = yDeproj - originDeproj;
        
        FVector mousePosInWorldWithZ = mMousePosInWorld + mZScreenAxis;
        FVector positionOnSphere = middleViewport - paintRay.CameraLocation;

        float longitude = FMath::Atan2(positionOnSphere.Y, positionOnSphere.X);
        float latitude = FMath::Acos(positionOnSphere.Z / 10.f);

        double angleX = projOnSpherelongitude - longitude;
        double angleY = projOnSpherelatitude - latitude;


        //FVector2f decalPixels = FVector2f(iViewport->GetMouseX() - iViewport->GetSizeXY().X / 2.f, iViewport->GetMouseY() - iViewport->GetSizeXY().Y / 2.f);
        //double angleX = FMath::DegreesToRadians( ( decalPixels.X / iViewport->GetSizeXY().X ) * iView->FOV);
        //double angleY = FMath::DegreesToRadians( ( decalPixels.Y / iViewport->GetSizeXY().X ) * iView->FOV);

        //FVector fakeScreenX = FVector( FMath::Sin(latitude + angleY)*FMath::Cos(longitude + angleX), FMath::Sin(angleX + longitude)* FMath::Sin(angleY + latitude), FMath::Cos(latitude+angleY));

        FVector rotationVector = (angleY / mXScreenAxis.Size()) * mXScreenAxis;
        FQuat quaternionRotation = FQuat::MakeFromRotationVector(rotationVector);

        FVector newPos = quaternionRotation.RotateVector(positionOnSphere);
        newPos = FVector(newPos.X * FMath::Cos(angleX) - newPos.Y * FMath::Sin(angleX), newPos.X * FMath::Sin(angleX) + newPos.Y * FMath::Cos(angleX), newPos.Z);

        mXScreenAxis = quaternionRotation.RotateVector(mXScreenAxis);
        mXScreenAxis = FVector(mXScreenAxis.X * FMath::Cos(angleX) - mXScreenAxis.Y * FMath::Sin(angleX), mXScreenAxis.X * FMath::Sin(angleX) + mXScreenAxis.Y * FMath::Cos(angleX), mXScreenAxis.Z);

        mYScreenAxis = quaternionRotation.RotateVector(mYScreenAxis);
        mYScreenAxis = FVector(mYScreenAxis.X * FMath::Cos(angleX) - mYScreenAxis.Y * FMath::Sin(angleX), mYScreenAxis.X * FMath::Sin(angleX) + mYScreenAxis.Y * FMath::Cos(angleX), mYScreenAxis.Z);

        UE_LOG(LogTemp, Display, TEXT("positionOnSphere: %lf, %lf, %lf"), positionOnSphere.X, positionOnSphere.Y, positionOnSphere.Z);
        UE_LOG(LogTemp, Display, TEXT("projOnSphere: %lf, %lf, %lf"), projOnSphere.X, projOnSphere.Y, projOnSphere.Z);

        UE_LOG(LogTemp, Display, TEXT("---Should rotate on X by: %lf---"), FMath::RadiansToDegrees(angleX));
        UE_LOG(LogTemp, Display, TEXT("---Should rotate on Y by: %lf---"), FMath::RadiansToDegrees(angleY));

        UE_LOG(LogTemp, Display, TEXT("NewPosition %lf, %lf, %lf"), newPos.X, newPos.Y, newPos.Z);
        UE_LOG(LogTemp, Display, TEXT("-----------------"));

        const FLinearColor brushCueColor = FLinearColor(1.0f, 1.0f, 0.3f);

        if (iPDI != NULL)
        {
            int numCircleSides = 128;
            // Draw brush circle
            DrawCircle(iPDI, mousePosInWorldWithZ, mXScreenAxis, mYScreenAxis, brushCueColor, drawingTool->GetBrushInstance()->GetSizeModifier(), numCircleSides, SDPG_World, 0.01f);
        }

        mousePosInWorldWithZ = mMousePosInWorld + mZScreenAxis * 10;

        if (iPDI != NULL)
        {
            int numCircleSides = 128;
            // Draw brush circle
            DrawCircle(iPDI, mousePosInWorldWithZ, mXScreenAxis, mYScreenAxis, brushCueColor, drawingTool->GetBrushInstance()->GetSizeModifier(), numCircleSides, SDPG_World, 0.01f);
        }

        mXScreenAxis /= mXScreenAxis.Size();
        mYScreenAxis /= mYScreenAxis.Size();
        mZScreenAxis /= mZScreenAxis.Size();
        //mZScreenAxis = mYScreenAxis.Cross(mXScreenAxis);

        //UE_LOG(LogTemp, Display, TEXT("mXScreenAxis: %lf, %lf, %lf"), mXScreenAxis.X, mXScreenAxis.Y, mXScreenAxis.Z);
        //UE_LOG(LogTemp, Display, TEXT("mYScreenAxis: %lf, %lf, %lf"), mYScreenAxis.X, mYScreenAxis.Y, mYScreenAxis.Z);
        //UE_LOG(LogTemp, Display, TEXT("mZScreenAxis: %lf, %lf, %lf, %lf"), mZScreenAxis.X, mZScreenAxis.Y, mZScreenAxis.Z, mZScreenAxis.Size());


        //FVector2f decalPixels = FVector2f(iViewport->GetMouseX() - iViewport->GetSizeXY().X / 2.f, iViewport->GetMouseY() - iViewport->GetSizeXY().Y / 2.f);
        //double angleX = FMath::DegreesToRadians((FMath::Sin((decalPixels.X / iViewport->GetSizeXY().X)*PI) * iView->FOV) / 2);
        //double angleY = FMath::DegreesToRadians((FMath::Sin((decalPixels.Y / iViewport->GetSizeXY().X)*PI) * iView->FOV) / 2);
        //double angleX = FMath::DegreesToRadians((decalPixels.X / iViewport->GetSizeXY().X) * iView->FOV);
        //double angleY = FMath::DegreesToRadians((decalPixels.Y / iViewport->GetSizeXY().X) * iView->FOV);
        //UE_LOG(LogTemp, Display, TEXT("angleX: %lf"), (FMath::Sin((decalPixels.X / iViewport->GetSizeXY().X) * PI) * iView->FOV) / 2);


        mFakeXScreenAxis = FVector( mXScreenAxis.X * FMath::Cos(angleX) - mXScreenAxis.Y * FMath::Sin(angleX), mXScreenAxis.X * FMath::Sin(angleX) - mXScreenAxis.Y * FMath::Cos(angleX), mXScreenAxis.Z );
        mFakeXScreenAxis /= mFakeXScreenAxis.Size();
        //mFakeXScreenAxis = mXScreenAxis - mFakeXScreenAxis;
        //FVector fakeYScreenAxis = FMath::Sin(angleX) - FMath::Cos(angleX);

        //UE_LOG(LogTemp, Display, TEXT("fakeScreenX: %lf, %lf, %lf, %lf"), fakeScreenX.X, fakeScreenX.Y, fakeScreenX.Z, fakeScreenX.Size());
        //UE_LOG(LogTemp, Display, TEXT("mFakeXScreenAxis: %lf, %lf, %lf, %lf"), mFakeXScreenAxis.X, mFakeXScreenAxis.Y, mFakeXScreenAxis.Z, mFakeXScreenAxis.Size());
        //UE_LOG(LogTemp, Display, TEXT("fakeYScreenAxis: %lf, %lf, %lf"), fakeYScreenAxis.X, fakeYScreenAxis.Y, fakeYScreenAxis.Z);


        //UE_LOG(LogTemp, Display, TEXT("mZScreenAxis: %lf, %lf, %lf"), mZScreenAxis.X, mZScreenAxis.Y, mZScreenAxis.Z);
        //UE_LOG(LogTemp, Display, TEXT("mMousePosInWorld: %lf, %lf, %lf"), mMousePosInWorld.X, mMousePosInWorld.Y, mMousePosInWorld.Z);

        //float t = (mZScreenAxis.X * originDeproj.X - mZScreenAxis.X * traceHitResult.Location.X + mZScreenAxis.Y * originDeproj.Y - mZScreenAxis.Y * traceHitResult.Location.Y + mZScreenAxis.Z * originDeproj.Z - mZScreenAxis.Z * traceHitResult.Location.Z) / (mZScreenAxis.X * mZScreenAxis.X + mZScreenAxis.Y * mZScreenAxis.Y + mZScreenAxis.Z * mZScreenAxis.Z);

        //mPointOnPlane = FVector(traceHitResult.Location.X + t * mZScreenAxis.X, traceHitResult.Location.Y + t * mZScreenAxis.Y, traceHitResult.Location.Z + t * mZScreenAxis.Z);
        //UE_LOG(LogTemp, Display, TEXT("t: %lf"), t);
        //UE_LOG(LogTemp, Display, TEXT("pointOnPlane: %lf, %lf, %lf"), mPointOnPlane.X, mPointOnPlane.Y, mPointOnPlane.Z);

        /*FVector2f decalPixels = FVector2f(iViewport->GetMouseX() - iViewport->GetSizeXY().X / 2.f, iViewport->GetMouseY() - iViewport->GetSizeXY().Y / 2.f);
        double angleX = FMath::DegreesToRadians(( decalPixels.X / iViewport->GetSizeXY().X ) * iView->FOV);
        double angleY = FMath::DegreesToRadians(( decalPixels.Y / iViewport->GetSizeXY().X ) * iView->FOV);

        UE_LOG(LogTemp, Display, TEXT("X: %lf"), (decalPixels.X / iViewport->GetSizeXY().X) * iView->FOV);
        UE_LOG(LogTemp, Display, TEXT("Y: %lf"), (decalPixels.Y / iViewport->GetSizeXY().X) * iView->FOV);

        mXScreenAxis.X = mXScreenAxis.X - (1 - FMath::Cos(angleX));
        mXScreenAxis.Y = mXScreenAxis.Y - FMath::Sin(angleX);
        mYScreenAxis.X = mYScreenAxis.X - (1 - FMath::Cos(angleY));
        mYScreenAxis.Y = mYScreenAxis.Y - FMath::Sin(angleY);
        mYScreenAxis.Z = mYScreenAxis.Z - FMath::Tan(angleY);

        if( mXScreenAxis.X > 1.f )
            mXScreenAxis.X-=2.f;

        if (mXScreenAxis.Y > 1.f)
            mXScreenAxis.Y-= 2.f;

        if (mXScreenAxis.X < -1.f)
            mXScreenAxis.X += 2.f;

        if (mXScreenAxis.Y < -1.f)
            mXScreenAxis.Y += 2.f;


        if (mYScreenAxis.X > 1.f)
            mYScreenAxis.X -= 2.f;

        if (mYScreenAxis.Y > 1.f)
            mYScreenAxis.Y -= 2.f;

        if (mYScreenAxis.X < -1.f)
            mYScreenAxis.X += 2.f;

        if (mYScreenAxis.Y < -1.f)
            mYScreenAxis.Y += 2.f;

        UE_LOG(LogTemp, Display, TEXT("mXScreenAxis: %lf, %lf, %lf"), mXScreenAxis.X, mXScreenAxis.Y, mXScreenAxis.Z);
        UE_LOG(LogTemp, Display, TEXT("mYScreenAxis: %lf, %lf, %lf"), mYScreenAxis.X, mYScreenAxis.Y, mYScreenAxis.Z);

        mXScreenAxis /= mXScreenAxis.Size();
        mYScreenAxis /= mYScreenAxis.Size();

        UE_LOG(LogTemp, Display, TEXT("SizeAxis: %lf, %lf"), mXScreenAxis.Size(), mYScreenAxis.Size());*/
        
        //mZScreenAxis = mYScreenAxis.Cross(mXScreenAxis);

        //UE_LOG(LogTemp, Display, TEXT("mZScreenAxis AFTER: %lf, %lf, %lf"), mZScreenAxis.X, mZScreenAxis.Y, mZScreenAxis.Z);

        //UE_LOG(LogTemp, Display, TEXT("SizeAxis: %lf, %lf"), mXScreenAxis.Size(), mYScreenAxis.Size());


        //Proj x between -1 and 1, 0 is center of viewport
        //Proj y between -1 and 1
        //Proj z ???
        //Proj w = shortest distance from cursor to hit location

        //float distZ = 1.f / proj.Z;
        //UE_LOG(LogTemp, Display, TEXT("mXScreenAxis: %lf, %lf, %lf"), mXScreenAxis.X, mXScreenAxis.Y, mXScreenAxis.Z);
        //UE_LOG(LogTemp, Display, TEXT("mYScreenAxis: %lf, %lf, %lf"), mYScreenAxis.X, mYScreenAxis.Y, mYScreenAxis.Z);
        //
        //UE_LOG(LogTemp, Display, TEXT("ViewportMouse: %ld, %ld"), iViewport->GetMouseX(), iViewport->GetMouseY());
        //UE_LOG(LogTemp, Display, TEXT("proj, %lf, %lf, %lf, %lf"), proj.X, proj.Y, proj.Z, proj.W );
        //UE_LOG(LogTemp, Display, TEXT("Location, %lf, %lf, %lf"), traceHitResult.Location.X, traceHitResult.Location.Y, traceHitResult.Location.Z );
        //UE_LOG(LogTemp, Display, TEXT("PaintRayStart, %lf, %lf, %lf"), paintRay.RayStart.X, paintRay.RayStart.Y, paintRay.RayStart.Z);
        //FVector locationInverse = meshAdapter->GetComponentToWorldMatrix().InverseTransformPosition(traceHitResult.Location);
        //UE_LOG(LogTemp, Display, TEXT("CM, %lf, %lf, %lf"), locationInverse.X, locationInverse.Y, locationInverse.Z);
        //FVector location = meshAdapter->GetComponentToWorldMatrix().TransformPosition(traceHitResult.Location);
        //UE_LOG(LogTemp, Display, TEXT("PM, %lf, %lf, %lf"), traceHitResult.Location.X - paintRay.RayStart.X, traceHitResult.Location.Y - paintRay.RayStart.Y, traceHitResult.Location.Z - paintRay.RayStart.Z);
        //
        //FVector differenceBetweenRayStartAndHitResult = FVector(traceHitResult.Location.X - paintRay.RayStart.X, traceHitResult.Location.Y - paintRay.RayStart.Y, traceHitResult.Location.Z - paintRay.RayStart.Z);

        //UE_LOG(LogTemp, Display, TEXT("%lf, %lf, %lf, %lf"), proj.X * distZ, proj.Y * distZ, 1.f / proj.Z, proj.W);
    }

}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::BuildPaintingTexture2DRenderTarget()
{
    if (mEditor->Component() == nullptr || mEditor->Texture() == nullptr)
        return;

    const int32 textureWidth = mEditor->Texture()->Source.GetSizeX();
    const int32 textureHeight = mEditor->Texture()->Source.GetSizeY();

    mPaintingTexture2DRenderTarget = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
    mPaintingTexture2DRenderTarget->ClearColor = FLinearColor(0, 0, 0, 0);
    mPaintingTexture2DRenderTarget->bNeedsTwoCopies = false;
    mPaintingTexture2DRenderTarget->InitCustomFormat(textureWidth, textureHeight, mEditor->Texture()->GetPixelFormat(), false);
    mPaintingTexture2DRenderTarget->UpdateResourceImmediate();
    mPaintingTexture2DRenderTarget->AddToRoot();


    mStrokeBufferRenderTarget2D = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
    mStrokeBufferRenderTarget2D->ClearColor = FLinearColor(0, 0, 0, 0);
    mStrokeBufferRenderTarget2D->bNeedsTwoCopies = false;
    mStrokeBufferRenderTarget2D->InitCustomFormat(textureWidth, textureHeight, mEditor->Texture()->GetPixelFormat(), false);
    mStrokeBufferRenderTarget2D->UpdateResourceImmediate();
    mStrokeBufferRenderTarget2D->AddToRoot();

    mSeamRenderTarget2D = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
    mSeamRenderTarget2D->ClearColor = FLinearColor(0, 0, 0, 0);
    mSeamRenderTarget2D->bNeedsTwoCopies = false;
    mSeamRenderTarget2D->InitCustomFormat(textureWidth, textureHeight, mEditor->Texture()->GetPixelFormat(), false);
    mSeamRenderTarget2D->UpdateResourceImmediate();
    mSeamRenderTarget2D->AddToRoot();


    const ERHIFeatureLevel::Type FeatureLevel = mEditor->Component()->GetWorld()->FeatureLevel;
    mEditor->Material()->OverrideTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, FeatureLevel);

    TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::GatherTextureTriangles(IMeshPaintGeometryAdapter* iAdapter, int32 iTriangleIndex, const int32 iVertexIndices[3], TArray<FTexturePaintTriangleInfo>* iTriangleInfo, TArray<FTexturePaintMeshSectionInfo>* iSectionInfos, int32 iUVChannelIndex)
{
    /** Retrieve triangles eligible for texture painting */
    bool bAdd = iSectionInfos->Num() == 0;
    for (const FTexturePaintMeshSectionInfo& sectionInfo : *iSectionInfos)
    {
        if (iTriangleIndex >= sectionInfo.FirstIndex && iTriangleIndex < sectionInfo.LastIndex)
        {
            bAdd = true;
            break;
        }
    }

    if (bAdd)
    {
        FTexturePaintTriangleInfo info;
        iAdapter->GetVertexPosition(iVertexIndices[0], info.TriVertices[0]);
        iAdapter->GetVertexPosition(iVertexIndices[1], info.TriVertices[1]);
        iAdapter->GetVertexPosition(iVertexIndices[2], info.TriVertices[2]);
        info.TriVertices[0] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[0]);
        info.TriVertices[1] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[1]);
        info.TriVertices[2] = iAdapter->GetComponentToWorldMatrix().TransformPosition(info.TriVertices[2]);
        iAdapter->GetTextureCoordinate(iVertexIndices[0], iUVChannelIndex, info.TriUVs[0]);
        iAdapter->GetTextureCoordinate(iVertexIndices[1], iUVChannelIndex, info.TriUVs[1]);
        iAdapter->GetTextureCoordinate(iVertexIndices[2], iUVChannelIndex, info.TriUVs[2]);
        iTriangleInfo->Add(info);
    }
}

TArray<::ULIS::FRectI> FOdysseyViewportDrawingEditorScreenBasedAdapter::GetMinimalRectanglesForTriangleSet(TArray<FTexturePaintTriangleInfo>& iTriangles, int iMaxWidth, int iMaxHeight)
{
    if( iTriangles.Num() == 0 )
        return TArray<::ULIS::FRectI>();

    TArray<::ULIS::FRectI> rects;

    //Get Biggest rectangle
    int minX = 100000000;
    int minY = 100000000;
    int maxX = 0;
    int maxY = 0;

    for (int i = 0; i < iTriangles.Num(); i++)
    {
        minX = ::ULIS::FMath::Min4(double(minX), iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X);
        minY = ::ULIS::FMath::Min4(double(minY), iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y);
        maxX = ::ULIS::FMath::Max4(double(maxX), iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X);
        maxY = ::ULIS::FMath::Max4(double(maxY), iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y);
    }
    rects.Add( ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY) );
    return rects;

    //First step, we get all bounding rectangles from the triangles
    for (int i = 0; i < iTriangles.Num(); i++)
    {
        rects.Add(::ULIS::FRectI::FromMinMax( FMath::Max( FMath::Min3(iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X) - 1, 0 ),
                                              FMath::Max( FMath::Min3(iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y) - 1, 0 ),
                                              FMath::Min( FMath::Max3(iTriangles[i].TrianglePoints[0].X, iTriangles[i].TrianglePoints[1].X, iTriangles[i].TrianglePoints[2].X) + 1, iMaxWidth ),
                                              FMath::Min( FMath::Max3(iTriangles[i].TrianglePoints[0].Y, iTriangles[i].TrianglePoints[1].Y, iTriangles[i].TrianglePoints[2].Y) + 1, iMaxHeight ) ));
    }
    
    TArray< ::ULIS::FRectI > finalRects;
    ::ULIS::TArray<::ULIS::FRectI> rectsToExclude;
    if( rects.Num() != 0 )
    {
        for (int i = 0; i < rects.Num()-1; i++)
        {
            rectsToExclude.PushBack(rects[i]);

            ::ULIS::TArray<::ULIS::FRectI> exclusionRectsTemp;
            for (int k = 0; k < rectsToExclude.Size(); k++)
            {
                rectsToExclude[k].Exclusion(rects[i+1], &exclusionRectsTemp);

                if ( exclusionRectsTemp[0] != rectsToExclude[k] )
                {
                    rectsToExclude.Erase(k);
                    int sizeOfRectsNotEmpty = 0;

                    for (int l = 0; l < exclusionRectsTemp.Size(); l++)
                    {
                        if( exclusionRectsTemp[l].Area() > 0 )
                        {
                            rectsToExclude.Insert(k + sizeOfRectsNotEmpty, exclusionRectsTemp[l]);
                            sizeOfRectsNotEmpty++;
                        }
                    }
                    k = k + sizeOfRectsNotEmpty - 1;
                }
            }
        }
        rectsToExclude.PushBack(rects.Last());
    }

        
    for (int j = 0; j < rectsToExclude.Size(); j++)
    {
        if ( rectsToExclude[j].Area() > 0 )
        {
            finalRects.Add(rectsToExclude[j]);
        }
    }

    
    return finalRects;
}

::ULIS::FEvent FOdysseyViewportDrawingEditorScreenBasedAdapter::StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = nullptr;
    if (mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());

    if( !drawingTool )
        return iStampParams.mEvent;

    if (!GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->HasFocus())
        return drawingTool->GetBrushInstance()->StampInternal(iStampParams);

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return iStampParams.mEvent;

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;


    FEditorViewportClient* viewportClient = (FEditorViewportClient*)mLastKnownViewport->GetClient();
    // Compute a world space ray from the screen space mouse coordinates
    FSceneViewFamilyContext viewFamily(FSceneViewFamily::ConstructionValues(
        viewportClient->Viewport,
        viewportClient->GetScene(),
        viewportClient->EngineShowFlags)
        .SetRealtimeUpdate(viewportClient->IsRealtime()));
    FSceneView* view = viewportClient->CalcSceneView(&viewFamily);

    //We need to shift the coordinates of the stamp, because here, we're working in viewport coordinates, not texture coordinates (1 px in viewport != 1px in coord)
    iStampParams.mPosition.x = iStampParams.mPosition.x + iStampParams.mBlock->Width() / 2.f;
    iStampParams.mPosition.y = iStampParams.mPosition.y + iStampParams.mBlock->Height() / 2.f;

    FHitResult traceHitResult(1.0f);
    const FVector rayEnd(mMousePosInWorld + mZScreenAxis * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(traceHitResult, mMousePosInWorld, rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    //FVector brushXAxis, brushYAxis;
    FVector brushXAxis = FVector( 1, 0, 0 );
    FVector brushYAxis = FVector( 0, 1, 0 );
    FVector brushZAxis = FVector( 0, 0, 1 );
    //traceHitResult.Normal.FindBestAxisVectors(brushXAxis,brushYAxis);
    const FMatrix worldToBrushMatrix = FMatrix(brushXAxis, brushYAxis, brushZAxis, traceHitResult.Location).Inverse();
    //UE_LOG(LogTemp, Display, TEXT("traceResult: %lf, %lf, %lf, %lf"), traceHitResult.Location.X, traceHitResult.Location.Y, traceHitResult.Location.Z, traceHitResult.Distance)

    // Convert trace to UV position
    FVector2D coord;
    if (UGameplayStatics::FindCollisionUV(traceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), coord))
    {
        //And here we shift back the coordinates, because we converted it to texture coordinates
        iStampParams.mPosition.x = coord.X * mEditor->Texture()->GetSurfaceWidth() - iStampParams.mBlock->Width() / 2.f;
        iStampParams.mPosition.y = coord.Y * mEditor->Texture()->GetSurfaceHeight() - iStampParams.mBlock->Height() / 2.f;

        mStrokeBufferTexture2D = NewUncompressedTextureFromBlockData(iStampParams.mBlock);
    }

    if( !mStrokeBufferTexture2D )
        return iStampParams.mEvent;

    TRefCountPtr< FOdysseyScreenPaintBatchedElementParameters > screenPaintBatchedElementParameters(new FOdysseyScreenPaintBatchedElementParameters());
    {
        screenPaintBatchedElementParameters->ShaderParams.Stroke2D = mStrokeBufferTexture2D;
        screenPaintBatchedElementParameters->ShaderParams.WorldToBrushMatrix = worldToBrushMatrix;
        screenPaintBatchedElementParameters->ShaderParams.TextureHitPoint = FVector2D( iStampParams.mPosition.x, iStampParams.mPosition.y );
        screenPaintBatchedElementParameters->ShaderParams.StampQuality = 1;
        screenPaintBatchedElementParameters->ShaderParams.xScreenAxis = mXScreenAxis;
        screenPaintBatchedElementParameters->ShaderParams.yScreenAxis = mYScreenAxis;
    }

    const ERHIFeatureLevel::Type featureLevel = mEditor->Component()->GetWorld()->FeatureLevel;

    FTextureRenderTargetResource* strokeRenderTargetResource = mStrokeBufferRenderTarget2D->GameThread_GetRenderTargetResource();
    FCanvas strokePaintCanvas(strokeRenderTargetResource, nullptr, 0, 0, 0, featureLevel);
    FHitProxyId strokePaintHitProxyId = strokePaintCanvas.GetHitProxyId();
    FBatchedElements* strokePaintBatchedElements = strokePaintCanvas.GetBatchedElements(FCanvas::ET_Triangle, screenPaintBatchedElementParameters, nullptr, SE_BLEND_Opaque);

    //Todo: make ellipseIntersectTriangles ?
    const TArray<uint32> vertexIndices = meshAdapter->GetMeshIndices();
    uint32 triIndices = vertexIndices.Num() / 3;
    TArray<FTexturePaintTriangleInfo> triangleInfo;
    TArray<FTexturePaintMeshSectionInfo> sectionInfo;
    for (uint32 i = 0; i < triIndices; i++)
    {
        const int32 indices[3] = { int32(vertexIndices[i * 3]), int32(vertexIndices[i * 3 + 1]), int32(vertexIndices[i * 3 + 2]) };
        GatherTextureTriangles(meshAdapter.Get(), i, indices, &triangleInfo, &sectionInfo, mEditor->GetUVIndexUsedByCurrentTexture());
    }

    // Process the influenced triangles - storing off a large list is much slower than processing in a single loop
    for (int32 CurIndex = 0; CurIndex < triangleInfo.Num(); ++CurIndex)
    {
        FTexturePaintTriangleInfo& CurTriangle = triangleInfo[CurIndex];
        FTexturePaintTriangleInfo CurTriangleCopy = triangleInfo[CurIndex];

        for (int32 TriVertexNum = 0; TriVertexNum < 3; ++TriVertexNum)
        {
            CurTriangle.TrianglePoints[TriVertexNum].X = CurTriangleCopy.TriUVs[TriVertexNum].X * mStrokeBufferRenderTarget2D->GetSurfaceWidth();
            CurTriangle.TrianglePoints[TriVertexNum].Y = CurTriangleCopy.TriUVs[TriVertexNum].Y * mStrokeBufferRenderTarget2D->GetSurfaceHeight();
        }

        // Vertex positions
        FVector4 Vert0(CurTriangle.TrianglePoints[0].X, CurTriangle.TrianglePoints[0].Y, 0, 1);
        FVector4 Vert1(CurTriangle.TrianglePoints[1].X, CurTriangle.TrianglePoints[1].Y, 0, 1);
        FVector4 Vert2(CurTriangle.TrianglePoints[2].X, CurTriangle.TrianglePoints[2].Y, 0, 1);

        // Vertex color
        FLinearColor Col0(CurTriangle.TriVertices[0].X, CurTriangle.TriVertices[0].Y, CurTriangle.TriVertices[0].Z);
        FLinearColor Col1(CurTriangle.TriVertices[1].X, CurTriangle.TriVertices[1].Y, CurTriangle.TriVertices[1].Z);
        FLinearColor Col2(CurTriangle.TriVertices[2].X, CurTriangle.TriVertices[2].Y, CurTriangle.TriVertices[2].Z);

        // Brush Paint triangle
        {
            int32 V0 = strokePaintBatchedElements->AddVertex(Vert0, CurTriangle.TriUVs[0], Col0, strokePaintHitProxyId);
            int32 V1 = strokePaintBatchedElements->AddVertex(Vert1, CurTriangle.TriUVs[1], Col1, strokePaintHitProxyId);
            int32 V2 = strokePaintBatchedElements->AddVertex(Vert2, CurTriangle.TriUVs[2], Col2, strokePaintHitProxyId);

            strokePaintBatchedElements->AddTriangle(V0, V1, V2, screenPaintBatchedElementParameters, SE_BLEND_Opaque);
        }
    }

    // Tell the rendering thread to draw any remaining batched elements
    {
        strokePaintCanvas.Flush_GameThread(true);
    }

    {
        ENQUEUE_RENDER_COMMAND(UpdateOdysseyScreenPaintRTCommandSeams)(
            [this, strokeRenderTargetResource](FRHICommandListImmediate& RHICmdList)
            {
                // Copy (resolve) the rendered image from the frame buffer to its render target texture
                RHICmdList.CopyToResolveTarget(
                    strokeRenderTargetResource->GetRenderTargetTexture(),		// Source texture
                    strokeRenderTargetResource->TextureRHI,
                    FResolveParams());									// Resolve parameters
            });
    }

    //Todo: if is seam drawing enabled
    if (true)
    {
        const float MinU = 0.0f;
        const float MinV = 0.0f;
        const float MaxU = 1.0f;
        const float MaxV = 1.0f;
        const float MinX = 0.0f;
        const float MinY = 0.0f;
        const float MaxX = mStrokeBufferRenderTarget2D->GetSurfaceWidth();
        const float MaxY = mStrokeBufferRenderTarget2D->GetSurfaceHeight();

        // Create a canvas for the render target.

        TRefCountPtr< FOdysseyScreenSeamsPaintBatchedElementParameters > seamsPaintBatchedElementParameters(new FOdysseyScreenSeamsPaintBatchedElementParameters());
        {
            seamsPaintBatchedElementParameters->ShaderParams.Stroke2D = mStrokeBufferRenderTarget2D;
            seamsPaintBatchedElementParameters->ShaderParams.SeamMaskRenderTarget = mSeamRenderTarget2D;
            seamsPaintBatchedElementParameters->ShaderParams.WidthPixelOffset = (float)(1.0f / mStrokeBufferRenderTarget2D->GetSurfaceWidth());
            seamsPaintBatchedElementParameters->ShaderParams.HeightPixelOffset = (float)(1.0f / mStrokeBufferRenderTarget2D->GetSurfaceHeight());
        }

        // Draw a quad to copy the texture over to the render target
        TArray< FCanvasUVTri >	TriangleList;
        FCanvasUVTri SingleTri;
        SingleTri.V0_Pos = FVector2D(MinX, MinY);
        SingleTri.V0_UV = FVector2D(MinU, MinV);
        SingleTri.V0_Color = FLinearColor::White;

        SingleTri.V1_Pos = FVector2D(MaxX, MinY);
        SingleTri.V1_UV = FVector2D(MaxU, MinV);
        SingleTri.V1_Color = FLinearColor::White;

        SingleTri.V2_Pos = FVector2D(MaxX, MaxY);
        SingleTri.V2_UV = FVector2D(MaxU, MaxV);
        SingleTri.V2_Color = FLinearColor::White;
        TriangleList.Add(SingleTri);

        SingleTri.V0_Pos = FVector2D(MaxX, MaxY);
        SingleTri.V0_UV = FVector2D(MaxU, MaxV);
        SingleTri.V0_Color = FLinearColor::White;

        SingleTri.V1_Pos = FVector2D(MinX, MaxY);
        SingleTri.V1_UV = FVector2D(MinU, MaxV);
        SingleTri.V1_Color = FLinearColor::White;

        SingleTri.V2_Pos = FVector2D(MinX, MinY);
        SingleTri.V2_UV = FVector2D(MinU, MinV);
        SingleTri.V2_Color = FLinearColor::White;
        TriangleList.Add(SingleTri);

        FCanvasTriangleItem TriItemList(TriangleList, nullptr);
        TriItemList.BatchedElementParameters = seamsPaintBatchedElementParameters;
        TriItemList.BlendMode = SE_BLEND_Opaque;
        strokePaintCanvas.DrawItem(TriItemList);

        {
            strokePaintCanvas.Flush_GameThread(true);
        }
    }


    TArray<::ULIS::FRectI> rects;
    rects = GetMinimalRectanglesForTriangleSet(triangleInfo, mStrokeBufferRenderTarget2D->GetSurfaceWidth(), mStrokeBufferRenderTarget2D->GetSurfaceHeight());

    mPixelFence.BeginFence();
    mPixelFence.Wait();


    for (int i = 0; i < rects.Num(); i++)
    {
        ENQUEUE_RENDER_COMMAND(UpdateOdysseyScreenPaintRTCommand)(
            [this, strokeRenderTargetResource, rects, i](FRHICommandListImmediate& RHICmdList)
            {
                if( mColorData.Num() < rects.Num() )
                    mColorData.Add(TArray<FLinearColor>());

                RHICmdList.ReadSurfaceData(strokeRenderTargetResource->GetRenderTargetTexture(), FIntRect(rects[i].x, rects[i].y, rects[i].x + rects[i].w, rects[i].y + rects[i].h), mColorData[i], FReadSurfaceDataFlags());
            });
    }

    mPixelFence.BeginFence();
    mPixelFence.Wait();

    //Resets the color of all pixels in mStrokeBufferRenderTarget2D for next stamp. Is there a better way to do it ?
    mStrokeBufferRenderTarget2D->UpdateResource();

    TArray<::ULIS::FEvent> eventBlend;
    ::ULIS::eFormat target_format = ::ULIS::eFormat::Format_RGBAF;
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(target_format);


    for (int i = 0; i < rects.Num(); i++)
    {
        TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> blockToStamp = MakeShared<::ULIS::FBlock>(static_cast<uint8*>(static_cast<void*>(mColorData[i].GetData())), rects[i].w, rects[i].h, ::ULIS::eFormat::Format_RGBAF);

        ULIS::FVec2F position = rects[i].Position();
        rects[i].x = 0;
        rects[i].y = 0;

        //Convert the destination if needed and Blend the folderBlock
        TArray<::ULIS::FEvent> eventConvertAndExecute = ULISUtils::ConvertAndExecute(drawingTool->GetBrushInstance()->GetBlock(), blockToStamp->Format(), rects[i], position, { iStampParams.mEvent },
            [this, &blockToStamp, &ctx, iStampParams](TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> ioDest, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) -> TArray<::ULIS::FEvent>
            {
                ::ULIS::FEvent eventBlend = FULISEventBuilder().RetainBlock(blockToStamp).Build();

                if (iStampParams.mAntiAliasing)
                {
                    ctx.BlendAA(
                        *blockToStamp
                        , *ioDest
                        , iRect
                        , iPos
                        , ::ULIS::eBlendMode(iStampParams.mBlendingMode)
                        , ::ULIS::eAlphaMode(iStampParams.mAlphaMode)
                        , FMath::Clamp(iStampParams.mFlow, 0.f, 1.f)
                        , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
                        , iWaitList.Num()
                        , iWaitList.GetData()
                        , &eventBlend
                    );
                }
                else
                {
                    ctx.Blend(
                        *blockToStamp,
                        *ioDest,
                        iRect,
                        iPos,
                        ::ULIS::eBlendMode(iStampParams.mBlendingMode),
                        ::ULIS::eAlphaMode(iStampParams.mAlphaMode),
                        FMath::Clamp(iStampParams.mFlow, 0.f, 1.f),
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        iWaitList.Num(),
                        iWaitList.GetData(),
                        &eventBlend
                    );
                }

                return { eventBlend };
            }
        );
        eventBlend.Append(eventConvertAndExecute);

        ctx.Flush();

        ::ULIS::FRectI invalidRectI(FMath::FloorToInt(position.x), FMath::FloorToInt(position.y), FMath::CeilToInt(rects[i].w + 2.f), FMath::CeilToInt(rects[i].h + 2.f));
        invalidRectI = invalidRectI & drawingTool->GetBrushInstance()->GetBlock()->Rect();
        if (invalidRectI.w > 0 && invalidRectI.h > 0)
            drawingTool->GetBrushInstance()->GetInvalidRects()->Add(invalidRectI);
    }

    ::ULIS::FEvent eventStampInternal;
    ctx.Dummy_OP(eventBlend.Num(), eventBlend.GetData(), &eventStampInternal);

    return eventStampInternal;
}

void FOdysseyViewportDrawingEditorScreenBasedAdapter::BindStampBrushInstance(UOdysseyBrushAssetBase* iBindBrush)
{
    IOdysseyViewportDrawingEditorAdapter::BindStampBrushInstance( iBindBrush );
    if (iBindBrush)
    {
        iBindBrush->AddContext( new FOdysseyViewportEditorBrushContext() );
    }
}

#undef LOCTEXT_NAMESPACE
