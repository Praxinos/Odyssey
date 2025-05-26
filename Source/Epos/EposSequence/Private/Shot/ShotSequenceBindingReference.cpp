// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
