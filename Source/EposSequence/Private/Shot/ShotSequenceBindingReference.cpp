// Copyright Epic Games, Inc. All Rights Reserved.

#include "Shot/ShotSequenceBindingReference.h"

#include "UniversalObjectLocator.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ShotSequenceBindingReference)


void FShotSequenceBindingReferences::AddBinding( const FGuid& ObjectId, UObject* InObject, UObject* InContext )
{
    FUniversalObjectLocator NewLocator( InObject, InContext );
    if( !NewLocator.IsEmpty() )
    {
        FMovieSceneBindingReferences::AddBinding( ObjectId, MoveTemp( NewLocator ) );
    }
}

void FShotSequenceBindingReferences::ResolveBinding( const FGuid& ObjectId, UObject* InContext, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const
{
    FMovieSceneBindingReferences::ResolveBinding( ObjectId, UE::UniversalObjectLocator::FResolveParams::SyncFind( InContext ), OutObjects );
}
