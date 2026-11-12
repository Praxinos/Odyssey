// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeCycle.h"
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
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
                 , EArianeAllocationModel::InstancedStruct )
{
}

FArianeCycle::FArianeCycle( UArianeDrawing* InDrawing
                          , const FName& InName
                          , EArianeAllocationModel InAllocationModel )
    : FArianeObject ( InDrawing
                    , InName
                    , InAllocationModel )
    , Color ( FColor::Black.WithAlpha(255) )
    , Material( nullptr )
    , Geometry3D ( new FArianeCycleGeometry3D( this ) )
{
}

FArianeCycleInvalidationFlags&
FArianeCycle::GetInvalidationFlags()
{
    FArianeCycleInvalidationFlags* InvalidationFlagsPtr = InstancedInvalidationFlags.GetMutablePtr<FArianeCycleInvalidationFlags>();

    if( InvalidationFlagsPtr == nullptr )
    {
        InstancedInvalidationFlags = FInstancedStruct::Make<FArianeCycleInvalidationFlags>();

        InvalidationFlagsPtr = InstancedInvalidationFlags.GetMutablePtr<FArianeCycleInvalidationFlags>();
    }

    return *InvalidationFlagsPtr;
}

void
FArianeCycle::ResetDrawing( UArianeDrawing* InDrawing )
{
    Super::ResetDrawing( InDrawing );

    Geometry3D = new FArianeCycleGeometry3D( this );
    ModelVertexCache.Empty();
}

void
FArianeCycle::PostReset()
{
    Super::PostReset();

    Invalidate( FArianeCycleInvalidationFlags().SetColor() );
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

uint32
FArianeCycle::EvaluateGraphCyclePointCount( FArianeGraph::FCycle* Cycle
                                          , std::vector<std::vector<FVector2D>>& EarcutContours )
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
    return EvaluateContourPointCount( Cycle ) + PointCount;
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
            FittedPosition = Node->OriginalWorldPosition;
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
                             , TArray<FVector2f>& OutUVs
                             , EArianeCycleFittingRule FittingRule )
{
    FArianeGraph::FSection* FirstSection = GraphCycle->ContourSections[0];
    uint32 FirstNodeIndex = GraphCycle->ContourNodeIndices[0];
    FArianeGraph::FNode* FirstNode = FirstSection->Nodes[FirstNodeIndex];
    int32 ArraySize = GraphCycle->ContourSections.Num();
    FVector FirstNodeFittedPosition = GetNodeFittedPosition( Graph, FirstNode, FittingRule );
    const FBox2D& GraphCycleBBox = GraphCycle->GetBBox();

    // Earcut will create indices on a Graph in a 2D coordinates system.
    EarcutContour.push_back( FirstNode->Position );
    // We will then use the indices to form triangles in the 3D corrdinates system, whether on the plane or fitted to the paths
    OutPoints.Emplace( WorldTransform.InverseTransformPosition( FirstNodeFittedPosition ) );
    OutUVs.Emplace( ( FirstNode->Position - GraphCycleBBox.Min ) / GraphCycleBBox.GetSize() );

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
            OutUVs.Emplace( ( NextNode->Position - GraphCycleBBox.Min ) / GraphCycleBBox.GetSize() );
        }
    }
}

