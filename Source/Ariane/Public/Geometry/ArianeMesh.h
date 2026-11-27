// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianeObject.h"

#include "ArianeMesh.generated.h"

class UMaterialInterface;
struct FArianeMeshRenderItem;

// The GPU buffers of a mesh (path, cycle). The buffers are only changed by render commands
class ARIANE_API FArianeMeshGeometry3D
{
    public:
        virtual ~FArianeMeshGeometry3D();
        FArianeMeshGeometry3D( FArianeObject* InObject );

        virtual void Build() = 0;

        // After the commands already enqueued (hidden item, buffer updates): the destructor runs on the render thread
        static void DeleteOnRenderThread( FArianeMeshGeometry3D* Geometry3D );

        const uint32 GetVertexCount() const;
        const FRawStaticIndexBuffer& GetIndexBuffer() const;
        FArianeObject* GetObject();
        FLocalVertexFactory* GetVertexFactory();
        void InitVertexFactory();
        const TArray<FDynamicMeshVertex>& GetMeshVertices();
        const TArray<uint32>& GetMeshIndices();

    protected:
        FArianeObject* Object;

        TArray<FDynamicMeshVertex> MeshVertices;
        TArray<uint32> MeshIndices;

        uint32 VertexCount;
        FPositionVertexBuffer PositionBuffer;
        FStaticMeshVertexBuffer StaticMeshVB;
        FColorVertexBuffer ColorBuffer;
        FRawStaticIndexBuffer IndexBuffer;
        FLocalVertexFactory* VertexFactory;
};

// An object drawn as a mesh: color, material and GPU buffers. Base of FArianePath and FArianeCycle
USTRUCT(BlueprintType)
struct ARIANE_API FArianeMesh : public FArianeObject
{
GENERATED_BODY()

public:
    static uint32 StaticClass() { return 0x1b07c52f; }; // value is crc32 FArianeMesh
    virtual uint32 GetClass() override { return StaticClass(); };
    virtual bool HasBaseClass( uint32 BaseClass );

public:
    virtual ~FArianeMesh();
    FArianeMesh();
    // InGeometry3D: created by the derived class (path or cycle geometry), owned by the mesh
    FArianeMesh( UArianeDrawing* InDrawing
               , const FName& InName
               , EArianeAllocationModel InAllocationModel
               , FArianeMeshGeometry3D* InGeometry3D );

public:
    const FColor& GetColor();
    void SetColor( const FColor& InColor );
    UMaterialInterface* GetMaterial();
    void SetMaterial( UMaterialInterface* InMaterialInterface );
    FArianeMeshGeometry3D& GetGeometry3D();

public:
    virtual void ResetDrawing( UArianeDrawing* InDrawing ) override;
    // Fills RenderItem from this mesh (see the top of ArianeLayerVector.cpp)
    virtual void UpdateRenderItem( FArianeMeshRenderItem& RenderItem );
    // Rewrites this mesh's item in the drawing and enqueues it
    virtual void RefreshRenderItem() override;
    // FArianeObject::UpdateTransform computed a new world matrix
    void RefreshRenderItemTransform();
    void SetRenderIndex( int32 InRenderIndex );

protected:
    // Before the Geometry3D is deleted: the proxy skips invisible items
    void HideRenderItem();

protected:
    UPROPERTY( EditAnywhere )
    FColor Color;

    UPROPERTY( EditAnywhere )
    UMaterialInterface* Material;

protected:
    FArianeMeshGeometry3D* Geometry3D;
    // Index of this mesh's item in the drawing's render items, assigned by RebuildRenderItems. Runtime only
    int32 RenderIndex;
};

template<>
struct TStructOpsTypeTraits<FArianeMesh> : public TStructOpsTypeTraitsBase2<FArianeMesh>
{
    enum
    {
        // Reflection copies (templates, spawnables) go property by property: runtime pointers keep the
        // default constructor values instead of being shared with the source.
        WithCopy = false
    };
};
