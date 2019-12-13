// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Engine/Blueprint.h"
#include "Framework/Commands/UICommandList.h"
#include "Toolkits/AssetEditorToolkit.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"
#include "BlueprintEditorModule.h"

class FSCSEditorTreeNode;
class UUserDefinedEnum;
class UUserDefinedStruct;
struct Rect;

class IOdysseyBrushEditor;
class FOdysseyBrushEditor;
class UUserDefinedEnum;
class UUserDefinedStruct;
class IDetailCustomization;
class FKismetCompilerContext;
struct FOdysseyBrushDebugger;

class  FAssetTypeActions_OdysseyBrush;

/** Delegate used to customize variable display */
DECLARE_DELEGATE_RetVal_OneParam(TSharedPtr<IDetailCustomization>, FOdysseyOnGetVariableCustomizationInstance, TSharedPtr<IOdysseyBrushEditor> /*OdysseyBrushEditor*/);

/**
 * Enum editor public interface
 */
class ODYSSEYBRUSHEDITOR_API IOdysseyUserDefinedEnumEditor : public FAssetEditorToolkit
{
};

/**
 * Enum editor public interface
 */
class ODYSSEYBRUSHEDITOR_API IOdysseyUserDefinedStructureEditor : public FAssetEditorToolkit
{
};

/**
 * OdysseyBrush editor public interface
 */
class ODYSSEYBRUSHEDITOR_API IOdysseyBrushEditor : public IBlueprintEditor
{
};

DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<class ISCSEditorCustomization>, FOdysseySCSEditorCustomizationBuilder, TSharedRef< IOdysseyBrushEditor > /* InOdysseyBrushEditor */);

/**
 * The blueprint editor module provides the blueprint editor application.
 */
class FOdysseyBrushEditorModule : public IModuleInterface,
    public IHasMenuExtensibility
{
public:
    // IModuleInterface interface
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
    // End of IModuleInterface interface

    /**
     * Creates an instance of a Kismet editor object.  Only virtual so that it can be called across the DLL boundary.
     *
     * Note: This function should not be called directly, use one of the following instead:
     *    - FKismetEditorUtilities::BringKismetToFocusAttentionOnObject
     *  - FAssetEditorManager::Get().OpenEditorForAsset
     *
     * @param    Mode                    Mode that this editor should operate in
     * @param    InitToolkitHost            When Mode is WorldCentric, this is the level editor instance to spawn this editor within
     * @param    OdysseyBrush                The blueprint object to start editing
     * @param    bShouldOpenInDefaultsMode    If true, the editor will open in defaults editing mode
     *
     * @return    Interface to the new OdysseyBrush editor
     */
    virtual TSharedRef<IOdysseyBrushEditor> CreateOdysseyBrushEditor(const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, UBlueprint* OdysseyBrush, bool bShouldOpenInDefaultsMode = false);
    virtual TSharedRef<IOdysseyBrushEditor> CreateOdysseyBrushEditor( const EToolkitMode::Type Mode, const TSharedPtr< IToolkitHost >& InitToolkitHost, const TArray< UBlueprint* >& BlueprintsToEdit );

    /**
     * Creates an instance of a Enum editor object.
     *
     * @param    Mode                    Mode that this editor should operate in
     * @param    InitToolkitHost            When Mode is WorldCentric, this is the level editor instance to spawn this editor within
     * @param    UDEnum                    The user-defined Enum to start editing
     *
     * @return    Interface to the new Enum editor
     */
    virtual TSharedRef<IOdysseyUserDefinedEnumEditor> CreateUserDefinedEnumEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UUserDefinedEnum* UDEnum);

    /**
     * Creates an instance of a Structure editor object.
     *
     * @param    Mode                    Mode that this editor should operate in
     * @param    InitToolkitHost            When Mode is WorldCentric, this is the level editor instance to spawn this editor within
     * @param    UDEnum                    The user-defined structure to start editing
     *
     * @return    Interface to the new Struct editor
     */
    virtual TSharedRef<IOdysseyUserDefinedStructureEditor> CreateUserDefinedStructEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, UUserDefinedStruct* UDStruct);

    /** Gets the extensibility managers for outside entities to extend blueprint editor's menus and toolbars */
    virtual TSharedPtr<FExtensibilityManager> GetMenuExtensibilityManager() override { return MenuExtensibilityManager; }

    /**  */
    DECLARE_EVENT_TwoParams(FOdysseyBrushEditorModule, FOdysseyBrushMenuExtensionEvent, TSharedPtr<FExtender>, UBlueprint*);
    FOdysseyBrushMenuExtensionEvent& OnGatherOdysseyBrushMenuExtensions() { return GatherOdysseyBrushMenuExtensions; }

    DECLARE_EVENT_ThreeParams(IOdysseyBrushEditor, FOnRegisterTabs, FWorkflowAllowedTabSet&, FName /** ModeName */, TSharedPtr<FOdysseyBrushEditor>);
    FOnRegisterTabs& OnRegisterTabsForEditor() { return RegisterTabsForEditor; }

    DECLARE_EVENT_OneParam(IOdysseyBrushEditor, FOnRegisterLayoutExtensions, FLayoutExtender&);
    FOnRegisterLayoutExtensions& OnRegisterLayoutExtensions() { return RegisterLayoutExtensions; }

    /**
     * Register a customization for interacting with the SCS editor
     * @param    InComponentName            The name of the component to customize behavior for
     * @param    InCustomizationBuilder    The delegate used to create customization instances
     */
    virtual void RegisterSCSEditorCustomization(const FName& InComponentName, FOdysseySCSEditorCustomizationBuilder InCustomizationBuilder);

    /**
     * Unregister a previously registered customization for interacting with the SCS editor
     * @param    InComponentName            The name of the component to customize behavior for
     */
    virtual void UnregisterSCSEditorCustomization(const FName& InComponentName);

    /**
     * Register a customization for for OdysseyBrush variables
     * @param    InStruct                The type of the variable to create the customization for
     * @param    InOnGetDetailCustomization    The delegate used to create customization instances
     */
    virtual void RegisterVariableCustomization(UStruct* InStruct, FOdysseyOnGetVariableCustomizationInstance InOnGetVariableCustomization);

    /**
     * Unregister a previously registered customization for BP variables
     * @param    InStruct                The type to create the customization for
     */
    virtual void UnregisterVariableCustomization(UStruct* InStruct);

    /**
     * Build a set of details customizations for the passed-in type, if possible.
     * @param    InStruct                The type to create the customization for
     * @param    InOdysseyBrushEditor        The OdysseyBrush Editor the customization will be created for
     */
    virtual TArray<TSharedPtr<IDetailCustomization>> CustomizeVariable(UStruct* InStruct, TSharedPtr<IOdysseyBrushEditor> InOdysseyBrushEditor);

    /** Delegate for binding functions to be called when the blueprint editor finishes getting created */
    DECLARE_EVENT_OneParam( FOdysseyBrushEditorModule, FOdysseyBrushEditorOpenedEvent, EBlueprintType );
    FOdysseyBrushEditorOpenedEvent& OnOdysseyBrushEditorOpened() { return OdysseyBrushEditorOpened; }

    /**
     * Exposes a way for other modules to fold in their own OdysseyBrush editor
     * commands (folded in with other BP editor commands, when the editor is
     * first opened).
     */
    virtual const TSharedRef<FUICommandList> GetsSharedOdysseyBrushEditorCommands() const { return SharedOdysseyBrushEditorCommands.ToSharedRef(); }

