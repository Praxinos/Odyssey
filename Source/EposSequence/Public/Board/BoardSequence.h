// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "EposMovieSceneSequence.h"
#include "LevelSequenceBindingReference.h"
#include "MovieScene.h"
#include "UObject/SoftObjectPtr.h"

#include "BoardSequence.generated.h"

class UNamingFormatter;

UCLASS()
 class EPOSSEQUENCE_API UBoardAssetUserData
    : public UAssetUserData
 {
    GENERATED_BODY()

public:
    /** The studio name. */
    UPROPERTY() //TODO: maybe use UPROPERTY( config, EditAnywhere, Category = Global ) to be editable in the UI ?
    FString StudioName { TEXT( "MyStudio" ) };

    /** The studio accronym. */
    UPROPERTY()
    FString StudioAccronym { TEXT( "MS" ) };

    /** The title of the production. */
    UPROPERTY()
    FString ProductionName { TEXT( "MyProductionTitle" ) };

    /** The accronym of the production. */
    UPROPERTY()
    FString ProductionAccronym { TEXT( "MPT" ) };

    /** The initials of the user. */
    UPROPERTY()
    FString Initials { TEXT( "MI" ) };

    /** The current index. */
    UPROPERTY()
    int32 Index { 0 };
 };

/*
 * Movie scene animation that represents the hierarchical levels of the storyboard.
 */
UCLASS( BlueprintType )
class EPOSSEQUENCE_API UBoardSequence
    : public UEposMovieSceneSequence
    , public IInterface_AssetUserData
{
public:
    GENERATED_BODY()

    UBoardSequence( const FObjectInitializer& ObjectInitializer );

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

#if WITH_EDITOR
    virtual ETrackSupport IsTrackSupported( TSubclassOf<class UMovieSceneTrack> InTrackClass ) const override;
    virtual FText GetDisplayName() const override;

//    virtual void GetAssetRegistryTagMetadata( TMap<FName, FAssetRegistryTagMetadata>& OutMetadata ) const override;
//    virtual void GetAssetRegistryTags( TArray<FAssetRegistryTag>& OutTags ) const override;
#endif

    //~ IInterface_AssetUserData interface
    virtual void AddAssetUserData( UAssetUserData* iUserData );
    virtual UAssetUserData* GetAssetUserDataOfClass( TSubclassOf<UAssetUserData> iUserDataClass );
    virtual const TArray<UAssetUserData*>* GetAssetUserDataArray() const;
    virtual void RemoveUserDataOfClass( TSubclassOf<UAssetUserData> iUserDataClass );

    //~ UEposMovieSceneSequence interface
    virtual bool IsResizable() const override;
    virtual void Resize( int32 iNewDuration ) override;

    virtual void SectionResized( UMovieSceneSection* iSection ) override;
    virtual void SectionAddedOrRemoved( UMovieSceneSection* iSection ) override;

public:
    UPROPERTY()
    UMovieScene* MovieScene;

    // The map should contain only one root cinecamera actor and its multiple components
    // Only one entry should have an invalid guid
    UPROPERTY()
    TMap< FGuid, FLevelSequenceBindingReference > ActorsBindingIdToReferences;

private:
    UNamingFormatter* mNamingFormatter;

    TArray<UAssetUserData*> mAssetUserData;
};
