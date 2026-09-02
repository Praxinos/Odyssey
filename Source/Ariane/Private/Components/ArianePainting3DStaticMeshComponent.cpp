// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Headers
#include "ArianePainting3DStaticMeshComponent.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeSegment.h"
#include "ArianeImage.h"
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
    FPolygonGroupID PolygonGroup = MeshDescriptionBuilder.AppendPolygonGroup( Path->GetMaterial()->GetFName() );

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
UArianePainting3DStaticMeshComponent::ConvertImageToStaticMesh( FMeshDescriptionBuilder& MeshDescriptionBuilder
                                                              , UArianeLayerDrawing* DrawingLayer )
{
    FArianeGroup* RootGroup = DrawingLayer->GetImage()->GetRootGroup();

    FArianeObject::Traverse( RootGroup
                           , [ &MeshDescriptionBuilder ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
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

        TArray<UMaterialInterface*> MaterialInterfaces;


        LayerStack->GetRootFolder()->Traverse(
            [ &MaterialInterfaces
            , ConvertedStaticMesh
            , &MeshDescriptionBuilder ]( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
            {
                UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

                if( DrawingLayer )
                {
                    UArianeImage* Image = DrawingLayer->GetImage();

                    Image->GetUsedMaterials( MaterialInterfaces );

                    for( int32 i = 0; i < MaterialInterfaces.Num(); i++ )
                    {
                        UMaterialInterface *MaterialInterface = MaterialInterfaces[i];

                        ConvertedStaticMesh->GetStaticMaterials().Add( FStaticMaterial( MaterialInterface
                                                                                      , MaterialInterface->GetFName() ) );
                    }

                    ConvertImageToStaticMesh( MeshDescriptionBuilder, DrawingLayer );
                }

                return UArianeLayerFolder::ETraversalReturnValue::Continue;
            } );

        FStaticMeshSourceModel& StaticMeshSourceModel = ConvertedStaticMesh->AddSourceModel();
        StaticMeshSourceModel.BuildSettings.bRecomputeNormals = false;
        StaticMeshSourceModel.BuildSettings.bRecomputeTangents = true;
        StaticMeshSourceModel.BuildSettings.bRemoveDegenerates = false;

        ConvertedStaticMesh->CreateMeshDescription(0, MoveTemp(MeshDescription));
        ConvertedStaticMesh->CommitMeshDescription(0);

        // <begin_explanation>
        // I after some debugging and troubles with materials, I found that I had to fill the Mesg's SectionInfoMap
        // before converting the mesh with Build() in order to have Material correctly attached to the sections (polygongroups).
        // I don't get the point of doing :
        // FPolygonGroupID PolygonGroup = MeshDescriptionBuilder.AppendPolygonGroup( Path->GetMaterial()->GetFName() );
        // in ConvertPathToStaticMesh then if the mapping is not automatically done. Anyways...
        const TPolygonGroupAttributesRef<FName> PolygonGroupNames = Attributes.GetPolygonGroupMaterialSlotNames();

        for ( const FPolygonGroupID PolygonGroupID : MeshDescription.PolygonGroups().GetElementIDs() )
        {
            int32 SectionIndex = PolygonGroupID.GetValue();
            FName MaterialSlotName = *PolygonGroupNames[PolygonGroupID].ToString();
            int32 MaterialIndex = ConvertedStaticMesh->GetMaterialIndexFromImportedMaterialSlotName( MaterialSlotName );

            ConvertedStaticMesh->GetSectionInfoMap().Set( 0
                                                        , SectionIndex
                                                        , FMeshSectionInfo(MaterialIndex) );
        }
        // <end_explanation>

        ConvertedStaticMesh->SetImportVersion( EImportStaticMeshVersion::LastVersion );
        ConvertedStaticMesh->InitResources();
        ConvertedStaticMesh->SetLightingGuid();
        ConvertedStaticMesh->Build( false );

#if WITH_EDITOR
        ConvertedStaticMesh->PostEditChange();
#endif

        SetStaticMesh(ConvertedStaticMesh);

        MarkRenderStateDirty();
    }
}
