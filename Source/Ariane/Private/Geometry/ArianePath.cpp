// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
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
FArianePathInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClass );
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::AND( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianePathInvalidationFlags::StaticClass() ) )
    {
        VertexAltered &= ((FArianePathInvalidationFlags&)RHS).VertexAltered;
        SegmentAltered &= ((FArianePathInvalidationFlags&)RHS).SegmentAltered;
        VertexAddedOrRemoved &= ((FArianePathInvalidationFlags&)RHS).VertexAddedOrRemoved;
        SegmentAddedOrRemoved &= ((FArianePathInvalidationFlags&)RHS).SegmentAddedOrRemoved;
    }

    Super::AND( RHS );

    return *this;
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::OR( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianePathInvalidationFlags::StaticClass() ) )
    {
        VertexAltered |= ((FArianePathInvalidationFlags&)RHS).VertexAltered;
        SegmentAltered |= ((FArianePathInvalidationFlags&)RHS).SegmentAltered;
        VertexAddedOrRemoved |= ((FArianePathInvalidationFlags&)RHS).VertexAddedOrRemoved;
        SegmentAddedOrRemoved |= ((FArianePathInvalidationFlags&)RHS).SegmentAddedOrRemoved;
    }

    Super::OR( RHS );

    return *this;
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::SetAll()
{
    VertexAltered  =
    SegmentAltered =
    VertexAddedOrRemoved  =
    SegmentAddedOrRemoved = 1;

    Super::SetAll();

    return *this;
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::Clear()
{
    Super::Clear();

    VertexAltered  =
    SegmentAltered =
    VertexAddedOrRemoved  =
    SegmentAddedOrRemoved = 0;

    return *this;
}

bool
FArianePathInvalidationFlags::HasAny()
{
    return ( VertexAltered
          || SegmentAltered
          || VertexAddedOrRemoved
          || SegmentAddedOrRemoved ) ? true : Super::HasAny();
}

//------------------- chain

FArianePath::Chain::~Chain()
{
}

FArianePath::Chain::Chain( FArianePath* Path, FArianeVertex* UnchainedVertex )
{
    FArianeVertex* CurrentVertex = UnchainedVertex;
    FArianeSegment* CurrentSegment = CurrentVertex->GetFirstSegment();

    Segments.Reserve( Path->GetSegments().Num() );

    LeadingVertex = CurrentVertex;

    CurrentVertex->SetChained( true );

    while( CurrentSegment )
    {
        FArianeVertex* nextVertex = CurrentSegment->GetOtherVertex( CurrentVertex );

        Segments.Push( CurrentSegment );

        if( nextVertex->IsChained() == false )
        {
            FArianeSegment* nextSegment = nextVertex->GetOtherSegment( CurrentSegment );

            nextVertex->SetChained( true );

            CurrentVertex = nextVertex;
            CurrentSegment = nextSegment;
        }
        else
        {
            CurrentVertex = nextVertex;
            CurrentSegment = nullptr;
        }
    }
}

// callback must return false to keep iterating
void
FArianePath::Chain::IterateSegments( TFunction<bool( FArianeVertex*, FArianeSegment*)> Callback ) const
{
    FArianeVertex* CurrentVertex = LeadingVertex;

    for( FArianeSegment* Segment : Segments )
    {
        FArianeVertex* NextVertex = Segment->GetOtherVertex( CurrentVertex );

        if( Callback( CurrentVertex, Segment ) == true )
        {
            return;
        }

        CurrentVertex = NextVertex;
    }
}


//--------------------- Vertex buffer

void FArianePathVertexBuffer::Resize(  uint32 InVertexCount, FRHICommandListBase& RHICmdList )
{
    VertexBufferRHI.SafeRelease();

    FRHIResourceCreateInfo ResourceInfo( TEXT( "Dynamic Vertex Buffer" ) );

    VertexCount = InVertexCount;

    VertexBufferRHI = RHICmdList.CreateVertexBuffer( VertexCount * sizeof( FDynamicMeshVertex )
                                                   , BUF_Dynamic | BUF_VertexBuffer | BUF_ShaderResource
                                                   , ERHIAccess::VertexOrIndexBuffer
                                                   , ResourceInfo );
}

void FArianePathVertexBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
    Resize( VertexCount, RHICmdList );
}

//--------------------- Path

FArianePath::~FArianePath()
{
}

FArianePath::FArianePath()
    : FArianeObject()
    , Geometry3D ( this )
    , LineType( EArianePathLineType::Tube )
    , Color ( 0, 0, 0, 255 )
{
    InvalidationFlags = new FArianePathInvalidationFlags();
}