private:
    /** Loads from ini a list of all events that should be auto created for Blueprints of a specific class */
    void PrepareAutoGeneratedDefaultEvents();

private:
    TSharedPtr<FExtensibilityManager> MenuExtensibilityManager;

    //
    FOdysseyBrushMenuExtensionEvent GatherOdysseyBrushMenuExtensions;

    /** Event called to allow external clients to register additional tabs for the specified editor */
    FOnRegisterTabs RegisterTabsForEditor;
    FOnRegisterLayoutExtensions RegisterLayoutExtensions;

    // Event to be called when the blueprint editor is opened
    FOdysseyBrushEditorOpenedEvent OdysseyBrushEditorOpened;

    /** Customizations for the SCS editor */
    TMap<FName, FOdysseySCSEditorCustomizationBuilder> SCSEditorCustomizations;

    /** Customizations for OdysseyBrush variables */
    TMap<UStruct*, FOdysseyOnGetVariableCustomizationInstance> VariableCustomizations;

    /**
     * A command list that can be passed around and isn't bound to an instance
     * of the blueprint editor.
     */
    TSharedPtr<FUICommandList> SharedOdysseyBrushEditorCommands;

    /** Handle to a registered LevelViewportContextMenUBlueprintExtender delegate */
    FDelegateHandle LevelViewportContextMenUBlueprintExtenderDelegateHandle;

    /** Reference to keep our custom configuration panel alive */
    TSharedPtr<SWidget> ConfigurationPanel;

    /** OdysseyBrush debugger state - refactor into SOdysseyBrushDebugger if needed */
    TUniquePtr<FOdysseyBrushDebugger> OdysseyBrushDebugger;

    TSharedPtr< FAssetTypeActions_OdysseyBrush >  OdysseyBrushAssetTypeActions;
};
