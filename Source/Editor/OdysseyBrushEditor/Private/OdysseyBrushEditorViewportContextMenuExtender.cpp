// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.


#include "CoreMinimal.h"
#include "Textures/SlateIcon.h"
#include "Framework/Commands/UIAction.h"
#include "Framework/Commands/UICommandList.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "EditorStyleSet.h"
#include "GameFramework/Actor.h"
#include "Engine/Blueprint.h"
#include "Engine/Selection.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Editor.h"
#include "Toolkits/AssetEditorManager.h"
#include "Kismet2/KismetEditorUtilities.h"
#include "CreateBlueprintFromActorDialog.h"

DEFINE_LOG_CATEGORY_STATIC(LogViewportOdysseyBrushMenu, Log, All);

#define LOCTEXT_NAMESPACE "OdysseyBrushEditorViewportContextMenuExtender"

/** OdysseyBrush class info for context menu */
struct FMenUBlueprintClass
{
    /** Name of the class */
    FString Name;

    /** OdysseyBrush for a kismet graph */
    TWeakObjectPtr<UBlueprint> OdysseyBrush;
};

/**
 * Called to edit code for the specified function symbol name
 *
 * @param    OdysseyBrush        OdysseyBrush to edit code for
 */
void EditKismetCodeFor( TWeakObjectPtr<UBlueprint> OdysseyBrushRef )
{
    // Navigate to this function (implemented in Kismet 2)!
    if (UBlueprint* OdysseyBrush = OdysseyBrushRef.Get())
    {
        // Open the blueprint
        // @todo toolkit major: Needs world-centric support (pass in LevelEditor.  See FLevelEditorActionCallbacks::OpenLevelOdysseyBrush)
        FAssetEditorManager::Get().OpenEditorForAsset( OdysseyBrush );
    }
    else
    {
        UE_LOG(LogViewportOdysseyBrushMenu, Warning, TEXT("Failed to find blueprint"));
    }
}

/**
* Fills in a sub-menu that shows all of the classes that can be edited
*
* @param    MenuBuilder        The sub-menu we're building up
*/
void FillEditCodeMenu( class FMenuBuilder& MenuBuilder, TArray< FMenUBlueprintClass > Classes)
{
    for( int32 CurClassIndex = 0; CurClassIndex < Classes.Num(); ++CurClassIndex )
    {
        FMenUBlueprintClass& CurClass = Classes[ CurClassIndex ];

        FText LabelName = FText::FromString( CurClass.Name );

        const FText ToolTipName = LOCTEXT("EditCodeMenu_ClassToolTip", "Opens this OdysseyBrush in the OdysseyBrush Editor");

        FUIAction UIAction;
        UIAction.ExecuteAction.BindStatic(
            &EditKismetCodeFor,
            CurClass.OdysseyBrush );

        MenuBuilder.AddMenuEntry( LabelName, ToolTipName, FSlateIcon(), UIAction );
    }
}

/**
* Called to recompile the out of date blueprint for the current selection set
*/
void RecompileOutOfDateKismetForSelection()
{
    int32 OdysseyBrushFailures = 0;

    // Run thru all selected actors, looking for out of date blueprints
    FSelectionIterator SelectedActorItr( GEditor->GetSelectedActorIterator() );
    for ( ; SelectedActorItr; ++SelectedActorItr)
    {
        AActor* CurrentActor = Cast<AActor>(*SelectedActorItr);

        UBlueprint* OdysseyBrush = Cast<UBlueprint>(CurrentActor->GetClass()->ClassGeneratedBy);
        if ((OdysseyBrush != NULL) && (!OdysseyBrush->IsUpToDate()))
        {
            FKismetEditorUtilities::CompileBlueprint(OdysseyBrush);
            if (OdysseyBrush->Status == BS_Error)
            {
                ++OdysseyBrushFailures;
            }
        }
    }

    if (OdysseyBrushFailures)
    {
        UE_LOG(LogViewportOdysseyBrushMenu, Warning, TEXT("%d blueprints failed to be recompiled"), OdysseyBrushFailures);
    }
}