FArianePath::FArianePath( UArianeLayerDrawing* InDrawingLayer )
    : FArianeObject ( InDrawingLayer )
    , Geometry3D ( this )
    , LineType ( EArianePathLineType::Tube )
    , Color ( 0, 0, 0, 255 )
{
    InvalidationFlags = new FArianePathInvalidationFlags();
}

/*
void
FArianePath::PostLoad()
{
    //if( Geometry3D == nullptr )
    {
        //Geometry3D = new FArianePathGeometry3D( this );

        InvalidationFlags = new FArianePathInvalidationFlags();

        Material = NewObject<UMaterial>();
    }
}
*/

const TArray<FArianePath::Chain>&
FArianePath::GetChains()
{
    return Chains;
}

FArianeVertex*
FArianePath::AllocVertex( const FVector& iPosition, const FVector& InNormal, double InRadius )
{
    InstancedVertices.Push( FInstancedStruct::Make<FArianeVertex>( this, iPosition, InNormal, InRadius ) );

    FArianeVertex* NewVertex = InstancedVertices.Last().GetMutablePtr<FArianeVertex>();

    return NewVertex;
}

void
FArianePath::RemoveVertex( FArianeVertex* Vertex, bool bRemoveFromInstancedVertices )
{
    Vertices.RemoveAll( [Vertex]( FArianeVertexID& VertexID ) -> bool
    {
        return ( Vertex == VertexID.GetVertex() ) ? true : false;
    } );

    Invalidate( FArianePathInvalidationFlags().SetVertexAddedOrRemoved() );

    if( bRemoveFromInstancedVertices )
    {
        InstancedVertices.RemoveAll( [Vertex]( FInstancedStruct& Struct ) -> bool
        {
            return ( Vertex == Struct.GetPtr<FArianeVertex>() ) ? true : false;
        } );
    }
}

FArianeVertex*
FArianePath::GetVertexByGuid( const FGuid& InGuid )
{
    for( FInstancedStruct& InstancedVertex : InstancedVertices )
    {
        FArianeVertex* Vertex = InstancedVertex.GetMutablePtr<FArianeVertex>();

        if( Vertex->Guid == InGuid )
        {
            return Vertex;
        }
    }

    return nullptr;
}

FArianeSegment*
FArianePath::GetSegmentByGuid( const FGuid& InGuid )
{
    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        if( Segment->Guid == InGuid )
        {
            return Segment;
        }
    }

    return nullptr;
}

FArianeSegment*
FArianePath::AllocSegment( FArianeVertex* Vertex0, FArianeVertex* Vertex1 )
{
    InstancedSegments.Push( FInstancedStruct::Make<FArianeSegment>( this, Vertex0, Vertex1 ) );

    FArianeSegment* NewSegment = InstancedSegments.Last().GetMutablePtr<FArianeSegment>();

    return NewSegment;
}

void
FArianePath::AddVertex( FArianeVertex* Vertex )
{
    Vertices.Add( FArianeVertexID( Vertex ) );

    Vertex->Invalidate();

    Invalidate( FArianePathInvalidationFlags().SetVertexAddedOrRemoved() );
}

void
FArianePath::AddSegment( FArianeSegment* Segment )
{
    Segment->Link();

    Segment->GetVertex(0)->Invalidate(); // will invalidate all connected segments for smoothing
    Segment->GetVertex(1)->Invalidate(); // will invalidate all connected segments for smoothing

    Segments.Add( FArianeSegmentID( Segment ) );

    Segment->Invalidate();

    Invalidate( FArianePathInvalidationFlags().SetSegmentAddedOrRemoved() );
}

void
FArianePath::RemoveSegment( FArianeSegment* Segment, bool bRemoveFromInstancedSegments )
{
    Segments.RemoveAll( [Segment]( FArianeSegmentID& SegmentID ) -> bool
    {
        return ( Segment == SegmentID.GetSegment() ) ? true : false;
    } );

    InvalidatedSegments.Remove( Segment );

    Segment->Unlink();

    Invalidate( FArianePathInvalidationFlags().SetSegmentAddedOrRemoved() );

    if( bRemoveFromInstancedSegments )
    {
        InstancedSegments.RemoveAll( [Segment]( const FInstancedStruct& Struct ) -> bool
        {
            return ( Segment == Struct.GetPtr<FArianeSegment>() ) ? true : false;
        } );
    }
}

TArray<FArianeSegmentID>&
FArianePath::GetSegments()
{
    return Segments;
}

TArray<FArianeSegment*>&
FArianePath::GetInvalidatedSegments()
{
    return InvalidatedSegments;
}

TArray<FArianeVertexID>&
FArianePath::GetVertices()
{
    return Vertices;
}

void
FArianePath::UpdateBounds()
{
    Bounds = FBoxSphereBounds();

    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        Bounds = Bounds + Segment->GetBounds();
    }
}

