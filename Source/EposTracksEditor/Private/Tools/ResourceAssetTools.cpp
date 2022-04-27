// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/ResourceAssetTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "EditorAssetLibrary.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "IMovieScenePlayer.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneSequence.h"
#include "ObjectTools.h"

#include "NamingConvention.h"
#include "PlaneActor.h"
#include "StoryNote.h"
#include "Tools/EposSequenceTools.h"

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
bool
MasterAssetTools::GetBackgroundVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    float use_background = UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "UseBackgroundColor" );

    return use_background >= 0.5f;
}
//static
void
MasterAssetTools::SetBackgroundVisilibity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, bool iBackgroundVisibility )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseBackgroundColor", iBackgroundVisibility ? 1.f : 0.f );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}
//static
void
MasterAssetTools::ToggleBackgroundVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    SetBackgroundVisilibity( iPlayer, iRootSequence, !GetBackgroundVisibility( iPlayer, iRootSequence ) );
}

//static
FLinearColor
MasterAssetTools::GetBackgroundColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    FLinearColor background_color = UMaterialEditingLibrary::GetMaterialInstanceVectorParameterValue( material, "BackgroundColor" );

    return background_color;
}
//static
void
MasterAssetTools::SetBackgroundColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, FLinearColor iBackgroundColor )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue( material, "BackgroundColor", iBackgroundColor );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseBackgroundColor", 1.f ); // Automatically use background if changing its color
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//-

//static
bool
MasterAssetTools::GetGridVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    float use_grid = UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "UseGrid" );

    return use_grid >= 0.5f;
}
//static
void
MasterAssetTools::SetGridVisilibity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, bool iGridVisibility )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseGrid", iGridVisibility ? 1.f : 0.f );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}
//static
void
MasterAssetTools::ToggleGridVisibility( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    SetGridVisilibity( iPlayer, iRootSequence, !GetGridVisibility( iPlayer, iRootSequence ) );
}

//static
FLinearColor
MasterAssetTools::GetGridColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    FLinearColor grid_color = UMaterialEditingLibrary::GetMaterialInstanceVectorParameterValue( material, "GridColor" );

    return grid_color;
}
//static
void
MasterAssetTools::SetGridColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, FLinearColor iGridColor )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );
    UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue( material, "GridColor", iGridColor );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseGrid", 1.f ); // Automatically use grid if changing its color
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//static
EGridType
MasterAssetTools::GetGridType( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    int32 grid_type = int32( UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "GridType" ) );
    grid_type = FMath::Clamp( grid_type, 0, 4 );

    return static_cast<EGridType>( grid_type );
}
//static
void
MasterAssetTools::SetGridType( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, EGridType iGridType )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    int32 grid_type = static_cast<int32>( iGridType );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "GridType", grid_type );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseGrid", 1.f ); // Automatically use grid if changing its type
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//---

//static
FLinearColor
MasterAssetTools::GetPreviousDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    FLinearColor previous_drawing_color = UMaterialEditingLibrary::GetMaterialInstanceVectorParameterValue( material, "PreviousDrawingColor" );

    return previous_drawing_color;
}

//static
void
MasterAssetTools::SetPreviousDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, FLinearColor iColor )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue( material, "PreviousDrawingColor", iColor.CopyWithNewOpacity( 1.f ) );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//static
float
MasterAssetTools::GetPreviousDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    float previous_drawing_opacity = UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "PreviousDrawingOpacity" );

    return previous_drawing_opacity;
}

//static
void
MasterAssetTools::SetPreviousDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, float iOpacity )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "PreviousDrawingOpacity", iOpacity );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//-

//static
FLinearColor
MasterAssetTools::GetNextDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    FLinearColor next_drawing_color = UMaterialEditingLibrary::GetMaterialInstanceVectorParameterValue( material, "NextDrawingColor" );

    return next_drawing_color;
}

