// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeCycle.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
// Unreal headers
#include "Engine/EngineBaseTypes.h"
#include "StaticMeshResources.h"
#include "RenderResource.h"
#include "MeshBatch.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawIndexBuffer.h"
#include "Materials/MaterialRenderProxy.h"

bool
FArianeCycleInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClass );
}

FArianeCycleInvalidationFlags&
FArianeCycleInvalidationFlags::AND( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeCycleInvalidationFlags::StaticClass() ) )
    {
        Dummy &= ((FArianeCycleInvalidationFlags&)RHS).Dummy;
    }

    Super::AND( RHS );

    return *this;
}

FArianeCycleInvalidationFlags&
FArianeCycleInvalidationFlags::OR( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeCycleInvalidationFlags::StaticClass() ) )
    {
        Dummy |= ((FArianeCycleInvalidationFlags&)RHS).Dummy;
    }

    Super::OR( RHS );

    return *this;
}

FArianeCycleInvalidationFlags&
FArianeCycleInvalidationFlags::SetAll()
{
    Dummy = 1;

    Super::SetAll();

    return *this;
}

FArianeCycleInvalidationFlags&
FArianeCycleInvalidationFlags::Clear()
{
    Super::Clear();

    Dummy = 0;

    return *this;
}

bool
FArianeCycleInvalidationFlags::HasAny()
{
    return ( Dummy ) ? true : Super::HasAny();
}

//--------------------- Vertex buffer



//--------------------- Path

FArianeCycle::~FArianeCycle()
{
}

FArianeCycle::FArianeCycle()
    : FArianeCycle( nullptr
                 , FName( "Ariane Cycle" )
                 , nullptr
                 , nullptr
                 , EArianeAllocationModel::InstancedStruct
                 , new FArianeCycleInvalidationFlags() )
{
}

FArianeCycle::FArianeCycle( UArianeLayerDrawing* InDrawingLayer
                          , const FName& InName
                          , FArianeGraph* Graph
                          , FArianeGraph::FCycle* GraphCycle
                          , EArianeAllocationModel InAllocationModel
                          , FArianeCycleInvalidationFlags* InInvalidationFlags )
    : FArianeObject ( InDrawingLayer
                    , InName
                    , InAllocationModel
                    , InInvalidationFlags ? InInvalidationFlags
                                          : new FArianeCycleInvalidationFlags() )
    , Color ( 0, 0, 0, 255 )
    , MaterialInterface ( nullptr )
    , Geometry3D ( this )
{
    ModelVertexCache.Empty();
    EarcutIndices.Empty();

    if( Graph && GraphCycle )
    {
        std::vector<std::vector<FVector2D>> Polygon = std::vector<std::vector<FVector2D>>( 1 );

        int32 ArraySize = GraphCycle->ContourSections.Num();

        Polygon[0].reserve( ArraySize );
        ModelVertexCache.Reserve( ArraySize );

        if ( ArraySize )
        {
            FArianeGraph::FSection* FirstSection = GraphCycle->ContourSections[0];
            uint32 FirstNodeIndex = GraphCycle->ContourNodeIndices[0];
            FArianeGraph::FNode* FirstNode = FirstSection->Nodes[FirstNodeIndex];

            Polygon[0].push_back( FirstNode->Position );
            ModelVertexCache.Emplace( FVector3f( FirstNode->OriginalPosition )
                                    , FVector3f::Zero() // TangentX
                                    , FVector3f::Zero() // TangentZ
                                    , FVector2f::Zero()
                                    , Color );

            for( int i = 0; i < ArraySize; i++ )
            {
                int n = ( i + 1 ) % ArraySize;
                FArianeGraph::FSection* Section = GraphCycle->ContourSections[i];
                uint32 SectionNodeIndex = GraphCycle->ContourNodeIndices[i];
                uint32 SectionNextNodeIndex = ( SectionNodeIndex == 0 ) ? 1 : 0;

                if( Section->GetClass() == FArianeGraph::FSectionLinear::StaticClass() )
                {
                    FArianeGraph::FSectionLinear* LinearSection = static_cast<FArianeGraph::FSectionLinear*>(Section);
                    FArianeGraph::FNode* NextNode = Section->Nodes[SectionNextNodeIndex];

                    Polygon[0].push_back( NextNode->Position );
                    ModelVertexCache.Emplace( FVector3f( NextNode->OriginalPosition )
                                            , FVector3f::Zero() // TangentX
                                            , FVector3f::Zero() // TangentZ
                                            , FVector2f::Zero()
                                            , Color );
                }
            }
        }

        // Triangulate. The result is a flat list of indices into the input vertices (numbered ring after
        // ring, so index 6 is {25, 75} here), three per triangle. Output triangles have a consistent
        // winding regardless of the input: counter-clockwise in a y-up coordinate system (clockwise in
        // y-down/screen space). Call std::reverse on the result if you need the opposite orientation.
        std::vector<uint32> indices = mapbox::earcut<uint32>(Polygon);
        // memcpy
        EarcutIndices.Append( &indices[0], indices.size() );
    }
}

