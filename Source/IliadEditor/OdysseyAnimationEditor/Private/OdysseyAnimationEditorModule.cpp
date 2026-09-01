// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyAnimationEditorModule.h"

#include "AssetToolsModule.h"
#include "PropertyEditorModule.h"
#include "Subsystems/PlacementSubsystem.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationActorFactory.h"
#include "OdysseyAnimationAssetTypeActions.h"
#include "OdysseyAnimationComponent.h"
#include "OdysseyAnimationMaterialSelectionDialog.h"
#include "OdysseyAnimationSettings.h"
#include "OdysseyAnimationSettingsCustomization.h"
#include "Materials/Material.h"
#include "Misc/CoreDelegates.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

/*-----------------------------------------------------------------------------
   FOdysseyAnimationEditorModule
-----------------------------------------------------------------------------*/

void
FOdysseyAnimationEditorModule::StartupModule()
{
    RegisterAssetTypeActions();

    RegisterPlacementFactories();

    RegisterPropertyCustomizations();

    FEditorDelegates::OnNewActorsDropped.AddRaw(
        this,
        &FOdysseyAnimationEditorModule::OnNewActorsDropped
    );
}

void
FOdysseyAnimationEditorModule::ShutdownModule()
{
    // Unregister Assets Type Actions
    UnregisterPropertyCustomizations();

    UnregisterPlacementFactories();

    UnregisterAssetTypeActions();

    FEditorDelegates::OnNewActorsDropped.RemoveAll(this);
}

void
FOdysseyAnimationEditorModule::RegisterAssetTypeActions()
{
    IAssetTools& assetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();

    // Create Asset Categories
    EAssetTypeCategories::Type category = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("Odyssey")), LOCTEXT("asset-category.name", "Odyssey"));

    category = EAssetTypeCategories::Type( EAssetTypeCategories::Animation | category );

    //Create Asset Types Actions
    mOdysseyTypeActions = MakeShareable(new FOdysseyAnimationAssetTypeActions(category));

    //Register created Asset Type Actions
    assetTools.RegisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

void
FOdysseyAnimationEditorModule::UnregisterAssetTypeActions()
{
    if (!FModuleManager::Get().IsModuleLoaded("AssetTools"))
        return;

    IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
    assetTools.UnregisterAssetTypeActions(mOdysseyTypeActions.ToSharedRef());
}

void
FOdysseyAnimationEditorModule::RegisterPlacementFactories()
{
    // This assumes that this delegate is called AFTER the one registered in UPlacementSubsystem::Initialize()
    FCoreDelegates::GetOnPostEngineInit().AddRaw( this, &FOdysseyAnimationEditorModule::RegisterFactoryDelayed );
}

void
FOdysseyAnimationEditorModule::UnregisterPlacementFactories()
{
    FCoreDelegates::GetOnPostEngineInit().RemoveAll( this );

    //check( GEditor );
    //UPlacementSubsystem* placementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
    //placementSubsystem->OnPlacementFactoriesRegistered().RemoveAll( this );
}

void
FOdysseyAnimationEditorModule::RegisterPropertyCustomizations()
{
    // import the PropertyEditor module...
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>( "PropertyEditor" );

    // to register our custom property
    PropertyModule.RegisterCustomPropertyTypeLayout(
        // This is the name of the Struct
        // this tells the property editor which is the struct property our customization will applied on.
        FOdysseyAnimationSettings::StaticStruct()->GetFName(),
        // this is where our MakeInstance() method is usefull
        FOnGetPropertyTypeCustomizationInstance::CreateStatic( &FOdysseyAnimationSettingsCustomization::MakeInstance ) );
}

void
FOdysseyAnimationEditorModule::UnregisterPropertyCustomizations()
{
    if( FModuleManager::Get().IsModuleLoaded( "PropertyEditor" ) )
    {
        FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>( "PropertyEditor" );
        PropertyModule.UnregisterCustomPropertyTypeLayout( FOdysseyAnimationSettings::StaticStruct()->GetFName() );

        PropertyModule.NotifyCustomizationModuleChanged();
    }
}