//static
void
MasterAssetTools::SetNextDrawingColor( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, FLinearColor iColor )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue( material, "NextDrawingColor", iColor.CopyWithNewOpacity( 1.f ) );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//static
float
MasterAssetTools::GetNextDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    float next_drawing_opacity = UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "NextDrawingOpacity" );

    return next_drawing_opacity;
}

//static
void
MasterAssetTools::SetNextDrawingOpacity( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, float iOpacity )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iPlayer, iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "NextDrawingOpacity", iOpacity );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//---

//static
UTexture2D*
MasterAssetTools::GetMasterTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    FString texture_path;
    FString texture_name;
    FString texture_pathname = NamingConvention::GetMasterTexturePathName( iPlayer, iRootSequence, texture_path, texture_name );

    //---

    TArray<FAssetData> asset_datas;
    assetRegistryModule.Get().GetAssetsByPackageName( *texture_pathname, asset_datas );
    if( !asset_datas.Num() )
    {
        assetToolsModule.Get().CreateUniqueAssetName( texture_pathname, "", oPackageName, oAssetName );
        //FString package_path = FPackageName::GetLongPackagePath( oPackageName );
        return nullptr;
    }

    oPackageName = asset_datas[0].PackageName.ToString();
    oAssetName = asset_datas[0].AssetName.ToString();
    return Cast<UTexture2D>( asset_datas[0].GetAsset() );
}

//static
UTexture2D*
MasterAssetTools::GetMasterTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    return GetMasterTexture2D( iPlayer, iRootSequence, package_name, asset_name );
}

//static
UTexture2D*
MasterAssetTools::CreateMasterTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    UTexture2D* texture_master = GetMasterTexture2D( iPlayer, iRootSequence, oPackageName, oAssetName );
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
MasterAssetTools::GetMasterMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, FString& oPackageName, FString& oAssetName )
{
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>( TEXT( "AssetRegistry" ) );

    FString material_path;
    FString material_name;
    FString material_pathname = NamingConvention::GetMasterMaterialPathName( iPlayer, iRootSequence, material_path, material_name );

    //---

    TArray<FAssetData> asset_datas;
    assetRegistryModule.Get().GetAssetsByPackageName( *material_pathname, asset_datas );
    if( !asset_datas.Num() )
    {
        assetToolsModule.Get().CreateUniqueAssetName( material_pathname, "", oPackageName, oAssetName );
        //FString package_path = FPackageName::GetLongPackagePath( oPackageName );
        return nullptr;
    }

    oPackageName = asset_datas[0].PackageName.ToString();
    oAssetName = asset_datas[0].AssetName.ToString();

    return Cast<UMaterialInstanceConstant>( asset_datas[0].GetAsset() );
}

//static
UMaterialInstanceConstant*
MasterAssetTools::GetMasterMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    return GetMasterMaterial( iPlayer, iRootSequence, package_name, asset_name );
}

