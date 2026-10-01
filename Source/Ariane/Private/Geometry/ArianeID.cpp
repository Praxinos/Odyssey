// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeID.h"
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianeObject.h"
#include "ArianePath.h"
#include "ArianeTag.h"

FArianeObjectID::FArianeObjectID()
    : Guid ( FGuid() )
    , AllocationModel (  EArianeAllocationModel::Unknown  )
    , CachedImageAllocatorGuid( FGuid() )
    , CachedObject( nullptr )
{
}

FArianeObjectID::FArianeObjectID( FArianeObject* Object )
    : CachedImageAllocatorGuid( FGuid() )
    , CachedObject( Object )
    , AllocationModel( Object ? Object->GetAllocationModel() : EArianeAllocationModel::InstancedStruct )
    , Guid ( Object ? Object->GetGuid() : FGuid() )
{
}

FArianeObject*
FArianeObjectID::GetObject( UArianeImage* Image )
{
    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        if( Image->GetAllocatorGuid() != CachedImageAllocatorGuid )
        {
            CachedObject = Image->GetObject( Guid );
            CachedImageAllocatorGuid = Image->GetAllocatorGuid();
        }
    }

    return CachedObject;
}

FGuid
FArianeObjectID::GetGuid() const
{
    return Guid;
}

///////////////////////// VertexID

FArianeVertexID::FArianeVertexID()
    : OwnerID( FArianeObjectID() )
    , Guid ( FGuid() )
    , AllocationModel (  EArianeAllocationModel::Unknown  )
    , CachedObjectAllocatorGuid ( FGuid() )
    , CachedVertex( nullptr )
{
}

FArianeVertexID::FArianeVertexID( FArianeVertex* Vertex )
    : OwnerID( Vertex->GetOwner() )
    , Guid ( Vertex->GetGuid() )
    , AllocationModel( Vertex->GetAllocationModel() )
    , CachedObjectAllocatorGuid ( FGuid() )
    , CachedVertex( Vertex )
{
}

FArianeVertex*
FArianeVertexID::GetVertex( UArianeImage* Image )
{
    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        FArianeObject* OwnerObject = OwnerID.GetObject( Image );

        if( OwnerObject->GetAllocatorGuid() != CachedObjectAllocatorGuid )
        {
            if( OwnerObject->HasBaseClass( FArianePath::StaticClass() ) )
            {
                FArianePath* Path = static_cast<FArianePath*>(OwnerObject);

                CachedVertex = Path->GetVertexByGuid( Guid );
                CachedObjectAllocatorGuid = OwnerObject->GetAllocatorGuid();
            }
        }
    }

    return CachedVertex;
};

////////////////////// SegmentID

FArianeSegmentID::FArianeSegmentID()
    : OwnerID( FArianeObjectID() )
    , Guid ( FGuid() )
    , AllocationModel (  EArianeAllocationModel::Unknown  )
    , CachedObjectAllocatorGuid ( FGuid() )
    , CachedSegment( nullptr )
{
}

FArianeSegmentID::FArianeSegmentID( FArianeSegment* Segment )
    : OwnerID( Segment->GetOwner() )
    , Guid ( Segment->GetGuid() )
    , AllocationModel( Segment->GetAllocationModel() )
    , CachedObjectAllocatorGuid ( FGuid() )
    , CachedSegment( Segment )
{
}

FArianeSegment*
FArianeSegmentID::GetSegment( UArianeImage* Image )
{
    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        FArianeObject* OwnerObject = OwnerID.GetObject( Image );

        if( OwnerObject->GetAllocatorGuid() != CachedObjectAllocatorGuid )
        {
            if( OwnerObject->HasBaseClass( FArianePath::StaticClass() ) )
            {
                FArianePath* Path = static_cast<FArianePath*>(OwnerObject);

                CachedSegment = Path->GetSegmentByGuid( Guid );
                CachedObjectAllocatorGuid = OwnerObject->GetAllocatorGuid();
            }
        }
    }

    return CachedSegment;
};

///////////////////////// TagID

FArianeTagID::FArianeTagID()
    : OwnerID( FArianeObjectID() )
    , Guid ( FGuid() )
    , AllocationModel( EArianeAllocationModel::Unknown )
    , CachedObjectAllocatorGuid ( FGuid() )
    , CachedTag( nullptr )
{
}

FArianeTagID::FArianeTagID( FArianeTag* Tag )
    : OwnerID( Tag->GetOwner() )
    , Guid ( Tag->GetGuid() )
    , AllocationModel( Tag->GetAllocationModel() )
    , CachedObjectAllocatorGuid ( FGuid() )
    , CachedTag( Tag )
{
}

FArianeTag*
FArianeTagID::GetTag( UArianeImage* Image )
{
    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        FArianeObject* OwnerObject = OwnerID.GetObject( Image );

        if( OwnerObject->GetAllocatorGuid() != CachedObjectAllocatorGuid )
        {
            CachedTag = OwnerObject->GetTagByGuid( Guid );
            CachedObjectAllocatorGuid = OwnerObject->GetAllocatorGuid();
        }
    }

    return CachedTag;
};
