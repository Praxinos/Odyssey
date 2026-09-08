// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeCycle.h"
#include "ArianeImage.h"
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
        PointAltered &= ((FArianeCycleInvalidationFlags&)RHS).PointAltered;
    }

    Super::AND( RHS );

    return *this;
}

FArianeCycleInvalidationFlags&
FArianeCycleInvalidationFlags::OR( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianeCycleInvalidationFlags::StaticClass() ) )
    {
        PointAltered |= ((FArianeCycleInvalidationFlags&)RHS).PointAltered;
    }

    Super::OR( RHS );

    return *this;
}

FArianeCycleInvalidationFlags&
FArianeCycleInvalidationFlags::SetAll()
{
    PointAltered = 1;

    Super::SetAll();

    return *this;
}

FArianeCycleInvalidationFlags&
FArianeCycleInvalidationFlags::Clear()
{
    Super::Clear();

    PointAltered = 0;

    return *this;
}

bool
FArianeCycleInvalidationFlags::HasAny()
{
    return ( PointAltered ) ? true : Super::HasAny();
}

//--------------------- Vertex buffer



//--------------------- Path

FArianeCycle::~FArianeCycle()
{
}

FArianeCycle::FArianeCycle()
    : FArianeCycle( nullptr
                 , FName( "Ariane Cycle" )
                 , EArianeAllocationModel::InstancedStruct
                 , new FArianeCycleInvalidationFlags() )
{
}

FArianeCycle::FArianeCycle( UArianeImage* InImage
                          , const FName& InName
                          , EArianeAllocationModel InAllocationModel
                          , FArianeCycleInvalidationFlags* InInvalidationFlags )
    : FArianeObject ( InImage
                    , InName
                    , InAllocationModel
                    , InInvalidationFlags ? InInvalidationFlags
                                          : new FArianeCycleInvalidationFlags() )
    , Color ( FColor::Black.WithAlpha(255) )
    , MaterialInterface ( nullptr )
    , Geometry3D ( this )
{
}

void
FArianeCycle::BuildModelVertexCache()
{
    ModelVertexCache.Empty();
    ModelVertexCache.Reserve( Points.Num() );

    for( FArianePoint& Point : Points )
    {
        ModelVertexCache.Emplace( FVector3f( Point.GetPosition() )
                                , FVector3f::Zero() // TangentX
                                , FVector3f::Zero() // TangentZ
                                , FVector2f::Zero()
                                , Color );
    }
}

// static
inline uint32
FArianeCycle::EvaluateSectionPointCount( FArianeGraph::FSection* Section )
{
    // Wevaluate the number of points a section will need for triangulation
    // For linear sections it's easy, a cycle will need 1 point for 1 section
    // for Cubic edge sections, it will need a percentage of the original cubic Segments fraction count. So, basically
    // we calculate a percentage no matter what type of section this is (linear or cubc), and if the result is zero, we force it to 1.
    double ratio = ( Section->EdgeT[1] - Section->EdgeT[0] );
    uint32 SectionPointCount = Section->Edge->FractionCount * ratio;

    return SectionPointCount ? 1 : SectionPointCount;
}

uint32
FArianeCycle::EvaluateContourPointCount( FArianeGraph::FCycle* Cycle )
{
    uint32 PointCount = 0;

    // for each section, we evaluate the number of points it will need for triangulation
    for( FArianeGraph::FSection* Section : Cycle->ContourSections )
    {
        PointCount += EvaluateSectionPointCount( Section );
    }

    return PointCount;
}

void
FArianeCycle::EvaluateGraphCyclePointCount( FArianeGraph::FCycle* Cycle
                                          , std::vector<std::vector<FVector2D>>& EarcutContours
                                          , TArray<FArianePoint>& Points )
{
    uint32 PointCount = 0;
    uint32 CycleCount = 1;

    EarcutContours.resize( 1 + Cycle->Children.Num() );

    // for each inner cycle, we evaluate the number of points it will need for triangulation
    for( FArianeGraph::FCycle* Child : Cycle->Children )
    {
        uint32 ContourPointCount = EvaluateContourPointCount( Child );

        EarcutContours[CycleCount++].reserve( ContourPointCount );

        PointCount += ContourPointCount;
    }

    // Add the result to this cycle's contour (the outer one)
    Points.Reserve( EvaluateContourPointCount( Cycle ) + PointCount );
}

