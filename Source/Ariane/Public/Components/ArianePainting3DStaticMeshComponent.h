// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "Components/StaticMeshComponent.h"

#include "ArianePainting3DStaticMeshComponent.generated.h"

class FMeshDescriptionBuilder;
class UArianeLayerDrawing;
class UStaticMesh;
struct FArianePath;

UCLASS()
class ARIANE_API UArianePainting3DStaticMeshComponent : public UStaticMeshComponent
{
    GENERATED_BODY()

public:
    //virtual UMeshDescription* GetMeshDescription(int32 LODIndex) const override;
    void ConvertToStaticMesh();

private:
    static void ConvertImageToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                        , UArianeLayerDrawing* DrawingLayer );
    static void ConvertPathToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                        , FArianePath* Path );
};