//static
UMaterialInstanceConstant*
MasterAssetTools::CreateMasterMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UTexture2D* iDefaultTexture, FString& oPackageName, FString& oAssetName )
{
    UMaterialInstanceConstant* material_master = GetMasterMaterial( iPlayer, iRootSequence, oPackageName, oAssetName );
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
UTexture2D*
ProjectAssetTools::GetTexture2D( UMovieSceneSequence* iSequence, UMaterialInstance* iMaterial )
{
    //TODO: iSequence may be null, check it if it must or not be the case

    if( !iMaterial )
        return nullptr;

    UTexture* texture;
    iMaterial->GetTextureParameterValue( TEXT( "DrawingTexture" ), texture );

    return Cast<UTexture2D>( texture );
}

//---

//static
UMaterialInstanceConstant*
ProjectAssetTools::CreateMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, FString& oPackageName, FString& oAssetName )
{
    FString package_name_tmp;
    FString asset_name_tmp;
    UTexture2D* master_texture = MasterAssetTools::CreateMasterTexture2D( iPlayer, iRootSequence, package_name_tmp, asset_name_tmp );
    if( !master_texture )
        return nullptr;

    UMaterialInstanceConstant* master_material = MasterAssetTools::CreateMasterMaterial( iPlayer, iRootSequence, master_texture, package_name_tmp, asset_name_tmp );
    if( !master_material )
        return nullptr;

    //---

    FString material_path;
    FString material_name;
    FString material_pathname = NamingConvention::GenerateMaterialAssetPathName( iPlayer, iRootSequence, iSequence, material_path, material_name );

    //---

    UMaterialInstanceConstantFactoryNew* factory = NewObject<UMaterialInstanceConstantFactoryNew>();
    factory->InitialParent = master_material;

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    UObject* new_object = assetToolsModule.Get().CreateAsset( material_name, material_path, UMaterialInstanceConstant::StaticClass(), factory );

    oPackageName = material_pathname;
    oAssetName = material_name;

    return Cast<UMaterialInstanceConstant>( new_object );
}

//static
UMaterialInstanceConstant*
ProjectAssetTools::CloneMaterial( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, UMaterialInstance* iMaterialToClone, FString& oPackageName, FString& oAssetName )
{
    FString material_path;
    FString material_name;
    FString material_pathname = NamingConvention::GenerateMaterialAssetPathName( iPlayer, iRootSequence, iSequence, material_path, material_name );

    //---

    UObject* new_object = UEditorAssetLibrary::DuplicateLoadedAsset( iMaterialToClone, material_pathname );

    oPackageName = material_pathname;
    oAssetName = material_name;

    return Cast<UMaterialInstanceConstant>( new_object );
}

//static
UTexture2D*
ProjectAssetTools::CreateTexture2D( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, FIntPoint iTextureSize, FString& oPackageName, FString& oAssetName )
{
    UTexture2D* texture_master = MasterAssetTools::GetMasterTexture2D( iPlayer, iRootSequence ); // The master texture always exists as CreateMaterial() should be called before CreateTexture2D() (as it takes a material parameter)
    if( !texture_master )
        return nullptr;

    FString texture_path;
    FString texture_name;
    FString texture_pathname = NamingConvention::GenerateTextureAssetPathName( iPlayer, iRootSequence, iSequence, iMaterial, texture_path, texture_name );

    //---

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    UObject* new_object = assetToolsModule.Get().CreateAsset( texture_name, texture_path, UTexture2D::StaticClass(), nullptr );

    UTexture2D* new_texture = Cast<UTexture2D>( new_object );

    oPackageName = texture_pathname;
    oAssetName = texture_name;

    //---

    // Init texture like in UTexture2DFactoryNew
    new_texture->Source.Init2DWithMipChain( iTextureSize.X, iTextureSize.Y, TSF_BGRA8 );

    if( new_texture->Source.IsValid() )
    {
        TArray64<uint8> TexturePixels;
        new_texture->Source.GetMipData( TexturePixels, 0 );

        uint8* DestData = new_texture->Source.LockMip( 0 );
        FMemory::Memset( DestData, 0, TexturePixels.Num() * sizeof( uint8 ) );
        new_texture->Source.UnlockMip( 0 );

        new_texture->PostEditChange();
    }

    return new_texture;
}

//static
UTexture*
ProjectAssetTools::CloneTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, UMaterialInterface* iMaterial, UTexture* iTextureToClone, FString& oPackageName, FString& oAssetName )
{
    FString texture_path;
    FString texture_name;
    FString texture_pathname = NamingConvention::GenerateTextureAssetPathName( iPlayer, iRootSequence, iSequence, iMaterial, texture_path, texture_name );

    //---

    UObject* new_object = UEditorAssetLibrary::DuplicateLoadedAsset( iTextureToClone, texture_pathname );

    oPackageName = texture_pathname;
    oAssetName = texture_name;

    return Cast<UTexture>( new_object );
}

