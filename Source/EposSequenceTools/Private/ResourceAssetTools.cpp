// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "ResourceAssetTools.h"

#include "AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneSequence.h"

#define LOCTEXT_NAMESPACE "ResourceAssetTools"

//---

////static
//void
//MasterAssetTools::CopyPluginToMasterAssets( UMovieSceneSequence* iRootSequence )
//{
//    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
//    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );
//
//    UObject* root_asset = iRootSequence->GetMovieScene()->GetOuter();
//    UPackage* root_package = root_asset->GetPackage();
//    FString master_folder = root_package->GetName() / "Master"; // ie. /Game/MyStoryboard2/Master
//
//    TArray<FAssetData> asset_datas;
//    assetRegistryModule.Get().GetAssetsByPackageName( *( master_folder / "BP_Plane" ), asset_datas );
//    if( asset_datas.Num() )
//        return;
//
//    TArray<FName> sources;
//    sources.Add( "/Epos/BP_Plane" );
//    //for( auto source : sources )
//    //{
//    //    FString SrcFilename;
//    //    UObject* ExistingObject = nullptr;
//    //    if( FPackageName::DoesPackageExist( *source.ToString(), nullptr, &SrcFilename ) )
//    //    {
//    //        UPackage* Pkg = LoadPackage( nullptr, *source.ToString(), LOAD_None ); // To find all the dependencies
//    //        if( Pkg )
//    //        {
//    //            FString Name = ObjectTools::SanitizeObjectName( FPaths::GetBaseFilename( SrcFilename ) );
//    //            ExistingObject = StaticFindObject( UObject::StaticClass(), Pkg, *Name );
//    //        }
//    //    }
//    //}
//
//    //FAdvancedCopyParams copy_params = FAdvancedCopyParams( sources, master_folder );
//    //copy_params.bShouldSuppressUI = true;
//    //copy_params.AddCustomization( UAdvancedCopyCustomization::StaticClass()->GetDefaultObject<UAdvancedCopyCustomization>() );
//
//    //assetToolsModule.Get().InitAdvancedCopyFromCopyParams( copy_params );
//
//    //const bool bPromptUserToSave = true;
//    //const bool bSaveMapPackages = true;
//    //const bool bSaveContentPackages = true;
//    //FEditorFileUtils::SaveDirtyPackages( bPromptUserToSave, bSaveMapPackages, bSaveContentPackages );
//
//    assetToolsModule.Get().BeginAdvancedCopyPackages( sources, master_folder );
//}

//---

//static
UTexture2D*
MasterAssetTools::GetMasterTexture2D( UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    UObject* root_asset = iRootSequence->GetMovieScene()->GetOuter();
    UPackage* root_package = root_asset->GetPackage();
    FString root_package_name = root_package->GetName() / "Master" / "T_Transparent"; // ie. /Game/MyStoryboard2

    TArray<FAssetData> asset_datas;
    assetRegistryModule.Get().GetAssetsByPackageName( *root_package_name, asset_datas );
    if( !asset_datas.Num() )
    {
        assetToolsModule.Get().CreateUniqueAssetName( root_package_name, "", oPackageName, oAssetName );
        //FString package_path = FPackageName::GetLongPackagePath( oPackageName );
        return nullptr;
    }

    oPackageName = asset_datas[0].PackageName.ToString();
    oAssetName = asset_datas[0].AssetName.ToString();
    return Cast<UTexture2D>( asset_datas[0].GetAsset() );
}

//static
UTexture2D*
MasterAssetTools::GetMasterTexture2D( UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    return GetMasterTexture2D( iRootSequence, package_name, asset_name );
}

//static
UTexture2D*
MasterAssetTools::CreateMasterTexture2D( UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    UTexture2D* texture_master = GetMasterTexture2D( iRootSequence, oPackageName, oAssetName );
    if( texture_master )
        return texture_master;

    UTexture2D* texture_root = LoadObject<UTexture2D>( nullptr, TEXT( "/Epos/T_Transparent" ) );
    if( !texture_root )
        return nullptr;

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = assetToolsModule.Get().DuplicateAsset( oAssetName, package_path, texture_root );

    return Cast<UTexture2D>( new_object );
}

//---

//static
UMaterialInstanceConstant*
MasterAssetTools::GetMasterMaterial( UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    UObject* root_asset = iRootSequence->GetMovieScene()->GetOuter();
    UPackage* root_package = root_asset->GetPackage();
    FString root_package_name = root_package->GetName() / "Master" / "MI_Plane"; // ie. /Game/MyStoryboard2

    TArray<FAssetData> asset_datas;
    assetRegistryModule.Get().GetAssetsByPackageName( *root_package_name, asset_datas );
    if( !asset_datas.Num() )
    {
        assetToolsModule.Get().CreateUniqueAssetName( root_package_name, "", oPackageName, oAssetName );
        //FString package_path = FPackageName::GetLongPackagePath( oPackageName );
        return nullptr;
    }

    oPackageName = asset_datas[0].PackageName.ToString();
    oAssetName = asset_datas[0].AssetName.ToString();
    return Cast<UMaterialInstanceConstant>( asset_datas[0].GetAsset() );
}

