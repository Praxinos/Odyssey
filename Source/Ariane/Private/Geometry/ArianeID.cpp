// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeID.h"
#include "ArianePainting3DComponent.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianeObject.h"
#include "ArianePath.h"

FArianeObjectID::FArianeObjectID()
    : Painting3DComponent( nullptr )
    , CachedObject( nullptr )
{
}

FArianeObjectID::FArianeObjectID( FArianeObject* Object )
    : CachedObject( nullptr )
{
    Painting3DComponent = Object->Painting3DComponent;

    Guid = Object->Guid;
}

FArianeObject*
FArianeObjectID::GetObject()
{
    if( Painting3DComponent )
    {
        if( CachedObject == nullptr )
        {
            CachedObject = Painting3DComponent->GetObject( Guid );
        }
    }

    return CachedObject;
}

void
FArianeObjectID::InvalidatePointerCache()
{
    CachedObject = nullptr;
}


///////////////////////// VertexID

FArianeVertexID::FArianeVertexID()
    : Painting3DComponent( nullptr )
    , CachedVertex( nullptr )
{
}

FArianeVertexID::FArianeVertexID( FArianeVertex* Vertex )
    : CachedVertex( nullptr )
{
    Painting3DComponent = Vertex->GetOwner()->Painting3DComponent;

    Guid = Vertex->Guid;

    OwnerGuid = Vertex->GetOwner()->Guid;
}

FArianeVertex*
FArianeVertexID::GetVertex()
{
    if( CachedVertex == nullptr )
    {
        FArianeObject* OwnerObject = Painting3DComponent->GetObject( OwnerGuid );

        if( OwnerObject )
        {
            FArianePath* Path = static_cast<FArianePath*>(OwnerObject);

            CachedVertex = Path->GetVertexByGuid( Guid );
        }
    }

    return CachedVertex;
};

void
FArianeVertexID::InvalidatePointerCache()
{
    CachedVertex = nullptr;
}

////////////////////// SegmentID

FArianeSegmentID::FArianeSegmentID()
    : Painting3DComponent( nullptr )
    , CachedSegment( nullptr )
{
}

FArianeSegmentID::FArianeSegmentID( FArianeSegment* Segment )
    : CachedSegment( nullptr )
{
    Painting3DComponent = Segment->GetOwner()->Painting3DComponent;

    Guid = Segment->Guid;

    OwnerGuid = Segment->GetOwner()->Guid;
}

FArianeSegment*
FArianeSegmentID::GetSegment()
{
    if( CachedSegment == nullptr )
    {
        FArianeObject* OwnerObject = Painting3DComponent->GetObject( OwnerGuid );

        if( OwnerObject )
        {
            FArianePath* Path = static_cast<FArianePath*>(OwnerObject);

            CachedSegment = Path->GetSegmentByGuid( Guid );
        }
    }

    return CachedSegment;
};

void
FArianeSegmentID::InvalidatePointerCache()
{
    CachedSegment = nullptr;
}
