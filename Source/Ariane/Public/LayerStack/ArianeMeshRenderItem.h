// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "Math/Ray.h"

class FArianeMeshGeometry3D;
class UMaterialInterface;

// Copy of what the proxy needs to draw one mesh (path or cycle). See the top of ArianeLayerVector.cpp
struct ARIANE_API FArianeMeshRenderItem
{
public:
    // Invisible: drawn by nobody until it's filled
    FArianeMeshRenderItem();

public:
    // StaticClass of the mesh (FArianeCycle: moved toward the camera against Z-fighting)
    uint32 MeshClass;
    // GPU buffers of the mesh: only changed by render commands
    FArianeMeshGeometry3D* Geometry3D;
    UMaterialInterface* Material;
    FLinearColor Color;
#if WITH_EDITOR
    FLinearColor HUDColor;
#endif
    FMatrix WorldMatrix;
    // Object space
    FBox BoundingBox;
    // Inherited from the parent groups. False for a deleted mesh until the next rebuild
    bool bVisible;
    // r.Ariane.ShowNormals only (paths), object space: vertex position and unit normal. Empty otherwise
    TArray<FRay3f> NormalRays;
};
