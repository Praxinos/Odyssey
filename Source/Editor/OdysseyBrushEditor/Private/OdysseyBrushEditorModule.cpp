// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "OdysseyBrushEditorModule.h"

#include "OdysseyBrushDebugger.h"
#include "Editor.h"
#include "Modules/ModuleManager.h"
#include "EditorUndoClient.h"
#include "Logging/TokenizedMessage.h"
#include "Misc/ConfigCacheIni.h"
#include "UObject/UObjectHash.h"
#include "Serialization/ArchiveReplaceObjectRef.h"
#include "OdysseyBrushEditor.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "Kismet2/KismetDebugUtilities.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Editor/LevelEditor/Public/LevelEditor.h"
#include "OdysseyUserDefinedEnumEditor.h"
#include "MessageLogInitializationOptions.h"
#include "IMessageLogListing.h"
#include "Developer/MessageLog/Public/MessageLogModule.h"
#include "Misc/UObjectToken.h"
#include "OdysseyInstancedStaticMeshSCSEditorCustomization.h"
#include "InstancedReferenceSubobjectHelper.h"
#include "ISettingsModule.h"
#include "OdysseyUserDefinedStructureEditor.h"
#include "EdGraphUtilities.h"
#include "BlueprintGraphPanelPinFactory.h"
#include "OdysseyWatchPointViewer.h"
#include "KismetCompiler.h"

#include "AssetToolsModule.h"
#include "AssetTypeActions_OdysseyBrush.h"
#include "EdGraphSchema_OdysseyBrush.h"
#include "Framework/Docking/TabManager.h"
#include "K2Node_CallFunction.h"
#include "Modules/ModuleManager.h"
#include "OdysseyBrushBlueprint.h"
#include "OdysseyBrushThumbnailRenderer.h"
#include "PropertyEditorModule.h"
#include "ThumbnailRendering/ThumbnailManager.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/Docking/SDockTab.h"
#include "WorkspaceMenuStructure.h"
#include "WorkspaceMenuStructureModule.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushEditorModule"

//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushEditorModule

TSharedRef<FExtender> ExtendLevelViewportContextMenuForBlueprints(const TSharedRef<FUICommandList> CommandList, TArray<AActor*> SelectedActors);

FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors LevelViewportContextMenUBlueprintExtender;

static void FocusOdysseyBrushEditorOnObject(const TSharedRef<IMessageToken>& Token)
{
    if( Token->GetType() == EMessageToken::Object )
    {
        const TSharedRef<FUObjectToken> UObjectToken = StaticCastSharedRef<FUObjectToken>(Token);
        if(UObjectToken->GetObject().IsValid())
        {
            FKismetEditorUtilities::BringKismetToFocusAttentionOnObject(UObjectToken->GetObject().Get());
        }
    }
}

struct FOdysseyBrushUndoRedoHandler : public FEditorUndoClient
{
    virtual void PostUndo(bool bSuccess) override;
    virtual void PostRedo(bool bSuccess) override;
};
static FOdysseyBrushUndoRedoHandler* UndoRedoHandler = nullptr;

void FixSubObjectReferencesPostUndoRedo(UObject* InObject)
{
    // Post undo/redo, these may have the correct Outer but are not referenced by the CDO's UProperties
    TArray<UObject*> SubObjects;
    GetObjectsWithOuter(InObject, SubObjects, false);

    // Post undo/redo, these may have the in-correct Outer but are incorrectly referenced by the CDO's UProperties
    TSet<FInstancedSubObjRef> PropertySubObjectReferences;
    UClass* ObjectClass = InObject->GetClass();
    FFindInstancedReferenceSubobjectHelper::GetInstancedSubObjects(InObject, PropertySubObjectReferences);

    TMap<UObject*, UObject*> OldToNewInstanceMap;
    for (UObject* PropertySubObject : PropertySubObjectReferences)
    {
        bool bFoundMatchingSubObject = false;
        for (UObject* SubObject : SubObjects)
        {
            // The property and sub-objects should have the same name.
            if (PropertySubObject->GetFName() == SubObject->GetFName())
            {
                // We found a matching property, we do not want to re-make the property
                bFoundMatchingSubObject = true;

                // Check if the properties have different outers so we can map old-to-new
                if (PropertySubObject->GetOuter() != InObject)
                {
                    OldToNewInstanceMap.Add(PropertySubObject, SubObject);
                }
                // Recurse on the SubObject to correct any sub-object/property references
                FixSubObjectReferencesPostUndoRedo(SubObject);
                break;
            }
        }

        // If the property referenced does not exist in the current context as a subobject, we need to duplicate it and fix up references
        // This will occur during post-undo/redo of deletions
        if (!bFoundMatchingSubObject)
        {
            UObject* NewSubObject = DuplicateObject(PropertySubObject, InObject, PropertySubObject->GetFName());

            // Don't forget to fix up all references and sub-object references
            OldToNewInstanceMap.Add(PropertySubObject, NewSubObject);
        }
    }

    FArchiveReplaceObjectRef<UObject> Replacer(InObject, OldToNewInstanceMap, false, false, false, false);
}

