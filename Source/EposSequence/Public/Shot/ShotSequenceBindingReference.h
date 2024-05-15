// Copyright Epic Games, Inc. All Rights Reserved.

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