//---

//static
UMaterialInstanceConstant*
ProjectAssetTools::CreateMaterialAndTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, FIntPoint iTextureSize )
{
    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CreateMaterial( iPlayer, iRootSequence, iSequence, package_name, asset_name );
    if( !new_material )
        return nullptr;

    UTexture2D* new_texture = CreateTexture2D( iPlayer, iRootSequence, iSequence, new_material, iTextureSize, package_name, asset_name );
    if( !new_texture )
    {
        UEditorAssetLibrary::DeleteLoadedAsset( new_material );
        return nullptr;
    }

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );

    return new_material;
}

//static
UMaterialInstanceConstant*
ProjectAssetTools::CreateMaterialAndTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, UMaterialInstance* iMaterialTemplate )
{
    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CreateMaterial( iPlayer, iRootSequence, iSequence, package_name, asset_name );
    if( !new_material )
        return nullptr;

    UTexture* texture;
    iMaterialTemplate->GetTextureParameterValue( TEXT( "DrawingTexture" ), texture );
    FIntPoint texture_size( texture->GetSurfaceWidth(), texture->GetSurfaceHeight() ); // For UTexture2D, GetSurfaceWidth() returns GetSizeX() which returns an int32, so it should be ok

    UTexture2D* new_texture = CreateTexture2D( iPlayer, iRootSequence, iSequence, new_material, texture_size, package_name, asset_name );
    if( !new_texture )
    {
        UEditorAssetLibrary::DeleteLoadedAsset( new_material );
        return nullptr;
    }

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );

    return new_material;
}

//static
UMaterialInstanceConstant*
ProjectAssetTools::CloneMaterialAndTexture( const IMovieScenePlayer& iPlayer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, UMaterialInstance* iMaterialToClone )
{
    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CloneMaterial( iPlayer, iRootSequence, iSequence, iMaterialToClone, package_name, asset_name );
    if( !new_material )
        return nullptr;

    UTexture* texture_to_clone;
    iMaterialToClone->GetTextureParameterValue( TEXT( "DrawingTexture" ), texture_to_clone );

    UTexture* new_texture = CloneTexture( iPlayer, iRootSequence, iSequence, new_material, texture_to_clone, package_name, asset_name );
    if( !new_texture )
    {
        UEditorAssetLibrary::DeleteLoadedAsset( new_material );
        return nullptr;
    }

    new_material->SetTextureParameterValueEditorOnly( TEXT( "DrawingTexture" ), new_texture );

    return new_material;
}

//---

//static
UStoryNote*
ProjectAssetTools::CreateNote( ISequencer& iSequencer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence )
{
    FString note_path;
    FString note_name;
    FString note_pathname = NamingConvention::GenerateNoteAssetPathName( iSequencer, iRootSequence, iSequence, note_path, note_name );

    //---

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    UObject* new_object = assetToolsModule.Get().CreateAsset( note_name, note_path, UStoryNote::StaticClass(), nullptr );
    UStoryNote* new_note = Cast<UStoryNote>( new_object );
    check( new_note );

    new_note->Text = TEXT( "Write a note here" ); // default text

    return new_note;
}

//static
UStoryNote*
ProjectAssetTools::CloneNote( ISequencer& iSequencer, UMovieSceneSequence* iRootSequence, UMovieSceneSequence* iSequence, UStoryNote* iNoteToClone )
{
    FString note_path;
    FString note_name;
    FString note_pathname = NamingConvention::GenerateNoteAssetPathName( iSequencer, iRootSequence, iSequence, note_path, note_name );

    //---

    UObject* new_object = UEditorAssetLibrary::DuplicateLoadedAsset( iNoteToClone, note_pathname );

    UStoryNote* new_note = Cast<UStoryNote>( new_object );

    return new_note;
}


#undef LOCTEXT_NAMESPACE
