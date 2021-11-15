// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/ResourceAssetTools.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "CineCameraActor.h"
#include "CineCameraComponent.h"
#include "EditorAssetLibrary.h"
#include "Factories/MaterialInstanceConstantFactoryNew.h"
#include "Factories/Texture2dFactoryNew.h"
#include "MaterialEditingLibrary.h"
#include "Materials/MaterialInstanceConstant.h"
#include "MovieSceneSequence.h"
#include "ObjectTools.h"

#include "Settings/EposTracksEditorSettings.h"
#include "StoryNote.h"

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
bool
MasterAssetTools::GetBackgroundVisibility( UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    float use_background = UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "UseBackgroundColor" );

    return use_background >= 0.5f;
}
//static
void
MasterAssetTools::SetBackgroundVisilibity( UMovieSceneSequence* iRootSequence, bool iBackgroundVisibility )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseBackgroundColor", iBackgroundVisibility ? 1.f : 0.f );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}
//static
void
MasterAssetTools::ToggleBackgroundVisibility( UMovieSceneSequence* iRootSequence )
{
    SetBackgroundVisilibity( iRootSequence, !GetBackgroundVisibility( iRootSequence ) );
}

//static
FLinearColor
MasterAssetTools::GetBackgroundColor( UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    FLinearColor background_color = UMaterialEditingLibrary::GetMaterialInstanceVectorParameterValue( material, "BackgroundColor" );

    return background_color;
}
//static
void
MasterAssetTools::SetBackgroundColor( UMovieSceneSequence* iRootSequence, FLinearColor iBackgroundColor )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue( material, "BackgroundColor", iBackgroundColor );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseBackgroundColor", 1.f ); // Automatically use background if changing its color
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//-

//static
bool
MasterAssetTools::GetGridVisibility( UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    float use_grid = UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "UseGrid" );

    return use_grid >= 0.5f;
}
//static
void
MasterAssetTools::SetGridVisilibity( UMovieSceneSequence* iRootSequence, bool iGridVisibility )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseGrid", iGridVisibility ? 1.f : 0.f );
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}
//static
void
MasterAssetTools::ToggleGridVisibility( UMovieSceneSequence* iRootSequence )
{
    SetGridVisilibity( iRootSequence, !GetGridVisibility( iRootSequence ) );
}

//static
FLinearColor
MasterAssetTools::GetGridColor( UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    FLinearColor grid_color = UMaterialEditingLibrary::GetMaterialInstanceVectorParameterValue( material, "GridColor" );

    return grid_color;
}
//static
void
MasterAssetTools::SetGridColor( UMovieSceneSequence* iRootSequence, FLinearColor iGridColor )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );
    UMaterialEditingLibrary::SetMaterialInstanceVectorParameterValue( material, "GridColor", iGridColor );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseGrid", 1.f ); // Automatically use grid if changing its color
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
}

//static
EGridType
MasterAssetTools::GetGridType( UMovieSceneSequence* iRootSequence )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    int32 grid_type = int32( UMaterialEditingLibrary::GetMaterialInstanceScalarParameterValue( material, "GridType" ) );
    grid_type = FMath::Clamp( grid_type, 0, 4 );

    return static_cast<EGridType>( grid_type );
}
//static
void
MasterAssetTools::SetGridType( UMovieSceneSequence* iRootSequence, EGridType iGridType )
{
    UMaterialInstanceConstant* material = MasterAssetTools::GetMasterMaterial( iRootSequence );

    int32 grid_type = static_cast<int32>( iGridType );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "GridType", grid_type );
    UMaterialEditingLibrary::SetMaterialInstanceScalarParameterValue( material, "UseGrid", 1.f ); // Automatically use grid if changing its type
    UMaterialEditingLibrary::UpdateMaterialInstance( material );
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
UMaterialInstanceConstant*
ProjectAssetTools::CloneMaterial( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInstance* iMaterialToClone, FString& oPackageName, FString& oAssetName )
{
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );

    UPackage* package = iSequence->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/shot0001_01

    assetToolsModule.Get().CreateUniqueAssetName( package_name, "_MI_01", oPackageName, oAssetName );

    UObject* new_object = UEditorAssetLibrary::DuplicateLoadedAsset( iMaterialToClone, oPackageName );

    return Cast<UMaterialInstanceConstant>( new_object );
}

