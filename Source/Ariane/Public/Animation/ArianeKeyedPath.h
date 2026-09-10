// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeKeyedObject.h"
#include "ArianeKeyedVertex.h"
#include "ArianeKeyedSegment.h"
#include "ArianeKeyedSegmentCubic.h"

#include "ArianeKeyedPath.generated.h"

struct FArianePath;

USTRUCT(BlueprintType)
struct ARIANE_API FArianeKeyedPath : public FArianeKeyedObject
{
GENERATED_BODY()

public:
    virtual ~FArianeKeyedPath();
    FArianeKeyedPath();
    FArianeKeyedPath( FArianePath* Path );
    virtual void PostLoad() override;
    virtual void PostEditUndo() override;
    FArianeKeyedVertex* GetKeyedVertex( const FGuid& VertexGuid );
    FArianeKeyedSegment* GetKeyedSegment( const FGuid& SegmentGuid );
    const FColor& GetKeyedColor() const;

protected:
    UPROPERTY( EditAnywhere )
    TArray<FArianeKeyedVertex> KeyedVertices;

    UPROPERTY( EditAnywhere )
    TArray<FArianeKeyedSegment> KeyedLinearSegments;

    UPROPERTY( EditAnywhere )
    TArray<FArianeKeyedSegmentCubic> KeyedCubicSegments;

    UPROPERTY( EditAnywhere )
    FColor KeyedColor;

    TArray<FArianeKeyedSegment*> KeyedSegments;
};