/**
 * Gathers all blueprints for the actors in question, outputting them to the classes array
 */
void GatherBlueprintsForActors( TArray< AActor* >& Actors, TArray< FMenUBlueprintClass >& Classes )
{
    struct Local
    {
        static void AddOdysseyBrush( TArray< FMenUBlueprintClass >& InClasses, const FString& ClassName, UBlueprint* OdysseyBrush = NULL )
        {
            check( !ClassName.IsEmpty() );

            // Check to see if we already have this class name in our list
            FMenUBlueprintClass* FoundClass = NULL;
            for( int32 CurClassIndex = 0; CurClassIndex < InClasses.Num(); ++CurClassIndex )
            {
                FMenUBlueprintClass& CurClass = InClasses[ CurClassIndex ];
                if( CurClass.Name == ClassName )
                {
                    FoundClass = &CurClass;
                    break;
                }
            }

            // Add a new class to our list if we need to
            if( FoundClass == NULL )
            {
                FoundClass = new( InClasses ) FMenUBlueprintClass();
                FoundClass->Name = ClassName;
                FoundClass->OdysseyBrush = OdysseyBrush;
            }
            else
            {
                check(FoundClass->OdysseyBrush.Get() == OdysseyBrush);
            }
        }
    };


    for( TArray< AActor* >::TIterator It( Actors ); It; ++It )
    {
        AActor* Actor = static_cast<AActor*>( *It );
        checkSlow( Actor->IsA(AActor::StaticClass()) );

        // Grab the class of this actor
        UClass* ActorClass = Actor->GetClass();
        check( ActorClass != NULL );

        // Walk the inheritance hierarchy for this class
        for( UClass* CurClass = ActorClass; CurClass != NULL; CurClass = CurClass->GetSuperClass() )
        {
            if (UBlueprint* OdysseyBrush = Cast<UBlueprint>(CurClass->ClassGeneratedBy))
            {
                // Class was created by a blueprint, so don't offer C++ editing of functions declared in it
                // Instead offer to edit the events and graphs of the blueprint

                Local::AddOdysseyBrush( Classes, CurClass->GetName(), OdysseyBrush );
            }
        }
    }
}

/**
 * Fills the OdysseyBrush menu with extra options
 */
