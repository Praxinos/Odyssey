// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyViewportDrawingEditorUtils.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"
#include "Materials/MaterialExpressionTextureBase.h"
#include "Materials/MaterialExpressionTextureSample.h"
#include "Materials/MaterialExpressionTextureSampleParameter.h"
#include "Engine/TextureRenderTarget2D.h"
#include "CanvasItem.h"
#include "CanvasTypes.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/Material.h"
#include "Engine/StaticMeshActor.h"
#include "StaticMeshResources.h"

void FOdysseyViewportDrawingEditorUtils::CopyTextureToRenderTargetTexture(UTexture* SourceTexture, UTextureRenderTarget2D* RenderTargetTexture, ERHIFeatureLevel::Type FeatureLevel)
{
    check(SourceTexture != nullptr);
    check(RenderTargetTexture != nullptr);

    // Grab the actual render target resource from the texture.  Note that we're absolutely NOT ALLOWED to
    // dereference this pointer.  We're just passing it along to other functions that will use it on the render
    // thread.  The only thing we're allowed to do is check to see if it's nullptr or not.
    FTextureRenderTargetResource* RenderTargetResource = RenderTargetTexture->GameThread_GetRenderTargetResource();
    check(RenderTargetResource != nullptr);

    // Create a canvas for the render target and clear it to black
    FCanvas Canvas(RenderTargetResource, nullptr, FGameTime(), FeatureLevel);

    const uint32 Width = RenderTargetTexture->GetSurfaceWidth();
    const uint32 Height = RenderTargetTexture->GetSurfaceHeight();

    // @todo MeshPaint: Need full color/alpha writes enabled to get alpha
    // @todo MeshPaint: Texels need to line up perfectly to avoid bilinear artifacts
    // @todo MeshPaint: Potential gamma issues here
    // @todo MeshPaint: Probably using CLAMP address mode when reading from source (if texels line up, shouldn't matter though.)

    // @todo MeshPaint: Should use scratch texture built from original source art (when possible!)
    //        -> Current method will have compression artifacts!

    // Grab the texture resource.  We only support 2D textures and render target textures here.
    FTexture* TextureResource = nullptr;
    TextureResource = SourceTexture->GetResource();
    check(TextureResource != nullptr);

    // Draw a quad to copy the texture over to the render target
    {
        const float MinU = 0.0f;
        const float MinV = 0.0f;
        const float MaxU = 1.0f;
        const float MaxV = 1.0f;
        const float MinX = 0.0f;
        const float MinY = 0.0f;
        const float MaxX = Width;
        const float MaxY = Height;

        FCanvasUVTri Tri1;
        FCanvasUVTri Tri2;
        Tri1.V0_Pos = FVector2D(MinX, MinY);
        Tri1.V0_UV = FVector2D(MinU, MinV);
        Tri1.V1_Pos = FVector2D(MaxX, MinY);
        Tri1.V1_UV = FVector2D(MaxU, MinV);
        Tri1.V2_Pos = FVector2D(MaxX, MaxY);
        Tri1.V2_UV = FVector2D(MaxU, MaxV);

        Tri2.V0_Pos = FVector2D(MaxX, MaxY);
        Tri2.V0_UV = FVector2D(MaxU, MaxV);
        Tri2.V1_Pos = FVector2D(MinX, MaxY);
        Tri2.V1_UV = FVector2D(MinU, MaxV);
        Tri2.V2_Pos = FVector2D(MinX, MinY);
        Tri2.V2_UV = FVector2D(MinU, MinV);
        Tri1.V0_Color = Tri1.V1_Color = Tri1.V2_Color = Tri2.V0_Color = Tri2.V1_Color = Tri2.V2_Color = FLinearColor::White;
        TArray< FCanvasUVTri > List;
        List.Add(Tri1);
        List.Add(Tri2);
        FCanvasTriangleItem TriItem(List, TextureResource);
        TriItem.BlendMode = SE_BLEND_Opaque;
        Canvas.DrawItem(TriItem);
    }

    // Tell the rendering thread to draw any remaining batched elements
    Canvas.Flush_GameThread(true);

    ENQUEUE_RENDER_COMMAND(UpdateMeshPaintRTCommand)(
        [RenderTargetResource](FRHICommandListImmediate& RHICmdList)
        {
            TransitionAndCopyTexture(RHICmdList, RenderTargetResource->GetRenderTargetTexture(), RenderTargetResource->TextureRHI, {});
        });
}

