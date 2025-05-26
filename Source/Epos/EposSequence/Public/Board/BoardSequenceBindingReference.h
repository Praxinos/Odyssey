// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneBindingReferences.h"
#include "BoardSequenceBindingReference.generated.h"

USTRUCT()
struct FBoardSequenceBindingReferences : public FMovieSceneBindingReferences
{
    GENERATED_BODY()

    // Base functions are not virtual, so they can't be overrided...

    void AddBinding( const FGuid& ObjectId, UObject* InObject, UObject* InContext );

    void ResolveBinding( const FGuid& ObjectId, UObject* InContext, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const;
};
