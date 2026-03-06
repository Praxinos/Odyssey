// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditor.h"
#include "ArianeEditorTool.h"
#include "ArianeEditorViewportToolkit.h"
#include "ArianeEditorToolTab.h"
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DActor.h"
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
// Unreal
#include "Toolkits/BaseToolkit.h"
#include "UObject/Object.h"
#include "EdMode.h"
#include "LevelEditor.h"

FArianeEditor::~FArianeEditor()
{
}

FArianeEditor::FArianeEditor( FArianeEditorViewportToolkit* iToolkit )
    : Toolkit( iToolkit )
    , Name("ArianeEditor")
    , CurrentTool ( nullptr )
{
}

FArianeEditorViewportToolkit*
FArianeEditor::GetToolkit()
{
    return Toolkit;
}

// Tools ------------------------------

const TArray<UArianeEditorTool*>&
FArianeEditor::GetTools()
{
    return Tools;
}

UArianeEditorTool*
FArianeEditor::GetCurrentTool()
{
    return CurrentTool;
}

void
FArianeEditor::RemoveTool( UArianeEditorTool* iTool )
{
    Tools.Remove( iTool );
}

void
FArianeEditor::AddTool( UArianeEditorTool* iTool )
{
    Tools.Add( iTool );
}

void
FArianeEditor::SetCurrentTool( UArianeEditorTool* iTool )
{
    OnPreChangeCurrentTool.Broadcast();

    if( CurrentTool )
    {
        CurrentTool->Unload();
    }

    CurrentTool = iTool;

    OnPostChangeCurrentTool.Broadcast();
}

void
FArianeEditor::InitTools()
{
    AddTool( NewObject<UArianeEditorPathDrawingTool>() );

    for( UArianeEditorTool* tool : Tools )
    {
        tool->SetEditor( this );
    }
}

// --------------------- Tabs

void
FArianeEditor::RemoveTab( TSharedPtr<FArianeEditorTab> iTab )
{
    Tabs.Remove( iTab );
}

void
FArianeEditor::AddTab( TSharedPtr<FArianeEditorTab> iTab)
{
    Tabs.Add( iTab );
}

void
FArianeEditor::InitTabs()
{
    AddTab( MakeShared<FArianeEditorToolTab>(this) );

    for (const TSharedPtr<FArianeEditorTab> tab : Tabs)
    {
        tab->Init();
    }
}

const TArray<TSharedPtr<FArianeEditorTab>>&
FArianeEditor::GetTabs() const
{
    return Tabs;
}

void
FArianeEditor::CloseAllTabs()
{
    for (TSharedPtr<FArianeEditorTab> tab : Tabs)
    {
        if ( tab->IsOpen() )
        {
            tab->Close();
        }
    }
}

void
FArianeEditor::RegisterTabSpawners()
{
    FLevelEditorModule& LevelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(TEXT("LevelEditor"));
    TSharedPtr<FTabManager> TabManager = LevelEditorModule.GetLevelEditorTabManager()->AsShared();
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = TabManager->AddLocalWorkspaceMenuCategory(FText::FromName( Name ));
    TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();

    for ( TSharedPtr<FArianeEditorTab> tab : Tabs )
    {
        tab->Register( workspaceMenuCategoryRef );
    }
}

void
FArianeEditor::UnregisterTabSpawners()
{
    for ( TSharedPtr<FArianeEditorTab> tab : Tabs )
    {
        tab->Unregister();
    }
}

// --------------------------

const FName&
FArianeEditor::GetId() const
{
    return Name;
}

void
FArianeEditor::Init()
{
    InitTools();
    InitTabs();
}

UWorld*
FArianeEditor::GetWorld()
{
    return Toolkit->GetEditorMode()->GetWorld();
}

void
FArianeEditor::AddPainting3DActor()
{
    GetWorld()->SpawnActor( AArianePainting3DActor::StaticClass() );
}

void
FArianeEditor::AddPainting3DComponent( const TArray<class AActor *> iActors )
{
    for( AActor* actor : iActors )
    {
        actor->AddComponentByClass( UArianePainting3DComponent::StaticClass(), false, FTransform(), false );
    }
}

FArianeEditor::FOnPreChangeCurrentTool&
FArianeEditor::OnPreChangeCurrentToolDelegate()
{
    return OnPreChangeCurrentTool;
}

FArianeEditor::FOnPostChangeCurrentTool&
FArianeEditor::OnPostChangeCurrentToolDelegate()
{
    return OnPostChangeCurrentTool;
}

void
FArianeEditor::AddReferencedObjects( FReferenceCollector& Collector )
{
    for ( TObjectPtr<UArianeEditorTool> tool : Tools )
    {
        Collector.AddReferencedObject(tool);
    }
}

FString
FArianeEditor::GetReferencerName() const
{
    return "FArianeEditor";
}