void
FArianeCycle::GraphCycleToCoords(  FArianeGraph* Graph
                                 , FArianeGraph::FCycle* GraphCycle
                                 , std::vector<std::vector<FVector2D>>& EarcutContours
                                 , TArray<FArianePoint>& OutPoints
                                 , TArray<FVector2f>& OutUVs
                                 , EArianeCycleFittingRule FittingRule )
{
    uint32 CycleCount = 1;

    uint32 PointCount = EvaluateGraphCyclePointCount( GraphCycle, EarcutContours );

    OutPoints.Reserve( PointCount );
    OutUVs.Reserve( PointCount );

    ContourToCoords( Graph, GraphCycle, EarcutContours[0], OutPoints, OutUVs, FittingRule );

    // for each inner cycle, we evaluate the number of points it will need for triangulation
    for( FArianeGraph::FCycle* Child : GraphCycle->Children )
    {
        ContourToCoords( Graph, Child, EarcutContours[CycleCount++], OutPoints, OutUVs, FittingRule );
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

        GraphCycleToCoords( Graph, GraphCycle, Polygon, Points, UVs, FittingRule );

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
}

void
FArianeCycle::Removed()
{
}

UMaterialInterface*
FArianeCycle::GetMaterial()
{
    return Material;
}

void
FArianeCycle::SetMaterial( UMaterialInterface* InMaterialInterface )
{
    Material = InMaterialInterface;

    Invalidate( FArianeCycleInvalidationFlags().SetColor() );
}

void
FArianeCycle::CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate )
{
    FArianeCycle* DestinationCycle = static_cast<FArianeCycle*>(DestinationObject);

    Super::CopySettings( DestinationObject, CopyArgs, false );

    DestinationCycle->Color = Color;
    DestinationCycle->Material = Material;

    if( bInvalidate )
    {
        DestinationCycle->Invalidate( FArianeCycleInvalidationFlags().SetAll() );
    }
}

FArianeObject*
FArianeCycle::CopyShape( const FCopyArgs& CopyArgs )
{
    FArianeCycle* CycleCopy = nullptr;

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

    if( Material == nullptr )
    {
        UMaterial* DefaultMaterial = Cast<UMaterial>( StaticLoadObject( UMaterial::StaticClass()
                                                                      , nullptr
                                                                      , TEXT("/Odyssey/Materials/ArianeDefaultMaterial.ArianeDefaultMaterial") ) );

        SetMaterial( DefaultMaterial );
    }

    Invalidate( FArianeCycleInvalidationFlags().SetPointAltered() );
}

void
FArianeCycle::PostLoad()
{
    Super::PostLoad();

    if( Material == nullptr )
    {
        UMaterial* DefaultMaterial = Cast<UMaterial>( StaticLoadObject( UMaterial::StaticClass()
                                                                      , nullptr
                                                                      , TEXT("/Odyssey/Materials/ArianeDefaultMaterial.ArianeDefaultMaterial") ) );

        SetMaterial( DefaultMaterial );
    }

    Invalidate( FArianeCycleInvalidationFlags().SetPointAltered() );
}

FArianeCycleGeometry3D&
FArianeCycle::GetGeometry3D()
{
    return *Geometry3D;
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

    Invalidate( FArianeCycleInvalidationFlags().SetColor() );
}

TArray<int32>&
FArianeCycle::GetEarcutIndices()
{
    return EarcutIndices;
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
    FArianeCycleInvalidationFlags* CyceInvalidationFlags = &GetInvalidationFlags();

    if( ( CyceInvalidationFlags->PointAltered )
     || ( CyceInvalidationFlags->Color ) )
    {
        Geometry3D->Build();
    }
}

TArray<FArianePoint>&
FArianeCycle::GetPoints()
{
    return Points;
}

