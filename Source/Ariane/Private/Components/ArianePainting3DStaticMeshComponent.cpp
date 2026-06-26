// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianePainting3DStaticMeshComponent.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeSegment.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerFolder.h"
// Unreal Headers
#include "MeshDescription.h"
#include "MeshDescriptionBuilder.h"
#include "StaticMeshAttributes.h"

// static
void
UArianePainting3DStaticMeshComponent::ConvertPathToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                                             , FArianePath* Path )
{
    TArray<int32> MeshVertexIDs;
    uint32 PathVertexCount = 0;
    uint32 PathIndexCount = 0;

    // Count the number of vertices for allocation
    for ( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        PathVertexCount += Segment->GetModelVertexCache().Num();
        PathIndexCount += Segment->GetIndexCache().Num();
    }

    // allocate memory IDs;
    MeshVertexIDs.Reserve( PathVertexCount );

    for ( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        for( const FDynamicMeshVertex& DynamicMeshVertex : Segment->GetModelVertexCache() )
        {
            int32 MeshVertexID = MeshDescriptionBuilder.AppendVertex( FVector( DynamicMeshVertex.Position ) );

            MeshVertexIDs.Add( MeshVertexID );
        }
    }

    uint32 VertexIDOffset = 0;

    // Allocate a polygon group
    FPolygonGroupID PolygonGroup = MeshDescriptionBuilder.AppendPolygonGroup();

    for ( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();
        const TArray<uint32>& SegmentIndexCache = Segment->GetIndexCache();
        const TArray<FDynamicMeshVertex>& SegmentModelVertexCache = Segment->GetModelVertexCache();

        for( int32 i = 0; i < SegmentIndexCache.Num(); i += 3 )
        {
            FVertexInstanceID MeshVertexInstanceID[3];

            for( uint32 j = 0; j < 3; j++ )
            {
                uint32 SegmentModelVertexID = SegmentIndexCache[i + j];
                const FDynamicMeshVertex& SegmentModelVertex = SegmentModelVertexCache[SegmentModelVertexID];
                int32 MeshVertexID = MeshVertexIDs[VertexIDOffset + SegmentModelVertexID];

                MeshVertexInstanceID[j] = MeshDescriptionBuilder.AppendInstance( MeshVertexID );

                MeshDescriptionBuilder.SetInstanceNormal( MeshVertexInstanceID[j]
                                                        , SegmentModelVertex.TangentZ.ToFVector() );
                MeshDescriptionBuilder.SetInstanceUV( MeshVertexInstanceID[j]
                                                    , FVector2D( SegmentModelVertex.TextureCoordinate[0] )
                                                    , 0 );
                MeshDescriptionBuilder.SetInstanceColor( MeshVertexInstanceID[j], FLinearColor( SegmentModelVertex.Color ) );
            }

            MeshDescriptionBuilder.AppendTriangle( MeshVertexInstanceID[2]
                                                 , MeshVertexInstanceID[1]
                                                 , MeshVertexInstanceID[0]
                                                 , PolygonGroup );
        }

        VertexIDOffset += SegmentModelVertexCache.Num();
    }

}

// static
void
UArianePainting3DStaticMeshComponent::ConvertLayerToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                                              , UArianeLayerDrawing* DrawingLayer )
{
    DrawingLayer->GetRootGroup()->Traverse(
        [ &MeshDescriptionBuilder ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            // convert path and path-like primitives
            if( Object->HasBaseClass( FArianePath::StaticClass() ) )
            {
                FArianePath* Path = static_cast<FArianePath*>(Object);

                ConvertPathToStaticMesh( MeshDescriptionBuilder, Path );
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );
}

void
UArianePainting3DStaticMeshComponent::ConvertToStaticMesh()
{
    FName UniqueMeshName = MakeUniqueObjectName( GetTransientPackage()
                                               , UStaticMesh::StaticClass()
                                               , TEXT("TransientPaintingMesh_"));
    UStaticMesh* ConvertedStaticMesh = NewObject<UStaticMesh>(GetTransientPackage(), UniqueMeshName, RF_Transient);
    FMeshDescription MeshDescription;
    FStaticMeshAttributes Attributes(MeshDescription);

    UArianePainting3DComponent* Painting3DComponent = Cast<UArianePainting3DComponent>(GetOuter());

    if( Painting3DComponent )
    {
        UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

        ConvertedStaticMesh->InitResources();
        ConvertedStaticMesh->SetLightingGuid();

        Attributes.Register();

        FMeshDescriptionBuilder MeshDescriptionBuilder;

        MeshDescriptionBuilder.SetMeshDescription( &MeshDescription );
        MeshDescriptionBuilder.EnablePolyGroups();
        MeshDescriptionBuilder.SetNumUVLayers(1);

        LayerStack->GetRootFolder()->Traverse(
            [ &MeshDescriptionBuilder ]( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
            {
                UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

                if( DrawingLayer )
                {
                    ConvertLayerToStaticMesh( MeshDescriptionBuilder, DrawingLayer );
                }

                return UArianeLayerFolder::ETraversalReturnValue::Continue;
            } );

        // At least one material must be added
        ConvertedStaticMesh->GetStaticMaterials().Add( FStaticMaterial() );

        FStaticMeshSourceModel& StaticMeshSourceModel = ConvertedStaticMesh->AddSourceModel();
        StaticMeshSourceModel.BuildSettings.bRecomputeNormals = false;
        StaticMeshSourceModel.BuildSettings.bRecomputeTangents = true;
        StaticMeshSourceModel.BuildSettings.bRemoveDegenerates = false;

        ConvertedStaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));

        /*if ( Material )
        {
            StaticMesh->GetStaticMaterials().Add( FStaticMaterial( Material ) );
        }*/

        ConvertedStaticMesh->SetImportVersion( EImportStaticMeshVersion::LastVersion );
        ConvertedStaticMesh->InitResources();
        ConvertedStaticMesh->SetLightingGuid();
        ConvertedStaticMesh->Build( false );
#if WITH_EDITOR
        ConvertedStaticMesh->PostEditChange();
#else
        ConvertedStaticMesh->UpdateResource();
#endif

        SetStaticMesh(ConvertedStaticMesh);

        UE_LOG( LogTemp
              , Warning
              , TEXT("UArianePainting3DStaticMeshComponent::ConvertToStaticMesh: %d %d %d")
              , GetStaticMesh().Get()
              , GetStaticMesh()->GetRenderData()
              , IsVisible() );

        //ConvertedStaticMesh->MarkPackageDirty();

    }
}
