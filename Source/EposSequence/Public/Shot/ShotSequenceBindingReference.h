// IDDN.FR.000.000000.000.S.X.0000.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2024

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneBindingReferences.h"
#include "ShotSequenceBindingReference.generated.h"

USTRUCT()
struct FShotSequenceBindingReferences: public FMovieSceneBindingReferences
{
    GENERATED_BODY()

    // Base functions are not virtual, so they can't be overrided...

    void AddBinding( const FGuid& ObjectId, UObject* InObject, UObject* InContext );

    void ResolveBinding( const FGuid& ObjectId, UObject* InContext, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const;
};
