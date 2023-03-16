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
    
        const ERHIFeatureLevel::Type featureLevel = mEditor->Component()->GetWorld()->FeatureLevel;
        mEditor->Material()->OverrideTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, featureLevel);
        
        //A simple copy is all we need for the texture based algorithm
        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mEditor->Texture(), mPaintingTexture2DRenderTarget, featureLevel);
        mState = eState::kIdleReady;
    }    
    
    //We're using true pixel value for this adapter, so we put 0 in meshMaxSize
    mEditor->GetGUI()->GetTopTab()->SetMeshMaxSize(0);

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

    checkf(drawingTool != nullptr, TEXT("Drawing tool is non existent"));

    //Check if we're currently drawing on the level editor viewport. If not, we're drawing normally
    if (!GCurrentLevelEditingViewportClient->GetEditorViewportWidget()->GetSceneViewport()->HasFocus())
        return drawingTool->GetBrushInstance()->StampInternal(iStampParams);
    
    // Check if we have a proper meshComponent to paint on
    const TSharedPtr<IMeshPaintGeometryAdapter>* meshAdapterPtr = mEditor->ComponentToAdapterMap().Find(mEditor->Component());
    if (!meshAdapterPtr)
        return drawingTool->GetBrushInstance()->StampInternal(iStampParams);


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

    const FViewportCursorLocation mouseViewportRay(view, viewportClient, iStampParams.mPosition.x, iStampParams.mPosition.y);

    FHitResult traceHitResult(1.0f);
    const FVector rayEnd(mouseViewportRay.GetOrigin() + mouseViewportRay.GetDirection() * HALF_WORLD_MAX);

    meshAdapter->LineTraceComponent(traceHitResult, mouseViewportRay.GetOrigin(), rayEnd, FCollisionQueryParams(SCENE_QUERY_STAT(Paint), true));

    // Convert trace to UV position
    FVector2D coord;
    if (UGameplayStatics::FindCollisionUV(traceHitResult, mEditor->GetUVIndexUsedByCurrentTexture(), coord))
    {
        //And here we shift back the coordinates, because we converted it to texture coordinates
        iStampParams.mPosition.x = coord.X * mEditor->Texture()->GetSurfaceWidth() - iStampParams.mBlock->Width() / 2.f;
        iStampParams.mPosition.y = coord.Y * mEditor->Texture()->GetSurfaceHeight() - iStampParams.mBlock->Height() / 2.f;
    }
    else
    {
        return iStampParams.mEvent;
    }

    return drawingTool->GetBrushInstance()->StampInternal(iStampParams);
}

#undef LOCTEXT_NAMESPACE