void FOdysseyViewportDrawingEditorUtils::RetrieveTexturesForComponent(const UMeshComponent* iComponent, TArray<FPaintableTexture>& oTextures)
{
    // Get the materials used by the mesh
    TArray<UMaterialInterface*> usedMaterials;
    iComponent->GetUsedMaterials(usedMaterials);

    for (int32 materialIndex = 0; materialIndex < usedMaterials.Num(); ++materialIndex)
    {
        int32 outDefaultIndex = 0;
        FOdysseyViewportDrawingEditorUtils::InternalQueryPaintableTextures(materialIndex, iComponent, outDefaultIndex, oTextures);
    }
}

void FOdysseyViewportDrawingEditorUtils::InternalQueryPaintableTextures(int32 iMaterialIndex, const UMeshComponent* iMeshComponent, int32& oDefaultIndex, TArray<struct FPaintableTexture>& ioTextureList)
{
    oDefaultIndex = INDEX_NONE;

    // We already know the material we are painting on, take it off the static mesh component
    UMaterialInterface* material = iMeshComponent->GetMaterial(iMaterialIndex);

    if (material != NULL)
    {
        FPaintableTexture paintableTexture;
        // Find all the unique textures used in the top material level of the selected actor materials

        // Only grab the textures from the top level of samples
        for ( UMaterialExpression* expression : material->GetMaterial()->GetExpressions())
        {
            UMaterialExpressionTextureBase* textureBase = Cast<UMaterialExpressionTextureBase>(expression);
            if (textureBase != NULL &&
                textureBase->Texture != NULL //&&
                //!textureBase->Texture->IsNormalMap() &&
                //!textureBase->Texture->VirtualTextureStreaming &&
                //!textureBase->Texture->HasHDRSource() )  // Currently HDR textures are not supported to paint on
                )
            {
                // Default UV channel to index 0.
                paintableTexture = FPaintableTexture(textureBase->Texture, 0);

                // Texture Samples can have UV's specified, check the first node for whether it has a custom UV channel set.
                // We only check the first as the Mesh paint mode does not support painting with UV's modified in the shader.
                UMaterialExpressionTextureSample* textureSample = Cast<UMaterialExpressionTextureSample>(expression);
                if (textureSample != NULL)
                {
                    UMaterialExpressionTextureCoordinate* TextureCoords = Cast<UMaterialExpressionTextureCoordinate>(textureSample->Coordinates.Expression);
                    if (TextureCoords != NULL)
                    {
                        // Store the uv channel, this is set when the texture is selected.
                        paintableTexture.UVChannelIndex = TextureCoords->CoordinateIndex;
                    }

                    // Handle texture parameter expressions
                    UMaterialExpressionTextureSampleParameter* textureSampleParameter = Cast<UMaterialExpressionTextureSampleParameter>(textureSample);
                    if (textureSampleParameter != NULL)
                    {
                        // Grab the overridden texture if it exists.
                        material->GetTextureParameterValue(textureSampleParameter->ParameterName, paintableTexture.Texture);
                    }
                }

                // note that the same texture will be added again if its UV channel differs.
                int32 textureIndex = ioTextureList.AddUnique(paintableTexture);

                // cache the first default index, if there is no previous info this will be used as the selected texture
                if ((oDefaultIndex == INDEX_NONE) && textureBase->IsDefaultMeshpaintTexture)
                {
                    oDefaultIndex = textureIndex;
                }
            }
        }
    }
}