void
FArianePath::InvalidateVertex( FArianeVertex* Vertex )
{
    InvalidatedVertices.Add( Vertex );
}

void
FArianePath::InvalidateSegment( FArianeSegment* Segment )
{
    InvalidatedSegments.Add( Segment );
}

void
FArianePath::SetLineType( EArianePathLineType InLineType )
{
    LineType = InLineType;

    Invalidate( FArianePathInvalidationFlags().SetSegmentAltered()
                                              .SetVertexAltered() );
}

EArianePathLineType
FArianePath::GetLineType()
{
    return LineType;
}

void
FArianePath::PostEditUndo()
{
    Super::PostEditUndo();

    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        Segment->PostEditUndo();
        Segment->Link();
    }

    Invalidate( FArianePathInvalidationFlags().SetSegmentAltered()
                                              .SetSegmentAddedOrRemoved()
                                              .SetVertexAltered()
                                              .SetVertexAddedOrRemoved() );
}

void
FArianePath::PostLoad()
{
    Super::PostLoad();

    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        Segment->PostLoad();
        Segment->Link();
    }

    Invalidate( FArianePathInvalidationFlags().SetSegmentAltered()
                                              .SetSegmentAddedOrRemoved()
                                              .SetVertexAltered()
                                              .SetVertexAddedOrRemoved() );
}

const FColor&
FArianePath::GetColor()
{
    return Color;
}

void
FArianePath::SetColor( const FColor& InColor )
{
    Color = InColor;

    Invalidate( FArianePathInvalidationFlags().SetColor() );
}

void
FArianePath::ExportProperties( FArianePath* DestPath )
{
    Super::ExportProperties( DestPath );

    DestPath->Color = Color;
    DestPath->LineType = LineType;
}

void
FArianePath::FindChains()
{
    Chains.Empty();

    // Clean
    for( FArianeVertexID& VertexID : Vertices )
    {
        VertexID.GetVertex()->SetChained( false );
    }

    // Mark
    for( FArianeVertexID& VertexID : Vertices )
    {
        if( VertexID.GetVertex()->IsChained() == false )
        {
            Chains.Emplace( this, VertexID.GetVertex() );
        }
    }
}

bool
FArianePath::Update( bool Recurse, bool bClearFlags )
{
    FArianePathInvalidationFlags* PathInvalidationFlags = static_cast<FArianePathInvalidationFlags*>(InvalidationFlags);

    FArianeObject::Update( Recurse , false );

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

    PathInvalidationFlags->Clear();

    return true; // update succeeded
}

void
FArianePath::InvalidateAllSegments()
{
    for( FArianeSegmentID& SegmentID : Segments )
    {
        InvalidatedSegments.Add( SegmentID.GetSegment() );
    }

    Invalidate( FArianePathInvalidationFlags().SetSegmentAltered() );
}

FArianePathGeometry3D&
FArianePath::GetGeometry3D()
{
    return Geometry3D;
}

