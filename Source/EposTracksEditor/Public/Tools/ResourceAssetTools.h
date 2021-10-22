// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class ACineCameraActor;
class FString;
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
    static UTexture2D* GetMasterTexture2D( UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName );
    static UTexture2D* GetMasterTexture2D( UMovieSceneSequence* iRootSequence );

    static UMaterialInstanceConstant* GetMasterMaterial( UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName );
    static UMaterialInstanceConstant* GetMasterMaterial( UMovieSceneSequence* iRootSequence );

private:
    static UTexture2D* CreateMasterTexture2D( UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName );
    static UMaterialInstanceConstant* CreateMasterMaterial( UMovieSceneSequence* iRootSequence, UTexture2D* iDefaultTexture, FString& oPackageName, FString& oAssetName );

    friend class ProjectAssetTools;

public:
    static bool         GetBackgroundVisibility( UMovieSceneSequence* iRootSequence );
    static void         SetBackgroundVisilibity( UMovieSceneSequence* iRootSequence, bool iBackgroundVisibility );
    static void         ToggleBackgroundVisibility( UMovieSceneSequence* iRootSequence );

    static FLinearColor GetBackgroundColor( UMovieSceneSequence* iRootSequence );
    static void         SetBackgroundColor( UMovieSceneSequence* iRootSequence, FLinearColor iBackgroundColor );

    static bool         GetGridVisibility( UMovieSceneSequence* iRootSequence );
    static void         SetGridVisilibity( UMovieSceneSequence* iRootSequence, bool iGridVisibility );
    static void         ToggleGridVisibility( UMovieSceneSequence* iRootSequence );

    static FLinearColor GetGridColor( UMovieSceneSequence* iRootSequence );
    static void         SetGridColor( UMovieSceneSequence* iRootSequence, FLinearColor iGridColor );

    static EGridType    GetGridType( UMovieSceneSequence* iRootSequence );
    static void         SetGridType( UMovieSceneSequence* iRootSequence, EGridType iGridType );
};

/*
 * The management of project assets
 */
class EPOSTRACKSEDITOR_API ProjectAssetTools
{
public:
    static UMaterialInstanceConstant* CreateMaterialAndTexture( UMovieSceneSequence* iSequence, ACineCameraActor* iCamera, UMovieSceneSequence* iRootSequence );
    static UMaterialInstanceConstant* CreateMaterialAndTexture( UMovieSceneSequence* iSequence, UMaterialInstance* iMaterialTemplate, UMovieSceneSequence* iRootSequence );
    static UMaterialInstanceConstant* CloneMaterialAndTexture( UMovieSceneSequence* iSequence, UMaterialInstance* iMaterialToClone, UMovieSceneSequence* iRootSequence );

    static UTexture2D* GetTexture2D( UMovieSceneSequence* iSequence, UMaterialInstance* iMaterial );

    static UStoryNote* CreateNote( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence );
    static UStoryNote* CloneNote( UMovieSceneSequence* iSequence, UStoryNote* iNoteToClone, UMovieSceneSequence* iRootSequence );

private:
    static UMaterialInstanceConstant* CreateMaterial( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName );
    static UMaterialInstanceConstant* CloneMaterial( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInstance* iMaterialToClone, FString& oPackageName, FString& oAssetName );
    static UTexture2D* CreateTexture2D( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInterface* iMaterial, FIntPoint iTextureSize, FString& oPackageName, FString& oAssetName );
    static UTexture*   CloneTexture( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInterface* iMaterial, UTexture* iTextureToClone, FString& oPackageName, FString& oAssetName );

    static FIntPoint ComputeTextureSize( ACineCameraActor* iCamera );
};