void FOdysseyAnimationEditorModule::OnNewActorsDropped(
    const TArray<UObject*>& DroppedObjects,
    const TArray<AActor*>& NewActors)
{
    const UOdysseyAnimationDialogSettings* Settings = GetDefault<UOdysseyAnimationDialogSettings>();

    TArray<AOdysseyAnimationActor*> AnimationActors;

    for (AActor* Actor : NewActors)
    {
        if (!Actor || !Actor->IsA<AOdysseyAnimationActor>() || Actor->HasAnyFlags(RF_Transient))
        {
            continue;
        }
        AOdysseyAnimationActor* AnimationActor = Cast<AOdysseyAnimationActor>(Actor);
        AnimationActors.Add(AnimationActor);
    }

    if( AnimationActors.Num() != 0 )
    {
        if (Settings->bApplyMaterialWithoutAsking && !Settings->DefaultMaterial.IsNull())
        {
            UMaterialInterface* Material = Settings->DefaultMaterial.LoadSynchronous();
            if (Material)
            {
                for(AOdysseyAnimationActor* AnimationActor : AnimationActors)
                    AnimationActor->GetAnimationComponent()->SetAnimationMaterial(Material);
            }
        }
        else
        {
            const FString DuplicatePackagePath = FPackageName::GetLongPackagePath(AnimationActors[0]->GetAnimationComponent()->GetAnimation()->GetOutermost()->GetName());
            TObjectPtr<UMaterialInterface> MaterialSelection = FMaterialSelectionDialog::Show(DuplicatePackagePath);
            for (AOdysseyAnimationActor* AnimationActor : AnimationActors)
                AnimationActor->GetAnimationComponent()->SetAnimationMaterial(MaterialSelection);
        }
    }
}

void
FOdysseyAnimationEditorModule::RegisterFactoryDelayed()
{
    // It's not possible to use the first delegate FCoreDelegates::OnPostEngineInit and then add a new delegate on placementSubsystem->OnPlacementFactoriesRegistered()
    // because PlacementFactoriesRegistered is called during OnPostEngineInit ( UPlacementSubsystem::Initialize() -> OnPostEngineInit -> UPlacementSubsystem::RegisterPlacementFactories() -> PlacementFactoriesRegistered.Broadcast() )
    // so placementSubsystem->OnPlacementFactoriesRegistered() is always called BEFORE our own further registration
    // ( furthermore, GEditor is not available in FOdysseyAnimationEditorModule::StartupModule in case we want to direclty called FOdysseyAnimationEditorModule::RegisterFactoryDelayed() (without indirectly attach to FCoreDelegates::OnPostEngineInit )
    //check( GEditor );
    //UPlacementSubsystem* placementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
    //placementSubsystem->OnPlacementFactoriesRegistered().AddRaw( this, &FOdysseyAnimationEditorModule::ReorderFactories );

    // So we call directly ReorderFactories() and still assume that our delegate in FCoreDelegates::OnPostEngineInit() is called after the one in UPlacementSubsystem::Initialize()
    ReorderFactories();
}

void
FOdysseyAnimationEditorModule::ReorderFactories()
{
    UPlacementSubsystem* placementSubsystem = GEditor->GetEditorSubsystem<UPlacementSubsystem>();
    if ( !placementSubsystem )
        return;

    TArray<TScriptInterface<IAssetFactoryInterface>> factories_to_move;

    UOdysseyAnimation* animation = NewObject<UOdysseyAnimation>( GetTransientPackage(), UOdysseyAnimation::StaticClass(), FName( TEXT( "Transient_Animation" ) ), RF_Transient );
    FAssetData asset_data( animation, false );

    // Find all the factories which can create UOdysseyAnimationActor
    // Unregister all the ones which are not UOdysseyAnimationActorFactory
    // Then register them again but at the end
    // So the first registered factory which can create UOdysseyAnimationActor is the UOdysseyAnimationActorFactory
    while( true )
    {
        TScriptInterface<IAssetFactoryInterface> factory = placementSubsystem->FindAssetFactoryFromAssetData( asset_data );
        UActorFactory* actorFactory = Cast<UActorFactory>( factory.GetObject() );
        if( !actorFactory )
            break;

        if( actorFactory->IsA<UOdysseyAnimationActorFactory>() )
            break;

        factories_to_move.Add( factory );
        placementSubsystem->UnregisterAssetFactory( factory );
    }

    for( TScriptInterface<IAssetFactoryInterface> factory_to_move : factories_to_move )
        placementSubsystem->RegisterAssetFactory( factory_to_move );
}

IMPLEMENT_MODULE( FOdysseyAnimationEditorModule, OdysseyAnimationEditor );

#undef LOCTEXT_NAMESPACE
