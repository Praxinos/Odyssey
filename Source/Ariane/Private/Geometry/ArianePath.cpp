// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianeSegmentCubic.h"
#include "ArianeImage.h"
#include "ArianeKeyedPath.h"
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

    Length = 0;
    LeadingVertex = CurrentVertex;

    CurrentVertex->SetChained( true );

    while( CurrentSegment )
    {
        FArianeVertex* nextVertex = CurrentSegment->GetOtherVertex( CurrentVertex );

        Segments.Push( CurrentSegment );

        Length += CurrentSegment->GetLength();

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

double
FArianePath::Chain::GetLength()
{
    return Length;
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
    FRHIBufferCreateDesc CreateBufferDesc = FRHIBufferCreateDesc( TEXT( "Dynamic Vertex Buffer" )
                                                                , VertexCount * sizeof( FDynamicMeshVertex )
                                                                , 0
                                                                , EBufferUsageFlags::Dynamic
                                                                | EBufferUsageFlags::VertexBuffer
                                                                | EBufferUsageFlags::ShaderResource );

    VertexCount = InVertexCount;

    VertexBufferRHI = RHICmdList.CreateBuffer( CreateBufferDesc );

/* FRHIResourceCreateInfo will be deprecated soon
    FRHIResourceCreateInfo ResourceInfo( TEXT( "Dynamic Vertex Buffer" ) );



    VertexBufferRHI = RHICmdList.CreateVertexBuffer( VertexCount * sizeof( FDynamicMeshVertex )
                                                   , BUF_Dynamic | BUF_VertexBuffer | BUF_ShaderResource
                                                   , ERHIAccess::VertexOrIndexBuffer
                                                   , ResourceInfo );
*/
}

void FArianePathVertexBuffer::InitRHI(FRHICommandListBase& RHICmdList)
{
    Resize( VertexCount, RHICmdList );
}

//--------------------- Path

FArianePath::~FArianePath()
{
    // free mallocated segments.
    for( FArianeSegmentID& SegmentID : Segments )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        if( Segment->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            delete Segment;
        }
    }

    Segments.Empty();

    // free mallocated vertices
    for( FArianeVertexID& VertexID : Vertices )
    {
        FArianeVertex* Vertex = VertexID.GetVertex();

        if( Vertex->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            delete Vertex;
        }
    }

    Vertices.Empty();
}

FArianePath::FArianePath()
    : FArianePath( nullptr
                 , FName( "Ariane Path" )
                 , EArianeAllocationModel::InstancedStruct
                 , new FArianePathInvalidationFlags() )
{
}

FArianePath::FArianePath( UArianeImage* InImage
                        , const FName& InName
                        , EArianeAllocationModel InAllocationModel
                        , FArianePathInvalidationFlags* InInvalidationFlags )
    : FArianeObject ( InImage
                    , InName
                    , InAllocationModel
                    , InInvalidationFlags ? InInvalidationFlags
                                          : new FArianePathInvalidationFlags() )
    , LineType ( EArianePathLineType::Tube )
    , Color ( FColor::Black.WithAlpha(255) )
    , MaterialInterface ( nullptr )
    , Geometry3D ( this )
{
}

bool
FArianePath::HasBaseClass( uint32 BaseClass )
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass(BaseClass);
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

void
FArianePath::Added()
{
    if( MaterialInterface && Image )
    {
        Image->IncrementMaterial( MaterialInterface );
    }
}

void
FArianePath::Removed()
{
    if( MaterialInterface && Image )
    {
        Image->DecrementMaterial( MaterialInterface );
    }
}

UMaterialInterface*
FArianePath::GetMaterial()
{
    return MaterialInterface;
}

void
FArianePath::SetMaterial( UMaterialInterface* InMaterialInterface )
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

const TArray<FArianePath::Chain>&
FArianePath::GetChains()
{
    return Chains;
}

FArianeVertex*
FArianePath::AllocVertex( const FVector& InPosition
                        , const FVector& InNormal
                        , double InRadius
                        , EArianeAllocationModel InAllocationModel )
{
    FArianeVertex* NewVertex = nullptr;

    if( InAllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedVertices.Push( FInstancedStruct::Make<FArianeVertex>( this
                                                                     , InPosition
                                                                     , InNormal
                                                                     , InRadius
                                                                     , InAllocationModel ) );

        NewVertex = InstancedVertices.Last().GetMutablePtr<FArianeVertex>();
    }

    if( InAllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewVertex = new FArianeVertex( this
                                     , InPosition
                                     , InNormal
                                     , InRadius
                                     , InAllocationModel );
    }

    return NewVertex;
}

