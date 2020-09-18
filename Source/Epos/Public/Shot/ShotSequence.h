// Copyright © 2020 Praxinos, Inc. All Rights Reserved.
// IDDN 

#pragma once

#include "LevelSequenceBindingReference.h"
#include "MovieSceneSequence.h"
#include "MovieScene.h"
#include "UObject/SoftObjectPtr.h"
#include "ShotSequence.generated.h"

/*
 * Movie scene animation that represents the first level of the storyboard.
 */
UCLASS( BlueprintType )
class EPOS_API UShotSequence : public UMovieSceneSequence
{
public:
    GENERATED_BODY()

    UShotSequence( const FObjectInitializer& ObjectInitializer );

    void Initialize();

    //~ UMovieSceneSequence interface
    virtual void BindPossessableObject( const FGuid& ObjectId, UObject& PossessedObject, UObject* Context ) override;
    virtual bool CanPossessObject( UObject& Object, UObject* InPlaybackContext ) const override;
    virtual bool CanRebindPossessable( const FMovieScenePossessable& InPossessable ) const override;
    virtual void LocateBoundObjects( const FGuid& ObjectId, UObject* Context, TArray<UObject*, TInlineAllocator<1>>& OutObjects ) const override;
    virtual UMovieScene* GetMovieScene() const override;
    virtual UObject* GetParentObject( UObject* Object ) const override;
    virtual void UnbindPossessableObjects( const FGuid& ObjectId ) override;
    virtual void UnbindObjects( const FGuid& ObjectId, const TArray<UObject*>& InObjects, UObject* Context ) override;
    virtual void UnbindInvalidObjects( const FGuid& ObjectId, UObject* Context ) override;

#if WITH_EDITOR
    virtual ETrackSupport IsTrackSupported( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const override;
//    virtual FText GetDisplayName() const override;
//
    virtual void GetAssetRegistryTagMetadata( TMap<FName, FAssetRegistryTagMetadata>& OutMetadata ) const override;
    virtual void GetAssetRegistryTags( TArray<FAssetRegistryTag>& OutTags ) const override;
#endif

public:

    UPROPERTY()
    UMovieScene* MovieScene;

    //--- Camera

    UPROPERTY()
    FGuid CameraBindingId;

    UPROPERTY()
    FLevelSequenceBindingReference CameraBindingReference;
    
    //--- Planes

	UPROPERTY()
	TMap< FGuid, FLevelSequenceBindingReference > BindingIdToReferences;
};