TArray<FVector2f>&
FArianeCycle::GetUVs()
{
    return UVs;
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
    struct FNormalAccu
    {
        FVector TangentX;
        FVector TangentZ;
        uint32 Count;
    };

    FArianeCycle* Cycle = GetCycle();
    TArray<int32>& SignedIndices = Cycle->GetEarcutIndices();
    TArray<FArianePoint>& CyclePoints = Cycle->GetPoints();
    TArray<FVector2f>& CycleUVs = Cycle->GetUVs();
    // helper to compute the normal vector for each vertex.
    TArray<FNormalAccu> NormalAccus;

    NormalAccus.SetNumZeroed( CyclePoints.Num() );

    MeshVertices.Empty();
    MeshIndices.Empty();

    MeshVertices.Reserve( Cycle->GetPoints().Num() );
    MeshIndices.SetNumUninitialized( SignedIndices.Num() );

    // signed to unsigned. All values are >= 0 anyways.
    FMemory::Memcpy( MeshIndices.GetData(), SignedIndices.GetData(), SignedIndices.Num() * sizeof( uint32 ) );

    // for each triangle, compute it's normal vector and UV Tangent Calculation
    for( int32 i = 0; i < MeshIndices.Num(); i += 3 )
    {
        uint32 Point0Index = MeshIndices[i];
        uint32 Point1Index = MeshIndices[i+1];
        uint32 Point2Index = MeshIndices[i+2];
        FArianePoint& Point0 = CyclePoints[Point0Index];
        FArianePoint& Point1 = CyclePoints[Point1Index];
        FArianePoint& Point2 = CyclePoints[Point2Index];
        FVector P0P1 = ( Point1.GetPosition() - Point0.GetPosition() );
        FVector P0P2 = ( Point2.GetPosition() - Point0.GetPosition() );
        FVector Cross = FVector::CrossProduct( P0P1, P0P2 ).GetSafeNormal();
        FVector TangentX = FVector::Zero();

        // UVs could be empty for cycles that were generated before UVs were implemented.
        if( CycleUVs.Num() )
        {
            FVector2f& UV0 = CycleUVs[Point0Index];
            FVector2f& UV1 = CycleUVs[Point1Index];
            FVector2f& UV2 = CycleUVs[Point2Index];
            FVector2f UV0UV1 = UV1 - UV0;
            FVector2f UV0UV2 = UV2 - UV0;
            float UVDeterminant = (UV0UV1.X * UV0UV2.Y - UV0UV2.X * UV0UV1.Y);


            if( UVDeterminant )
            {
                float InvUVDeterminant = 1.0f / UVDeterminant;

                TangentX = (P0P1 * UV0UV2.Y - P0P2 * UV0UV1.Y) * InvUVDeterminant;
            }
        }

        NormalAccus[Point0Index].TangentX += TangentX;
        NormalAccus[Point0Index].TangentZ += Cross;
        NormalAccus[Point0Index].Count++;

        NormalAccus[Point1Index].TangentX += TangentX;
        NormalAccus[Point1Index].TangentZ += Cross;
        NormalAccus[Point1Index].Count++;

        NormalAccus[Point2Index].TangentX += TangentX;
        NormalAccus[Point2Index].TangentZ += Cross;
        NormalAccus[Point2Index].Count++;
    }

    // average the result
    for( FNormalAccu& NormalAccu : NormalAccus )
    {
        if( NormalAccu.Count )
        {
            NormalAccu.TangentZ = ( NormalAccu.TangentZ / NormalAccu.Count ).GetSafeNormal();

            // Gram-Schmidt to force TangentX to be perpendicular to TangentZ
            NormalAccu.TangentX = ( NormalAccu.TangentX - NormalAccu.TangentZ * FVector::DotProduct( NormalAccu.TangentZ
                                                                                                   , NormalAccu.TangentX ) ).GetSafeNormal();
        }
    }

    for( int32 i = 0; i < CyclePoints.Num(); i++  )
    {
        FArianePoint& Point = CyclePoints[i];
        FVector2f UV = FVector2f::Zero();

        if ( CycleUVs.Num() )
        {
            UV = CycleUVs[i];
        }

        MeshVertices.Emplace( FVector3f( Point.GetPosition() )
                            , FVector3f( NormalAccus[i].TangentX ) // TangentX
                            , FVector3f( NormalAccus[i].TangentZ )// TangentZ
                            , UV
                            , Cycle->GetColor() );
    }

    InitVertexFactory();

    // VectorLayer can be null in animation keys
    if( Cycle->GetDrawing()->GetVectorLayer().IsValid() )
    {
        // send the vertex data to the graphic card.
        Cycle->GetDrawing()->GetVectorLayer()->MarkRenderStateDirty();
    }
}
