// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeID.generated.h"

struct FArianeObject;
struct FArianeSegment;
struct FArianeVertex;
class UArianePainting3DComponent;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeObjectID
{
    GENERATED_BODY()

    public:
        ~FArianeObjectID(){};
        FArianeObjectID();
        FArianeObjectID( FArianeObject* Object );

        FArianeObject* GetObject();

    public:
        UPROPERTY( EditAnywhere )
        UArianePainting3DComponent* Painting3DComponent;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

    protected:
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

        FArianeVertex* GetVertex();

    public:
        UPROPERTY( EditAnywhere )
        UArianePainting3DComponent* Painting3DComponent;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FGuid OwnerGuid;

    protected:
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

        FArianeSegment* GetSegment();

    public:
        UPROPERTY( EditAnywhere )
        UArianePainting3DComponent* Painting3DComponent;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FGuid OwnerGuid;

    protected:
        FArianeSegment* CachedSegment;
};
