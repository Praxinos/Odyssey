// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "SOdysseyBrushEditorToolbar.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Misc/CoreMisc.h"
#include "Widgets/Layout/SBorder.h"
#include "Misc/MessageDialog.h"
#include "Modules/ModuleManager.h"
#include "UObject/UObjectHash.h"
#include "OdysseyBrushEditor.h"
#include "Widgets/Layout/SSpacer.h"
#include "ISourceControlModule.h"
#include "SourceControlHelpers.h"
#include "OdysseyBrushEditorCommands.h"
#include "Kismet2/DebuggerCommands.h"
#include "Engine/LevelScriptBlueprint.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "GraphEditorActions.h"
#include "IAssetTools.h"
#include "IAssetTypeActions.h"
#include "AssetToolsModule.h"
#include "OdysseyBrushEditorModes.h"
#include "Widgets/SToolTip.h"
#include "IDocumentation.h"
#include "SOdysseyBrushEditorSelectedDebugObjectWidget.h"
#include "DesktopPlatformModule.h"
#include "SOdysseyBrushRevisionMenu.h"

#define LOCTEXT_NAMESPACE "OdysseyBrushEditorToolbar"

//////////////////////////////////////////////////////////////////////////
// SOdysseyBrushModeSeparator

class SOdysseyBrushModeSeparator : public SBorder
{
public:
    SLATE_BEGIN_ARGS(SOdysseyBrushModeSeparator) {}
    SLATE_END_ARGS()

    void Construct(const FArguments& InArg)
    {
        SBorder::Construct(
            SBorder::FArguments()
            .BorderImage(FEditorStyle::GetBrush("OdysseyBrushEditor.PipelineSeparator"))
            .Padding(0.0f)
            );
    }

    // SWidget interface
    virtual FVector2D ComputeDesiredSize(float) const override
    {
        const float Height = 20.0f;
        const float Thickness = 16.0f;
        return FVector2D(Thickness, Height);
    }
    // End of SWidget interface
};

//////////////////////////////////////////////////////////////////////////
// FKismet2Menu

void FKismet2Menu::FillFileMenUBlueprintSection( FMenuBuilder& MenuBuilder, FOdysseyBrushEditor& Kismet )
{
    MenuBuilder.BeginSection("FileOdysseyBrush", LOCTEXT("OdysseyBrushHeading", "OdysseyBrush"));
    {
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().CompileBlueprint );
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().RefreshAllNodes );
        //MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().ReparentOdysseyBrush );
        /*
        MenuBuilder.AddWrapperSubMenu(
            LOCTEXT("Diff", "Diff"),
            LOCTEXT("OdysseyBrushEditorDiffToolTip", "Diff against previous revisions"),
            FOnGetContent::CreateStatic< FOdysseyBrushEditor& >( &FKismet2Menu::MakeDiffMenu, Kismet),
            FSlateIcon());
        MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().BeginOdysseyBrushMerge);
        */
    }
    MenuBuilder.EndSection();

    /*
    // Only show the developer menu on machines with the solution (assuming they can build it)
    FString SolutionPath;
    if(FDesktopPlatformModule::Get()->GetSolutionPath(SolutionPath))
    {
        MenuBuilder.BeginSection("FileDeveloper");
        {
            MenuBuilder.AddSubMenu(
                LOCTEXT("DeveloperMenu", "Developer"),
                LOCTEXT("DeveloperMenu_ToolTip", "Open the developer menu"),
                FNewMenuDelegate::CreateStatic( &FKismet2Menu::FillDeveloperMenu ),
                true);
        }
        MenuBuilder.EndSection();
    }
    */
}