//static
UMaterialInstanceConstant*
MasterAssetTools::GetMasterMaterial( UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    return GetMasterMaterial( iRootSequence, package_name, asset_name );
}

//static
UMaterialInstanceConstant*
MasterAssetTools::CreateMasterMaterial( UMovieSceneSequence* iRootSequence, UTexture2D* iDefaultTexture, FString& oPackageName, FString& oAssetName )
{
    UMaterialInstanceConstant* material_master = GetMasterMaterial( iRootSequence, oPackageName, oAssetName );
    if( material_master )
        return material_master;

    UMaterial* material_root = LoadObject<UMaterial>( nullptr, TEXT( "/Epos/M_Plane.M_Plane" ) );
    if( !material_root )
        return nullptr;

    UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
    factory->InitialParent = material_root;

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = assetToolsModule.Get().CreateAsset( oAssetName, package_path, UMaterialInstanceConstant::StaticClass(), factory );

    material_master = Cast<UMaterialInstanceConstant>( new_object );

    //---

    material_master->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), iDefaultTexture );
    material_master->SetTextureParameterValueEditorOnly( TEXT( "PreviousDrawingTexture" ), iDefaultTexture );
    material_master->SetTextureParameterValueEditorOnly( TEXT( "NextDrawingTexture" ), iDefaultTexture );

    // For information, as they were replaced by a float parameter to switch between grid or not
    //FStaticParameterSet static_params;
    //material_master->GetStaticParameterValues( static_params );
    //for( auto& parameter : static_params.StaticSwitchParameters )
    //    parameter.bOverride = true;
    //material_master->UpdateStaticPermutation( static_params );

    //// Needed to compute all cases during creation, to have all shaders computed
    //for( int combination = 0; combination < FMath::Pow( 2, static_params.StaticSwitchParameters.Num() ); combination++ )
    //{
    //    //UE_LOG( LogTemp, Warning, TEXT( "combination: %d" ), combination );
    //    for( int i = 0; i < static_params.StaticSwitchParameters.Num(); i++ )
    //    {
    //        static_params.StaticSwitchParameters[i].Value = combination & ( 1 << i );
    //        //UE_LOG( LogTemp, Warning, TEXT( "i: %d - value: %d" ), i, static_params.StaticSwitchParameters[i].Value );
    //        material_master->UpdateStaticPermutation( static_params );
    //    }
    //}

    return material_master;
}

//---
//---
//---

//static
UMaterialInstanceConstant*
ProjectAssetTools::CreateMaterial( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    FString package_name_tmp;
    FString asset_name_tmp;
    UTexture2D* master_texture = MasterAssetTools::CreateMasterTexture2D( iRootSequence, package_name_tmp, asset_name_tmp );
    if( !master_texture )
        return nullptr;

    UMaterialInstanceConstant* master_material = MasterAssetTools::CreateMasterMaterial( iRootSequence, master_texture, package_name_tmp, asset_name_tmp );
    if( !master_material )
        return nullptr;

    //---

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );

    UPackage* package = iSequence->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/shot0001_01

    assetToolsModule.Get().CreateUniqueAssetName( package_name, "_MI_01", oPackageName, oAssetName );

    UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
    factory->InitialParent = master_material;

    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = assetToolsModule.Get().CreateAsset( oAssetName, package_path, UMaterialInstanceConstant::StaticClass(), factory );

    return Cast<UMaterialInstanceConstant>( new_object );
}

//static
UTexture2D*
ProjectAssetTools::CreateTexture2D( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInterface* iMaterial, FString& oPackageName, FString& oAssetName )
{
    UTexture2D* texture_master = MasterAssetTools::GetMasterTexture2D( iRootSequence ); // The master texture always exists as CreateMaterial() should be called before CreateTexture2D() (as it takes a material parameter)
    if( !texture_master )
        return nullptr;

    UPackage* package = iMaterial->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/M_Plane_Basic_Inst

    FAssetToolsModule& Module = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    Module.Get().CreateUniqueAssetName( package_name, "_T_01", oPackageName, oAssetName );

    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = Module.Get().DuplicateAsset( oAssetName, package_path, texture_master );

    return Cast<UTexture2D>( new_object );
}

//---

//static
UMaterialInstanceConstant*
ProjectAssetTools::CreateMaterialAndTexture( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CreateMaterial( iSequence, iRootSequence, package_name, asset_name );
    if( !new_material )
        return nullptr;

    UTexture2D* new_texture = CreateTexture2D( iSequence, iRootSequence, new_material, package_name, asset_name );
    if( !new_texture )
    {
        new_material->MarkPendingKill();
        return nullptr;
    }

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );

    return new_material;
}

#undef LOCTEXT_NAMESPACE