void FixSubObjectReferencesPostUndoRedo(const FTransaction* Transaction)
{
    TArray<UBlueprint*> ModifiedBlueprints;

    // Look at the transaction this function is responding to, see if any object in it has an outermost of the OdysseyBrush
    if (Transaction != nullptr)
    {
        TArray<UObject*> TransactionObjects;
        Transaction->GetTransactionObjects(TransactionObjects);
        for (UObject* Object : TransactionObjects)
        {
            UBlueprint* OdysseyBrush = nullptr;

            while (Object != nullptr && OdysseyBrush == nullptr)
            {
                OdysseyBrush = Cast<UBlueprint>(Object);
                Object = Object->GetOuter();
            }

            if (OdysseyBrush != nullptr)
            {
                ModifiedBlueprints.AddUnique(OdysseyBrush);
            }
        }
    }

    // Transaction affects the Blueprints this editor handles, so react as necessary
    for (UBlueprint* OdysseyBrush : ModifiedBlueprints)
    {
        FixSubObjectReferencesPostUndoRedo(OdysseyBrush->GeneratedClass->GetDefaultObject());
        // Will cause a call to RefreshEditors()
        FBlueprintEditorUtils::MarkBlueprintAsStructurallyModified(OdysseyBrush);
    }
}

void FOdysseyBrushUndoRedoHandler::PostUndo(bool bSuccess)
{
    FixSubObjectReferencesPostUndoRedo(GEditor->Trans->GetTransaction(GEditor->Trans->GetQueueLength() - GEditor->Trans->GetUndoCount()));
}

void FOdysseyBrushUndoRedoHandler::PostRedo(bool bSuccess)
{
    // Note: We add 1 to get the correct slot, because the transaction buffer will have decremented the UndoCount prior to getting here.
    if( GEditor->Trans->GetQueueLength() > 0 )
    {
        FixSubObjectReferencesPostUndoRedo(GEditor->Trans->GetTransaction(GEditor->Trans->GetQueueLength() - (GEditor->Trans->GetUndoCount() + 1)));
    }
}