void FKismet2Menu::FillDeveloperMenu( FMenuBuilder& MenuBuilder )
{
    /*
    MenuBuilder.BeginSection("FileDeveloperCompilerSettings", LOCTEXT("CompileOptionsHeading", "Compiler Settings"));
    {
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().SaveIntermediateBuildProducts );
    }
    MenuBuilder.EndSection();

    MenuBuilder.BeginSection("GenerateNativeCode", LOCTEXT("Cpp", "C++"));
    {
        MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().GenerateNativeCode);
    }
    MenuBuilder.EndSection();

    if (false)
    {
        MenuBuilder.BeginSection("FileDeveloperFindReferences");
        {
            MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().FindReferencesFromClass );
            MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().FindReferencesFromOdysseyBrush );
            MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().FindReferencesFromOdysseyBrush );
        }
        MenuBuilder.EndSection();
    }

    MenuBuilder.BeginSection("SchemaDeveloperSettings", LOCTEXT("SchemaDevUtilsHeading", "Schema Utilities"));
    {
        MenuBuilder.AddMenuEntry(FOdysseyBrushEditorCommands::Get().ShowActionMenuItemSignatures);
    }
    MenuBuilder.EndSection();
    */
}

void FKismet2Menu::FillEditMenu( FMenuBuilder& MenuBuilder )
{
    MenuBuilder.BeginSection("EditSearch", LOCTEXT("EditMenu_SearchHeading", "Search") );
    {
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().FindInOdysseyBrush );
        //MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().FindInBlueprints );
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().DeleteUnusedVariables );
    }
    MenuBuilder.EndSection();
}

void FKismet2Menu::FillViewMenu( FMenuBuilder& MenuBuilder )
{
    MenuBuilder.BeginSection("ViewPinVisibility", LOCTEXT("ViewMenu_PinVisibilityHeading", "Pin Visibility") );
    {
        MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().ShowAllPins);
        MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().HideNoConnectionNoDefaultPins);
        MenuBuilder.AddMenuEntry(FGraphEditorCommands::Get().HideNoConnectionPins);
    }
    MenuBuilder.EndSection();

    MenuBuilder.BeginSection("ViewZoom", LOCTEXT("ViewMenu_ZoomHeading", "Zoom") );
    {
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().ZoomToWindow );
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().ZoomToSelection );
    }
    MenuBuilder.EndSection();
}

void FKismet2Menu::FillDebugMenu( FMenuBuilder& MenuBuilder )
{
    /*
    MenuBuilder.BeginSection("DebugBreakpoints", LOCTEXT("DebugMenu_BreakpointHeading", "Breakpoints") );
    {
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().DisableAllBreakpoints );
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().EnableAllBreakpoints );
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().ClearAllBreakpoints );
    }
    MenuBuilder.EndSection();

    MenuBuilder.BeginSection("DebugWatches", LOCTEXT("DebugMenu_WatchHeading", "Watches") );
    {
        MenuBuilder.AddMenuEntry( FOdysseyBrushEditorCommands::Get().ClearAllWatches );
    }
    MenuBuilder.EndSection();
    */
}

void FKismet2Menu::SetupOdysseyBrushEditorMenu( TSharedPtr< FExtender > Extender, FOdysseyBrushEditor& OdysseyBrushEditor)
{
    // Extend the File menu with asset actions
    Extender->AddMenuExtension(
        "FileLoadAndSave",
        EExtensionHook::After,
        OdysseyBrushEditor.GetToolkitCommands(),
        FMenuExtensionDelegate::CreateStatic< FOdysseyBrushEditor& >( &FKismet2Menu::FillFileMenUBlueprintSection, OdysseyBrushEditor ) );

    // Extend the Edit menu
    Extender->AddMenuExtension(
        "EditHistory",
        EExtensionHook::After,
        OdysseyBrushEditor.GetToolkitCommands(),
        FMenuExtensionDelegate::CreateStatic( &FKismet2Menu::FillEditMenu ) );

    // Add additional blueprint editor menus
    {
        struct Local
        {
            static void AddOdysseyBrushEditorMenus( FMenuBarBuilder& MenuBarBuilder )
            {
                // View
                MenuBarBuilder.AddPullDownMenu(
                    LOCTEXT("ViewMenu", "View"),
                    LOCTEXT("ViewMenu_ToolTip", "Open the View menu"),
                    FNewMenuDelegate::CreateStatic( &FKismet2Menu::FillViewMenu ),
                    "View");

                /*
                // Debug
                MenuBarBuilder.AddPullDownMenu(
                    LOCTEXT("DebugMenu", "Debug"),
                    LOCTEXT("DebugMenu_ToolTip", "Open the debug menu"),
                    FNewMenuDelegate::CreateStatic( &FKismet2Menu::FillDebugMenu ),
                    "Debug");
                */
            }
        };

        Extender->AddMenuBarExtension(
            "Edit",
            EExtensionHook::After,
            OdysseyBrushEditor.GetToolkitCommands(),
            FMenuBarExtensionDelegate::CreateStatic( &Local::AddOdysseyBrushEditorMenus ) );
    }
}

