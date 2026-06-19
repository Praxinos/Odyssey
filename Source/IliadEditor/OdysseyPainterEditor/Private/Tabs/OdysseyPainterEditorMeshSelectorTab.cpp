// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorMeshSelectorTab.h"

#include "Mesh/SOdysseyMeshSelector.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorMeshSelectorTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_MeshSelector"); //Keep ColorSelector instead of ColorWheel because changing that ID would show an empty panel to users who already opened the previous ColorSelector Panel
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorMeshSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorMeshSelectorTab::~FOdysseyPainterEditorMeshSelectorTab()
{
}

FOdysseyPainterEditorMeshSelectorTab::FOdysseyPainterEditorMeshSelectorTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "mesh-selector-tab.name", "Mesh Selector" ), FSlateIcon( "OdysseyStyle", "PainterEditor.Mesh16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorMeshSelectorTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorMeshSelectorTab::CreateWidget()
{
    return SNew( SOdysseyMeshSelector )
            .MeshSelector(mEditor->GetMeshSelector())
            .OnMeshChanged_Raw(this, &FOdysseyPainterEditorMeshSelectorTab::OnMeshSelected );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorMeshSelectorTab::OnMeshSelected( UStaticMesh* iMesh )
{
}

#undef LOCTEXT_NAMESPACE
