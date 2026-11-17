// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"

#include "ArianePainting3DStaticMeshComponent.generated.h"

class FMeshDescriptionBuilder;
class UArianeLayerVector;
class UMaterialInterface;
class UStaticMesh;
class UArianePainting3DComponent;
struct FArianePath;
struct FArianeCycle;

UCLASS()
class ARIANE_API UArianePainting3DStaticMeshComponent : public UStaticMeshComponent
{
    GENERATED_BODY()

    struct FUniqueMaterial
    {
    public:
        FUniqueMaterial( UMaterialInterface* InOriginalMaterialInterface
                       , UMaterialInterface* InMaterialInterface
                       , FColor InColor
                       , float InPixelDepthOffset )
            : OriginalMaterialInterface ( InOriginalMaterialInterface )
            , MaterialInterface ( InMaterialInterface )
            , Color ( InColor )
            , PixelDepthOffset ( InPixelDepthOffset )
        {
        }

    public:
        UMaterialInterface* OriginalMaterialInterface;
        UMaterialInterface* MaterialInterface;
        FColor Color;
        float PixelDepthOffset;
    };

public:
    //virtual UMeshDescription* GetMeshDescription(int32 LODIndex) const override;
    void ConvertToStaticMesh( TArray<UMaterialInstanceConstant*>& OutMaterialInstanceConstants );

private:
    void ConvertDrawingToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                   , UArianeLayerVector* VectorLayer );
    void ConvertPathToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                , FArianePath* Path );
    void ConvertCycleToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                        , FArianeCycle* Cycle );
    void ConvertMaterials( UStaticMesh* ConvertedStaticMesh
                         , TArray<UMaterialInstanceConstant*>& OutMaterialInstanceConstants );

    FUniqueMaterial* GetUniqueMaterial( UMaterialInterface* MaterialInterface
                                      , const FColor& RequestedColor
                                      , float RequestedPDO );

    FUniqueMaterial* CreateUniqueMaterial( UArianePainting3DComponent* Painting3DComponent
                                         , UMaterialInterface* MaterialInterface
                                         , const FColor& RequestedColor
                                         , float RequestedPDO
                                         , TArray<UMaterialInstanceConstant*>& OutMaterialInstanceConstants );

private:
    TArray<FUniqueMaterial> UniqueMaterials;
};