/** Delegate called to diff a specific revision with the current */
static void OnDiffRevisionPicked(FRevisionInfo const& RevisionInfo, TWeakObjectPtr<UBlueprint> OdysseyBrushObj)
{
    if (OdysseyBrushObj.IsValid())
    {
        bool const bIsLevelScriptBlueprint = FBlueprintEditorUtils::IsLevelScriptBlueprint(OdysseyBrushObj.Get());
        FString const Filename = SourceControlHelpers::PackageFilename(bIsLevelScriptBlueprint ? OdysseyBrushObj.Get()->GetOuter()->GetPathName() : OdysseyBrushObj.Get()->GetPathName());

        ISourceControlProvider& SourceControlProvider = ISourceControlModule::Get().GetProvider();

        // Get the SCC state
        FSourceControlStatePtr SourceControlState = SourceControlProvider.GetState(Filename, EStateCacheUsage::Use);
        if (SourceControlState.IsValid())
        {
            for (int32 HistoryIndex = 0; HistoryIndex < SourceControlState->GetHistorySize(); HistoryIndex++)
            {
                TSharedPtr<ISourceControlRevision, ESPMode::ThreadSafe> Revision = SourceControlState->GetHistoryItem(HistoryIndex);
                check(Revision.IsValid());
                if (Revision->GetRevision() == RevisionInfo.Revision)
                {
                    // Get the revision of this package from source control
                    FString PreviousTempPkgName;
                    if (Revision->Get(PreviousTempPkgName))
                    {
                        // Try and load that package
                        UPackage* PreviousTempPkg = LoadPackage(NULL, *PreviousTempPkgName, LOAD_DisableCompileOnLoad);

                        if (PreviousTempPkg != NULL)
                        {
                            UObject* PreviousAsset = NULL;

                            // If its a levelscript blueprint, find the previous levelscript blueprint in the map
                            if (bIsLevelScriptBlueprint)
                            {
                                TArray<UObject *> ObjectsInOuter;
                                GetObjectsWithOuter(PreviousTempPkg, ObjectsInOuter);

                                // Look for the level script blueprint for this package
                                for (int32 Index = 0; Index < ObjectsInOuter.Num(); Index++)
                                {
                                    UObject* Obj = ObjectsInOuter[Index];
                                    if (ULevelScriptBlueprint* ObjAsOdysseyBrush = Cast<ULevelScriptBlueprint>(Obj))
                                    {
                                        PreviousAsset = ObjAsOdysseyBrush;
                                        break;
                                    }
                                }
                            }
                            // otherwise its a normal OdysseyBrush
                            else
                            {
                                FString PreviousAssetName = FPaths::GetBaseFilename(Filename, true);
                                PreviousAsset = FindObject<UObject>(PreviousTempPkg, *PreviousAssetName);
                            }

                            if (PreviousAsset != NULL)
                            {
                                FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>(TEXT("AssetTools"));
                                FRevisionInfo OldRevision = { Revision->GetRevision(), Revision->GetCheckInIdentifier(), Revision->GetDate() };
                                FRevisionInfo CurrentRevision = { TEXT(""), Revision->GetCheckInIdentifier(), Revision->GetDate() };
                                AssetToolsModule.Get().DiffAssets(PreviousAsset, OdysseyBrushObj.Get(), OldRevision, CurrentRevision);
                            }
                        }
                        else
                        {
                            FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("SourceControl.HistoryWindow", "UnableToLoadAssets", "Unable to load assets to diff. Content may no longer be supported?"));
                        }
                    }
                    break;
                }
            }
        }
    }
}

