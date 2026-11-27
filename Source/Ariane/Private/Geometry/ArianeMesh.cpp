// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeMesh.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
// Unreal headers
#include "Engine/World.h"

//--------------------- Geometry3D

FArianeMeshGeometry3D::~FArianeMeshGeometry3D()
{
    // Some rendering commands use the vertex factory, flush them first
    FlushRenderingCommands();

    if( VertexFactory )
    {
        PositionBuffer.ReleaseResource();
        StaticMeshVB.ReleaseResource();
        ColorBuffer.ReleaseResource();
        IndexBuffer.ReleaseResource();
        VertexFactory->ReleaseResource();

        delete VertexFactory;
    }
}

FArianeMeshGeometry3D::FArianeMeshGeometry3D( FArianeObject* InObject )
    : Object( InObject )
    , VertexCount( 0 )
    , VertexFactory( nullptr )
{

}

const uint32
FArianeMeshGeometry3D::GetVertexCount() const
{
    return VertexCount;
}

const FRawStaticIndexBuffer&
FArianeMeshGeometry3D::GetIndexBuffer() const
{
    return IndexBuffer;
}

FArianeObject*
FArianeMeshGeometry3D::GetObject()
{
     return Object;
}

FLocalVertexFactory*
FArianeMeshGeometry3D::GetVertexFactory()
{
    return VertexFactory;
}

const TArray<FDynamicMeshVertex>& FArianeMeshGeometry3D::GetMeshVertices()
{
    return MeshVertices;
}

const TArray<uint32>& FArianeMeshGeometry3D::GetMeshIndices()
{
    return MeshIndices;
}

void
FArianeMeshGeometry3D::InitVertexFactory()
{
    // VectorLayer can be null in orphan Drawings (animation keys)
    UWorld* World = Object->GetDrawing()->GetVectorLayer().IsValid() ? Object->GetDrawing()->GetVectorLayer()->GetWorld()
                                                                    : nullptr ;

    if( World && MeshVertices.Num() && MeshIndices.Num() )
    {
        if( VertexFactory == nullptr )
        {
            VertexFactory = new FLocalVertexFactory( World->GetFeatureLevel(), "Object Vertex Factory" );
        }

        ENQUEUE_RENDER_COMMAND(StaticMeshVertexBuffersLegacyInit)(
            [ this
            ,  VerticesAsync = CopyTemp(MeshVertices) ] ( FRHICommandListImmediate& RHICmdList )
            {
                FLocalVertexFactory::FDataType Data;

                VertexCount = VerticesAsync.Num();

                if( PositionBuffer.IsInitialized() == false ) PositionBuffer.InitResource( RHICmdList );
                if( StaticMeshVB.IsInitialized()   == false ) StaticMeshVB.InitResource( RHICmdList );
                if( ColorBuffer.IsInitialized()    == false ) ColorBuffer.InitResource( RHICmdList );

                if( VertexCount )
                {
                    PositionBuffer.Init( VertexCount );
                    StaticMeshVB.Init( VertexCount, 1 );
                    ColorBuffer.Init( VertexCount );

                    for ( uint32 i = 0; i < VertexCount; ++i )
                    {
                        FVector3f TangentX = VerticesAsync[i].TangentX.ToFVector3f();
                        FVector3f TangentZ = VerticesAsync[i].TangentZ.ToFVector3f();

                        PositionBuffer.VertexPosition( i ) = VerticesAsync[i].Position;
                        ColorBuffer.VertexColor( i ) = VerticesAsync[i].Color;
                        StaticMeshVB.SetVertexUV( i, 0, VerticesAsync[i].TextureCoordinate[0] );
                        StaticMeshVB.SetVertexTangents( i, TangentX, TangentX.Cross( TangentZ ), TangentZ );
                    }

                    // Copy RAM to VRAM
                    PositionBuffer.UpdateRHI( RHICmdList );
                    StaticMeshVB.UpdateRHI( RHICmdList );
                    ColorBuffer.UpdateRHI( RHICmdList );

                    PositionBuffer.BindPositionVertexBuffer( VertexFactory, Data );
                    StaticMeshVB.BindTangentVertexBuffer( VertexFactory, Data );
                    StaticMeshVB.BindPackedTexCoordVertexBuffer( VertexFactory, Data );
                    ColorBuffer.BindColorVertexBuffer( VertexFactory, Data );

                    VertexFactory->SetData( RHICmdList, Data );

                    // Init / update the factory after SetData
                    if (!VertexFactory->IsInitialized()) {
                        VertexFactory->InitResource(RHICmdList);
                    } else {
                        VertexFactory->UpdateRHI(RHICmdList);
                    }
                }
                else
                {
                    if (VertexFactory->IsInitialized()) VertexFactory->ReleaseResource();
                    if (PositionBuffer.IsInitialized()) PositionBuffer.ReleaseResource();
                    if (StaticMeshVB.IsInitialized()) StaticMeshVB.ReleaseResource();
                    if (ColorBuffer.IsInitialized()) ColorBuffer.ReleaseResource();

                    VertexCount = 0;
                }
            } );

        ENQUEUE_RENDER_COMMAND(IndexBufferInit)(
            [ this
            , IndicesAsync = CopyTemp(MeshIndices) ] ( FRHICommandListImmediate& RHICmdList )
            {
                uint32 IndexCount = IndicesAsync.Num();

                if( IndexCount )
                {
                    IndexBuffer.SetIndices( IndicesAsync, EIndexBufferStride::Type::Force32Bit );

                    if( IndexBuffer.IsInitialized() )
                    {
                        IndexBuffer.UpdateRHI( RHICmdList );
                    }
                    else
                    {
                        IndexBuffer.InitResource( RHICmdList );
                    }
                }
                else
                {
                    if( IndexBuffer.IsInitialized() ) IndexBuffer.ReleaseResource();
                }
            } );
    }
}

//--------------------- Mesh

FArianeMesh::~FArianeMesh()
{
    delete Geometry3D;
}

FArianeMesh::FArianeMesh()
    : FArianeMesh( nullptr
                 , FName( "Ariane Mesh" )
                 , EArianeAllocationModel::InstancedStruct
                 , nullptr )
{
}

FArianeMesh::FArianeMesh( UArianeDrawing* InDrawing
                        , const FName& InName
                        , EArianeAllocationModel InAllocationModel
                        , FArianeMeshGeometry3D* InGeometry3D )
    : FArianeObject ( InDrawing
                    , InName
                    , InAllocationModel )
    , Color ( FColor::Black.WithAlpha(255) )
    , Material ( nullptr )
    , Geometry3D ( InGeometry3D )
{
}

bool
FArianeMesh::HasBaseClass( uint32 BaseClass )
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClass );
}

const FColor&
FArianeMesh::GetColor()
{
    return Color;
}

void
FArianeMesh::SetColor( const FColor& InColor )
{
    Color = InColor;

    Invalidate( FArianeObjectInvalidationFlags().SetColor() );
}

UMaterialInterface*
FArianeMesh::GetMaterial()
{
    return Material;
}

void
FArianeMesh::SetMaterial( UMaterialInterface* InMaterialInterface )
{
    Material = InMaterialInterface;

    Invalidate( FArianeObjectInvalidationFlags().SetColor() );
}

FArianeMeshGeometry3D&
FArianeMesh::GetGeometry3D()
{
    return *Geometry3D;
}