FArianePathGeometry3D::~FArianePathGeometry3D()
{
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

FArianePathGeometry3D::FArianePathGeometry3D( FArianePath* InPath )
    : Path( InPath )
    , VertexFactory ( nullptr )
{
}

FVector
FArianePathGeometry3D::GetTangentVectorAt( FArianeSegment* Segment
                                         , FVector* OptionalPerpendicularVector
                                         , double T
                                         , bool bNormalize )
{
    FVector SegmentVector =  Segment->GetVertex(1)->GetPosition() - Segment->GetVertex(0)->GetPosition();
    FVector TangentVector = FVector::Zero();

    if( ( T == 0.0f ) || ( T == 1.0f ) )
    {
        FArianeVertex* Vertex = Segment->GetVertex( static_cast<uint32>(T) );
        FVector AverageVector = FVector::Zero();

        if( Vertex->GetSegments().Num() == 2 )
        {
            for( FArianeSegment* ConnectedSegment : Vertex->GetSegments() )
            {
                FVector ConnectedSegmentVector = ConnectedSegment->GetVectorLeavingFromVertex( Vertex, true );

                AverageVector += ConnectedSegmentVector;
            }

            AverageVector /= Vertex->GetSegments().Num();

            if( AverageVector.IsNearlyZero() == false )
            {
                FVector Perpendicular = OptionalPerpendicularVector ? *OptionalPerpendicularVector
                                                                    : AverageVector.Cross( SegmentVector );

                TangentVector = Perpendicular.Cross( AverageVector );
            }
        }
    }

    if( TangentVector.IsNearlyZero() )
    {
        TangentVector = SegmentVector;
    }

    // Let's go in the same direction as the segment
    if( TangentVector.Dot( SegmentVector ) < 0.0f )
    {
        TangentVector = -TangentVector;
    }

    if( bNormalize )
    {
        TangentVector.Normalize();
    }

    return TangentVector;
}

/*
*
// Scheme when valence equals 2
// Vertex        = V
// CloserVertex  = CV
// FurtherVertex = FV
//
// FV   V    FV
// °    °    °
//  \  / \  /
//   \/   \/
//   °     °
//   CV   CV

FVector
FArianePathGeometry3D::GetTangentVectorAt( FArianeSegment* Segment
                                         , const FVector& PerpendicularVector
                                         , double T
                                         , bool bNormalize )
{
    FVector SegmentVector =  Segment->GetVertex(1)->GetPosition() - Segment->GetVertex(0)->GetPosition();
    FVector TangentVector = FVector::Zero();

    if( ( T == 0.0f ) || ( T == 1.0f ) )
    {
        FArianeVertex* Vertex = Segment->GetVertex( static_cast<uint32>(T) );
        FVector AverageVector = FVector::Zero();

        if( Vertex->GetSegments().Num() == 2 )
        {
            for( FArianeSegment* ConnectedSegment : Vertex->GetSegments() )
            {
                FVector ConnectedSegmentVector = ConnectedSegment->GetVectorLeavingFromVertex( Vertex, true );

                AverageVector += ConnectedSegmentVector;
            }

            AverageVector /= Vertex->GetSegments().Num();

            TangentVector = AverageVector.Cross( PerpendicularVector );
        }
        else
        {
            TangentVector = SegmentVector;
        }
    }

    if( TangentVector.IsZero() )
    {
        TangentVector = SegmentVector;
    }

    // Let's go in the same direction as the segment
    if( TangentVector.Dot( SegmentVector ) < 0.0f )
    {
        TangentVector = -TangentVector;
    }

    if( bNormalize )
    {
        TangentVector.Normalize();
    }

    return TangentVector;
}
*/

/*
FVector
FArianePathGeometry3D::GetPerpendicularVector( FArianeVertex* Vertex
                                             , FVector& InOutPreviousPerpendicularVector
                                             , bool bNormalize )
{
    FVector PerpendicularVector = InOutPreviousPerpendicularVector; // default if none is found

    if( Vertex->GetSegments().Num() <= 2 )
    {
        FArianeVertex *NeighbourVertices[5] = { nullptr
                                              , nullptr
                                              , Vertex
                                              , nullptr
                                              , nullptr };
        FArianeVertex* CloserVertices[2] = { nullptr, nullptr };
        FArianeVertex* FurtherVertices[2] = { nullptr, nullptr };
        static uint32 TriangleIndices[3][3] = { { 0, 1, 2 }, {  2, 1, 3 }, { 3, 4, 2 } };
        FVector SumVector = FVector( 0.0f, 0.0f, 0.0f );
        uint32 CloserVertexCount = 0;
        uint32 CrossCount = 0;

        for( FArianeSegment* Segment : Vertex->GetSegments() )
        {
            FArianeVertex *CloserVertex = Segment->GetOtherVertex( Vertex );

            CloserVertices[CloserVertexCount] = CloserVertex;

            if( CloserVertex->GetSegments().Num() <= 2 )
            {
                for( FArianeSegment* OtherSegment : CloserVertex->GetSegments() )
                {
                    if( OtherSegment != Segment )
                    {
                        FArianeVertex *FurtherVertex = OtherSegment->GetOtherVertex( CloserVertex );

                        FurtherVertices[CloserVertexCount] = FurtherVertex;

                        break;
                    }
                }
            }

            CloserVertexCount++;
        }

        if( CloserVertices[0] )
        {
            NeighbourVertices[0] = FurtherVertices[0];
            NeighbourVertices[1] = CloserVertices[0];
        }

        if( CloserVertices[1] )
        {
            NeighbourVertices[3] = CloserVertices[1];
            NeighbourVertices[4] = FurtherVertices[1];
        }

        for( uint32 i = 0; i < 3; i++ )
        {
            uint32 TriVertexIndex0 = TriangleIndices[i][0];
            uint32 TriVertexIndex1 = TriangleIndices[i][1];
            uint32 TriVertexIndex2 = TriangleIndices[i][2];
            FArianeVertex* TriVertex0 = NeighbourVertices[TriVertexIndex0];
            FArianeVertex* TriVertex1 = NeighbourVertices[TriVertexIndex1];
            FArianeVertex* TriVertex2 = NeighbourVertices[TriVertexIndex2];

            if(  TriVertex0 && TriVertex1 && TriVertex2 )
            {
                FVector V0V1 = TriVertex1->GetPosition() - TriVertex0->GetPosition();
                FVector V0V2 = TriVertex2->GetPosition() - TriVertex0->GetPosition();
                FVector CrossVector = V0V1.Cross( V0V2 );

                if( CrossVector.Normalize() )
                {
                    if( ( InOutPreviousPerpendicularVector.IsZero() == false )
                     && ( InOutPreviousPerpendicularVector.Dot( CrossVector ) < 0.0f ) )
                    {
                        CrossVector = -CrossVector;
                    }

                    SumVector += CrossVector;

                    CrossCount++;

                    if( CrossVector.IsZero() == false )
                    {
                        InOutPreviousPerpendicularVector = CrossVector;
                    }
                }
            }
        }

        if( CrossCount )
        {
            PerpendicularVector = SumVector / CrossCount;

            if( bNormalize )
            {
                PerpendicularVector.Normalize();
            }
        }
    }

    return PerpendicularVector;
}
*/

void
FArianePathGeometry3D::BuildSegmentAsFlat( FArianeSegment* Segment
                                         , FVector& InOutPreviousPerpendicularVector )
{
    FArianeVertex* SegmentVertices[2] = { Segment->GetVertex(0)
                                        , Segment->GetVertex(1) };
    double Radius0 = SegmentVertices[0]->GetRadius();
    double Radius1 = SegmentVertices[1]->GetRadius();
    double DeltaRadius = Radius1 - Radius0;
    FVector Normal0 = SegmentVertices[0]->GetNormal();
    FVector Normal1 = SegmentVertices[1]->GetNormal();
    FVector DeltaNormal = Normal1 - Normal0;
    FVector SegmentVector = SegmentVertices[1]->GetPosition() - SegmentVertices[0]->GetPosition();

    Segment->AllocateCache( ( Segment->GetFractionCount() + 1 ) * 2
                          , ( Segment->GetFractionCount() * 2 ) );

    TArray<FDynamicMeshVertex>& ModelVertexCache = const_cast<TArray<FDynamicMeshVertex>&>(Segment->GetModelVertexCache());
    TArray<uint32>& IndexCache = const_cast<TArray<uint32>&>(Segment->GetIndexCache());
    const TArray<FArianeSegment::Fraction>& Fractions = Segment->GetFractions();
    const TArray<FArianeSegment::FractionStep>& FractionSteps = Segment->GetFractionSteps();

    for( int32 FractionStepIndex = 0; FractionStepIndex < FractionSteps.Num(); FractionStepIndex++ )
    {
        const FArianeSegment::FractionStep& Step = FractionSteps[FractionStepIndex];
        uint32 ModelVertexOffset = FractionStepIndex * 2;
        double PointRadius = Radius0 + ( DeltaRadius * Step.T );
        FVector PerpendicularVector = Normal0 + ( DeltaNormal * Step.T );
        FVector TangentVector = GetTangentVectorAt( Segment
                                                  , nullptr
                                                  , Step.T
                                                  , false );

        if( PerpendicularVector.Normalize() )
        {
            FVector UpVector = PerpendicularVector.Cross( TangentVector );
            FDynamicMeshVertex* ModelVertex0 = &ModelVertexCache[ModelVertexOffset+0];
            FDynamicMeshVertex* ModelVertex1 = &ModelVertexCache[ModelVertexOffset+1];

            if( UpVector.Normalize() )
            {
                FVector NewPosition0 = Step.Point->GetPosition() + ( UpVector * PointRadius );
                FVector NewPosition1 = Step.Point->GetPosition() - ( UpVector * PointRadius );

                ModelVertex0->Position.X = NewPosition0.X;
                ModelVertex0->Position.Y = NewPosition0.Y;
                ModelVertex0->Position.Z = NewPosition0.Z;

                ModelVertex0->Color = Path->GetColor();

                ModelVertex1->Position.X = NewPosition1.X;
                ModelVertex1->Position.Y = NewPosition1.Y;
                ModelVertex1->Position.Z = NewPosition1.Z;

                ModelVertex1->Color = Path->GetColor();
            }
        }
    }

    for( int32 FractionIndex = 0; FractionIndex < Fractions.Num(); FractionIndex++ )
    {
        const FArianeSegment::Fraction& SegmentFraction = Fractions[FractionIndex];
        uint32 ModelVertexOffset0 =   FractionIndex * 2;
        uint32 ModelVertexOffset1 = ( FractionIndex + 1 ) * 2;
        FVector3f SampleVec0 = ModelVertexCache[ModelVertexOffset0 + 1].Position
                             - ModelVertexCache[ModelVertexOffset0 + 0].Position;
        FVector3f SampleVec1 = ModelVertexCache[ModelVertexOffset1 + 1].Position
                             - ModelVertexCache[ModelVertexOffset1 + 0].Position;

        bool Twisted = ( SampleVec0.Dot( SampleVec1 ) < 0.0f ) ? true : false;

        uint32 Triangle0Index = ( FractionIndex * 2 * 3 ); // 2 triangles per quad, 3 indexes per tirangle
        uint32 Triangle1Index = Triangle0Index + 3;

        if( Twisted == false )
        {
            // first triangle
            IndexCache[Triangle0Index+0] = ModelVertexOffset1 + 1;
            IndexCache[Triangle0Index+1] = ModelVertexOffset0 + 1;
            IndexCache[Triangle0Index+2] = ModelVertexOffset0;

            // second triangle
            IndexCache[Triangle1Index+0] = ModelVertexOffset0;
            IndexCache[Triangle1Index+1] = ModelVertexOffset1;
            IndexCache[Triangle1Index+2] = ModelVertexOffset1 + 1;
        }
        else
        {
            // first triangle
            IndexCache[Triangle0Index+0] = ModelVertexOffset1;
            IndexCache[Triangle0Index+1] = ModelVertexOffset0 + 1;
            IndexCache[Triangle0Index+2] = ModelVertexOffset0;

            // second triangle
            IndexCache[Triangle1Index+0] = ModelVertexOffset0;
            IndexCache[Triangle1Index+1] = ModelVertexOffset1 + 1;
            IndexCache[Triangle1Index+2] = ModelVertexOffset1;
        }
    }
}

void
FArianePathGeometry3D::BuildSegmentAsTube( FArianeSegment* Segment
                                         , FVector& InOutPreviousPerpendicularVector )
{
    FArianeVertex* SegmentVertices[2] = { Segment->GetVertex(0)
                                        , Segment->GetVertex(1) };
    double Radius0 = SegmentVertices[0]->GetRadius();
    double Radius1 = SegmentVertices[1]->GetRadius();
    double DeltaRadius = Radius1 - Radius0;
    uint32 Divisions = 12;
    //FVector PerpendicularVector0 = GetPerpendicularVector( SegmentVertices[0], InOutPreviousPerpendicularVector, false );
    //FVector PerpendicularVector1 = GetPerpendicularVector( SegmentVertices[1], InOutPreviousPerpendicularVector, false );
    //FVector DiffPerpendicularVector = PerpendicularVector1 - PerpendicularVector0;
    //FVector UpVector0 = GetUpVector( SegmentVertices[0], false );
    //FVector UpVector1 = GetUpVector( SegmentVertices[1], false );
    //FVector DiffUpVector = UpVector1 - UpVector0;

    Segment->AllocateCache( ( Segment->GetFractionCount() + 1 ) * Divisions
                          , ( Segment->GetFractionCount() * 2 ) * Divisions );

    TArray<FDynamicMeshVertex>& ModelVertexCache = const_cast<TArray<FDynamicMeshVertex>&>(Segment->GetModelVertexCache());
    TArray<uint32>& IndexCache = const_cast<TArray<uint32>&>(Segment->GetIndexCache());
    const TArray<FArianeSegment::Fraction>& Fractions = Segment->GetFractions();
    const TArray<FArianeSegment::FractionStep>& FractionSteps = Segment->GetFractionSteps();

    for( int32 FractionStepIndex = 0; FractionStepIndex < FractionSteps.Num(); FractionStepIndex++ )
    {
        const FArianeSegment::FractionStep& Step = FractionSteps[FractionStepIndex];
        uint32 ModelVertexOffset = FractionStepIndex * Divisions;
        //FVector PerpendicularVector = PerpendicularVector0 + ( DiffPerpendicularVector * PointT );
        //FVector UpVector = UpVector0 + ( DiffUpVector * PointT );
        //FVector TangentVector = PerpendicularVector.Cross( UpVector );
        //FVector PerpendicularVector = SegmentVector.Cross( Painting3DComponent->GetUpVector() );
        double PointRadius = Radius0 + ( DeltaRadius * Step.T );
        FVector TangentVector = GetTangentVectorAt( Segment
                                                  , nullptr
                                                  , Step.T
                                                  , false );

        FVector PerpendicularVector = TangentVector.Cross( Path->GetDrawingLayer()->GetLayerStack()->GetPainting3DComponent()->GetUpVector() );

        PerpendicularVector.Normalize();

        if( PerpendicularVector.IsZero() == false )
        {
            float AngleInDegrees = 0.0f;
            float StepAngle = ( float ) 360 / Divisions;

            for( uint32 j = 0; j < Divisions; j++ )
            {
                FRotator Rotator = UKismetMathLibrary::RotatorFromAxisAndAngle( TangentVector, AngleInDegrees );
                FDynamicMeshVertex* ModelVertex = &ModelVertexCache[ModelVertexOffset+j];
                FVector RotatedPosition = Step.Point->GetPosition() + ( Rotator.RotateVector( PerpendicularVector ) * PointRadius );

                ModelVertex->Position.X = RotatedPosition.X;
                ModelVertex->Position.Y = RotatedPosition.Y;
                ModelVertex->Position.Z = RotatedPosition.Z;

                ModelVertex->Color = Path->GetColor();

                AngleInDegrees += StepAngle;
            }
        }
    }

    for( int32 FractionIndex = 0; FractionIndex < Fractions.Num(); FractionIndex++ )
    {
        const FArianeSegment::Fraction& SegmentFraction = Fractions[FractionIndex];
        uint32 ModelVertexOffset0 =   FractionIndex       * Divisions;
        uint32 ModelVertexOffset1 = ( FractionIndex + 1 ) * Divisions;
        FVector3f SampleVec0 = ModelVertexCache[ModelVertexOffset0 + 1].Position
                             - ModelVertexCache[ModelVertexOffset0 + 0].Position;
        FVector3f SampleVec1 = ModelVertexCache[ModelVertexOffset1 + 1].Position
                             - ModelVertexCache[ModelVertexOffset1 + 0].Position;

        bool Twisted = ( SampleVec0.Dot( SampleVec1 ) < 0.0f ) ? true : false;

        for( uint32 i = 0, j = ( Divisions * 2 ) - 1; i < Divisions; i++, j-- )
        {
            uint32 Triangle0Index = ( FractionIndex * Divisions * 2 * 3 ) + ( i * 2 * 3 ); // 2 triangles per quad, 3 indexes per tirangle
            uint32 Triangle1Index = Triangle0Index + 3;

            if( Twisted == false )
            {
                // first triangle
                IndexCache[Triangle0Index+0] = ModelVertexOffset1 + ( ( i + 1 ) % Divisions );
                IndexCache[Triangle0Index+1] = ModelVertexOffset0 + ( ( i + 1 ) % Divisions );
                IndexCache[Triangle0Index+2] = ModelVertexOffset0 + ( ( i     ) % Divisions );

                // second triangle
                IndexCache[Triangle1Index+0] = ModelVertexOffset0 + ( ( i     ) % Divisions );
                IndexCache[Triangle1Index+1] = ModelVertexOffset1 + ( ( i     ) % Divisions );
                IndexCache[Triangle1Index+2] = ModelVertexOffset1 + ( ( i + 1 ) % Divisions );
            }
            else
            {
                // first triangle
                IndexCache[Triangle0Index+0] = ModelVertexOffset1 + ( ( j - 1 ) % Divisions );
                IndexCache[Triangle0Index+1] = ModelVertexOffset0 + ( ( i + 1 ) % Divisions );
                IndexCache[Triangle0Index+2] = ModelVertexOffset0 + ( ( i     ) % Divisions );

                // second triangle
                IndexCache[Triangle1Index+0] = ModelVertexOffset0 + ( ( i     ) % Divisions );
                IndexCache[Triangle1Index+1] = ModelVertexOffset1 + ( ( j     ) % Divisions );
                IndexCache[Triangle1Index+2] = ModelVertexOffset1 + ( ( j - 1 ) % Divisions );
            }
        }
    }
}

const FRawStaticIndexBuffer&
FArianePathGeometry3D::GetIndexBuffer() const
{
    return IndexBuffer;
}

FArianePath*
FArianePathGeometry3D::GetPath()
{
    return Path;
}

FLocalVertexFactory*
FArianePathGeometry3D::GetVertexFactory()
{
    return VertexFactory;
}

const uint32
FArianePathGeometry3D::GetVertexCount() const
{
    return VertexCount;
}

void
FArianePathGeometry3D::InitVertexFactory( TArray<FDynamicMeshVertex>& Vertices
                                        , TArray<uint32>& Indices )
{
    if( VertexFactory == nullptr )
    {
        VertexFactory = new FLocalVertexFactory( Path->GetDrawingLayer()->GetLayerStack()->GetPainting3DComponent()->GetWorld()->GetFeatureLevel(), "Path Vertex Factory" );
    }

    ENQUEUE_RENDER_COMMAND(StaticMeshVertexBuffersLegacyInit)(
        [ this
        ,  VerticesAsync = MoveTemp(Vertices) ] ( FRHICommandListImmediate& RHICmdList )
        {
            FLocalVertexFactory::FDataType Data;

            VertexCount = VerticesAsync.Num();

            if( PositionBuffer.IsInitialized() == false ) PositionBuffer.InitResource( RHICmdList );
            if( StaticMeshVB.IsInitialized() == false ) StaticMeshVB.InitResource( RHICmdList );
            if( ColorBuffer.IsInitialized() == false ) ColorBuffer.InitResource( RHICmdList );

            PositionBuffer.Init( VertexCount );
            StaticMeshVB.Init( VertexCount, 1 );
            ColorBuffer.Init( VertexCount );

            for ( uint32 i = 0; i < VertexCount; ++i )
            {
                PositionBuffer.VertexPosition( i ) = VerticesAsync[i].Position;
                ColorBuffer.VertexColor( i ) = VerticesAsync[i].Color;
                StaticMeshVB.SetVertexUV( i, 0, VerticesAsync[i].TextureCoordinate[0] );
                StaticMeshVB.SetVertexTangents( i, FVector3f( 1, 0, 0 ), FVector3f( 0, 1, 0 ), FVector3f( 0, 0, 1 ) );
            }

            // Copy RAM to VRAM
            PositionBuffer.UpdateRHI( RHICmdList );
            StaticMeshVB.UpdateRHI( RHICmdList );
            ColorBuffer.UpdateRHI( RHICmdList );

            PositionBuffer.BindPositionVertexBuffer( VertexFactory, Data );
            StaticMeshVB.BindTangentVertexBuffer( VertexFactory, Data );
            StaticMeshVB.BindPackedTexCoordVertexBuffer( VertexFactory, Data );
            ColorBuffer.BindColorVertexBuffer( VertexFactory, Data );

            VertexFactory->SetData( Data );

            // Init / update the factory after SetData
            if (!VertexFactory->IsInitialized()) {
                VertexFactory->InitResource(RHICmdList);
            } else {
                VertexFactory->UpdateRHI(RHICmdList);
            }
        } );

    ENQUEUE_RENDER_COMMAND(IndexBufferInit)(
        [ this
        , IndicesAsync = MoveTemp(Indices) ] ( FRHICommandListImmediate& RHICmdList )
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
        } );
}