TSharedRef<SWidget> FKismet2Menu::MakeDiffMenu(FOdysseyBrushEditor& Kismet)
{
    if (ISourceControlModule::Get().IsEnabled() && ISourceControlModule::Get().GetProvider().IsAvailable())
    {
        UBlueprint* OdysseyBrushObj = Kismet.GetBlueprintObj();
        if(OdysseyBrushObj)
        {
            TWeakObjectPtr<UBlueprint> OdysseyBrushPtr = OdysseyBrushObj;
            // Add our async SCC task widget
            return SNew(SOdysseyBrushRevisionMenu, OdysseyBrushObj)
                .OnRevisionSelected_Static(&OnDiffRevisionPicked, OdysseyBrushPtr);
        }
        else
        {
            // if OdysseyBrushObj is null then this means that multiple blueprints are selected
            FMenuBuilder MenuBuilder(true, NULL);
            MenuBuilder.AddMenuEntry( LOCTEXT("NoRevisionsForMultipleBlueprints", "Multiple blueprints selected"),
                FText(), FSlateIcon(), FUIAction() );
            return MenuBuilder.MakeWidget();
        }
    }

    FMenuBuilder MenuBuilder(true, NULL);
    MenuBuilder.AddMenuEntry( LOCTEXT("SourceControlDisabled", "Source control is disabled"),
        FText(), FSlateIcon(), FUIAction() );
    return MenuBuilder.MakeWidget();
}



//////////////////////////////////////////////////////////////////////////
// FFullOdysseyBrushEditorCommands

