// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneSequenceID.h"

class ACineCameraActor;
class APlaneActor;
class FString;
class ISequencer;
class IMovieScenePlayer;
class UMaterialInstanceConstant;
class UMaterialInterface;
class UMovieSceneSequence;
class UStoryNote;
class UTexture2D;

/** The grid type which must correspond to the value inside the material instance */
enum class EGridType
{
    kNone = 0,
    k2x2,
    k3x3,
    kCrosshair,
    kAbatment,
};

/*
 * The management of master assets (copied from the plugins content directory to the project)
 */
class EPOSTRACKSEDITOR_API MasterAssetTools
{
public:
    static UTexture2D* GetMasterTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPackageName, FString& oAssetName );
    static UTexture2D* GetMasterTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    static UMaterialInstanceConstant* GetMasterMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPackageName, FString& oAssetName );
    static UMaterialInstanceConstant* GetMasterMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

private:
    static UTexture2D* CreateMasterTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPackageName, FString& oAssetName );
    static UMaterialInstanceConstant* CreateMasterMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UTexture2D* iDefaultTexture, FString& oPackageName, FString& oAssetName );

    friend class ProjectAssetTools;

public:
    static bool         GetBackgroundVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetBackgroundVisilibity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iBackgroundVisibility );
    static void         ToggleBackgroundVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    static FLinearColor GetBackgroundColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetBackgroundColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FLinearColor iBackgroundColor );

    static bool         GetGridVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetGridVisilibity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, bool iGridVisibility );
    static void         ToggleGridVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );

    static FLinearColor GetGridColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetGridColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FLinearColor iGridColor );

    static EGridType    GetGridType( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetGridType( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, EGridType iGridType );

public:
    static FLinearColor GetPreviousDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetPreviousDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FLinearColor iColor );

    static float        GetPreviousDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetPreviousDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, float iOpacity );

    static FLinearColor GetNextDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetNextDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FLinearColor iColor );

    static float        GetNextDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static void         SetNextDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, float iOpacity );
};

/*
 * The management of project assets
 */
class EPOSTRACKSEDITOR_API ProjectAssetTools
{
public:
    static UMaterialInstanceConstant* CreateMaterialAndTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FIntPoint iTextureSize );
    static UMaterialInstanceConstant* CreateMaterialAndTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UTexture2D* iTexture );
    static UMaterialInstanceConstant* CreateMaterialAndTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMaterialInstance* iMaterialTemplate );
    static UMaterialInstanceConstant* CloneMaterialAndTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMaterialInstance* iMaterialToClone );

    static UTexture2D* GetTexture2D( UMovieSceneSequence* iSequence, UMaterialInstance* iMaterial );

    static UStoryNote* CreateNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID );
    static UStoryNote* CloneNote( ISequencer& iSequencer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UStoryNote* iNoteToClone );

private:
    static UMaterialInstanceConstant* CreateMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, FString& oPackageName, FString& oAssetName );
    static UMaterialInstanceConstant* CloneMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMaterialInstance* iMaterialToClone, FString& oPackageName, FString& oAssetName );
    static UTexture2D* CreateTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMaterialInterface* iMaterial, FIntPoint iTextureSize, FString& oPackageName, FString& oAssetName );
    static UTexture*   CloneTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iSequence, FMovieSceneSequenceIDRef iSequenceID, UMaterialInterface* iMaterial, UTexture* iTextureToClone, FString& oPackageName, FString& oAssetName );
};
