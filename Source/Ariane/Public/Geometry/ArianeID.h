// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeID.generated.h"

struct FArianeObject;
struct FArianeSegment;
struct FArianeVertex;
struct FArianeTag;
class UArianeLayerDrawing;
class UArianeImage;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeObjectID
{
    GENERATED_BODY()

    public:
        ~FArianeObjectID(){};
        FArianeObjectID();
        FArianeObjectID( FArianeObject* Object );

        FArianeObject* GetObject();
        void InvalidateCache();

    public:
        UPROPERTY( EditAnywhere )
        UArianeImage* Image;

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
        void InvalidateCache();

    public:
        UPROPERTY( EditAnywhere )
        UArianeImage* Image;

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
        void InvalidateCache();

    public:
        UPROPERTY( EditAnywhere )
        UArianeImage* Image;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FGuid OwnerGuid;

    protected:
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

        FArianeTag* GetTag();
        void InvalidateCache();

    public:
        UPROPERTY( EditAnywhere )
        UArianeImage* Image;

        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FGuid OwnerGuid;

    protected:
        FArianeTag* CachedTag;
};