void
FArianePath::RemoveVertex( FArianeVertex* Vertex, bool bUnallocate )
{
    Vertices.RemoveAll( [Vertex]( FArianeVertexID& VertexID ) -> bool
    {
        return ( Vertex == VertexID.GetVertex() ) ? true : false;
    } );

    Invalidate( FArianePathInvalidationFlags().SetVertexAddedOrRemoved() );

    if( bUnallocate )
    {
        if( Vertex->GetAllocationModel() == EArianeAllocationModel::InstancedStruct )
        {
            InstancedVertices.RemoveAll( [Vertex]( FInstancedStruct& Struct ) -> bool
            {
                return ( Vertex == Struct.GetPtr<FArianeVertex>() ) ? true : false;
            } );
        }

        if( Vertex->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            delete Vertex;
        }
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
FArianePath::AllocSegment( FArianeVertex* Vertex0
                         , FArianeVertex* Vertex1
                         , EArianeAllocationModel InAllocationModel )
{
    FArianeSegment* NewSegment = nullptr;

    if( InAllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedSegments.Push( FInstancedStruct::Make<FArianeSegment>( this
                                                                      , Vertex0
                                                                      , Vertex1
                                                                      , InAllocationModel ) );

        NewSegment = InstancedSegments.Last().GetMutablePtr<FArianeSegment>();
    }

    if( InAllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewSegment = new FArianeSegment( this, Vertex0, Vertex1, InAllocationModel );
    }

    return NewSegment;
}


FArianeSegmentCubic*
FArianePath::AllocCubicSegment( FArianeVertex* Vertex0
                              , const FVector& Handle0
                              , const FVector& Handle1
                              , FArianeVertex* Vertex1
                              , EArianeAllocationModel InAllocationModel )
{
    return AllocCubicSegment( Vertex0
                            , Handle0.X
                            , Handle0.Y
                            , Handle0.Z
                            , Handle1.X
                            , Handle1.Y
                            , Handle1.Z
                            , Vertex1
                            , InAllocationModel );
}

FArianeSegmentCubic*
FArianePath::AllocCubicSegment( FArianeVertex* Vertex0
                              , double Handle0X
                              , double Handle0Y
                              , double Handle0Z
                              , double Handle1X
                              , double Handle1Y
                              , double Handle1Z
                              , FArianeVertex* Vertex1
                              , EArianeAllocationModel InAllocationModel )
{
    FArianeSegmentCubic* NewCubicSegment = nullptr;

    if( InAllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedSegments.Push( FInstancedStruct::Make<FArianeSegmentCubic>( this
                                                                           , Vertex0
                                                                           , Handle0X
                                                                           , Handle0Y
                                                                           , Handle0Z
                                                                           , Handle1X
                                                                           , Handle1Y
                                                                           , Handle1Z
                                                                           , Vertex1
                                                                           , InAllocationModel ) );
        NewCubicSegment = InstancedSegments.Last().GetMutablePtr<FArianeSegmentCubic>();
    }

    if( InAllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewCubicSegment = new FArianeSegmentCubic( this
                                                 , Vertex0
                                                 , Handle0X
                                                 , Handle0Y
                                                 , Handle0Z
                                                 , Handle1X
                                                 , Handle1Y
                                                 , Handle1Z
                                                 , Vertex1
                                                 , InAllocationModel );
    }

    return NewCubicSegment;
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
FArianePath::RemoveSegment( FArianeSegment* Segment, bool bUnallocate )
{
    Segments.RemoveAll( [Segment]( FArianeSegmentID& SegmentID ) -> bool
    {
        return ( Segment == SegmentID.GetSegment() ) ? true : false;
    } );

    InvalidatedSegments.Remove( Segment );

    Segment->Unlink();

    Invalidate( FArianePathInvalidationFlags().SetSegmentAddedOrRemoved() );

    if( bUnallocate )
    {
        if( Segment->GetAllocationModel() == EArianeAllocationModel::InstancedStruct )
        {
            InstancedSegments.RemoveAll( [Segment]( const FInstancedStruct& Struct ) -> bool
            {
                return ( Segment == Struct.GetPtr<FArianeSegment>() ) ? true : false;
            } );
        }

        if( Segment->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            delete Segment;
        }
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
FArianePath::AlterRadius( double RatioRadius )
{
    for( FArianeVertexID& VertexID : Vertices )
    {
        FArianeVertex* Vertex = VertexID.GetVertex();

        Vertex->SetRadius( Vertex->GetRadius() * RatioRadius );
    }
}

void
FArianePath::CopySettings( FArianeObject* DestinationObject, const FCopyArgs& CopyArgs, bool bInvalidate )
{
    FArianePath* DestinationPath = static_cast<FArianePath*>(DestinationObject);

    Super::CopySettings( DestinationObject, CopyArgs, false );

    DestinationPath->LineType = LineType;
    DestinationPath->Color = Color;
    DestinationPath->MaterialInterface = MaterialInterface;

    if( bInvalidate )
    {
        DestinationPath->Invalidate( FArianePathInvalidationFlags().SetAll() );
    }
}

FArianeObject*
FArianePath::CopyShape( const FCopyArgs& CopyArgs )
{
    TArray<FArianeVertex*> LookupTable;
    FArianePath* PathCopy = nullptr;
    uint32 VertexID = 0;

    PathCopy = CopyArgs.Image->AllocPath( nullptr, Name, CopyArgs.AllocationModel );

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

    return PathCopy;
}

void
FArianePath::UpdateBoundingBox( EUpdateFlags UpdateFlags )
{
    FArianePathInvalidationFlags* PathInvalidationFlags = static_cast<FArianePathInvalidationFlags*>(InvalidationFlags);

    if( PathInvalidationFlags->VertexAltered
     || PathInvalidationFlags->VertexAddedOrRemoved
     || PathInvalidationFlags->SegmentAltered
     || PathInvalidationFlags->SegmentAddedOrRemoved )
    {
        // FBox(ForceInit) creates an invalid box
        FBox CombinedBox(ForceInit);

        for (FArianeSegmentID& SegmentID : Segments)
        {
            FArianeSegment* Segment = SegmentID.GetSegment();

            CombinedBox += Segment->GetBoundingBox();
        }

        BoundingBox = CombinedBox.IsValid ? FBox(CombinedBox)
                                          : FBox(ForceInit);
    }
}

void
FArianePath::InvalidateVertex( FArianeVertex* Vertex )
{
    InvalidatedVertices.Add( Vertex );

    Invalidate( FArianePathInvalidationFlags().SetVertexAltered() );
}

void
FArianePath::InvalidateSegment( FArianeSegment* Segment )
{
    InvalidatedSegments.Add( Segment );

    Invalidate( FArianePathInvalidationFlags().SetSegmentAltered() );
}

void
FArianePath::SetLineType( EArianePathLineType InLineType )
{
    LineType = InLineType;

    InvalidateAllSegments();
    InvalidateAllVertices();
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

    for( FInstancedStruct& InstancedVertex : InstancedVertices )
    {
        FArianeVertex* Vertex = InstancedVertex.GetMutablePtr<FArianeVertex>();

        Vertex->PostEditUndo();

        AddVertex( Vertex );
    }

    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        Segment->PostEditUndo();

        AddSegment( Segment );
    }

    if( MaterialInterface == nullptr )
        MaterialInterface = GEngine->VertexColorMaterial;

    //if( MaterialInterface )
        Image->IncrementMaterial( MaterialInterface );

    Invalidate( FArianePathInvalidationFlags().SetSegmentAltered()
                                              .SetSegmentAddedOrRemoved()
                                              .SetVertexAltered()
                                              .SetVertexAddedOrRemoved() );
}

void
FArianePath::PostLoad()
{
    Super::PostLoad();

    for( FInstancedStruct& InstancedVertex : InstancedVertices )
    {
        FArianeVertex* Vertex = InstancedVertex.GetMutablePtr<FArianeVertex>();

        Vertex->PostEditUndo();

        AddVertex( Vertex );
    }

    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        Segment->PostLoad();

        AddSegment( Segment );
    }

    if( MaterialInterface == nullptr )
        MaterialInterface = GEngine->VertexColorMaterial;

    //if( MaterialInterface )
        Image->IncrementMaterial( MaterialInterface );

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

    // we need to reconstruct Model Vertices
    InvalidateAllSegments();

    Invalidate( FArianePathInvalidationFlags().SetColor() );
}

void
FArianePath::ExportProperties( FArianeObject* DestObject )
{
    Super::ExportProperties( DestObject );

    if( DestObject->GetClass() == FArianePath::StaticClass() )
    {
        FArianePath* DestPath = static_cast<FArianePath*>(DestObject);

        DestPath->Color = Color;
        DestPath->LineType = LineType;
    }
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

    // Mark Vertices with valence 1 in priority
    for( FArianeVertexID& VertexID : Vertices )
    {
        FArianeVertex* Vertex = VertexID.GetVertex();

        if( ( Vertex->IsChained() == false ) && ( Vertex->GetSegments().Num() == 1 ) )
        {
            Chains.Emplace( this, VertexID.GetVertex() );
        }
    }

    // if there are still some unmarked vertices, this normally means there are in a loop
    for( FArianeVertexID& VertexID : Vertices )
    {
        FArianeVertex* Vertex = VertexID.GetVertex();

        if( ( Vertex->IsChained() == false ) && ( Vertex->GetSegments().Num() == 2 ) )
        {
            Chains.Emplace( this, VertexID.GetVertex() );
        }
    }
}

void
FArianePath::UpdateShape( EUpdateFlags UpdateFlags )
{
    FArianePathInvalidationFlags* PathInvalidationFlags = static_cast<FArianePathInvalidationFlags*>(InvalidationFlags);

    //FArianeObject::Update( Recurse , false );

    if( PathInvalidationFlags->VertexAddedOrRemoved
     || PathInvalidationFlags->SegmentAddedOrRemoved )
    {
        FindChains();
    }

    if( PathInvalidationFlags->VertexAltered
     || PathInvalidationFlags->VertexAddedOrRemoved
     || PathInvalidationFlags->SegmentAltered
     || PathInvalidationFlags->SegmentAddedOrRemoved
     || PathInvalidationFlags->Color )
    {
        Geometry3D.Build();
    }

    //PathInvalidationFlags->Clear();

    //return true; // update succeeded
}

void
FArianePath::InvalidateAllSegments()
{
    for( FArianeSegmentID& SegmentID : Segments )
    {
        SegmentID.GetSegment()->Invalidate();
    }
}

void
FArianePath::InvalidateAllVertices()
{
    for( FArianeVertexID& VertexID : Vertices )
    {
        InvalidatedVertices.Add( VertexID.GetVertex() );
    }

    Invalidate( FArianePathInvalidationFlags().SetVertexAltered() );
}

void
FArianePath::Animate( const FArianeKeyedObject* KeyedObject, const FArianeKeyedObject* NextKeyedObject, float T )
{
    const FArianeKeyedPath* KeyedPath = static_cast<const FArianeKeyedPath*>(KeyedObject);
    const FArianeKeyedPath* NextKeyedPath = static_cast<const FArianeKeyedPath*>(NextKeyedObject);

    Super::Animate( KeyedObject, NextKeyedObject, T );

    if( NextKeyedPath && ( T > 0.0f ) )
    {
        FColor KeyedColor = KeyedPath->GetKeyedColor();
        FColor NextKeyedColor = NextKeyedPath->GetKeyedColor();
        FColor AnimColor = FColor( KeyedColor.R + ( NextKeyedColor.R - KeyedColor.R ) * T
                                 , KeyedColor.G + ( NextKeyedColor.G - KeyedColor.G ) * T
                                 , KeyedColor.B + ( NextKeyedColor.B - KeyedColor.B ) * T
                                 , KeyedColor.A + ( NextKeyedColor.A - KeyedColor.A ) * T );

        SetColor( AnimColor );

        for( FArianeVertexID& VertexID : Vertices )
        {
            FArianeVertex* Vertex = VertexID.GetVertex();
            const FArianeKeyedVertex* KeyedVertex = const_cast<FArianeKeyedPath*>(KeyedPath)->GetKeyedVertex( Vertex->GetGuid() );
            const FArianeKeyedVertex* NextKeyedVertex = const_cast<FArianeKeyedPath*>(NextKeyedPath)->GetKeyedVertex( Vertex->GetGuid() );

            if( KeyedVertex && NextKeyedVertex )
            {
                FVector DeltaPosition = NextKeyedVertex->GetPosition() - KeyedVertex->GetPosition();
                double DeltaRadius = NextKeyedVertex->GetRadius() - KeyedVertex->GetRadius();

                Vertex->SetPosition( KeyedVertex->GetPosition() + ( DeltaPosition * T ) );
                Vertex->SetRadius( KeyedVertex->GetRadius() + ( DeltaRadius * T ) );
            }
        }

        for( FArianeSegmentID& SegmentID : Segments )
        {
            FArianeSegment* Segment = SegmentID.GetSegment();
            const FArianeKeyedSegment* KeyedSegment = const_cast<FArianeKeyedPath*>(KeyedPath)->GetKeyedSegment( Segment->GetGuid() );
            const FArianeKeyedSegment* NextKeyedSegment = const_cast<FArianeKeyedPath*>(NextKeyedPath)->GetKeyedSegment( Segment->GetGuid() );

            if( KeyedSegment && NextKeyedSegment )
            {
                if( Segment->GetClass() == FArianeSegmentCubic::StaticClass() )
                {
                    const FArianeKeyedSegmentCubic* KeyedCubicSegment = static_cast<const FArianeKeyedSegmentCubic*>(KeyedSegment);
                    const FArianeKeyedSegmentCubic* NextKeyedCubicSegment = static_cast<const FArianeKeyedSegmentCubic*>(NextKeyedSegment);

                    FArianeSegmentCubic* CubicSegment = static_cast<FArianeSegmentCubic*>(Segment);
                    FVector Handle0DeltaPosition = NextKeyedCubicSegment->GetHandlePosition(0) - KeyedCubicSegment->GetHandlePosition(0);
                    FVector Handle1DeltaPosition = NextKeyedCubicSegment->GetHandlePosition(1) - KeyedCubicSegment->GetHandlePosition(1);

                    CubicSegment->GetHandle((uint32)0)->SetPosition( KeyedCubicSegment->GetHandlePosition(0) + ( Handle0DeltaPosition * T ) );
                    CubicSegment->GetHandle((uint32)1)->SetPosition( KeyedCubicSegment->GetHandlePosition(1) + ( Handle1DeltaPosition * T ) );
                }
            }
        }
    }
}

FArianePathGeometry3D&
FArianePath::GetGeometry3D()
{
    return Geometry3D;
}

// static
FArianeVertex*
FArianePath::GetStitchingVertex( FArianeVertex* Vertex
                               , FArianeSegment* Segment
                               , const TArray<FArianeVertex*>& PickedVertexArray
                               , FVector& OutHandle )
{
    FArianeVertex *CurrentVertex = Vertex;
    FArianeSegment *CurrentSegment = Segment;

    do
    {
        // return the nextVertex if it is not marked for deletion. Then it will be stitched with its counterpart, if any.
        if( PickedVertexArray.Find( CurrentVertex ) == INDEX_NONE )
        {
            if( CurrentSegment->GetClass() == FArianeSegmentCubic::StaticClass() )
            {
                FArianeSegmentCubic* CurrentCubicSegment = static_cast<FArianeSegmentCubic*>(CurrentSegment);

                OutHandle = CurrentCubicSegment->GetHandle( CurrentVertex )->GetPosition();
            }

            return CurrentVertex;
        }

        // move to the next segment
        CurrentSegment = CurrentVertex->GetOtherSegment( CurrentSegment );
        CurrentVertex = CurrentSegment ? CurrentSegment->GetOtherVertex( CurrentVertex ) : nullptr;

    } while( ( CurrentVertex ) && ( CurrentVertex != Vertex ) );
                                  // loop detection

    return nullptr;
}

bool
FArianePath::DeleteVertex( const TArray<FArianeVertex*>& VerticesToRemove
                         , TArray<FArianeVertex*>* OutRemovedVertices
                         , TArray<FArianeSegment*>* OutRemovedSegments
                         , TArray<FArianeSegment*>* OutAddedSegments )
{
    uint32 SegmentClass = Segments.Num() ? Segments[0].GetSegment()->GetClass()
                                         : FArianeSegment::StaticClass();

    // only used internally by FOdysseyVectorPath::DeletePoint(). Declare in CPP file
    struct FStitchingPair
    {
        FArianeVertex* Vertex[2];
        FVector Handle[2];

        FStitchingPair( FArianeVertex* Vertex0, const FVector& Handle0
                      , FArianeVertex* Vertex1, const FVector& Handle1 )
        {
            // Note: ordering will ease comparisons between stitching pairs.
            Vertex[0] = Vertex0 < Vertex1 ? Vertex0 : Vertex1;
            Handle[0] = Vertex0 < Vertex1 ? Handle0 : Handle1;

            Vertex[1] = Vertex0 < Vertex1 ? Vertex1 : Vertex0;
            Handle[1] = Vertex0 < Vertex1 ? Handle1 : Handle0;
        }
    };

    TArray<FArianeVertex*> ExtendedVertices;
    TArray<FStitchingPair> StitchingPairs;

    StitchingPairs.Reserve( 10 );
    ExtendedVertices.Reserve( 10 );

    ExtendedVertices = VerticesToRemove;

    // first step
    // Build stitching pairs by finding a vertex that is not doomed for deletion on both sides.
    for( FArianeVertex* Vertex : VerticesToRemove )
    {
        uint32 segmentCount = Vertex->GetSegments().Num();

        if( segmentCount )
        {
            FVector Handle[2];
            FArianeSegment* Segment0 = Vertex->GetFirstSegment();
            FArianeSegment* Segment1 = Vertex->GetOtherSegment( Segment0 );
            FArianeVertex* StitchingVertices[2] = { Segment0 ? GetStitchingVertex( Segment0->GetOtherVertex(Vertex)
                                                                                 , Segment0
                                                                                 , VerticesToRemove
                                                                                 , Handle[0] ) : nullptr
                                                  , Segment1 ? GetStitchingVertex( Segment1->GetOtherVertex(Vertex)
                                                                                 , Segment1
                                                                                 , VerticesToRemove
                                                                                 , Handle[1] ) : nullptr };

            if( StitchingVertices[0] == StitchingVertices[1] ) // e.g loops
            {
                if( StitchingVertices[0] )
                {
                    // for vertices that were not picked but that cannot be stitched, delete them as well.
                    // they could aready be part of the picked vertices. We have to check if that's not already the case.
                    if( ExtendedVertices.Find( StitchingVertices[0] ) == INDEX_NONE )
                    {
                        ExtendedVertices.Add( StitchingVertices[0] );
                    }
                }
            }
            else // StitchingVertices[0] != StitchingVertices[1]
            {
                if ( StitchingVertices[0] && StitchingVertices[1] )
                {
                    FStitchingPair StitchingPair = FStitchingPair( StitchingVertices[0], Handle[0]
                                                                 , StitchingVertices[1], Handle[1] );

                    if( StitchingPairs.FindByPredicate( [StitchingPair]( FStitchingPair& StoredPair)  -> bool
                                                        {
                                                            return ( StoredPair.Vertex[0] == StitchingPair.Vertex[0] )
                                                                && ( StoredPair.Vertex[1] == StitchingPair.Vertex[1] );
                                                        } ) == nullptr )
                    {
                        StitchingPairs.Add( StitchingPair );
                    }
                }
                else
                {
                    if( StitchingVertices[0] && ( StitchingVertices[0]->GetSegments().Num() == 1 ) )
                    {
                        // for vertices that were not picked but that cannot be stitched, delete them as well.
                        // they could aready be part of the picked vertices. We have to check if that's not already the case.
                        if( ExtendedVertices.Find( StitchingVertices[0] ) == INDEX_NONE )
                        {
                            ExtendedVertices.Add( StitchingVertices[0] );
                        }
                    }

                    if( StitchingVertices[1] && ( StitchingVertices[1]->GetSegments().Num() == 1 ) )
                    {
                        // for vertices that were not picked but that cannot be stitched, delete them as well.
                        // they could aready be part of the picked vertices. We have to check if that's not already the case.
                        if( ExtendedVertices.Find( StitchingVertices[1] ) == INDEX_NONE )
                        {
                            ExtendedVertices.Add( StitchingVertices[1] );
                        }
                    }
                }
            }
        }
    }

    // second step
    // the actual deletion
    for( FArianeVertex* ExtendedVertex : ExtendedVertices )
    {
        // Note: work on a copy of the list, because deletion will alter the segment list
        TArray<FArianeSegment*> VertexSegments = ExtendedVertex->GetSegments();

        for( FArianeSegment* VertexSegment : VertexSegments )
        {
            if( OutRemovedSegments )
            {
                RemoveSegment( VertexSegment, false );
                // for custom undoing if needed
                OutRemovedSegments->Add( VertexSegment );
            }
            else
            {
                RemoveSegment( VertexSegment, true );
            }
        }

        if( OutRemovedVertices )
        {
            RemoveVertex( ExtendedVertex, false );
            // for custom undoing if needed
            OutRemovedVertices->Add( ExtendedVertex );
        }
        else
        {
            RemoveVertex( ExtendedVertex, true );
        }
    }

    // third step
    // stitch
    for( FStitchingPair& StitchingPair : StitchingPairs )
    {
        FArianeVertex* StitchingVertex0 = StitchingPair.Vertex[0];
        FArianeVertex* StitchingVertex1 = StitchingPair.Vertex[1];
        FArianeSegment* StitchedSegment = nullptr;

        if ( SegmentClass == FArianeSegment::StaticClass() )
        {
            StitchedSegment = AllocSegment( StitchingVertex0
                                          , StitchingVertex1
                                          , AllocationModel );
        }

        if ( SegmentClass == FArianeSegmentCubic::StaticClass() )
        {
            FVector Handle0 = StitchingPair.Handle[0];
            FVector Handle1 = StitchingPair.Handle[1];

            StitchedSegment = AllocCubicSegment( StitchingVertex0
                                               , Handle0.X
                                               , Handle0.Y
                                               , Handle0.Z
                                               , Handle1.X
                                               , Handle1.Y
                                               , Handle1.Z
                                               , StitchingVertex1
                                               , AllocationModel );
        }

        if( StitchedSegment )
        {
            AddSegment( StitchedSegment );

            // for custom undoing if needed
            if( OutAddedSegments )
            {
                OutAddedSegments->Add( StitchedSegment );
            }
        }
    }

    InvalidateAllSegments();

    return ( GetSegments().Num() ) ? false : true; // return true if the path is now empty
}

FArianePathGeometry3D::~FArianePathGeometry3D()
{
}

FArianePathGeometry3D::FArianePathGeometry3D( FArianePath* InPath )
    : FArianeObjectGeometry3D( InPath )
{
}

FVector
FArianePathGeometry3D::GetLeavingVectorAt( FArianeSegment* Segment
                                         , double T )
{
    if( T == 0.0f )
    {
        return Segment->GetTangentVectorAt( 0.0f, true );
    }

    if( T == 1.0f )
    {
        FArianeSegment* OtherSegment = Segment->GetVertex(1)->GetOtherSegment( Segment );

        return OtherSegment ? OtherSegment->GetVectorLeavingFromVertex( Segment->GetVertex(1), true )
                            : Segment->GetTangentVectorAt( 1.0f, true );
    }

    return Segment->GetTangentVectorAt( T, true );
}

FVector
FArianePathGeometry3D::GetTangentVectorAt( FArianeSegment* Segment
                                         , double T
                                         , bool bNormalize )
{
    FVector SegmentVector =  Segment->GetVertex(1)->GetPosition() - Segment->GetVertex(0)->GetPosition();
    FVector TangentVector = FVector::Zero();

    if( ( T == 0.0f ) || ( T == 1.0f ) )
    {
        FArianeVertex* Vertex = Segment->GetVertex( static_cast<uint32>(T) );
        FVector AverageVector = FVector::Zero();
        const TArray<FArianeSegment*> ConnectedSegments = Vertex->GetSegments();

        if( Vertex->GetSegments().Num() == 2 )
        {
            // For 3 points ABC
            FArianeSegment* OtherSegment = Vertex->GetOtherSegment( Segment );
            FVector VectorBA = Segment->GetVectorLeavingFromVertex( Vertex, true );
            FVector VectorBC = OtherSegment->GetVectorLeavingFromVertex( Vertex, true );

            TangentVector = SegmentVector.Dot( VectorBA ) > 0.0f ? (   VectorBA - VectorBC )
                                                                 : ( - VectorBA + VectorBC );
        }
    }

    if( bNormalize )
    {
        TangentVector.Normalize();
    }

    if( TangentVector.IsNearlyZero() )
    {
        TangentVector = Segment->GetTangentVectorAt( T, true );
    }

/*
    if( TangentVector.IsNearlyZero(0.001f) )
    {
        TangentVector = SegmentVector.GetSafeNormal();
    }
*/

    // Let's go in the same direction as the segment
/*
    if( TangentVector.Dot( SegmentVector ) < 0.0f )
    {
        TangentVector = -TangentVector;
    }
*/
    return TangentVector;
}

void
FArianePathGeometry3D::BuildSegmentAsFlat( FArianeSegment* Segment
                                         , double SegmentT0
                                         , double SegmentT1
                                         , FVector& InOutPreviousPerpendicularVector )
{
    FArianePath* Path = GetPath();
    FArianeVertex* SegmentVertices[2] = { Segment->GetVertex(0)
                                        , Segment->GetVertex(1) };
    double Radius0 = SegmentVertices[0]->GetRadius();
    double Radius1 = SegmentVertices[1]->GetRadius();
    double DeltaRadius = Radius1 - Radius0;
    FVector Normal0 = SegmentVertices[0]->GetNormal();
    FVector Normal1 = SegmentVertices[1]->GetNormal();
    FVector DeltaNormal = Normal1 - Normal0;
    FVector SegmentVector = SegmentVertices[1]->GetPosition() - SegmentVertices[0]->GetPosition();
    double SegmentDeltaT = SegmentT1 - SegmentT0;

    Segment->AllocateCache( ( Segment->GetFractionCount() + 1 ) * 2
                          , ( Segment->GetFractionCount() * 2 ) );

    TArray<FDynamicMeshVertex>& ModelVertexCache = const_cast<TArray<FDynamicMeshVertex>&>(Segment->GetModelVertexCache());
    TArray<uint32>& IndexCache = const_cast<TArray<uint32>&>(Segment->GetIndexCache());
    const TArray<FArianeSegment::FFraction>& Fractions = Segment->GetFractions();
    const TArray<FArianeSegment::FFractionStep>& FractionSteps = Segment->GetFractionSteps();

    for( int32 FractionStepIndex = 0; FractionStepIndex < FractionSteps.Num(); FractionStepIndex++ )
    {
        const FArianeSegment::FFractionStep& Step = FractionSteps[FractionStepIndex];
        uint32 ModelVertexOffset = FractionStepIndex * 2;
        double PointRadius = Radius0 + ( DeltaRadius * Step.T );
        FVector PerpendicularVector = Normal0 + ( DeltaNormal * Step.T );
        FVector TangentVector = GetTangentVectorAt( Segment
                                                  , Step.T
                                                  , true );
        double VertexU = (double) FractionStepIndex / ( FractionSteps.Num() - 1 );

        if( PerpendicularVector.Normalize() )
        {
            FVector UpVector = PerpendicularVector.Cross( TangentVector );
            FDynamicMeshVertex* ModelVertex0 = &ModelVertexCache[ModelVertexOffset+0];
            FDynamicMeshVertex* ModelVertex1 = &ModelVertexCache[ModelVertexOffset+1];

            // We do not check that UpVector's has a length. In case it does not, that way all vertices will be located at Step's position
            FVector NewPosition0 = Step.Point->GetPosition() + ( UpVector * PointRadius );
            FVector NewPosition1 = Step.Point->GetPosition() - ( UpVector * PointRadius );

            ModelVertex0->Position.X = NewPosition0.X;
            ModelVertex0->Position.Y = NewPosition0.Y;
            ModelVertex0->Position.Z = NewPosition0.Z;

            ModelVertex0->Color = Path->GetColor();

            ModelVertex0->TextureCoordinate[0].X = SegmentT0 + ( VertexU * SegmentDeltaT );
            ModelVertex0->TextureCoordinate[0].Y = 1.0f;

            ModelVertex0->TangentX = TangentVector;
            ModelVertex0->TangentZ = PerpendicularVector;

            ModelVertex1->Position.X = NewPosition1.X;
            ModelVertex1->Position.Y = NewPosition1.Y;
            ModelVertex1->Position.Z = NewPosition1.Z;

            ModelVertex1->Color = Path->GetColor();

            ModelVertex1->TextureCoordinate[0].X = SegmentT0 + ( VertexU * SegmentDeltaT );
            ModelVertex1->TextureCoordinate[0].Y = 0.0f;

            ModelVertex1->TangentX = TangentVector;
            ModelVertex1->TangentZ = PerpendicularVector;
        }
    }

    for( int32 FractionIndex = 0; FractionIndex < Fractions.Num(); FractionIndex++ )
    {
        const FArianeSegment::FFraction& SegmentFraction = Fractions[FractionIndex];
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
                                         , double SegmentT0
                                         , double SegmentT1
                                         , FVector& InOutPreviousPerpendicularVector )
{
    FArianePath* Path = GetPath();
    FArianeVertex* SegmentVertices[2] = { Segment->GetVertex(0)
                                        , Segment->GetVertex(1) };
    FVector SegmentVector = SegmentVertices[1]->GetPosition()
                          - SegmentVertices[0]->GetPosition();
    double SegmentRadius0 = SegmentVertices[0]->GetRadius();
    double SegmentRadius1 = SegmentVertices[1]->GetRadius();
    double SegmentDeltaRadius = SegmentRadius1 - SegmentRadius0;
    double SegmentDeltaT = SegmentT1 - SegmentT0;
    uint32 QuadDivisions = 12;
    uint32 VertexDivisions = QuadDivisions + 1;

    // Nb: The tube is not a closed one we duplicate the vertex at seam, this is required for proper UV coords
    // That's why we create "Divisions + 1" vertices for each ring
    Segment->AllocateCache( ( Segment->GetFractionCount() + 1 ) * VertexDivisions
                          , ( Segment->GetFractionCount() * 2 ) * QuadDivisions );

    TArray<FDynamicMeshVertex>& ModelVertexCache = const_cast<TArray<FDynamicMeshVertex>&>(Segment->GetModelVertexCache());
    TArray<uint32>& IndexCache = const_cast<TArray<uint32>&>(Segment->GetIndexCache());
    const TArray<FArianeSegment::FFraction>& Fractions = Segment->GetFractions();
    const TArray<FArianeSegment::FFractionStep>& FractionSteps = Segment->GetFractionSteps();

    if( InOutPreviousPerpendicularVector.IsNearlyZero() )
    {
        if (SegmentVector.GetSafeNormal().Dot(FVector::UpVector) <  1.0f)
        {
            InOutPreviousPerpendicularVector = SegmentVector.Cross( FVector::UpVector );
            InOutPreviousPerpendicularVector.Normalize();
        }
        else
        {
            FVector RightVector = Path->GetImage()->GetDrawingLayer()->GetLayerStack()->GetPainting3DComponent()->GetRightVector();

            InOutPreviousPerpendicularVector = SegmentVector.Cross( RightVector );
            InOutPreviousPerpendicularVector.Normalize();
        }
    }

    SegmentVertices[0]->SetNormal( InOutPreviousPerpendicularVector );

    for( int32 FractionStepIndex = 0; FractionStepIndex < FractionSteps.Num(); FractionStepIndex++ )
    {
        const FArianeSegment::FFractionStep& Step = FractionSteps[FractionStepIndex];
        uint32 ModelVertexOffset = FractionStepIndex * VertexDivisions;
        double PointRadius = SegmentRadius0 + ( SegmentDeltaRadius * Step.T );
        FVector TangentVector = GetTangentVectorAt( Segment
                                                  , Step.T
                                                  , true );
        double VertexU = (double) FractionStepIndex / ( FractionSteps.Num() - 1 );
        //FVector LeadingVector = GetLeavingVectorAt( Segment
        //                                          , Step.T );
        //FVector TangentVector = SegmentVector.GetSafeNormal();

        FVector ProjectedVector = TangentVector.Dot( InOutPreviousPerpendicularVector ) * TangentVector;
        FVector PerpendicularVector = InOutPreviousPerpendicularVector - ProjectedVector;

        PerpendicularVector.Normalize();

        if( PerpendicularVector.IsNearlyZero() )
        {
            PerpendicularVector = InOutPreviousPerpendicularVector;
        }
/*
        UE_LOG( LogTemp, Warning, TEXT("PerpendicularVector: %X %f %f %f - TangentVector: %f %f %f ")
                                                                          , Segment
                                                                          , PerpendicularVector.X
                                                                          , PerpendicularVector.Y
                                                                          , PerpendicularVector.Z
                                                                          , TangentVector.X
                                                                          , TangentVector.Y
                                                                          , TangentVector.Z );
*/
        if( TangentVector.IsNearlyZero() || PerpendicularVector.IsNearlyZero() )
        {
            UE_LOG( LogTemp, Warning, TEXT("BuildSegmentAsTube"));
        }

        if( PerpendicularVector.IsZero() == false )
        {
            float AngleInDegrees = 0.0f;
            float StepAngle = ( float ) 360 / QuadDivisions;
            //FVector PerpendicularAverage = ( InOutPreviousPerpendicularVector + PerpendicularVector ) * 0.5f;

            for( uint32 j = 0; j < VertexDivisions; j++ )
            {
                FRotator Rotator = UKismetMathLibrary::RotatorFromAxisAndAngle( TangentVector, AngleInDegrees );
                FDynamicMeshVertex* ModelVertex = &ModelVertexCache[ModelVertexOffset+j];
                FVector StepPosition = Step.Point->GetPosition();
                FVector RotatedPosition = StepPosition + ( Rotator.RotateVector( PerpendicularVector ) * PointRadius );

                ModelVertex->Position.X = RotatedPosition.X;
                ModelVertex->Position.Y = RotatedPosition.Y;
                ModelVertex->Position.Z = RotatedPosition.Z;

                ModelVertex->Color = Path->GetColor();

                ModelVertex->TextureCoordinate[0].X = SegmentT0 + ( VertexU * SegmentDeltaT );
                ModelVertex->TextureCoordinate[0].Y = (double) j / QuadDivisions;

                FVector NormalVector = ( RotatedPosition - StepPosition ).GetSafeNormal();
                ModelVertex->TangentX = TangentVector;
                ModelVertex->TangentZ = NormalVector;

                AngleInDegrees += StepAngle;
            }
        }

        InOutPreviousPerpendicularVector = PerpendicularVector;
    }

    SegmentVertices[1]->SetNormal( InOutPreviousPerpendicularVector );

    for( int32 FractionIndex = 0; FractionIndex < Fractions.Num(); FractionIndex++ )
    {
        const FArianeSegment::FFraction& SegmentFraction = Fractions[FractionIndex];
        uint32 ModelVertexOffset0 =   FractionIndex       * VertexDivisions;
        uint32 ModelVertexOffset1 = ( FractionIndex + 1 ) * VertexDivisions;
        FVector3f SampleVec0 = ModelVertexCache[ModelVertexOffset0 + 1].Position
                             - ModelVertexCache[ModelVertexOffset0 + 0].Position;
        FVector3f SampleVec1 = ModelVertexCache[ModelVertexOffset1 + 1].Position
                             - ModelVertexCache[ModelVertexOffset1 + 0].Position;

        bool Twisted = ( SampleVec0.Dot( SampleVec1 ) < 0.0f ) ? true : false;

        // for( uint32 i = 0, j = ( Divisions * 2 ) - 1; i < Divisions; i++, j-- ) // commented-out: version with twist-detection.
        for( uint32 i = 0; i < QuadDivisions; i++ )
        {
            uint32 Triangle0Index = ( FractionIndex * QuadDivisions * 2 * 3 ) + ( i * 2 * 3 ); // 2 triangles per quad, 3 indexes per tirangle
            uint32 Triangle1Index = Triangle0Index + 3;

            if( Twisted == false )
            {
                // first triangle
                IndexCache[Triangle0Index+0] = ModelVertexOffset1 + ( ( i + 1 ) );
                IndexCache[Triangle0Index+1] = ModelVertexOffset0 + ( ( i + 1 ) );
                IndexCache[Triangle0Index+2] = ModelVertexOffset0 + ( ( i     ) );

                // second triangle
                IndexCache[Triangle1Index+0] = ModelVertexOffset0 + ( ( i     ) );
                IndexCache[Triangle1Index+1] = ModelVertexOffset1 + ( ( i     ) );
                IndexCache[Triangle1Index+2] = ModelVertexOffset1 + ( ( i + 1 ) );
            }
/* there should be no twisting with the Parallel Transport method
            else
            {
                // first triangle
                IndexCache[Triangle0Index+0] = ModelVertexOffset1 + ( ( j - 1 ) );
                IndexCache[Triangle0Index+1] = ModelVertexOffset0 + ( ( i + 1 ) );
                IndexCache[Triangle0Index+2] = ModelVertexOffset0 + ( ( i     ) );

                // second triangle
                IndexCache[Triangle1Index+0] = ModelVertexOffset0 + ( ( i     ) );
                IndexCache[Triangle1Index+1] = ModelVertexOffset1 + ( ( j     ) );
                IndexCache[Triangle1Index+2] = ModelVertexOffset1 + ( ( j - 1 ) );
            }
*/
        }
    }
}

FArianePath*
FArianePathGeometry3D::GetPath()
{
    return static_cast<FArianePath*>(Object);
}

void
FArianePathGeometry3D::Build()
{
    FArianePath* Path = GetPath();
    uint32 TotalModelVertexCount = 0;
    uint32 TotalIndexCount = 0;
    FVector PreviousPerpendicularVector = FVector::Zero();

    MeshVertices.Empty();
    MeshIndices.Empty();

    for( const FArianePath::Chain& Chain : Path->GetChains() )
    {
        FArianeSegment* FirstSegment = Chain.Segments[0];
        bool bForceRebuild = false;
        double T0 = 0.0f;

        Chain.IterateSegments( [ this
                                , Path
                                , &Chain
                                , &bForceRebuild
                                , &PreviousPerpendicularVector
                                , &T0 ] ( FArianeVertex* Vertex, FArianeSegment* Segment ) -> bool
        {
            FArianeVertex* OtherVertex = Segment->GetOtherVertex( Vertex );
            double AverageRadius = ( OtherVertex->GetRadius() + Vertex->GetRadius() ) * 0.5f;
            // We multiply by 2 because we take the diameter to estimate T1
            double T1 = T0;

            if( Segment->IsInvalidated() )
            {
                Segment->Update();

                bForceRebuild = true;

                PreviousPerpendicularVector = Segment->GetVertex(0)->GetNormal();
            }

            switch( Path->GetLineType() )
            {
                case EArianePathLineType::Flat :
                {
                    T1 = AverageRadius ? T0 + ( Segment->GetLength() / ( AverageRadius * 2 ) ) : 0.0f;

                    if( bForceRebuild )
                    {
                        BuildSegmentAsFlat( Segment
                                          , T0
                                          , T1
                                          , PreviousPerpendicularVector );
                        // for flat paths, the perpendicular vector is independent, related to the initial drawing plane (for now)
                        // so we can update only segments that are invalidated (but for simplicity we still iterate on all segments).
                        bForceRebuild = false;
                    }
                }
                break;

                case EArianePathLineType::Tube :
                {
                    T1 = AverageRadius ? T0 + ( Segment->GetLength() / ( AverageRadius * 2 * PI ) ) : 0.0f;

                    if( bForceRebuild )
                    {
                        BuildSegmentAsTube( Segment
                                          , T0
                                          , T1
                                          , PreviousPerpendicularVector );
                        // however for tubes,  a segment perpendicular vector depends on the previous segment perpendicular vector
                        // bForceRebuild = true; // commented-out because useless, but left for clarity
                    }
                }
                break;

                default:
                break;
            }

            T0 = T1;

            return false; // continue
        } );
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

    InitVertexFactory();

    // DrawingLayer can be null in animation keys
    if( Path->GetImage()->GetDrawingLayer().IsValid() )
    {
        Path->GetImage()->GetDrawingLayer()->MarkRenderStateDirty();
    }
}