void FillOdysseyBrushOptions(FMenuBuilder& MenuBuilder, TArray<AActor*> SelectedActors)
{
    // Gather OdysseyBrush classes for this actor
    TArray< FMenUBlueprintClass > OdysseyBrushClasses;
    GatherBlueprintsForActors( SelectedActors, OdysseyBrushClasses );

    MenuBuilder.BeginSection("ActorOdysseyBrush", LOCTEXT("BlueprintsHeading", "Blueprints") );

    // Adds the "Create OdysseyBrush..." menu option if valid.
    {
        int NumOdysseyBrushableActors = 0;
        bool IsOdysseyBrushBased = OdysseyBrushClasses.Num() > 1;

        if(!OdysseyBrushClasses.Num())
        {
            for(auto It(SelectedActors.CreateIterator());It;++It)
            {
                AActor* Actor = *It;
                if( FKismetEditorUtilities::CanCreateBlueprintOfClass(Actor->GetClass()))
                {
                    NumOdysseyBrushableActors++;
                }
            }
        }

        const bool bCanHarvestComponentsForOdysseyBrush = (!IsOdysseyBrushBased && (NumOdysseyBrushableActors > 0));

        if(bCanHarvestComponentsForOdysseyBrush)
        {
            AActor* ActorOverride = nullptr;
            FUIAction CreateOdysseyBrushAction( FExecuteAction::CreateStatic( &FCreateBlueprintFromActorDialog::OpenDialog, true, ActorOverride ) );
            MenuBuilder.AddMenuEntry(LOCTEXT("CreateOdysseyBrush", "Create OdysseyBrush..."), LOCTEXT("CreateOdysseyBrush_Tooltip", "Harvest Components from Selected Actors and create OdysseyBrush"), FSlateIcon(FEditorStyle::GetStyleSetName(), "Kismet.HarvestOdysseyBrushFromActors"), CreateOdysseyBrushAction);
        }
    }

    // Check to see if we have any classes with functions to display
    if( OdysseyBrushClasses.Num() > 0 )
    {
        {
            UBlueprint* FirstOdysseyBrush = OdysseyBrushClasses[0].OdysseyBrush.Get();

            // Determine if the selected objects that have blueprints are all of the same class, and if they are all up to date
            bool bAllAreSameType = true;
            bool bAreAnyNotUpToDate = false;
            for (int32 ClassIndex = 0; ClassIndex < OdysseyBrushClasses.Num(); ++ClassIndex)
            {
                UBlueprint* CurrentOdysseyBrush = OdysseyBrushClasses[ClassIndex].OdysseyBrush.Get();

                bAllAreSameType = bAllAreSameType && (CurrentOdysseyBrush == FirstOdysseyBrush);

                if (CurrentOdysseyBrush != NULL)
                {
                    bAreAnyNotUpToDate |= !CurrentOdysseyBrush->IsUpToDate();
                }
            }

            // For a single selected class, we show a top level item (saves 2 clicks); otherwise we show the full hierarchy
            if (bAllAreSameType && (FirstOdysseyBrush != NULL))
            {
                // Shortcut to edit the blueprint directly, saves two clicks
                FUIAction UIAction;
                UIAction.ExecuteAction.BindStatic(
                    &EditKismetCodeFor,
                    /*OdysseyBrush=*/ MakeWeakObjectPtr(FirstOdysseyBrush) );

                const FText Label = LOCTEXT("EditOdysseyBrush", "Edit OdysseyBrush");
                const FText Description = FText::Format( LOCTEXT("EditOdysseyBrush_ToolTip", "Opens {0} in the OdysseyBrush editor"), FText::FromString( FirstOdysseyBrush->GetName() ) );

                MenuBuilder.AddMenuEntry( Label, Description, FSlateIcon(), UIAction );
            }
            else
            {
                // More than one type of blueprint is selected, so add a sub-menu for "Edit Kismet Code"
                MenuBuilder.AddSubMenu(
                    LOCTEXT("EditOdysseyBrushSubMenu", "Edit OdysseyBrush"),
                    LOCTEXT("EditOdysseyBrushSubMenu_ToolTip", "Shows Blueprints that can be opened for editing"),
                    FNewMenuDelegate::CreateStatic( &FillEditCodeMenu, OdysseyBrushClasses ) );
            }

            // For any that aren't up to date, we offer a compile blueprints button
            if (bAreAnyNotUpToDate)
            {
                // Shortcut to edit the blueprint directly, saves two clicks
                FUIAction UIAction;
                UIAction.ExecuteAction.BindStatic(&RecompileOutOfDateKismetForSelection);

                const FText Label = LOCTEXT("CompileOutOfDateBPs", "Compile Out-of-Date Blueprints");
                const FText Description = LOCTEXT("CompileOutOfDateBPs_ToolTip", "Compiles out-of-date blueprints for selected actors");

                MenuBuilder.AddMenuEntry( Label, Description, FSlateIcon(), UIAction );
            }
        }
    }
    MenuBuilder.EndSection();
}

/**
 * Extends the level viewport context menu with blueprint-specific menu items
 */
TSharedRef<FExtender> ExtendLevelViewportContextMenuForBlueprints(const TSharedRef<FUICommandList> CommandList, TArray<AActor*> SelectedActors)
{
    TSharedPtr<FExtender> Extender = MakeShareable(new FExtender);

    Extender->AddMenuExtension("LevelViewportEdit", EExtensionHook::Before, CommandList,
        FMenuExtensionDelegate::CreateStatic(&FillOdysseyBrushOptions, SelectedActors));

    return Extender.ToSharedRef();
}

#undef LOCTEXT_NAMESPACE
