// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorTextureBasedAdapter.h"
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

    IOdysseyViewportDrawingEditorAdapter::PrepareAdapterForPainting();

    /*
    if (mEditor->Material())
    {
        UE_LOG(LogTemp, Warning, TEXT("BeforeOverride: GetUsedTextures:"));
        TArray<UTexture*> Textures;
        mEditor->Material()->GetUsedTextures(Textures, EMaterialQualityLevel::Num, true, GMaxRHIFeatureLevel, true);
        UE_LOG(LogTemp, Warning, TEXT("Does material Use Texture %d"), DoesMaterialUseTexture(mEditor->Material(), mEditor->Texture()));
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
        
        mPaintingTexture2DRenderTarget = NewObject<UTextureRenderTarget2D>(GetTransientPackage(), NAME_None, RF_Transient);
        mPaintingTexture2DRenderTarget->ClearColor = FLinearColor(0, 0, 0, 0);
        mPaintingTexture2DRenderTarget->bNeedsTwoCopies = false;
        mPaintingTexture2DRenderTarget->InitAutoFormat(textureWidth, textureHeight);
        mPaintingTexture2DRenderTarget->UpdateResourceImmediate();
        mPaintingTexture2DRenderTarget->AddToRoot();
        
        mPreviousMipSettings = mEditor->Texture()->MipGenSettings;
        mEditor->Texture()->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
        mEditor->Texture()->UpdateResource();
        FTextureCompilingManager::Get().FinishCompilation({ mEditor->Texture() });
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);

        mState = eState::kIdleReady;
    }    
    
    //We're using true pixel value for this adapter, so we put 0 in meshMaxSize
    //mEditor->GetGUI()->GetTopTab()->SetMeshMaxSize(0);
    mEditor->GetRasterDrawingTool()->SetBaseSize(0);

    /*
    if (mEditor->Material())
    {
        UE_LOG(LogTemp, Warning, TEXT("AfterOverride: GetUsedTextures:"));
        TArray<UTexture*> Textures;
        mEditor->Material()->GetUsedTextures(Textures, EMaterialQualityLevel::Num, true, GMaxRHIFeatureLevel, true);
        UE_LOG(LogTemp, Warning, TEXT("Does material Use Texture %d"), DoesMaterialUseTexture(mEditor->Material(), mEditor->Texture()));
        for (int i = 0; i < Textures.Num(); i++)
        {
            UE_LOG(LogTemp, Warning, TEXT("%s"), *(Textures[i]->GetFName()).ToString())
        }
        UE_LOG(LogTemp, Warning, TEXT("----------------------------------"))
    }*/
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::StartPainting()
{
    mEditor->GetSelectedTool()->OnMouseDown(mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton);

    //A simple copy is all we need for the texture based algorithm
    if( mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::Paint()
{
    mEditor->GetSelectedTool()->OnMouseDrag(mCurrentStrokeRay.mPoint);

    //A simple copy is all we need for the texture based algorithm
    if (mPaintingTexture2DRenderTarget)
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::FinishPainting()
{
    mStopDrawing = false;
    mEditor->GetSelectedTool()->OnMouseUp(mCurrentStrokeRay.mPoint, EKeys::LeftMouseButton);

    //A simple copy is all we need for the texture based algorithm
    if ( mPaintingTexture2DRenderTarget )
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::Tick(float iDelta)
{
    if( mEditor->Texture() && mPaintingTexture2DRenderTarget )
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, GEditor->GetEditorWorldContext().World()->FeatureLevel);
}

void FOdysseyViewportDrawingEditorTextureBasedAdapter::RenderInteractorWidget(const FSceneView* iView, FViewport* iViewport, FPrimitiveDrawInterface* iPDI)
{
}

::ULIS::FEvent FOdysseyViewportDrawingEditorTextureBasedAdapter::StampOverride(UOdysseyBrushAssetBase::FStampParams iStampParams)
{
    UOdysseyPainterEditorRasterDrawingTool* drawingTool = nullptr;
    if (mEditor->GetSelectedTool()->IsA(UOdysseyPainterEditorRasterDrawingTool::StaticClass()))
        drawingTool = Cast<UOdysseyPainterEditorRasterDrawingTool>(mEditor->GetSelectedTool());

    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());

    TSharedPtr<IMeshPaintGeometryAdapter> meshAdapter = *meshAdapterPtr;

    FHitResult lastTraceHitResult(1.0f);
    const FVector lastRayEnd(mLastStrokeRay.mRayOrigin + mLastStrokeRay.mRayDirection * HALF_WORLD_MAX);

    FHitResult currentTraceHitResult(1.0f);
    const FVector currentRayEnd(mCurrentStrokeRay.mRayOrigin + mCurrentStrokeRay.mRayDirection * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(lastTraceHitResult, mLastStrokeRay.mRayOrigin, lastRayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));
    meshAdapter->LineTraceComponent(currentTraceHitResult, mCurrentStrokeRay.mRayOrigin, currentRayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    // Convert trace to UV position
    FVector2D lastCoord;
    FVector2D currentCoord;
    UGameplayStatics::FindCollisionUV(lastTraceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), lastCoord);
    UGameplayStatics::FindCollisionUV(currentTraceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), currentCoord);

    if (::ULIS::FMath::Dist(lastCoord.X, lastCoord.Y, currentCoord.X, currentCoord.Y) > 0.1f)
    {
        mStopDrawing = true;
        return iStampParams.mEvent;
    }

    if(drawingTool && mStopDrawing == false)
        return drawingTool->GetBrushInstance()->StampInternal(iStampParams);
    else
        return iStampParams.mEvent;

}

#undef LOCTEXT_NAMESPACE