void FOdysseyBrushEditorModule::StartupModule()
{
    check(GEditor);

    delete UndoRedoHandler;
    UndoRedoHandler = new FOdysseyBrushUndoRedoHandler();
    GEditor->RegisterForUndo(UndoRedoHandler);

    MenuExtensibilityManager = MakeShareable(new FExtensibilityManager);
    SharedOdysseyBrushEditorCommands = MakeShareable(new FUICommandList);

    OdysseyBrushDebugger = MakeUnique<FOdysseyBrushDebugger>();

    // Have to check GIsEditor because right now editor modules can be loaded by the game
    // Once LoadModule is guaranteed to return NULL for editor modules in game, this can be removed
    // Without this check, loading the level editor in the game will crash
    if (GIsEditor)
    {
        // Extend the level viewport context menu to handle blueprints
        LevelViewportContextMenUBlueprintExtender = FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors::CreateStatic(&ExtendLevelViewportContextMenuForBlueprints);
        FLevelEditorModule& LevelEditorModule = FModuleManager::Get().LoadModuleChecked<FLevelEditorModule>("LevelEditor");
        auto& MenuExtenders = LevelEditorModule.GetAllLevelViewportContextMenuExtenders();
        MenuExtenders.Add(LevelViewportContextMenUBlueprintExtender);
        LevelViewportContextMenUBlueprintExtenderDelegateHandle = MenuExtenders.Last().GetHandle();
    }

    /////////////////
    // Register the asset type
    IAssetTools&  AssetTools = FModuleManager::LoadModuleChecked< FAssetToolsModule >( "AssetTools" ).Get();

    //EAssetTypeCategories::Type OdysseyBrushAssetCategoryBit = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("OdysseyBrush")), LOCTEXT("OdysseyBrushAssetCategory", "OdysseyBrush"));
    EAssetTypeCategories::Type OdysseyBrushAssetCategoryBit = EAssetTypeCategories::Type::Basic;

    OdysseyBrushAssetTypeActions = MakeShareable( new  FAssetTypeActions_OdysseyBrush( OdysseyBrushAssetCategoryBit ) );
    AssetTools.RegisterAssetTypeActions( OdysseyBrushAssetTypeActions.ToSharedRef() );

    // Register the thumbnail renderers
    UThumbnailManager::Get().RegisterCustomRenderer( UOdysseyBrush::StaticClass(), UOdysseyBrushThumbnailRenderer::StaticClass() );
    /////////////////

    FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
    FMessageLogInitializationOptions InitOptions;
    InitOptions.bShowFilters = true;
    InitOptions.bShowPages = true;
    MessageLogModule.RegisterLogListing("OdysseyBrushLog", LOCTEXT("OdysseyBrushLog", "OdysseyBrush Log"), InitOptions);

    // Listen for clicks in log so we can focus on the object, might have to restart K2 if the K2 tab has been closed
    MessageLogModule.GetLogListing("OdysseyBrushLog")->OnMessageTokenClicked().AddStatic( &FocusOdysseyBrushEditorOnObject );

    // Also listen for clicks in the PIE log, runtime errors with Blueprints may post clickable links there
    MessageLogModule.GetLogListing("PIE")->OnMessageTokenClicked().AddStatic( &FocusOdysseyBrushEditorOnObject );

    // Add a page for pre-loading of the editor
    MessageLogModule.GetLogListing("OdysseyBrushLog")->NewPage(LOCTEXT("PreloadLogPageLabel", "Editor Load"));

    // Register internal SCS editor customizations
    RegisterSCSEditorCustomization("InstancedStaticMeshComponent", FOdysseySCSEditorCustomizationBuilder::CreateStatic(&FInstancedStaticMeshSCSEditorCustomization::MakeInstance));
    RegisterSCSEditorCustomization("HierarchicalInstancedStaticMeshComponent", FOdysseySCSEditorCustomizationBuilder::CreateStatic(&FInstancedStaticMeshSCSEditorCustomization::MakeInstance));

    TSharedPtr<FBlueprintGraphPanelPinFactory> OdysseyBrushGraphPanelPinFactory = MakeShareable(new FBlueprintGraphPanelPinFactory());
    FEdGraphUtilities::RegisterVisualPinFactory(OdysseyBrushGraphPanelPinFactory);

    PrepareAutoGeneratedDefaultEvents();

    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
    }
}