void
FArianePathGeometry3D::Build()
{
    uint32 TotalModelVertexCount = 0;
    uint32 TotalIndexCount = 0;
    TArray<FDynamicMeshVertex> MeshVertices;
    TArray<uint32> MeshIndices;
    FVector PreviousPerpendicularVector = Path->GetDrawingLayer()->GetLayerStack()->GetPainting3DComponent()->GetUpVector();//FVector::Zero();

    // TODO : update  invalidated segments only
    for( FArianeSegment* Segment : Path->GetInvalidatedSegments() )
    {
        FArianeVertex* segmentVertices[2] = { Segment->GetVertex(0)
                                            , Segment->GetVertex(1) };
        uint32 SegmentIndexCount = 0;
        FDynamicMeshVertex storedVertex;

        Segment->Update();

        switch( Path->GetLineType() )
        {
            case EArianePathLineType::Flat :
                BuildSegmentAsFlat( Segment, PreviousPerpendicularVector );
            break;

            case EArianePathLineType::Tube :
                BuildSegmentAsTube( Segment, PreviousPerpendicularVector );
            break;

            default:
            break;
        }
    }

    Path->GetInvalidatedSegments().Empty();

    for( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        TotalModelVertexCount += Segment->GetModelVertexCache().Num();
        TotalIndexCount += Segment->GetIndexCache().Num();
    }

    MeshVertices.SetNum( TotalModelVertexCount );
    MeshIndices.SetNum( TotalIndexCount );

    TotalModelVertexCount = 0;
    TotalIndexCount = 0;

    for( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        if( Segment->GetIndexCache().Num() )
        {
            const TArray<FDynamicMeshVertex>& SegmentModelVertexCache = Segment->GetModelVertexCache();
            const TArray<uint32>& SegmentIndices = Segment->GetIndexCache();

            memcpy( &MeshVertices[TotalModelVertexCount]
                  , &SegmentModelVertexCache[0]
                  ,  SegmentModelVertexCache.Num() * sizeof( FDynamicMeshVertex ) );

            memcpy( &MeshIndices[TotalIndexCount]
                  , &SegmentIndices[0]
                  ,  SegmentIndices.Num() * sizeof( uint32 ) );

            // renumber indices
            for( int32 i = TotalIndexCount, j = 0; j < SegmentIndices.Num(); i++, j++ )
            {
                MeshIndices[i] += TotalModelVertexCount;
            }

            TotalModelVertexCount += SegmentModelVertexCache.Num();
            TotalIndexCount += SegmentIndices.Num();
        }
    }

    //if( ModelVertices.Num() )
    {
        //VertexBuffers.InitModelBuffers( ModelVertices );
        //IndexBuffer.SetIndices( Indices, EIndexBufferStride::Type::Force32Bit );

        if( MeshVertices.Num() )
        {
            InitVertexFactory( MeshVertices, MeshIndices );
        }
    }
}
