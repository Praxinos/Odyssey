// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

class ACineCameraActor;
class FString;
class UMaterialInstanceConstant;
class UMaterialInterface;
class UMovieSceneSequence;
class UTexture2D;

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
};

/*
 * The management of project assets
 */
class EPOSTRACKSEDITOR_API ProjectAssetTools
{
public:
    static UMaterialInstanceConstant* CreateMaterialAndTexture( UMovieSceneSequence* iSequence, ACineCameraActor* iCamera, UMovieSceneSequence* iRootSequence );

private:
    static UMaterialInstanceConstant* CreateMaterial( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName );
    static UTexture2D* CreateTexture2D( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInterface* iMaterial, FIntPoint iTextureSize, FString& oPackageName, FString& oAssetName );

    static FIntPoint ComputeTextureSize( ACineCameraActor* iCamera );
};