void FFullOdysseyBrushEditorCommands::RegisterCommands()
{
    UI_COMMAND(Compile, "Compile", "Compile the blueprint", EUserInterfaceActionType::Button, FInputChord());

    UI_COMMAND(SaveOnCompile_Never, "Never", "Sets the save-on-compile option to 'Never', meaning that your Blueprints will not be saved when they are compiled", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_COMMAND(SaveOnCompile_SuccessOnly, "On Success Only", "Sets the save-on-compile option to 'Success Only', meaning that your Blueprints will be saved whenever they are successfully compiled", EUserInterfaceActionType::RadioButton, FInputChord());
    UI_COMMAND(SaveOnCompile_Always, "Always", "Sets the save-on-compile option to 'Always', meaning that your Blueprints will be saved whenever they are compiled (even if there were errors)", EUserInterfaceActionType::RadioButton, FInputChord());

    UI_COMMAND(SwitchToScriptingMode, "Graph", "Switches to Graph Editing Mode", EUserInterfaceActionType::ToggleButton, FInputChord());
    UI_COMMAND(SwitchToOdysseyBrushDefaultsMode, "Defaults", "Switches to Class Defaults Mode", EUserInterfaceActionType::ToggleButton, FInputChord());
    UI_COMMAND(SwitchToComponentsMode, "Components", "Switches to Components Mode", EUserInterfaceActionType::ToggleButton, FInputChord());

    UI_COMMAND(EditGlobalOptions, "Class Settings", "Edit Class Settings (Previously known as OdysseyBrush Props)", EUserInterfaceActionType::ToggleButton, FInputChord());
    UI_COMMAND(EditClassDefaults, "Class Defaults", "Edit the initial values of your class.", EUserInterfaceActionType::ToggleButton, FInputChord());

    UI_COMMAND(JumpToErrorNode, "Jump to Error Node", "When enabled, then the OdysseyBrush will snap focus to nodes producing an error during compilation", EUserInterfaceActionType::ToggleButton, FInputChord());
}

//////////////////////////////////////////////////////////////////////////
// Static FOdysseyBrushEditorToolbar Helpers

namespace OdysseyBrushEditorToolbarImpl
{
    static TSharedRef<SWidget> GenerateCompileOptionsWidget(TSharedRef<FUICommandList> CommandList);
    static void MakeSaveOnCompileSubMenu(FMenuBuilder& InMenuBuilder);
    static void MakeCompileDeveloperSubMenu(FMenuBuilder& InMenuBuilder);
};

static TSharedRef<SWidget> OdysseyBrushEditorToolbarImpl::GenerateCompileOptionsWidget(TSharedRef<FUICommandList> CommandList)
{
    FMenuBuilder MenuBuilder(/*bShouldCloseWindowAfterMenuSelection =*/true, CommandList);

    const FFullOdysseyBrushEditorCommands& Commands = FFullOdysseyBrushEditorCommands::Get();

    // @TODO: disable the menu and change up the tooltip when all sub items are disabled
    MenuBuilder.AddSubMenu(
        LOCTEXT("SaveOnCompileSubMenu", "Save on Compile"),
        LOCTEXT("SaveOnCompileSubMenu_ToolTip", "Determines how the OdysseyBrush is saved whenever you compile it."),
        FNewMenuDelegate::CreateStatic(&OdysseyBrushEditorToolbarImpl::MakeSaveOnCompileSubMenu));

    MenuBuilder.AddMenuEntry(Commands.JumpToErrorNode);

//     MenuBuilder.AddSubMenu(
//         LOCTEXT("DevCompileSubMenu", "Developer"),
//         LOCTEXT("DevCompileSubMenu_ToolTip", "Advanced settings that aid in devlopment/debugging of the OdysseyBrush system as a whole."),
//         FNewMenuDelegate::CreateStatic(&OdysseyBrushEditorToolbarImpl::MakeCompileDeveloperSubMenu));

    return MenuBuilder.MakeWidget();
}

static void OdysseyBrushEditorToolbarImpl::MakeSaveOnCompileSubMenu(FMenuBuilder& InMenuBuilder)
{
    const FFullOdysseyBrushEditorCommands& Commands = FFullOdysseyBrushEditorCommands::Get();
    InMenuBuilder.AddMenuEntry(Commands.SaveOnCompile_Never);
    InMenuBuilder.AddMenuEntry(Commands.SaveOnCompile_SuccessOnly);
    InMenuBuilder.AddMenuEntry(Commands.SaveOnCompile_Always);
}

static void OdysseyBrushEditorToolbarImpl::MakeCompileDeveloperSubMenu(FMenuBuilder& InMenuBuilder)
{
    const FOdysseyBrushEditorCommands& EditorCommands = FOdysseyBrushEditorCommands::Get();
    InMenuBuilder.AddMenuEntry(EditorCommands.SaveIntermediateBuildProducts);
    InMenuBuilder.AddMenuEntry(EditorCommands.ShowActionMenuItemSignatures);
}


//////////////////////////////////////////////////////////////////////////
// FOdysseyBrushEditorToolbar

void FOdysseyBrushEditorToolbar::AddOdysseyBrushEditorModesToolbar(TSharedPtr<FExtender> Extender)
{

}

void FOdysseyBrushEditorToolbar::AddOdysseyBrushGlobalOptionsToolbar(TSharedPtr<FExtender> Extender)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();

    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        OdysseyBrushEditorPtr->GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP( this, &FOdysseyBrushEditorToolbar::FillOdysseyBrushGlobalOptionsToolbar ) );
}

void FOdysseyBrushEditorToolbar::AddCompileToolbar(TSharedPtr<FExtender> Extender)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();

    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::Before,
        OdysseyBrushEditorPtr->GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP( this, &FOdysseyBrushEditorToolbar::FillCompileToolbar ) );
}