bool
FOdysseyViewportDrawingEditorUtils::GenerateSeamMask(UMeshComponent* MeshComponent, int32 UVSet, UTextureRenderTarget2D* SeamRenderTexture, UTexture* Texture, UTextureRenderTarget2D* RenderTargetTexture)
{
    UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(MeshComponent);
    if (StaticMeshComponent == nullptr)
    {
        return false;
    }

    const int32 PaintingMeshLODIndex = 0;

    check(StaticMeshComponent != nullptr);
    check(StaticMeshComponent->GetStaticMesh() != nullptr);
    check(SeamRenderTexture != nullptr);
    check(StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[PaintingMeshLODIndex].VertexBuffers.StaticMeshVertexBuffer.GetNumTexCoords() > (uint32)UVSet);

    bool RetVal = false;

    FStaticMeshLODResources& LODModel = StaticMeshComponent->GetStaticMesh()->GetRenderData()->LODResources[PaintingMeshLODIndex];

    const uint32 Width = SeamRenderTexture->GetSurfaceWidth();
    const uint32 Height = SeamRenderTexture->GetSurfaceHeight();

    // Grab the actual render target resource from the texture.  Note that we're absolutely NOT ALLOWED to
    // dereference this pointer.  We're just passing it along to other functions that will use it on the render
    // thread.  The only thing we're allowed to do is check to see if it's nullptr or not.
    FTextureRenderTargetResource* RenderTargetResource = SeamRenderTexture->GameThread_GetRenderTargetResource();
    check(RenderTargetResource != nullptr);

    int32 NumElements = StaticMeshComponent->GetNumMaterials();
    UTexture* TargetTexture2D = Texture;

    // Store info that tells us if the element material uses our target texture so we don't have to do a usestexture() call for each tri.  We will
    // use this info to eliminate triangles that do not use our texture.
    TArray< bool > ElementUsesTargetTexture;
    ElementUsesTargetTexture.AddZeroed(NumElements);
    for (int32 ElementIndex = 0; ElementIndex < NumElements; ElementIndex++)
    {
        ElementUsesTargetTexture[ElementIndex] = false;

        UMaterialInterface* ElementMat = StaticMeshComponent->GetMaterial(ElementIndex);
        if (ElementMat != nullptr)
        {
            ElementUsesTargetTexture[ElementIndex] |= DoesMaterialUseTexture(ElementMat, TargetTexture2D);

            if (ElementUsesTargetTexture[ElementIndex] == false && RenderTargetTexture != nullptr)
            {
                // If we didn't get a match on our selected texture, we'll check to see if the the material uses a
                //  render target texture override that we put on during painting.
                ElementUsesTargetTexture[ElementIndex] |= DoesMaterialUseTexture(ElementMat, RenderTargetTexture);
            }
        }
    }

    // Make sure we're dealing with triangle lists
    FIndexArrayView Indices = LODModel.IndexBuffer.GetArrayView();
    const uint32 NumIndexBufferIndices = Indices.Num();
    check(NumIndexBufferIndices % 3 == 0);
    const uint32 NumTriangles = NumIndexBufferIndices / 3;

    static TArray< int32 > InfluencedTriangles;
    InfluencedTriangles.Empty(NumTriangles);

    // For each triangle in the mesh
    for (uint32 TriIndex = 0; TriIndex < NumTriangles; ++TriIndex)
    {
        // At least one triangle vertex was influenced.
        bool bAddTri = false;

        // Check to see if the sub-element that this triangle belongs to actually uses our paint target texture in its material
        for (int32 ElementIndex = 0; ElementIndex < NumElements; ElementIndex++)
        {
            //FStaticMeshElement& Element = LODModel.Elements[ ElementIndex ];
            FStaticMeshSection& Element = LODModel.Sections[ElementIndex];


            if ((TriIndex >= Element.FirstIndex / 3) &&
                (TriIndex < Element.FirstIndex / 3 + Element.NumTriangles))
            {

                // The triangle belongs to this element, now we need to check to see if the element material uses our target texture.
                if (TargetTexture2D != nullptr && ElementUsesTargetTexture[ElementIndex] == true)
                {
                    bAddTri = true;
                }

                // Triangles can only be part of one element so we do not need to continue to other elements.
                break;
            }

        }

        if (bAddTri)
        {
            InfluencedTriangles.Add(TriIndex);
        }

    }

    {
        // Create a canvas for the render target and clear it to white
        FCanvas Canvas(RenderTargetResource, nullptr, FGameTime(), GEditor->GetEditorWorldContext().World()->GetFeatureLevel());
        Canvas.Clear(FLinearColor::White);

        TArray<FCanvasUVTri> TriList;
        FCanvasUVTri EachTri;
        EachTri.V0_Color = FLinearColor::Black;
        EachTri.V1_Color = FLinearColor::Black;
        EachTri.V2_Color = FLinearColor::Black;

        for (int32 CurIndex = 0; CurIndex < InfluencedTriangles.Num(); ++CurIndex)
        {
            const int32 TriIndex = InfluencedTriangles[CurIndex];

            // Grab the vertex indices and points for this triangle
            FVector2D TriUVs[3];
            FVector2D UVMin(99999.9f, 99999.9f);
            FVector2D UVMax(-99999.9f, -99999.9f);
            for (int32 TriVertexNum = 0; TriVertexNum < 3; ++TriVertexNum)
            {
                const int32 VertexIndex = Indices[TriIndex * 3 + TriVertexNum];
                TriUVs[TriVertexNum] = FVector2D(LODModel.VertexBuffers.StaticMeshVertexBuffer.GetVertexUV(VertexIndex, UVSet));

                // Update bounds
                float U = TriUVs[TriVertexNum].X;
                float V = TriUVs[TriVertexNum].Y;

                if (U < UVMin.X)
                {
                    UVMin.X = U;
                }
                if (U > UVMax.X)
                {
                    UVMax.X = U;
                }
                if (V < UVMin.Y)
                {
                    UVMin.Y = V;
                }
                if (V > UVMax.Y)
                {
                    UVMax.Y = V;
                }

            }

            // If the triangle lies entirely outside of the 0.0-1.0 range, we'll transpose it back
            FVector2D UVOffset(0.0f, 0.0f);
            if (UVMax.X > 1.0f)
            {
                UVOffset.X = -FMath::FloorToInt(UVMin.X);
            }
            else if (UVMin.X < 0.0f)
            {
                UVOffset.X = 1.0f + FMath::FloorToInt(-UVMax.X);
            }

            if (UVMax.Y > 1.0f)
            {
                UVOffset.Y = -FMath::FloorToInt(UVMin.Y);
            }
            else if (UVMin.Y < 0.0f)
            {
                UVOffset.Y = 1.0f + FMath::FloorToInt(-UVMax.Y);
            }


            // Note that we "wrap" the texture coordinates here to handle the case where the user
            // is painting on a tiling texture, or with the UVs out of bounds.  Ideally all of the
            // UVs would be in the 0.0 - 1.0 range but sometimes content isn't setup that way.
            // @todo MeshPaint: Handle triangles that cross the 0.0-1.0 UV boundary?
            FVector2D TrianglePoints[3];
            for (int32 TriVertexNum = 0; TriVertexNum < 3; ++TriVertexNum)
            {
                TriUVs[TriVertexNum].X += UVOffset.X;
                TriUVs[TriVertexNum].Y += UVOffset.Y;

                TrianglePoints[TriVertexNum].X = TriUVs[TriVertexNum].X * Width;
                TrianglePoints[TriVertexNum].Y = TriUVs[TriVertexNum].Y * Height;
            }

            EachTri.V0_Pos = TrianglePoints[0];
            EachTri.V0_UV = TriUVs[0];
            EachTri.V0_Color = FLinearColor::Black;
            EachTri.V1_Pos = TrianglePoints[1];
            EachTri.V1_UV = TriUVs[1];
            EachTri.V1_Color = FLinearColor::Black;
            EachTri.V2_Pos = TrianglePoints[2];
            EachTri.V2_UV = TriUVs[2];
            EachTri.V2_Color = FLinearColor::Black;
            TriList.Add(EachTri);
        }

        if( TriList.Num() == 0 )
            return RetVal;

        // Setup the tri render item with the list of tris
        FCanvasTriangleItem TriItem(TriList, RenderTargetResource);
        TriItem.BlendMode = SE_BLEND_Opaque;
        // And render it
        Canvas.DrawItem(TriItem);
        // Tell the rendering thread to draw any remaining batched elements
        Canvas.Flush_GameThread(true);
    }


    {
        ENQUEUE_RENDER_COMMAND(UpdateMeshPaintRTCommand5)(
            [RenderTargetResource](FRHICommandListImmediate& RHICmdList)
            {
                TransitionAndCopyTexture(RHICmdList, RenderTargetResource->GetRenderTargetTexture(), RenderTargetResource->TextureRHI, {});
            });
    }

    return RetVal;
}
