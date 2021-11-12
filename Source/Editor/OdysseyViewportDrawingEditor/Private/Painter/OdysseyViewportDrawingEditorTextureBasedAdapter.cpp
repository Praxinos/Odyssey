// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorTextureBasedAdapter.h"
#include "MeshPaintHelpers.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorTextureBasedAdapter"

FOdysseyViewportDrawingEditorTextureBasedAdapter::~FOdysseyViewportDrawingEditorTextureBasedAdapter()
{
    if( mEditor->Component() && mEditor->Texture() )
        IMeshPaintGeometryAdapter::DefaultApplyOrRemoveTextureOverride(mEditor->Component(), mEditor->Texture(), nullptr);
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
        mPaintingTexture2DRenderTarget->InitCustomFormat(textureWidth,textureHeight,PF_R8G8B8A8,false);
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
        FOdysseyStrokePoint currentStrokePoint = mCurrentStrokeRay.mStrokePoint;
        FOdysseyStrokePoint lastStrokePoint = mLastStrokeRay.mStrokePoint;

        currentStrokePoint.x = coord.X * mEditor->Texture()->GetSurfaceWidth();
        currentStrokePoint.y = coord.Y * mEditor->Texture()->GetSurfaceHeight();
        lastStrokePoint.x = coord.X * mEditor->Texture()->GetSurfaceWidth();
        lastStrokePoint.y = coord.Y * mEditor->Texture()->GetSurfaceHeight();
        
        mEditor->PaintEngine()->BeginStroke(currentStrokePoint, lastStrokePoint);
    }

    //A simple copy is all we need for the texture based algorithm
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
        mEditor->PaintEngine()->PushStroke(mCurrentStrokeRay.mStrokePoint);
    }

    //A simple copy is all we need for the texture based algorithm
    TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::FinishPainting()
{
    mEditor->PaintEngine()->EndStroke();

    //A simple copy is all we need for the texture based algorithm
    TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::Tick()
{
    mEditor->PaintEngine()->Tick();
    if( mEditor->Texture() && mPaintingTexture2DRenderTarget )
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

#undef LOCTEXT_NAMESPACE
