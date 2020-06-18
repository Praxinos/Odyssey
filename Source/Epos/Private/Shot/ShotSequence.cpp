// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#include "Shot/ShotSequence.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include "MovieScene.h"

//---

UShotSequence::UShotSequence( const FObjectInitializer& ObjectInitializer )
    : Super( ObjectInitializer )
    , MovieScene( nullptr )
{
    //bParentContextsAreSignificant = true;
}

void UShotSequence::Initialize()
{
    MovieScene = NewObject<UMovieScene>( this, NAME_None, RF_Transactional );

    MovieScene->SetEvaluationType( EMovieSceneEvaluationType::WithSubFrames );

    FFrameRate TickResolution( 24000, 1 );
    MovieScene->SetTickResolutionDirectly( TickResolution );

    FFrameRate DisplayRate( 24, 1 );
    MovieScene->SetDisplayRate( DisplayRate );
}

void UShotSequence::BindPossessableObject( const FGuid& ObjectId, UObject& PossessedObject, UObject* Context )
{
    //if (UActorComponent* Component = Cast<UActorComponent>(&PossessedObject))
    //{
    //	const FName ComponentName = Component->GetFName();
    //	BoundActorComponents.Add(ObjectId, ComponentName);
    //}
}

bool UShotSequence::CanPossessObject( UObject& Object, UObject* InPlaybackContext ) const
{
    //return Object.IsA<AActor>() || Object.IsA<UActorComponent>();
    return nullptr;
}

void UShotSequence::LocateBoundObjects( const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const
{
    //AActor* Actor = Cast<AActor>(Context);
    //if (Actor == nullptr)
    //{
    //	return;
    //}

    //const FName* ComponentName = BoundActorComponents.Find(ObjectId);
    //if (ComponentName == nullptr)
    //{
    //	return;
    //}

    //if (UActorComponent* FoundComponent = FindObject<UActorComponent>(Actor, *ComponentName->ToString(), false))
    //{
    //	OutObjects.Add(FoundComponent);
    //}
}

UMovieScene* UShotSequence::GetMovieScene() const
{
    return MovieScene;
}

UObject* UShotSequence::GetParentObject( UObject* Object ) const
{
    //if (UActorComponent* Component = Cast<UActorComponent>(Object))
    //{
    //	return Component->GetOwner();
    //}

    return nullptr;
}

void UShotSequence::UnbindPossessableObjects( const FGuid& ObjectId )
{
    //BoundActorComponents.Remove(ObjectId);
}

void UShotSequence::UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context )
{
    //BoundActorComponents.Remove(ObjectId);
}

void UShotSequence::UnbindInvalidObjects( const FGuid& ObjectId, UObject* Context )
{
    //BoundActorComponents.Remove(ObjectId);
}

//#if WITH_EDITOR
//FText UShotSequence::GetDisplayName() const
//{
//	return UMovieSceneSequence::GetDisplayName();
//}
//
//void UShotSequence::GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const
//{
//	Super::GetAssetRegistryTags(OutTags);
//
//	if (BoundActorClass != nullptr)
//	{
//		FAssetRegistryTag Tag("BoundActorClass", BoundActorClass->GetName(), FAssetRegistryTag::TT_Alphabetical);
//		OutTags.Add(Tag);
//	}
//	else
//	{
//		OutTags.Emplace("BoundActorClass", "(None)", FAssetRegistryTag::TT_Alphabetical);
//	}
//}
//
//void UShotSequence::GetAssetRegistryTagMetadata(TMap<FName, FAssetRegistryTagMetadata>& OutMetadata) const
//{
//	Super::GetAssetRegistryTagMetadata(OutMetadata);
//
//	OutMetadata.Add(
//		"BoundActorClass",
//		FAssetRegistryTagMetadata()
//			.SetDisplayName(NSLOCTEXT("TemplateSequence", "BoundActorClass_Label", "Bound Actor Class"))
//			.SetTooltip(NSLOCTEXT("TemplateSequence", "BoundActorClass_Tooltip", "The type of actor bound to this template sequence"))
//		);
//}
//
//#endif