void FOdysseyBrushEditorToolbar::AddNewToolbar(TSharedPtr<FExtender> Extender)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();

    Extender->AddToolBarExtension(
        "MyOdysseyBrush",
        EExtensionHook::After,
        OdysseyBrushEditorPtr->GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP( this, &FOdysseyBrushEditorToolbar::FillNewToolbar ) );
}

void FOdysseyBrushEditorToolbar::AddScriptingToolbar(TSharedPtr<FExtender> Extender)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();

    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        OdysseyBrushEditorPtr->GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP( this, &FOdysseyBrushEditorToolbar::FillScriptingToolbar ) );
}

void FOdysseyBrushEditorToolbar::AddDebuggingToolbar(TSharedPtr<FExtender> Extender)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();

    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        OdysseyBrushEditorPtr->GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP( this, &FOdysseyBrushEditorToolbar::FillDebuggingToolbar ) );
}

void FOdysseyBrushEditorToolbar::AddComponentsToolbar(TSharedPtr<FExtender> Extender)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();

    Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        OdysseyBrushEditorPtr->GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP( this, &FOdysseyBrushEditorToolbar::FillComponentsToolbar ) );
}

void FOdysseyBrushEditorToolbar::FillOdysseyBrushEditorModesToolbar(FToolBarBuilder& ToolbarBuilder)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr->GetBlueprintObj();

    TAttribute<FName> GetActiveMode(OdysseyBrushEditorPtr.ToSharedRef(), &FOdysseyBrushEditor::GetCurrentMode);
    FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(OdysseyBrushEditorPtr.ToSharedRef(), &FOdysseyBrushEditor::SetCurrentMode);

    TArray< TSharedPtr< SWidget > > ToolbarWidgets = GenerateToolbarWidgets( OdysseyBrushObj, GetActiveMode, SetActiveMode );

    for( const auto& Widget : ToolbarWidgets )
    {
        OdysseyBrushEditorPtr->AddToolbarWidget( Widget.ToSharedRef() );
    }
}

void FOdysseyBrushEditorToolbar::FillOdysseyBrushGlobalOptionsToolbar(FToolBarBuilder& ToolbarBuilder)
{
    const FFullOdysseyBrushEditorCommands& Commands = FFullOdysseyBrushEditorCommands::Get();
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr->GetBlueprintObj();

    ToolbarBuilder.BeginSection("Settings");

    if(OdysseyBrushObj != NULL)
    {
        ToolbarBuilder.AddToolBarButton(Commands.EditGlobalOptions);
        ToolbarBuilder.AddToolBarButton(Commands.EditClassDefaults);
    }

    ToolbarBuilder.EndSection();
}

void FOdysseyBrushEditorToolbar::FillCompileToolbar(FToolBarBuilder& ToolbarBuilder)
{
    const FFullOdysseyBrushEditorCommands& Commands = FFullOdysseyBrushEditorCommands::Get();
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr->GetBlueprintObj();

    ToolbarBuilder.BeginSection("Compile");
    if (OdysseyBrushObj != NULL)
    {
        ToolbarBuilder.AddToolBarButton( Commands.Compile,
                                     NAME_None,
                                     TAttribute<FText>(),
                                     TAttribute<FText>(this, &FOdysseyBrushEditorToolbar::GetStatusTooltip),
                                     TAttribute<FSlateIcon>(this, &FOdysseyBrushEditorToolbar::GetStatusImage),
                                     FName(TEXT("CompileBlueprint")));

        /*
        FUIAction TempCompileOptionsCommand;
        ToolbarBuilder.AddComboButton(
            TempCompileOptionsCommand,
            FOnGetContent::CreateStatic(&OdysseyBrushEditorToolbarImpl::GenerateCompileOptionsWidget, OdysseyBrushEditorPtr->GetToolkitCommands()),
            LOCTEXT("BlupeintCompileOptions_ToolbarName",    "Compile Options"),
            LOCTEXT("BlupeintCompileOptions_ToolbarTooltip", "Options to customize how Blueprints compile"),
            TAttribute<FSlateIcon>(),
            true
        );
        */
    }
    ToolbarBuilder.EndSection();
}

