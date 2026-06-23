// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeID.h"
#include "ArianeLayerDrawing.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianeObject.h"
#include "ArianePath.h"
#include "ArianeTag.h"

FArianeObjectID::FArianeObjectID()
    : DrawingLayer( nullptr )
    , CachedObject( nullptr )
{
}

FArianeObjectID::FArianeObjectID( FArianeObject* Object )
    : DrawingLayer( nullptr )
    , CachedObject( ( Object && ( Object->GetAllocationModel() == EArianeAllocationModel::OperatingSystem ) ) ? Object : nullptr )
{
    if( Object )
    {
        DrawingLayer = Object->GetDrawingLayer();

        Guid = Object->GetGuid();
    }
}

FArianeObject*
FArianeObjectID::GetObject()
{
    if( DrawingLayer )
    {
        if( CachedObject == nullptr )
        {
            CachedObject = DrawingLayer->GetObject( Guid );
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
    : DrawingLayer( nullptr )
    , CachedVertex( nullptr )
{
}

FArianeVertexID::FArianeVertexID( FArianeVertex* Vertex )
    : CachedVertex( ( Vertex && ( Vertex->GetAllocationModel() == EArianeAllocationModel::OperatingSystem ) ) ? Vertex : nullptr )
{
    DrawingLayer = Vertex->GetOwner()->GetDrawingLayer();

    Guid = Vertex->GetGuid();

    OwnerGuid = Vertex->GetOwner()->GetGuid();
}

FArianeVertex*
FArianeVertexID::GetVertex()
{
    if( CachedVertex == nullptr )
    {
        FArianeObject* OwnerObject = DrawingLayer->GetObject( OwnerGuid );

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
    : DrawingLayer( nullptr )
    , CachedSegment( nullptr )
{
}

FArianeSegmentID::FArianeSegmentID( FArianeSegment* Segment )
    : CachedSegment( ( Segment && ( Segment->GetAllocationModel() == EArianeAllocationModel::OperatingSystem ) ) ? Segment : nullptr )
{
    DrawingLayer = Segment->GetOwner()->GetDrawingLayer();

    Guid = Segment->GetGuid();

    OwnerGuid = Segment->GetOwner()->GetGuid();
}

FArianeSegment*
FArianeSegmentID::GetSegment()
{
    if( CachedSegment == nullptr )
    {
        FArianeObject* OwnerObject = DrawingLayer->GetObject( OwnerGuid );

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
    : DrawingLayer( nullptr )
    , CachedTag( nullptr )
{
}

FArianeTagID::FArianeTagID( FArianeTag* Tag )
    : CachedTag( ( Tag && ( Tag->GetAllocationModel() == EArianeAllocationModel::OperatingSystem ) ) ? Tag : nullptr )
{
    DrawingLayer = Tag->GetOwner()->GetDrawingLayer();

    Guid = Tag->GetGuid();

    OwnerGuid = Tag->GetOwner()->GetGuid();
}

FArianeTag*
FArianeTagID::GetTag()
{
    if( CachedTag == nullptr )
    {
        FArianeObject* OwnerObject = DrawingLayer->GetObject( OwnerGuid );

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
