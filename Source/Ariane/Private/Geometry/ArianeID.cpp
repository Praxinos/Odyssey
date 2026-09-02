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
    : Image( nullptr )
    , CachedObject( nullptr )
{
}

FArianeObjectID::FArianeObjectID( FArianeObject* Object )
    : Image( nullptr )
    , CachedObject( nullptr )
{
    if( Object )
    {
        if( Object->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            CachedObject = Object;
        }

        if( Object->GetAllocationModel() == EArianeAllocationModel::InstancedStruct )
        {
            Image = Object->GetImage();

            Guid = Object->GetGuid();
        }
    }
}

FArianeObject*
FArianeObjectID::GetObject()
{
    if( Image )
    {
        if( CachedObject == nullptr )
        {
            CachedObject = Image->GetObject( Guid );
        }
    }

    return CachedObject;
}

void
FArianeObjectID::InvalidateCache()
{
    CachedObject = nullptr;
}


///////////////////////// VertexID

FArianeVertexID::FArianeVertexID()
    : Image( nullptr )
    , CachedVertex( nullptr )
{
}

FArianeVertexID::FArianeVertexID( FArianeVertex* Vertex )
    : Image( nullptr )
    , CachedVertex( nullptr )
{
    if( Vertex )
    {
        if( Vertex->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            CachedVertex = Vertex;
        }

        if( Vertex->GetAllocationModel() == EArianeAllocationModel::InstancedStruct )
        {
            Image = Vertex->GetOwner()->GetImage();

            Guid = Vertex->GetGuid();

            OwnerGuid = Vertex->GetOwner()->GetGuid();
        }
    }
}

FArianeVertex*
FArianeVertexID::GetVertex()
{
    if( CachedVertex == nullptr )
    {
        FArianeObject* OwnerObject = Image->GetObject( OwnerGuid );

        if( OwnerObject )
        {
            FArianePath* Path = static_cast<FArianePath*>(OwnerObject);

            CachedVertex = Path->GetVertexByGuid( Guid );
        }
    }

    return CachedVertex;
};

void
FArianeVertexID::InvalidateCache()
{
    CachedVertex = nullptr;
}

////////////////////// SegmentID

FArianeSegmentID::FArianeSegmentID()
    : Image( nullptr )
    , CachedSegment( nullptr )
{
}

FArianeSegmentID::FArianeSegmentID( FArianeSegment* Segment )
    : Image( nullptr )
    , CachedSegment( nullptr )
{
    if( Segment )
    {
        if( Segment->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            CachedSegment = Segment;
        }

        if( Segment->GetAllocationModel() == EArianeAllocationModel::InstancedStruct )
        {
            Image = Segment->GetOwner()->GetImage();

            Guid = Segment->GetGuid();

            OwnerGuid = Segment->GetOwner()->GetGuid();
        }
    }
}

FArianeSegment*
FArianeSegmentID::GetSegment()
{
    if( CachedSegment == nullptr )
    {
        FArianeObject* OwnerObject = Image->GetObject( OwnerGuid );

        if( OwnerObject )
        {
            FArianePath* Path = static_cast<FArianePath*>(OwnerObject);

            CachedSegment = Path->GetSegmentByGuid( Guid );
        }
    }

    return CachedSegment;
};

void
FArianeSegmentID::InvalidateCache()
{
    CachedSegment = nullptr;
}

///////////////////////// TagID

FArianeTagID::FArianeTagID()
    : Image( nullptr )
    , CachedTag( nullptr )
{
}

FArianeTagID::FArianeTagID( FArianeTag* Tag )
    : Image( nullptr )
    , CachedTag( nullptr )
{
    if( Tag )
    {
        if( Tag->GetAllocationModel() == EArianeAllocationModel::OperatingSystem )
        {
            CachedTag = Tag;
        }

        if( Tag->GetAllocationModel() == EArianeAllocationModel::InstancedStruct )
        {
            Image = Tag->GetOwner()->GetImage();

            Guid = Tag->GetGuid();

            OwnerGuid = Tag->GetOwner()->GetGuid();
        }
    }
}

FArianeTag*
FArianeTagID::GetTag()
{
    if( CachedTag == nullptr )
    {
        FArianeObject* OwnerObject = Image->GetObject( OwnerGuid );

        if( OwnerObject )
        {
            CachedTag = OwnerObject->GetTagByGuid( Guid );
        }
    }

    return CachedTag;
};

void
FArianeTagID::InvalidateCache()
{
    CachedTag = nullptr;
}