void FOdysseyBrushEditorToolbar::FillNewToolbar(FToolBarBuilder& ToolbarBuilder)
{
    const FOdysseyBrushEditorCommands& Commands = FOdysseyBrushEditorCommands::Get();

    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr->GetBlueprintObj();

    ToolbarBuilder.BeginSection("AddNew");
    if (OdysseyBrushObj != NULL)
    {
        ToolbarBuilder.AddToolBarButton(Commands.AddNewVariable, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), FName(TEXT("BPEAddNewVariable")));
        ToolbarBuilder.AddToolBarButton(Commands.AddNewFunction, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), FName(TEXT("BPEAddNewFunction")));
        ToolbarBuilder.AddToolBarButton(Commands.AddNewMacroDeclaration, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), FName(TEXT("BPEAddNewMacro")));
        // Add New Animation Graph isn't supported right now.
        //ToolbarBuilder.AddToolBarButton(Commands.AddNewEventGraph, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), FName(TEXT("BPEAddNewEventGraph")));
        //ToolbarBuilder.AddToolBarButton(Commands.AddNewDelegate, NAME_None, TAttribute<FText>(), TAttribute<FText>(), TAttribute<FSlateIcon>(), FName(TEXT("BPEAddNewDelegate")));
    }
    ToolbarBuilder.EndSection();
}

void FOdysseyBrushEditorToolbar::FillScriptingToolbar(FToolBarBuilder& ToolbarBuilder)
{
    const FOdysseyBrushEditorCommands& Commands = FOdysseyBrushEditorCommands::Get();

    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr->GetBlueprintObj();

    ToolbarBuilder.BeginSection("Script");

    ToolbarBuilder.AddToolBarButton( FOdysseyBrushEditorCommands::Get().FindInOdysseyBrush,
                                     NAME_None,
                                     TAttribute<FText>(),
                                     TAttribute<FText>(),
                                     TAttribute<FSlateIcon>( FSlateIcon(FEditorStyle::GetStyleSetName(), "BlueprintEditor.FindInBlueprint") ) );

    ToolbarBuilder.EndSection();
}

void FOdysseyBrushEditorToolbar::FillDebuggingToolbar(FToolBarBuilder& ToolbarBuilder)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr->GetBlueprintObj();

    ToolbarBuilder.BeginSection("Debugging");
    if (OdysseyBrushObj)
    {
        FPlayWorldCommands::BuildToolbar(ToolbarBuilder);

        if (OdysseyBrushObj->BlueprintType != BPTYPE_MacroLibrary)
        {
            // Selected debug actor button
            ToolbarBuilder.AddWidget(SNew(SOdysseyBrushEditorSelectedDebugObjectWidget, OdysseyBrushEditorPtr));
        }
    }
    ToolbarBuilder.EndSection();
}

void FOdysseyBrushEditorToolbar::FillComponentsToolbar(FToolBarBuilder& ToolbarBuilder)
{
    TSharedPtr<FOdysseyBrushEditor> OdysseyBrushEditorPtr = OdysseyBrushEditor.Pin();
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditorPtr->GetBlueprintObj();

#if 0 // restore this if we ever need the ability to toggle component editing on/off
    ToolbarBuilder.BeginSection("Components");
        ToolbarBuilder.AddToolBarButton(FSCSCommands::Get().ToggleComponentEditing);
    ToolbarBuilder.EndSection();
#endif

    ToolbarBuilder.BeginSection("ComponentsViewport");
        ToolbarBuilder.AddToolBarButton(FOdysseyBrushEditorCommands::Get().EnableSimulation);
    ToolbarBuilder.EndSection();
}