void FOdysseyBrushEditorModule::ShutdownModule()
{
    if (ISettingsModule* SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings"))
    {
        SettingsModule->UnregisterSettings("Project", "Engine", "Blueprints");
        ConfigurationPanel = TSharedPtr<SWidget>();
    }
    // we're intentionally leaking UndoRedoHandler because the GEditor may be garbage when ShutdownModule is called:

    /////////////////
    if( !UObjectInitialized() )
    {
        return;
    }

    // Only unregister if the asset tools module is loaded.  We don't want to forcibly load it during shutdown phase.
    check( OdysseyBrushAssetTypeActions.IsValid() );
    if( FModuleManager::Get().IsModuleLoaded( "AssetTools" ) )
    {
        FModuleManager::GetModuleChecked< FAssetToolsModule >( "AssetTools" ).Get().UnregisterAssetTypeActions( OdysseyBrushAssetTypeActions.ToSharedRef() );
    }
    OdysseyBrushAssetTypeActions.Reset();

    // Unregister the thumbnail renderers
    UThumbnailManager::Get().UnregisterCustomRenderer( UOdysseyBrush::StaticClass() );
    /////////////////

    // Cleanup all information for auto generated default event nodes by this module
    FKismetEditorUtilities::UnregisterAutoBlueprintNodeCreation(this);

    SharedOdysseyBrushEditorCommands.Reset();
    MenuExtensibilityManager.Reset();

    // Remove level viewport context menu extenders
    if ( FModuleManager::Get().IsModuleLoaded( "LevelEditor" ) )
    {
        FLevelEditorModule& LevelEditorModule = FModuleManager::LoadModuleChecked<FLevelEditorModule>( "LevelEditor" );
        LevelEditorModule.GetAllLevelViewportContextMenuExtenders().RemoveAll([&](const FLevelEditorModule::FLevelViewportMenuExtender_SelectedActors& Delegate) {
            return Delegate.GetHandle() == LevelViewportContextMenUBlueprintExtenderDelegateHandle;
        });
    }

    FMessageLogModule& MessageLogModule = FModuleManager::LoadModuleChecked<FMessageLogModule>("MessageLog");
    MessageLogModule.UnregisterLogListing("OdysseyBrushLog");

    // Unregister internal SCS editor customizations
    UnregisterSCSEditorCustomization("InstancedStaticMeshComponent");

    UEdGraphPin::ShutdownVerification();
}


TSharedRef<IOdysseyBrushEditor> FOdysseyBrushEditorModule::CreateOdysseyBrushEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UBlueprint* OdysseyBrush, bool bShouldOpenInDefaultsMode)
{
    TSharedRef< FOdysseyBrushEditor > NewOdysseyBrushEditor( new FOdysseyBrushEditor() );

    TArray<UBlueprint*> Blueprints;
    Blueprints.Add(OdysseyBrush);
    NewOdysseyBrushEditor->InitOdysseyBrushEditor(Mode, InitToolkitHost, Blueprints, bShouldOpenInDefaultsMode);

    for(auto It(SCSEditorCustomizations.CreateConstIterator()); It; ++It)
    {
        NewOdysseyBrushEditor->RegisterSCSEditorCustomization(It->Key, It->Value.Execute(NewOdysseyBrushEditor));
    }

    WatchViewer::UpdateWatchListFromOdysseyBrush(OdysseyBrush);

    EBlueprintType const BPType = OdysseyBrush ? (EBlueprintType)OdysseyBrush->BlueprintType : BPTYPE_Normal;
    OdysseyBrushEditorOpened.Broadcast(BPType);

    return NewOdysseyBrushEditor;
}

TSharedRef<IOdysseyBrushEditor> FOdysseyBrushEditorModule::CreateOdysseyBrushEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, const TArray< UBlueprint* >& BlueprintsToEdit )
{
    TSharedRef< FOdysseyBrushEditor > NewOdysseyBrushEditor( new FOdysseyBrushEditor() );

    NewOdysseyBrushEditor->InitOdysseyBrushEditor(Mode, InitToolkitHost, BlueprintsToEdit, true);

    for(auto It(SCSEditorCustomizations.CreateConstIterator()); It; ++It)
    {
        NewOdysseyBrushEditor->RegisterSCSEditorCustomization(It->Key, It->Value.Execute(NewOdysseyBrushEditor));
    }

    for (UBlueprint* OdysseyBrush : BlueprintsToEdit)
    {
        WatchViewer::UpdateWatchListFromOdysseyBrush(OdysseyBrush);
    }

    EBlueprintType const BPType = ( (BlueprintsToEdit.Num() > 0) && (BlueprintsToEdit[0] != NULL) )
        ? (EBlueprintType) BlueprintsToEdit[0]->BlueprintType
        : BPTYPE_Normal;

    OdysseyBrushEditorOpened.Broadcast(BPType);

    return NewOdysseyBrushEditor;
}

TSharedRef<IOdysseyUserDefinedEnumEditor> FOdysseyBrushEditorModule::CreateUserDefinedEnumEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UUserDefinedEnum* UDEnum)
{
    TSharedRef<FUserDefinedEnumEditor> UserDefinedEnumEditor(new FUserDefinedEnumEditor());
    UserDefinedEnumEditor->InitEditor(Mode, InitToolkitHost, UDEnum);
    return UserDefinedEnumEditor;
}