FVector
FArianeCycle::GetNodeFittedPosition( FArianeGraph* Graph, FArianeGraph::FNode* Node, EArianeCycleFittingRule FittingRule )
{
    FVector FittedPosition;

    switch( FittingRule )
    {
        case EArianeCycleFittingRule::FitToPlane :
            FittedPosition = Graph->GetNodeWorldPositionOnPlane( Node );
        break;

        case EArianeCycleFittingRule::FitToPaths :
            FittedPosition = Node->OriginalWorlPosition;
        break;

        default :
            FittedPosition = FVector::Zero();
        break;
    }

    return FittedPosition;
}

void
FArianeCycle::ContourToCoords( FArianeGraph* Graph
                             , FArianeGraph::FCycle* GraphCycle
                             , std::vector<FVector2D>& EarcutContour
                             , TArray<FArianePoint>& OutPoints
                             , EArianeCycleFittingRule FittingRule )
{
    FArianeGraph::FSection* FirstSection = GraphCycle->ContourSections[0];
    uint32 FirstNodeIndex = GraphCycle->ContourNodeIndices[0];
    FArianeGraph::FNode* FirstNode = FirstSection->Nodes[FirstNodeIndex];
    int32 ArraySize = GraphCycle->ContourSections.Num();
    FVector FirstNodeFittedPosition = GetNodeFittedPosition( Graph, FirstNode, FittingRule );

    // Earcut will create indices on a Graph in a 2D coordinates system.
    EarcutContour.push_back( FirstNode->Position );
    // We will then use the indices to form triangles in the 3D corrdinates system, whether on the plane or fitted to the paths
    OutPoints.Emplace( WorldTransform.InverseTransformPosition( FirstNodeFittedPosition ) );

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
            FVector NextNodeFittedPosition = GetNodeFittedPosition( Graph, NextNode, FittingRule );

            // Earcut will create indices on a Graph in a 2D coordinates system.
            EarcutContour.push_back( NextNode->Position );
            // We will then use the indices to form triangles in the 3D corrdinates system, whether on the plane or fitted to the paths
            OutPoints.Emplace( WorldTransform.InverseTransformPosition( NextNodeFittedPosition ) );
        }
    }
}

void
FArianeCycle::GraphCycleToCoords(  FArianeGraph* Graph
                                 , FArianeGraph::FCycle* GraphCycle
                                 , std::vector<std::vector<FVector2D>>& EarcutContours
                                 , TArray<FArianePoint>& OutPoints
                                 , EArianeCycleFittingRule FittingRule )
{
    uint32 CycleCount = 1;

    EvaluateGraphCyclePointCount( GraphCycle, EarcutContours, OutPoints );

    ContourToCoords( Graph, GraphCycle, EarcutContours[0], OutPoints, FittingRule );

    // for each inner cycle, we evaluate the number of points it will need for triangulation
    for( FArianeGraph::FCycle* Child : GraphCycle->Children )
    {
        ContourToCoords( Graph, Child, EarcutContours[CycleCount++], OutPoints, FittingRule );
    }
}

