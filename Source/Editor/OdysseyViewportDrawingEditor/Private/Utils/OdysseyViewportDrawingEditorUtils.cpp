// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyViewportDrawingEditorUtils.h"
#include "Materials/MaterialExpressionTextureCoordinate.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorUtils"

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

#undef LOCTEXT_NAMESPACE // "OdysseyViewportDrawingEditorUtils"