TSharedRef<IOdysseyUserDefinedStructureEditor> FOdysseyBrushEditorModule::CreateUserDefinedStructEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UUserDefinedStruct* UDStruct)
{
    TSharedRef<FUserDefinedStructureEditor> UserDefinedStructureEditor(new FUserDefinedStructureEditor());
    UserDefinedStructureEditor->InitEditor(Mode, InitToolkitHost, UDStruct);
    return UserDefinedStructureEditor;
}

void FOdysseyBrushEditorModule::RegisterSCSEditorCustomization(const FName& InComponentName, FOdysseySCSEditorCustomizationBuilder InCustomizationBuilder)
{
    SCSEditorCustomizations.Add(InComponentName, InCustomizationBuilder);
}

void FOdysseyBrushEditorModule::UnregisterSCSEditorCustomization(const FName& InComponentName)
{
    SCSEditorCustomizations.Remove(InComponentName);
}

void FOdysseyBrushEditorModule::RegisterVariableCustomization(UStruct* InStruct, FOdysseyOnGetVariableCustomizationInstance InOnGetVariableCustomization)
{
    VariableCustomizations.Add(InStruct, InOnGetVariableCustomization);
}

void FOdysseyBrushEditorModule::UnregisterVariableCustomization(UStruct* InStruct)
{
    VariableCustomizations.Remove(InStruct);
}

TArray<TSharedPtr<IDetailCustomization>> FOdysseyBrushEditorModule::CustomizeVariable(UStruct* InStruct, TSharedPtr<IOdysseyBrushEditor> InOdysseyBrushEditor)
{
    TArray<TSharedPtr<IDetailCustomization>> DetailsCustomizations;
    TArray<UStruct*> ParentStructsToQuery;
    if (InStruct)
    {
        ParentStructsToQuery.Add(InStruct);

        UStruct* ParentStruct = InStruct->GetSuperStruct();
        while (ParentStruct && ParentStruct->IsA(UClass::StaticClass()))
        {
            ParentStructsToQuery.Add(ParentStruct);
            ParentStruct = ParentStruct->GetSuperStruct();
        }

        for (UStruct* StructToQuery : ParentStructsToQuery)
        {
            FOdysseyOnGetVariableCustomizationInstance* CustomizationDelegate = VariableCustomizations.Find(StructToQuery);
            if (CustomizationDelegate && CustomizationDelegate->IsBound())
            {
                TSharedPtr<IDetailCustomization> Customization = CustomizationDelegate->Execute(InOdysseyBrushEditor);
                if(Customization.IsValid())
                {
                    DetailsCustomizations.Add(Customization);
                }
            }
        }
    }

    return DetailsCustomizations;
}

void FOdysseyBrushEditorModule::PrepareAutoGeneratedDefaultEvents()
{
    // Load up all default events that should be spawned for Blueprints that are children of specific classes
    const FString ConfigSection = TEXT("DefaultEventNodes");
    const FString SettingName = TEXT("Node");
    TArray< FString > NodeSpawns;
    GConfig->GetArray(*ConfigSection, *SettingName, NodeSpawns, GEditorPerProjectIni);

    for(FString CurrentNodeSpawn : NodeSpawns)
    {
        FString TargetClassName;
        if(!FParse::Value(*CurrentNodeSpawn, TEXT("TargetClass="), TargetClassName))
        {
            // Could not find a class name, cannot continue with this line
            continue;
        }

        UClass* FoundTargetClass = FindObject<UClass>(ANY_PACKAGE, *TargetClassName, true);
        if(FoundTargetClass)
        {
            FString TargetEventFunction;
            if(!FParse::Value(*CurrentNodeSpawn, TEXT("TargetEvent="), TargetEventFunction))
            {
                // Could not find a class name, cannot continue with this line
                continue;
            }

            FName TargetEventFunctionName(*TargetEventFunction);
            if ( FoundTargetClass->FindFunctionByName(TargetEventFunctionName) )
            {
                FKismetEditorUtilities::RegisterAutoGeneratedDefaultEvent(this, FoundTargetClass, FName(*TargetEventFunction));
            }
        }
    }
}

IMPLEMENT_MODULE( FOdysseyBrushEditorModule, OdysseyBrushEditor );


#undef LOCTEXT_NAMESPACE
