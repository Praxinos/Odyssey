// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
// Ariane Header
#include "ArianeCoreEnums.h"

#include "ArianeID.generated.h"

struct FArianeObject;
struct FArianeSegment;
struct FArianeVertex;
struct FArianeTag;
class UArianeLayerVector;
class UArianeDrawing;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeObjectID
{
    GENERATED_BODY()

    public:
        ~FArianeObjectID(){};
        FArianeObjectID();
        FArianeObjectID( FArianeObject* Object );

        FArianeObject* GetObject( UArianeDrawing* Drawing );
        FGuid GetGuid() const;

    protected:
        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        EArianeAllocationModel AllocationModel;

    protected:
        FGuid CachedDrawingAllocatorGuid;
        FArianeObject* CachedObject;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianeVertexID
{
    GENERATED_BODY()

    public:
        ~FArianeVertexID(){};
        FArianeVertexID();
        FArianeVertexID( FArianeVertex* InVertex );

        FArianeVertex* GetVertex( UArianeDrawing* Drawing );

    protected:
        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        EArianeAllocationModel AllocationModel;

    protected:
        FGuid CachedObjectAllocatorGuid;
        FArianeVertex* CachedVertex;
};


USTRUCT(BlueprintType)
struct ARIANE_API FArianeSegmentID
{
    GENERATED_BODY()

    public:
        ~FArianeSegmentID(){};
        FArianeSegmentID();
        FArianeSegmentID( FArianeSegment* InSegment );

        FArianeSegment* GetSegment( UArianeDrawing* Drawing );

    public:
        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        EArianeAllocationModel AllocationModel;

    protected:
        FGuid CachedObjectAllocatorGuid;
        FArianeSegment* CachedSegment;
};

USTRUCT(BlueprintType)
struct ARIANE_API FArianeTagID
{
    GENERATED_BODY()

    public:
        ~FArianeTagID(){};
        FArianeTagID();
        FArianeTagID( FArianeTag* InTag );

        FArianeTag* GetTag( UArianeDrawing* Drawing );

    public:
        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        EArianeAllocationModel AllocationModel;

    protected:
        FGuid CachedObjectAllocatorGuid;
        FArianeTag* CachedTag;
};
