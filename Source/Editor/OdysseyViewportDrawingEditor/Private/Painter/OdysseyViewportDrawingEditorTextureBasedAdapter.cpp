// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorTextureBasedAdapter.h"
#include "Tools/DrawingTool/OdysseyDrawingTool.h"
#include "MeshPaintHelpers.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorTextureBasedAdapter"

FOdysseyViewportDrawingEditorTextureBasedAdapter::~FOdysseyViewportDrawingEditorTextureBasedAdapter()
{
    RemoveTextureOverride();
}

FOdysseyViewportDrawingEditorTextureBasedAdapter::FOdysseyViewportDrawingEditorTextureBasedAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor) :
    IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(iEditor)
{
    PrepareAdapterForPainting();
}


void FOdysseyViewportDrawingEditorTextureBasedAdapter::PrepareAdapterForPainting()
{
    if( mEditor->Component() == nullptr || mEditor->Texture() == nullptr )
        return;

    if (mEditor->Texture()->MipGenSettings == TextureMipGenSettings::TMGS_NoMipmaps)
    {
        mState = eState::kIdleReady;
        return;
    }

    /*
    if (mEditor->Material())
    {
        UE_LOG(LogTemp, Warning, TEXT("BeforeOverride: GetUsedTextures:"));
        TArray<UTexture*> Textures;
        mEditor->Material()->GetUsedTextures(Textures, EMaterialQualityLevel::Num, true, GMaxRHIFeatureLevel, true);
        UE_LOG(LogTemp, Warning, TEXT("%d"), DoesMaterialUseTexture(mEditor->Material(), mEditor->Texture()));
        for (int i = 0; i < Textures.Num(); i++)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s"), *(Textures[i]->GetFName()).ToString())
        }
        UE_LOG(LogTemp, Warning, TEXT("----------------------------------"))
    }*/

    if( mState == eState::kIdle )
    {
        const int32 textureWidth = mEditor->Texture()->Source.GetSizeX();
        const int32 textureHeight = mEditor->Texture()->Source.GetSizeY();
        
        mPaintingTexture2DRenderTarget = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),NAME_None,RF_Transient);
        mPaintingTexture2DRenderTarget->AddToRoot();
        mPaintingTexture2DRenderTarget->InitCustomFormat(textureWidth,textureHeight,mEditor->Texture()->GetPixelFormat(),false);
        mPaintingTexture2DRenderTarget->UpdateResourceImmediate();

        //IMeshPaintGeometryAdapter::DefaultApplyOrRemoveTextureOverride(mEditor->Component(), mEditor->Texture(), mPaintingTexture2DRenderTarget);
        const ERHIFeatureLevel::Type FeatureLevel = mEditor->Component()->GetWorld()->FeatureLevel;
        mEditor->Material()->OverrideTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, FeatureLevel);

        //A simple copy is all we need for the texture based algorithm
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
        mState = eState::kIdleReady;
    }    
    
    /*
    if (mEditor->Material())
    {
        UE_LOG(LogTemp, Warning, TEXT("AfterOverride: GetUsedTextures:"));
        TArray<UTexture*> Textures;
        mEditor->Material()->GetUsedTextures(Textures, EMaterialQualityLevel::Num, true, GMaxRHIFeatureLevel, true);
        UE_LOG(LogTemp, Warning, TEXT("%d"), DoesMaterialUseTexture(mEditor->Material(), mEditor->Texture()));
        for (int i = 0; i < Textures.Num(); i++)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s"), *(Textures[i]->GetFName()).ToString())
        }
        UE_LOG(LogTemp, Warning, TEXT("----------------------------------"))
    }*/
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::StartPainting()
{
    // Check if we have a proper meshComponent to paint on
    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return;

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    // Ray trace
    FHitResult traceHitResult(1.0f);
    const FVector rayEnd(mCurrentStrokeRay.mRayOrigin + mCurrentStrokeRay.mRayDirection * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(traceHitResult, mCurrentStrokeRay.mRayOrigin, rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    // Convert trace to UV position
    FVector2D coord;
    if (UGameplayStatics::FindCollisionUV(traceHitResult, 0, coord))
    {
        FOdysseyPoint currentStrokePoint = mCurrentStrokeRay.mStrokePoint;
        FOdysseyPoint lastStrokePoint = mLastStrokeRay.mStrokePoint;

        currentStrokePoint.x = coord.X * mEditor->Texture()->GetSurfaceWidth();
        currentStrokePoint.y = coord.Y * mEditor->Texture()->GetSurfaceHeight();
        lastStrokePoint.x = coord.X * mEditor->Texture()->GetSurfaceWidth();
        lastStrokePoint.y = coord.Y * mEditor->Texture()->GetSurfaceHeight();

        currentStrokePoint.ComputeRelativeParameters(lastStrokePoint);

        //ES: That feels weird, as we don't have the "PointInViewport", we only have the "PointInTexture", and we don't know what mouse button is being pressed.
        mEditor->GetSelectedTool()->OnMouseDown(currentStrokePoint, currentStrokePoint, EKeys::LeftMouseButton);
    }

    //A simple copy is all we need for the texture based algorithm
    if( mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::Paint()
{
    // Check if we have a proper meshComponent to paint on
    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return;

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    // Ray trace
    FHitResult traceHitResult(1.0f);
    const FVector rayEnd(mCurrentStrokeRay.mRayOrigin + mCurrentStrokeRay.mRayDirection * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(traceHitResult, mCurrentStrokeRay.mRayOrigin, rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    // Convert trace to UV position
    FVector2D coord;
    if (UGameplayStatics::FindCollisionUV(traceHitResult, 0, coord))
    {
        mCurrentStrokeRay.mStrokePoint.x = coord.X * mEditor->Texture()->GetSurfaceWidth();
        mCurrentStrokeRay.mStrokePoint.y = coord.Y * mEditor->Texture()->GetSurfaceHeight();
        //ES: That feels weird, as we don't have the "PointInViewport", we only have the "PointInTexture", and we don't know what mouse button is being pressed.
        mEditor->GetSelectedTool()->OnMouseDrag(mCurrentStrokeRay.mStrokePoint, mCurrentStrokeRay.mStrokePoint);
        //mEditor->StrokeEngine()->To(mCurrentStrokeRay.mStrokePoint);
    }

    //A simple copy is all we need for the texture based algorithm
    if (mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::FinishPainting()
{
    //ES: That feels weird, as we don't have the "PointInViewport", we only have the "PointInTexture", and we don't know what mouse button is being pressed.
    mEditor->GetSelectedTool()->OnMouseUp(mCurrentStrokeRay.mStrokePoint, mCurrentStrokeRay.mStrokePoint, EKeys::LeftMouseButton);
    //mEditor->StrokeEngine()->End();

    //A simple copy is all we need for the texture based algorithm
    if ( mPaintingTexture2DRenderTarget )
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::Tick()
{
    if( mEditor->Texture() && mPaintingTexture2DRenderTarget )
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

#undef LOCTEXT_NAMESPACE