bool
FArianeCycle::HasBaseClass( uint32 BaseClass )
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass(BaseClass);
}

/*
void
FArianeCycle::PostLoad()
{
    //if( Geometry3D == nullptr )
    {
        //Geometry3D = new FArianeCycleGeometry3D( this );

        InvalidationFlags = new FArianeCycleInvalidationFlags();

        Material = NewObject<UMaterial>();
    }
}
*/

void
FArianeCycle::Added()
{
    if( MaterialInterface && DrawingLayer )
    {
        DrawingLayer->IncrementMaterial( MaterialInterface );
    }
}

void
FArianeCycle::Removed()
{
    if( MaterialInterface && DrawingLayer )
    {
        DrawingLayer->DecrementMaterial( MaterialInterface );
    }
}

UMaterialInterface*
FArianeCycle::GetMaterial()
{
    return MaterialInterface;
}

void
FArianeCycle::SetMaterial( UMaterialInterface* InMaterialInterface )
{
    // remove the current material from the used material list
    if( MaterialInterface && DrawingLayer )
    {
        DrawingLayer->DecrementMaterial( MaterialInterface );
    }

    // add the new material to the used material list
    if( InMaterialInterface && DrawingLayer )
    {
        DrawingLayer->IncrementMaterial( InMaterialInterface );
    }

    MaterialInterface = InMaterialInterface;
}

void
FArianeCycle::CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate )
{
    FArianeCycle* DestinationPath = static_cast<FArianeCycle*>(DestinationObject);

    Super::CopySettings( DestinationObject, CopyArgs, false );
/*
    DestinationPath->LineType = LineType;
    DestinationPath->Color = Color;
    DestinationPath->MaterialInterface = MaterialInterface;
*/

    if( bInvalidate )
    {
        DestinationPath->Invalidate( FArianeCycleInvalidationFlags().SetAll() );
    }
}

FArianeObject*
FArianeCycle::CopyShape( const FCopyArgs& CopyArgs )
{
    TArray<FArianeVertex*> LookupTable;
    FArianeCycle* CycleCopy = nullptr;
    uint32 VertexID = 0;
/*
    PathCopy = CopyArgs.DrawingLayer->AllocPath( nullptr, Name, CopyArgs.AllocationModel );

    LookupTable.Reserve ( Vertices.Num() );

    // Copy Geometry. First, vertices.
    for( FArianeVertexID& OriginalVertexID : Vertices )
    {
        FArianeVertex* OriginalVertex = OriginalVertexID.GetVertex();
        FVector OriginalVertexPosition = OriginalVertex->GetPosition();
        FVector OriginalVertexNormal = OriginalVertex->GetNormal();
        double OriginalVertexRadius = OriginalVertex->GetRadius();
        FArianeVertex* NewVertex = PathCopy->AllocVertex( OriginalVertexPosition
                                                        , OriginalVertexNormal
                                                        , OriginalVertexRadius
                                                        , CopyArgs.AllocationModel );

        // Don't check if NewVertex is null, it must not be.
        NewVertex->SetHandleAligned( OriginalVertex->IsHandleAligned() );

        LookupTable.Add( NewVertex );

        OriginalVertex->SetID( VertexID++ );

        PathCopy->AddVertex( NewVertex );
    }

    // Copy Geometry. Second, segments.
    for( FArianeSegmentID& OriginalSegmentID : Segments )
    {
        FArianeSegment* OriginalSegment = OriginalSegmentID.GetSegment();
        FArianeVertex* Vertex0 = static_cast<FArianeVertex*>( OriginalSegment->GetVertex((uint32)0) );
        FArianeVertex* Vertex1 = static_cast<FArianeVertex*>( OriginalSegment->GetVertex((uint32)1) );
        FArianeSegment* NewSegment = nullptr;

        if( OriginalSegment->GetClass() == FArianeSegmentCubic::StaticClass() )
        {
            FArianeSegmentCubic* OriginalCubicSegment = static_cast<FArianeSegmentCubic*>(OriginalSegment);
            FArianeHandleSegment* OriginalHandle0 = OriginalCubicSegment->GetHandle((uint32)0);
            FArianeHandleSegment* OriginalHandle1 = OriginalCubicSegment->GetHandle((uint32)1);
            FVector OriginalHandle0Position = OriginalHandle0->GetPosition();
            FVector OriginalHandle1Position = OriginalHandle1->GetPosition();

            NewSegment = PathCopy->AllocCubicSegment ( LookupTable[Vertex0->GetID()]
                                                     , OriginalHandle0Position.X
                                                     , OriginalHandle0Position.Y
                                                     , OriginalHandle0Position.Z
                                                     , OriginalHandle1Position.X
                                                     , OriginalHandle1Position.Y
                                                     , OriginalHandle1Position.Z
                                                     , LookupTable[Vertex1->GetID()]
                                                     , CopyArgs.AllocationModel );
        }

        if( OriginalSegment->GetClass() == FArianeSegment::StaticClass() )
        {
            NewSegment = PathCopy->AllocSegment ( LookupTable[Vertex0->GetID()]
                                                , LookupTable[Vertex1->GetID()]
                                                , CopyArgs.AllocationModel );
        }

        // Don't check if NewSegment is null, it must not be.
        PathCopy->AddSegment( NewSegment );
    }
*/
    return CycleCopy;
}