//static
UTexture2D*
ProjectAssetTools::CreateTexture2D( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInterface* iMaterial, FIntPoint iTextureSize, FString& oPackageName, FString& oAssetName )
{
    UTexture2D* texture_master = MasterAssetTools::GetMasterTexture2D( iRootSequence ); // The master texture always exists as CreateMaterial() should be called before CreateTexture2D() (as it takes a material parameter)
    if( !texture_master )
        return nullptr;

    UPackage* package = iMaterial->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/M_Plane_Basic_Inst

    FAssetToolsModule& Module = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    Module.Get().CreateUniqueAssetName( package_name, "_T_01", oPackageName, oAssetName );

    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    FString package_path = FPackageName::GetLongPackagePath( oPackageName );
    UObject* new_object = assetToolsModule.Get().CreateAsset( oAssetName, package_path, UTexture2D::StaticClass(), nullptr );
    UTexture2D* new_texture = Cast<UTexture2D>( new_object );

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
ProjectAssetTools::CloneTexture( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence, UMaterialInterface* iMaterial, UTexture* iTextureToClone, FString& oPackageName, FString& oAssetName )
{
    UPackage* package = iMaterial->GetPackage();
    FString package_name = package->GetName(); // ie. /Game/MyStoryboard2/M_Plane_Basic_Inst

    FAssetToolsModule& Module = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    Module.Get().CreateUniqueAssetName( package_name, "_T_01", oPackageName, oAssetName );

    UObject* new_object = UEditorAssetLibrary::DuplicateLoadedAsset( iTextureToClone, oPackageName );

    UTexture* new_texture = Cast<UTexture>( new_object );

    return new_texture;
}

//---

FIntPoint
ProjectAssetTools::ComputeTextureSize( ACineCameraActor* iCamera )
{
    float camera_ratio = iCamera->GetCineCameraComponent()->AspectRatio;

    const UEposTracksEditorSettings* settings = GetDefault<UEposTracksEditorSettings>();
    int32 height = settings->TextureSettings.Height;

    int32 width = int32( height * camera_ratio );
    if( width % 4 )
        width += ( 4 - width % 4 ); // To always have a multiple of 4 (like the height)

    width = FMath::Clamp( width, 16, 8192 );

    return FIntPoint( width, height );
}

//---

//static
UMaterialInstanceConstant*
ProjectAssetTools::CreateMaterialAndTexture( UMovieSceneSequence* iSequence, ACineCameraActor* iCamera, UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CreateMaterial( iSequence, iRootSequence, package_name, asset_name );
    if( !new_material )
        return nullptr;

    FIntPoint texture_size = ComputeTextureSize( iCamera );

    UTexture2D* new_texture = CreateTexture2D( iSequence, iRootSequence, new_material, texture_size, package_name, asset_name );
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
ProjectAssetTools::CreateMaterialAndTexture( UMovieSceneSequence* iSequence, UMaterialInstance* iMaterialTemplate, UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CreateMaterial( iSequence, iRootSequence, package_name, asset_name );
    if( !new_material )
        return nullptr;

    UTexture* texture;
    iMaterialTemplate->GetTextureParameterValue( TEXT( "DrawingTexture" ), texture );
    FIntPoint texture_size( texture->GetSurfaceWidth(), texture->GetSurfaceHeight() ); // For UTexture2D, GetSurfaceWidth() returns GetSizeX() which returns an int32, so it should be ok

    UTexture2D* new_texture = CreateTexture2D( iSequence, iRootSequence, new_material, texture_size, package_name, asset_name );
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
ProjectAssetTools::CloneMaterialAndTexture( UMovieSceneSequence* iSequence, UMaterialInstance* iMaterialToClone, UMovieSceneSequence* iRootSequence )
{
    FString package_name;
    FString asset_name;
    UMaterialInstanceConstant* new_material = CloneMaterial( iSequence, iRootSequence, iMaterialToClone, package_name, asset_name );
    if( !new_material )
        return nullptr;

    UTexture* texture_to_clone;
    iMaterialToClone->GetTextureParameterValue( TEXT( "DrawingTexture" ), texture_to_clone );

    UTexture* new_texture = CloneTexture( iSequence, iRootSequence, new_material, texture_to_clone, package_name, asset_name );
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
ProjectAssetTools::CreateNote( UMovieSceneSequence* iSequence, UMovieSceneSequence* iRootSequence )
{
    FAssetToolsModule& assetToolsModule = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );

    UPackage* package = iRootSequence->GetPackage();
    FString package_pathname = package->GetName(); // ie. /Game/MyStoryboard2
    FString package_name = FPaths::GetBaseFilename( iSequence->GetPackage()->GetName() ); // ie. shot_0002_01

    FString note_package_name;
    FString note_asset_name;
    assetToolsModule.Get().CreateUniqueAssetName( FPaths::Combine( package_pathname, TEXT( "Notes" ), package_name ), "_N_01", note_package_name, note_asset_name );

    FString package_path = FPackageName::GetLongPackagePath( note_package_name );
    UObject* new_object = assetToolsModule.Get().CreateAsset( note_asset_name, package_path, UStoryNote::StaticClass(), nullptr );
    UStoryNote* new_note = Cast<UStoryNote>( new_object );
    check( new_note );

    new_note->Text = TEXT( "Write a note here" ); // default text

    return new_note;
}

//static
UStoryNote*
ProjectAssetTools::CloneNote( UMovieSceneSequence* iSequence, UStoryNote* iNoteToClone, UMovieSceneSequence* iRootSequence )
{
    UPackage* package = iRootSequence->GetPackage();
    FString package_pathname = package->GetName(); // ie. /Game/MyStoryboard2
    FString package_name = FPaths::GetBaseFilename( iSequence->GetPackage()->GetName() ); // ie. shot_0002_01

    FString note_package_name;
    FString note_asset_name;
    FAssetToolsModule& Module = FModuleManager::GetModuleChecked<FAssetToolsModule>( "AssetTools" );
    Module.Get().CreateUniqueAssetName( FPaths::Combine( package_pathname, TEXT( "Notes" ), package_name ), "_N_01", note_package_name, note_asset_name );

    UObject* new_object = UEditorAssetLibrary::DuplicateLoadedAsset( iNoteToClone, note_package_name );

    UStoryNote* new_note = Cast<UStoryNote>( new_object );

    return new_note;
}


#undef LOCTEXT_NAMESPACE
