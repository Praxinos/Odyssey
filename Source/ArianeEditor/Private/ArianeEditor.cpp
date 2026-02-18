// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEditor.h"
#include "ArianeEditorTool.h"
#include "ArianeEditorToolTab.h"
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DActor.h"
#include "PathDrawingTool/ArianeEditorPathDrawingTool.h"
// Unreal
#include "Toolkits/BaseToolkit.h"
#include "UObject/Object.h"
#include "EdMode.h"

FArianeEditor::~FArianeEditor()
{
}

FArianeEditor::FArianeEditor( FModeToolkit* iToolkit )
    : mToolkit( iToolkit )
    , mName("ArianeEditor")
    , mCurrentTool ( nullptr )
{
}

FModeToolkit*
FArianeEditor::GetToolkit()
{
    return mToolkit;
}

// Tools ------------------------------

const TArray<UArianeEditorTool*>&
FArianeEditor::GetTools()
{
    return mTools;
}

UArianeEditorTool*
FArianeEditor::GetCurrentTool()
{
    return mCurrentTool;
}

void
FArianeEditor::RemoveTool( UArianeEditorTool* iTool )
{
    mTools.Remove( iTool );
}

void
FArianeEditor::AddTool( UArianeEditorTool* iTool )
{
    mTools.Add( iTool );
}

void
FArianeEditor::SetCurrentTool( UArianeEditorTool* iTool )
{
    OnPreChangeCurrentTool.Broadcast();

    if( mCurrentTool )
    {
        mCurrentTool->Unload();
    }

    mCurrentTool = iTool;

    OnPostChangeCurrentTool.Broadcast();
}

void
FArianeEditor::InitTools()
{
    AddTool( NewObject<UArianeEditorPathDrawingTool>() );

    for( UArianeEditorTool* tool : mTools )
    {
        tool->SetEditor( this );
    }
}

// --------------------- Tabs

void
FArianeEditor::RemoveTab( TSharedPtr<FArianeEditorTab> iTab )
{
    mTabs.Remove( iTab );
}

void
FArianeEditor::AddTab( TSharedPtr<FArianeEditorTab> iTab)
{
    mTabs.Add( iTab );
}

void
FArianeEditor::InitTabs()
{
    AddTab( MakeShared<FArianeEditorToolTab>(this) );

    for (const TSharedPtr<FArianeEditorTab> tab : mTabs)
    {
        tab->Init();
    }
}

const TArray<TSharedPtr<FArianeEditorTab>>&
FArianeEditor::GetTabs() const
{
    return mTabs;
}

void
FArianeEditor::CloseAllTabs()
{
    for (TSharedPtr<FArianeEditorTab> tab : mTabs)
    {
        if ( tab->IsOpen() )
        {
            tab->Close();
        }
    }
}

void
FArianeEditor::RegisterTabSpawners( const TSharedRef<FTabManager>& iTabManager )
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(FText::FromName( mName ));
    TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();

    for ( TSharedPtr<FArianeEditorTab> tab : mTabs )
    {
        tab->SetTabManager( iTabManager );
        tab->Register( workspaceMenuCategoryRef );
    }
}

void
FArianeEditor::UnregisterTabSpawners( const TSharedRef< FTabManager >& iTabManager )
{
    for ( TSharedPtr<FArianeEditorTab> tab : mTabs )
    {
        tab->Unregister();
    }
}

// --------------------------

const FName&
FArianeEditor::GetId() const
{
    return mName;
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
    return mToolkit->GetEditorMode()->GetWorld();
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

        //actor->AddComponent( );
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
    for ( TObjectPtr<UArianeEditorTool> tool : mTools )
    {
        Collector.AddReferencedObject(tool);
    }
}

FString
FArianeEditor::GetReferencerName() const
{
    return "FArianeEditor";
}