void
FArianeCycle::UpdateBounds()
{
    Bounds = FBoxSphereBounds(ForceInit);

/*
    for( FArianeSegmentID& SegmentID : Segments )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        Bounds = Bounds + Segment->GetBounds();
    }
*/
}

void
FArianeCycle::PostEditUndo()
{
    Super::PostEditUndo();

    if( MaterialInterface == nullptr )
        MaterialInterface = GEngine->VertexColorMaterial;

    //if( MaterialInterface )
        DrawingLayer->IncrementMaterial( MaterialInterface );

    Invalidate( FArianeCycleInvalidationFlags().SetDummy() );
}

void
FArianeCycle::PostLoad()
{
    Super::PostLoad();

    if( MaterialInterface == nullptr )
        MaterialInterface = GEngine->VertexColorMaterial;

    //if( MaterialInterface )
        DrawingLayer->IncrementMaterial( MaterialInterface );

    Invalidate( FArianeCycleInvalidationFlags().SetDummy() );
}

FArianeCycleGeometry3D&
FArianeCycle::GetGeometry3D()
{
    return Geometry3D;
}

const FColor&
FArianeCycle::GetColor()
{
    return Color;
}

void
FArianeCycle::SetColor( const FColor& InColor )
{
    Color = InColor;

    for( FDynamicMeshVertex& ModelVertex : ModelVertexCache )
    {
        ModelVertex.Color = Color;
    }

    Invalidate( FArianeCycleInvalidationFlags().SetColor() );
}

TArray<uint32>&
FArianeCycle::GetEarcutIndices()
{
    return EarcutIndices;
}

TArray<FDynamicMeshVertex>&
FArianeCycle::GetModelVertexCache()
{
    return ModelVertexCache;
}

void
FArianeCycle::ExportProperties( FArianeObject* DestObject )
{
    Super::ExportProperties( DestObject );

    if( DestObject->GetClass() == FArianeCycle::StaticClass() )
    {
        FArianeCycle* DestCycle = static_cast<FArianeCycle*>(DestObject);

        //DestPath->Color = Color;
        //DestPath->LineType = LineType;
    }
}

void
FArianeCycle::UpdateShape( EUpdateFlags UpdateFlags )
{
    FArianeCycleInvalidationFlags* PathInvalidationFlags = static_cast<FArianeCycleInvalidationFlags*>(InvalidationFlags);
/*
    if( PathInvalidationFlags->VertexAddedOrRemoved
     || PathInvalidationFlags->SegmentAddedOrRemoved )
    {
        FindChains();
    }

    if( PathInvalidationFlags->VertexAltered
     || PathInvalidationFlags->VertexAddedOrRemoved
     || PathInvalidationFlags->SegmentAltered
     || PathInvalidationFlags->SegmentAddedOrRemoved )
    {
        Geometry3D.Build();

        UpdateBounds();
    }
*/
    Geometry3D.Build();
}

FArianeCycleGeometry3D::~FArianeCycleGeometry3D()
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

FArianeCycleGeometry3D::FArianeCycleGeometry3D( FArianeCycle* InCycle )
    : FArianeObjectGeometry3D( InCycle )
{
}

FArianeCycle*
FArianeCycleGeometry3D::GetCycle()
{
    return static_cast<FArianeCycle*>(Object);
}

void
FArianeCycleGeometry3D::Build()
{
    FArianeCycle* Cycle = GetCycle();

    InitVertexFactory( Cycle->GetModelVertexCache(), Cycle->GetEarcutIndices() );
}