FSlateIcon FOdysseyBrushEditorToolbar::GetStatusImage() const
{
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditor.Pin()->GetBlueprintObj();
    EBlueprintStatus Status = OdysseyBrushObj->Status;

    // For macro types, always show as up-to-date, since we don't compile them
    if (OdysseyBrushObj->BlueprintType == BPTYPE_MacroLibrary)
    {
        Status = BS_UpToDate;
    }

    switch (Status)
    {
    default:
    case BS_Unknown:
    case BS_Dirty:
        return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Unknown");
    case BS_Error:
        return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Error");
    case BS_UpToDate:
        return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Good");
    case BS_UpToDateWithWarnings:
        return FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.Status.Warning");
    }
}

FText FOdysseyBrushEditorToolbar::GetStatusTooltip() const
{
    UBlueprint* OdysseyBrushObj = OdysseyBrushEditor.Pin()->GetBlueprintObj();
    EBlueprintStatus Status = OdysseyBrushObj->Status;

    // For macro types, always show as up-to-date, since we don't compile them
    if (OdysseyBrushObj->BlueprintType == BPTYPE_MacroLibrary)
    {
        Status = BS_UpToDate;
    }

    switch (Status)
    {
    default:
    case BS_Unknown:
        return LOCTEXT("Recompile_Status", "Unknown status; should recompile");
    case BS_Dirty:
        return LOCTEXT("Dirty_Status", "Dirty; needs to be recompiled");
    case BS_Error:
        return LOCTEXT("CompileError_Status", "There was an error during compilation, see the log for details");
    case BS_UpToDate:
        return LOCTEXT("GoodToGo_Status", "Good to go");
    case BS_UpToDateWithWarnings:
        return LOCTEXT("GoodToGoWarning_Status", "There was a warning during compilation, see the log for details");
    }
}

TArray< TSharedPtr< SWidget> > FOdysseyBrushEditorToolbar::GenerateToolbarWidgets(const UBlueprint* OdysseyBrushObj, TAttribute<FName> ActiveModeGetter, FOnModeChangeRequested ActiveModeSetter)
{
    TArray< TSharedPtr< SWidget> > Ret;
    if (!OdysseyBrushObj ||
        (!FBlueprintEditorUtils::IsLevelScriptBlueprint(OdysseyBrushObj)
        && !FBlueprintEditorUtils::IsInterfaceBlueprint(OdysseyBrushObj)
        && !OdysseyBrushObj->bIsNewlyCreated)
        )
    {
        // Left side padding
        Ret.Add(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

        Ret.Add(
            SNew(SModeWidget, FOdysseyBrushEditorApplicationModes::GetLocalizedMode(FOdysseyBrushEditorApplicationModes::StandardOdysseyBrushEditorMode), FOdysseyBrushEditorApplicationModes::StandardOdysseyBrushEditorMode)
            .OnGetActiveMode(ActiveModeGetter)
            .OnSetActiveMode(ActiveModeSetter)
            .CanBeSelected(OdysseyBrushObj != NULL)
            .ToolTip(IDocumentation::Get()->CreateToolTip(
                LOCTEXT("GraphModeButtonTooltip", "Switch to Graph Editing Mode"),
                NULL,
                TEXT("Shared/Editors/OdysseyBrushEditor"),
                TEXT("GraphMode")))
            .ToolTipText(LOCTEXT("GraphModeButtonTooltip", "Switch to Graph Editing Mode"))
            .IconImage(FEditorStyle::GetBrush("FullOdysseyBrushEditor.SwitchToScriptingMode"))
            .SmallIconImage(FEditorStyle::GetBrush("FullOdysseyBrushEditor.SwitchToScriptingMode.Small"))
            .AddMetaData<FTagMetaData>(FTagMetaData(TEXT("GraphMode")))
        );

        // Right side padding
        Ret.Add(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));
    }
    return Ret;
}

#undef LOCTEXT_NAMESPACE
