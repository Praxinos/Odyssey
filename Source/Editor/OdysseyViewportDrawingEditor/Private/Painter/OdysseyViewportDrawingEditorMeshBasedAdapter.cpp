// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyViewportDrawingEditorMeshBasedAdapter.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorMeshBasedAdapter"

FOdysseyViewportDrawingEditorMeshBasedAdapter::~FOdysseyViewportDrawingEditorMeshBasedAdapter()
{

}

FOdysseyViewportDrawingEditorMeshBasedAdapter::FOdysseyViewportDrawingEditorMeshBasedAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor) :
    IOdysseyViewportDrawingEditorAdapter::IOdysseyViewportDrawingEditorAdapter(iEditor)
{
}


void FOdysseyViewportDrawingEditorMeshBasedAdapter::PrepareAdapterForPainting()
{
//    check(iMeshComponent != nullptr);
//    check(mTexturePaintingCurrentMeshComponent == nullptr);
//    check(mPaintingTexture2D == nullptr);
//
//    const auto featureLevel = iMeshComponent->GetWorld()->FeatureLevel;
//
//    UTexture2D* texture2D = mEditor->Texture();
//    if(texture2D == nullptr)
//    {
//        return;
//    }
//
//    bool bStartedPainting = false;
//    FPaintTexture2DData* textureData = GetPaintTargetData(texture2D);
//
//    // Check all the materials on the mesh to see if the user texture is there
//    int32 materialIndex = 0;
//    UMaterialInterface* materialToCheck = iMeshComponent->GetMaterial(materialIndex);
//
//    while(materialToCheck != nullptr)
//    {
//        bool bIsTextureUsed = TexturePaintHelpers::DoesMeshComponentUseTexture(iMeshComponent,texture2D);
//
//        if(!bIsTextureUsed && (textureData != nullptr) && (textureData->PaintRenderTargetTexture != nullptr))
//        {
//            bIsTextureUsed = TexturePaintHelpers::DoesMeshComponentUseTexture(iMeshComponent,textureData->PaintRenderTargetTexture);
//        }
//
//        if(bIsTextureUsed && !bStartedPainting)
//        {
//            bool bIsSourceTextureStreamedIn = texture2D->IsFullyStreamedIn();
//
//            if(!bIsSourceTextureStreamedIn)
//            {
//                // We found that this texture is used in one of the meshes materials but not fully loaded, we will
//                //   attempt to fully stream in the texture before we try to do anything with it.
//                texture2D->SetForceMipLevelsToBeResident(30.0f);
//                texture2D->WaitForStreaming();
//
//                // We do a quick sanity check to make sure it is streamed fully streamed in now.
//                bIsSourceTextureStreamedIn = texture2D->IsFullyStreamedIn();
//            }
//
//            if(bIsSourceTextureStreamedIn)
//            {
//                const int32 textureWidth = texture2D->Source.GetSizeX();
//                const int32 textureHeight = texture2D->Source.GetSizeY();
//
//                if(textureData == nullptr)
//                {
//                    textureData = AddPaintTargetData(texture2D);
//                }
//                check(textureData != nullptr);
//
//                // Create our render target texture
//                if(textureData->PaintRenderTargetTexture == nullptr ||
//                    textureData->PaintRenderTargetTexture->GetSurfaceWidth() != textureWidth ||
//                    textureData->PaintRenderTargetTexture->GetSurfaceHeight() != textureHeight)
//                {
//                    textureData->PaintRenderTargetTexture = nullptr;
//                    textureData->PaintRenderTargetTexture = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),NAME_None,RF_Transient);
//                    textureData->PaintRenderTargetTexture->bNeedsTwoCopies = true;
//                    const bool bForceLinearGamma = true;
//                    textureData->PaintRenderTargetTexture->InitCustomFormat(textureWidth,textureHeight,PF_A16B16G16R16,bForceLinearGamma);
//                    textureData->PaintRenderTargetTexture->UpdateResourceImmediate();
//
//                    //Duplicate the texture we are painting and store it in the transient package. This texture is a backup of the data incase we want to revert before commiting.
//                    textureData->PaintingTexture2DDuplicate = (UTexture2D*)StaticDuplicateObject(texture2D,GetTransientPackage(),*FString::Printf(TEXT("%s_TEMP"),*texture2D->GetName()));
//                }
//                textureData->PaintRenderTargetTexture->AddressX = texture2D->AddressX;
//                textureData->PaintRenderTargetTexture->AddressY = texture2D->AddressY;
//
//                const int32 brushTargetTextureWidth = textureWidth;
//                const int32 brushTargetTextureHeight = textureHeight;
//
//                // Create the rendertarget used to store our paint delta
//                if(mBrushRenderTargetTexture == nullptr ||
//                    mBrushRenderTargetTexture->GetSurfaceWidth() != brushTargetTextureWidth ||
//                    mBrushRenderTargetTexture->GetSurfaceHeight() != brushTargetTextureHeight)
//                {
//                    mBrushRenderTargetTexture = nullptr;
//                    mBrushRenderTargetTexture = NewObject<UTextureRenderTarget2D>(GetTransientPackage(),NAME_None,RF_Transient);
//                    const bool bForceLinearGamma = true;
//                    mBrushRenderTargetTexture->ClearColor = FLinearColor::Transparent;
//                    mBrushRenderTargetTexture->bNeedsTwoCopies = true;
//                    mBrushRenderTargetTexture->InitCustomFormat(brushTargetTextureWidth,brushTargetTextureHeight,PF_A16B16G16R16,bForceLinearGamma);
//                    mBrushRenderTargetTexture->UpdateResourceImmediate();
//                    mBrushRenderTargetTexture->AddressX = textureData->PaintRenderTargetTexture->AddressX;
//                    mBrushRenderTargetTexture->AddressY = textureData->PaintRenderTargetTexture->AddressY;
//                }
//
//                bStartedPainting = true;
//            }
//        }
//
//        // @todo MeshPaint: Here we override the textures on the mesh with the render target.  The problem is that other meshes in the scene that use
//        //    this texture do not get the override. Do we want to extend this to all other selected meshes or maybe even to all meshes in the scene?
//        if(bIsTextureUsed && bStartedPainting && !textureData->PaintingMaterials.Contains(materialToCheck))
//        {
//            textureData->PaintingMaterials.AddUnique(materialToCheck);
//
//            iGeometryInfo.ApplyOrRemoveTextureOverride(texture2D,textureData->PaintRenderTargetTexture);
//        }
//
//        materialIndex++;
//        materialToCheck = iMeshComponent->GetMaterial(materialIndex);
//    }
//
//    if(bStartedPainting)
//    {
//        mTexturePaintingCurrentMeshComponent = iMeshComponent;
//
//        check(texture2D != nullptr);
//        mPaintingTexture2D = texture2D;
//
//        mStrokeBufferTexture2D = NewObject<UTexture2D>(GetTransientPackage(),FName(),RF_Transient);
//        InitTextureWithBlockData(mEditor->PaintEngine()->EditedBlock(),mStrokeBufferTexture2D,TSF_BGRA8);
//        mStrokeBufferTexture2D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
//        mStrokeBufferTexture2D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
//        mStrokeBufferTexture2D->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
//        mStrokeBufferTexture2D->UpdateResource();
//        mStrokeBufferTexture2D->PostEditChange();
//
//        mStrokeBufferTexture3D = NewObject<UTexture2D>(GetTransientPackage(),FName(),RF_Transient);
//        InitTextureWithBlockData(mEditor->PaintEngine()->EditedBlock(),mStrokeBufferTexture3D,TSF_BGRA8);
//        mStrokeBufferTexture3D->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
//        mStrokeBufferTexture3D->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
//        mStrokeBufferTexture3D->LODGroup = TextureGroup::TEXTUREGROUP_Pixels2D;
//        mStrokeBufferTexture3D->UpdateResource();
//        mStrokeBufferTexture3D->PostEditChange();
//
//        // OK, now we need to make sure our render target is filled in with data
//        //TexturePaintHelpers::SetupInitialRenderTargetData(textureData->PaintingTexture2D,textureData->PaintRenderTargetTexture);
//
//
//		if (textureData->PaintingTexture2D->Source.IsValid())
//		{
//			TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
//		}
//		else
//		{
//			check(textureData->PaintingTexture2D->IsFullyStreamedIn());
//			TexturePaintHelpers::CopyTextureToRenderTargetTexture(textureData->PaintingTexture2D, textureData->PaintRenderTargetTexture, GEditor->GetEditorWorldContext().World()->FeatureLevel);
//		}
//    }
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::StartPainting()
{

}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::Paint()
{
//    // We bail early if there are no influenced triangles
//    if(iInfluencedTriangles.Num() <= 0)
//    {
//        return;
//    }
//
//    if( mBeginPosition == FVector2D(0,0))
//        mBeginPosition = FVector2D(mLastEvent.x / mPaintingTexture2D->Source.GetSizeX(), mLastEvent.y / mPaintingTexture2D->Source.GetSizeY());
//
//    //UE_LOG(LogTemp, Display, TEXT("triangles: %d"), iInfluencedTriangles.Num() );
//
//    const auto featureLevel = GEditor->GetEditorWorldContext().World()->FeatureLevel;
//
//    FPaintTexture2DData* textureData = GetPaintTargetData(mPaintingTexture2D);
//    check(textureData != nullptr && textureData->PaintRenderTargetTexture != nullptr);
//
//    // Copy the current image to the brush rendertarget texture.
//    {
//        check(mBrushRenderTargetTexture != nullptr);
//        TexturePaintHelpers::CopyTextureToRenderTargetTexture(mPaintingTexture2D,mBrushRenderTargetTexture,featureLevel);
//    }
//
//    const bool bEnableSeamPainting = false;
//
//    FVector BrushXAxis,BrushYAxis;
//    iHitResult.Normal.FindBestAxisVectors(BrushXAxis,BrushYAxis);
//    const FMatrix worldToBrushMatrix = FMatrix(BrushXAxis, BrushYAxis, iHitResult.Normal, iHitResult.Location).InverseFast();
//
//    // Grab the actual render target resource from the textures.  Note that we're absolutely NOT ALLOWED to
//    // dereference these pointers.  We're just passing them along to other functions that will use them on the render
//    // thread.  The only thing we're allowed to do is check to see if they are nullptr or not.
//    FTextureRenderTargetResource* brushRenderTargetResource = mBrushRenderTargetTexture->GameThread_GetRenderTargetResource();
//    //mStrokeBufferTexture3D->Resource->TextureRHI = brushRenderTargetResource->GetRenderTargetTexture();
//
//    check(brushRenderTargetResource != nullptr);
//
//    // Create a canvas for the brush render target.
//    FCanvas brushPaintCanvas(brushRenderTargetResource,nullptr,0,0,0,featureLevel);
//
//    InvalidateTextureFromData(mEditor->PaintEngine()->PaintBlock(), mStrokeBufferTexture2D);
//
//    // Parameters for brush paint
//    TRefCountPtr< FOdysseyMeshPaintBatchedElementParameters > meshPaintBatchedElementParameters(new FOdysseyMeshPaintBatchedElementParameters());
//    {
//        meshPaintBatchedElementParameters->ShaderParams.Stroke2D = mStrokeBufferTexture2D;
//        meshPaintBatchedElementParameters->ShaderParams.WorldToBrushMatrix = worldToBrushMatrix;
//        meshPaintBatchedElementParameters->ShaderParams.TextureHitPoint = mBeginPosition;
//    }
//
//    FBatchedElements* brushPaintBatchedElements = brushPaintCanvas.GetBatchedElements(FCanvas::ET_Triangle,meshPaintBatchedElementParameters,nullptr,SE_BLEND_Opaque);
//    brushPaintBatchedElements->AddReserveVertices(iInfluencedTriangles.Num() * 3);
//    brushPaintBatchedElements->AddReserveTriangles(iInfluencedTriangles.Num(),nullptr,SE_BLEND_Opaque);
//
//    FHitProxyId brushPaintHitProxyId = brushPaintCanvas.GetHitProxyId();
//
//    // Process the influenced triangles - storing off a large list is much slower than processing in a single loop
//    for(int32 curIndex = 0; curIndex < iInfluencedTriangles.Num(); ++curIndex)
//    {
//        FTexturePaintTriangleInfo& curTriangle = iInfluencedTriangles[curIndex];
//
//        FVector2D uvMin(99999.9f,99999.9f);
//        FVector2D uvMax(-99999.9f,-99999.9f);
//
//        // Transform the triangle and update the UV bounds
//        for(int32 triVertexNum = 0; triVertexNum < 3; ++triVertexNum)
//        {
//            // Update bounds
//            float u = curTriangle.TriUVs[triVertexNum].X;
//            float v = curTriangle.TriUVs[triVertexNum].Y;
//
//            if(u < uvMin.X)
//            {
//                uvMin.X = u;
//            }
//            if(u > uvMax.X)
//            {
//                uvMax.X = u;
//            }
//            if(v < uvMin.Y)
//            {
//                uvMin.Y = v;
//            }
//            if(v > uvMax.Y)
//            {
//                uvMax.Y = v;
//            }
//        }
//
//        // If the triangle lies entirely outside of the 0.0-1.0 range, we'll transpose it back
//        FVector2D uvOffset(0.0f,0.0f);
//        if(uvMax.X > 1.0f)
//        {
//            uvOffset.X = -FMath::FloorToFloat(uvMin.X);
//        } else if(uvMin.X < 0.0f)
//        {
//            uvOffset.X = 1.0f + FMath::FloorToFloat(-uvMax.X);
//        }
//
//        if(uvMax.Y > 1.0f)
//        {
//            uvOffset.Y = -FMath::FloorToFloat(uvMin.Y);
//        } else if(uvMin.Y < 0.0f)
//        {
//            uvOffset.Y = 1.0f + FMath::FloorToFloat(-uvMax.Y);
//        }
//
//        // Note that we "wrap" the texture coordinates here to handle the case where the user
//        // is painting on a tiling texture, or with the UVs out of bounds.  Ideally all of the
//        // UVs would be in the 0.0 - 1.0 range but sometimes content isn't setup that way.
//        // @todo MeshPaint: Handle triangles that cross the 0.0-1.0 UV boundary?
//        for(int32 triVertexNum = 0; triVertexNum < 3; ++triVertexNum)
//        {
//            curTriangle.TriUVs[triVertexNum].X += uvOffset.X;
//            curTriangle.TriUVs[triVertexNum].Y += uvOffset.Y;
//
//            // @todo: Need any half-texel offset adjustments here? Some info about offsets and MSAA here: http://drilian.com/2008/11/25/understanding-half-pixel-and-half-texel-offsets/
//            // @todo: MeshPaint: Screen-space texture coords: http://diaryofagraphicsprogrammer.blogspot.com/2008/09/calculating-screen-space-texture.html
//            curTriangle.TrianglePoints[triVertexNum].X = curTriangle.TriUVs[triVertexNum].X * textureData->PaintRenderTargetTexture->GetSurfaceWidth();
//            curTriangle.TrianglePoints[triVertexNum].Y = curTriangle.TriUVs[triVertexNum].Y * textureData->PaintRenderTargetTexture->GetSurfaceHeight();
//        }
//
//        // Vertex positions
//        FVector4 vert0(curTriangle.TrianglePoints[0].X,curTriangle.TrianglePoints[0].Y,0,1);
//        FVector4 vert1(curTriangle.TrianglePoints[1].X,curTriangle.TrianglePoints[1].Y,0,1);
//        FVector4 vert2(curTriangle.TrianglePoints[2].X,curTriangle.TrianglePoints[2].Y,0,1);
//
//        //UE_LOG(LogTemp,Display,TEXT("------------------------------------"));
//        /*UE_LOG(LogTemp,Display,TEXT("vert0x: %lf, vert0y: %lf,vert0z: %lf, vert0w: %lf"),vert0.X,vert0.Y,vert0.Z,vert0.W);
//        UE_LOG(LogTemp,Display, TEXT("vert1x: %lf, vert1y: %lf,vert1z: %lf, vert1w: %lf"), vert1.X, vert1.Y, vert1.Z, vert1.W);
//        UE_LOG(LogTemp,Display,TEXT("vert2x: %lf, vert2y: %lf,vert2z: %lf, vert2w: %lf"),vert2.X,vert2.Y,vert2.Z,vert2.W);*/
//
//
//        // Vertex color
//        FLinearColor col0(curTriangle.TriVertices[0].X,curTriangle.TriVertices[0].Y,curTriangle.TriVertices[0].Z);
//        FLinearColor col1(curTriangle.TriVertices[1].X,curTriangle.TriVertices[1].Y,curTriangle.TriVertices[1].Z);
//        FLinearColor col2(curTriangle.TriVertices[2].X,curTriangle.TriVertices[2].Y,curTriangle.TriVertices[2].Z);
//        /*UE_LOG(LogTemp,Display,TEXT("col0 R: %lf, col0 G: %lf,col0 B: %lf, col0 A: %lf"),col0.R,col0.G,col0.B,col0.A);
//        UE_LOG(LogTemp,Display,TEXT("col1 R: %lf, col1 G: %lf,col1 B: %lf, col1 A: %lf"),col1.R,col1.G,col1.B,col1.A);
//        UE_LOG(LogTemp,Display,TEXT("col2 R: %lf, col2 G: %lf,col2 B: %lf, col2 A: %lf"),col2.R,col2.G,col2.B,col2.A);*/
//
//
//        // Brush Paint triangle
//        {
//            int32 v0 = brushPaintBatchedElements->AddVertex(vert0,curTriangle.TriUVs[0],col0,brushPaintHitProxyId);
//            int32 v1 = brushPaintBatchedElements->AddVertex(vert1,curTriangle.TriUVs[1],col1,brushPaintHitProxyId);
//            int32 v2 = brushPaintBatchedElements->AddVertex(vert2,curTriangle.TriUVs[2],col2,brushPaintHitProxyId);
//            /*UE_LOG(LogTemp,Display,TEXT("curTriangle.TriUVs[0]x: %lf, curTriangle.TriUVs[0]y: %lf"),curTriangle.TriUVs[0].X,curTriangle.TriUVs[0].Y);
//            UE_LOG(LogTemp,Display,TEXT("curTriangle.TriUVs[1]x: %lf, curTriangle.TriUVs[1]y: %lf"),curTriangle.TriUVs[1].X,curTriangle.TriUVs[1].Y);
//            UE_LOG(LogTemp,Display,TEXT("curTriangle.TriUVs[2]x: %lf, curTriangle.TriUVs[2]y: %lf"),curTriangle.TriUVs[2].X,curTriangle.TriUVs[2].Y);*/
//
//            brushPaintBatchedElements->AddTriangle(v0,v1,v2,meshPaintBatchedElementParameters,SE_BLEND_Opaque);
//        }
//    }
//
//    // Tell the rendering thread to draw any remaining batched elements
//    {
//        brushPaintCanvas.Flush_GameThread(true);
//
//        textureData->bIsPaintingTexture2DModified = true;
//    }
//
//    {
//        ENQUEUE_RENDER_COMMAND(UpdateMeshPaintRTCommand1)(
//            [brushRenderTargetResource](FRHICommandListImmediate& RHICmdList)
//        {
//            // Copy (resolve) the rendered image from the frame buffer to its render target texture
//            RHICmdList.CopyToResolveTarget(
//                brushRenderTargetResource->GetRenderTargetTexture(),	// Source texture
//                brushRenderTargetResource->TextureRHI,
//                FResolveParams());										// Resolve parameters
//
//        });
//    }
//
//    //mBrushRenderTargetTexture->bGPUSharedFlag = true;
//
//    //brushRenderTargetResource->ReadPixelsPtr((FColor*)(mController->GetData()->PaintEngine()->StrokeBlock3D()->GetArray().GetData()));
//    //mBrushRenderTargetTexture->UpdateTexture2D(mStrokeBufferTexture3D, ETextureSourceFormat::TSF_RGBA16 );
//    //CopyURenderTargetPixelDataIntoBlock(mController->GetData()->PaintEngine()->StrokeBlock3D()->GetArray().GetData(), mBrushRenderTargetTexture);
//    //CopyUTexturePixelDataIntoBlock(mController->GetData()->PaintEngine()->StrokeBlock3D(), mStrokeBufferTexture2D);
//    /*TArray<::ul3::FRect> rects;
//    ::ul3::FRect rect = ::ul3::FRect(0,0,1023,1023);
//    rects.Add(rect);
//    mController->GetData()->PaintEngine()->BlendStrokeBlockInPreviewBlock(rects);
//    mController->OnPaintEngineStrokeChanged(rects);*/
//
//    //mController->GetData()->PaintEngine()->UpdateTmpTileMapFromTriangles( iInfluencedTriangles );
//
//    //CopyUTexturePixelDataIntoBlock(mController->GetData()->PaintEngine()->StrokeBlock3D(), mPaintingTexture2D);
//    //mController->GetData()->PaintEngine()->Tick();
//    
//    TexturePaintHelpers::CopyTextureToRenderTargetTexture(mPaintingTexture2D,textureData->PaintRenderTargetTexture,featureLevel);
//    
//    FlushRenderingCommands();
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::FinishPainting()
{
//    if(mTexturePaintingCurrentMeshComponent != nullptr)
//    {
//        check(mPaintingTexture2D != nullptr);
//
//        FPaintTexture2DData* textureData = GetPaintTargetData(mPaintingTexture2D);
//        check(textureData);
//
//        // Commit to the texture source art but don't do any compression, compression is saved for the CommitAllPaintedTextures function.
//        if(textureData->bIsPaintingTexture2DModified == true)
//        {
//            const int32 texWidth = textureData->PaintRenderTargetTexture->SizeX;
//            const int32 texHeight = textureData->PaintRenderTargetTexture->SizeY;
//            TArray< FColor > texturePixels;
//            texturePixels.AddUninitialized(texWidth * texHeight);
//
//            FlushRenderingCommands();
//            // NOTE: You are normally not allowed to dereference this pointer on the game thread! Normally you can only pass the pointer around and
//            //  check for NULLness.  We do it in this context, however, and it is only ok because this does not happen every frame and we make sure to flush the
//            //  rendering thread.
//            FTextureRenderTargetResource* renderTargetResource = textureData->PaintRenderTargetTexture->GameThread_GetRenderTargetResource();
//            check(renderTargetResource != nullptr);
//            renderTargetResource->ReadPixels(texturePixels);
//
//            {
//                FScopedTransaction transaction(LOCTEXT("MeshPaintMode_TexturePaint_Transaction","Texture Paint"));
//
//                // For undo
//                textureData->PaintingTexture2D->SetFlags(RF_Transactional);
//                textureData->PaintingTexture2D->Modify();
//
//                // Store source art
//                FColor* colors = (FColor*)textureData->PaintingTexture2D->Source.LockMip(0);
//                check(textureData->PaintingTexture2D->Source.CalcMipSize(0) == texturePixels.Num() * sizeof(FColor));
//                FMemory::Memcpy(colors,texturePixels.GetData(),texturePixels.Num() * sizeof(FColor));
//                textureData->PaintingTexture2D->Source.UnlockMip(0);
//
//                // If render target gamma used was 1.0 then disable SRGB for the static texture
//                //textureData->PaintingTexture2D->SRGB = FMath::Abs(renderTargetResource->GetDisplayGamma() - 1.0f) >= KINDA_SMALL_NUMBER;
//
//                textureData->PaintingTexture2D->bHasBeenPaintedInEditor = true;
//            }
//        }
//
//        mPaintingTexture2D = nullptr;
//        mTexturePaintingCurrentMeshComponent = nullptr;
//    }
}

void FOdysseyViewportDrawingEditorMeshBasedAdapter::Tick()
{
}

#undef LOCTEXT_NAMESPACE
