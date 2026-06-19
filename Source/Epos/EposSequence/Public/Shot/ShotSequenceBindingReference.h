// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
