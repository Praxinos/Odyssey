// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "EposMovieSceneSequence.h"
#include "MovieScene.h"
#include "UObject/SoftObjectPtr.h"

#include "LevelSequenceBindingReference.h"
#include "Shot/ShotSequenceBindingReference.h"
#include "SequenceNameElements.h"

#include "ShotSequence.generated.h"

class UNamingFormatter;

/**
 * Movie scene animation that represents the last level of the storyboard.
 */
UCLASS( BlueprintType )
class EPOSSEQUENCE_API UShotSequence
    : public UEposMovieSceneSequence
{
public:
    GENERATED_BODY()

    UShotSequence( const FObjectInitializer& ObjectInitializer );

    void Initialize( FFrameRate iTickRate, FFrameRate iDisplayRate );

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

    virtual const FMovieSceneBindingReferences* GetBindingReferences() const override;

    virtual void PostLoad() override;

#if WITH_EDITOR
    virtual ETrackSupport IsTrackSupportedImpl( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const override;
    virtual FText GetDisplayName() const override;
//
    virtual void GetAssetRegistryTagMetadata( TMap<FName, FAssetRegistryTagMetadata>& OutMetadata ) const override;
    virtual void GetAssetRegistryTags( FAssetRegistryTagsContext ioContext ) const override;
#endif

    //~ UEposMovieSceneSequence interface
    virtual bool IsResizable() const override;
    virtual void Resize( int32 iNewDuration ) override;

    virtual void SectionResized( UMovieSceneSection* iSection ) override;
    virtual void SectionAddedOrRemoved( UMovieSceneSection* iSection ) override;

public:
    UPROPERTY()
    TObjectPtr<UMovieScene> MovieScene;

    // The list should only contain one root cinecamera actor and its multiple components
    UPROPERTY()
    FShotSequenceBindingReferences CameraBindingReferences;
    UPROPERTY()
    FShotSequenceBindingReferences PlanesBindingReferences;
    UPROPERTY()
    FShotSequenceBindingReferences ActorsBindingReferences;

    UPROPERTY()
    TMap< FGuid, FLevelSequenceBindingReference > CameraBindingIdToReferences_DEPRECATED;
    UPROPERTY()
    TMap< FGuid, FLevelSequenceBindingReference > PlanesBindingIdToReferences_DEPRECATED;
    UPROPERTY()
    TMap< FGuid, FLevelSequenceBindingReference > ActorsBindingIdToReferences_DEPRECATED;

    UPROPERTY(EditAnywhere, Category=NamingConvention)
    FShotNameElements NameElements;

private:
    UNamingFormatter* mNamingFormatter;
};