void
FArianeCycle::ImportGraphCycle( FArianeGraph* Graph, FArianeGraph::FCycle* GraphCycle, EArianeCycleFittingRule FittingRule )
{
    Points.Empty();
    EarcutIndices.Empty();

    if( GraphCycle )
    {
        std::vector<std::vector<FVector2D>> Polygon;

        GraphCycleToCoords( Graph, GraphCycle, Polygon, Points, FittingRule );

        // Triangulate. The result is a flat list of indices into the input vertices (numbered ring after
        // ring, so index 6 is {25, 75} here), three per triangle. Output triangles have a consistent
        // winding regardless of the input: counter-clockwise in a y-up coordinate system (clockwise in
        // y-down/screen space). Call std::reverse on the result if you need the opposite orientation.
        std::vector<uint32> indices = mapbox::earcut<uint32>(Polygon);
        // memcpy
        EarcutIndices.SetNumUninitialized( indices.size() );
        // uint32 to int32 array. All values are >= 0 anyways.
        // UProperty macro does not accept uint32 and we need it for the reflection system.
        FMemory::Memcpy( EarcutIndices.GetData(), indices.data(), indices.size() * sizeof( uint32 ) );
    }

    Invalidate( FArianeCycleInvalidationFlags().SetPointAltered() );
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

void
FArianeCycle::Added()
{
    if( MaterialInterface && Image )
    {
        Image->IncrementMaterial( MaterialInterface );
    }
}

void
FArianeCycle::Removed()
{
    if( MaterialInterface && Image )
    {
        Image->DecrementMaterial( MaterialInterface );
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
    if( MaterialInterface && Image )
    {
        Image->DecrementMaterial( MaterialInterface );
    }

    // add the new material to the used material list
    if( InMaterialInterface && Image )
    {
        Image->IncrementMaterial( InMaterialInterface );
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
FArianeCycle::UpdateBoundingBox( EUpdateFlags UpdateFlags )
{
    FVector Min = FVector (  DBL_MAX,  DBL_MAX,  DBL_MAX );
    FVector Max = FVector ( -DBL_MAX, -DBL_MAX, -DBL_MAX );

    BoundingBox = FBox(ForceInit);

    for( FArianePoint& Point : Points )
    {
        FVector PointPosition = Point.GetPosition();

        if( PointPosition.X < Min.X ) Min.X = PointPosition.X;
        if( PointPosition.Y < Min.Y ) Min.Y = PointPosition.Y;
        if( PointPosition.Z < Min.Z ) Min.Z = PointPosition.Z;
        if( PointPosition.X > Max.X ) Max.X = PointPosition.X;
        if( PointPosition.Y > Max.Y ) Max.Y = PointPosition.Y;
        if( PointPosition.Z > Max.Z ) Max.Z = PointPosition.Z;

        BoundingBox.IsValid = 1;
    }

    BoundingBox.Min = Min;
    BoundingBox.Max = Max;
}

void
FArianeCycle::PostEditUndo()
{
    Super::PostEditUndo();

    if( MaterialInterface == nullptr )
        MaterialInterface = GEngine->VertexColorMaterial;

    //if( MaterialInterface )
        Image->IncrementMaterial( MaterialInterface );

    Invalidate( FArianeCycleInvalidationFlags().SetPointAltered() );
}

void
FArianeCycle::PostLoad()
{
    Super::PostLoad();

    if( MaterialInterface == nullptr )
        MaterialInterface = GEngine->VertexColorMaterial;

    //if( MaterialInterface )
        Image->IncrementMaterial( MaterialInterface );

    Invalidate( FArianeCycleInvalidationFlags().SetPointAltered() );
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

TArray<int32>&
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

    if( ( PathInvalidationFlags->PointAltered )
     || ( PathInvalidationFlags->Color ) )
    {
        Geometry3D.Build();
    }
}

FArianeCycleGeometry3D::~FArianeCycleGeometry3D()
{
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
    TArray<int32>& SignedIndices = Cycle->GetEarcutIndices();
    TArray<uint32> UnsignedIndices;

    UnsignedIndices.SetNumUninitialized( SignedIndices.Num() );
    // signed to unsigned. All values are >= 0 anyways.
    FMemory::Memcpy( UnsignedIndices.GetData(), SignedIndices.GetData(), SignedIndices.Num() * sizeof( uint32 ) );

    Cycle->BuildModelVertexCache();

    InitVertexFactory( Cycle->GetModelVertexCache(), UnsignedIndices );

    // DrawingLayer can be null in animation keys
    if( Cycle->GetImage()->GetDrawingLayer().IsValid() )
    {
        // send the vertex data to the graphic card.
        Cycle->GetImage()->GetDrawingLayer()->MarkRenderStateDirty();
    }
}
