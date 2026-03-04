// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "ArianeID.h"
#include "ArianePoint.h"
#include "ArianeVertex.generated.h"

struct FArianeObject;
struct FArianeSegment;
class UArianePainting3DComponent;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeVertex : public FArianePoint
{
    GENERATED_BODY()

    public:
        ~FArianeVertex();
        FArianeVertex(){};
        FArianeVertex( FArianeObject* Owner, const FVector& iPosition, const FVector& InNormal, double InRadius );

        void AddSegment( FArianeSegment* iSegment );
        void RemoveSegment( FArianeSegment* iSegment );

        const TArray<FArianeSegment*>& GetSegments();
        FArianeObject* GetOwner();
        double  GetRadius();

        void SetNormal( const FVector& InNormal );
        const FVector& GetNormal();

        virtual void PostEditUndo(){};

    public:
        UPROPERTY( EditAnywhere )
        FGuid Guid;

        UPROPERTY( EditAnywhere )
        FArianeObjectID OwnerID;

        UPROPERTY( EditAnywhere )
        double Radius;

        UPROPERTY( EditAnywhere )
        FVector Normal;

    protected:
        TArray<FArianeSegment*> Segments;
};
