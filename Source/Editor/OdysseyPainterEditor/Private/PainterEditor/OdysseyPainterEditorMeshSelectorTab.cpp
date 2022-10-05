// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorMeshSelectorTab.h"

#include "Mesh/SOdysseyMeshSelector.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorMeshSelectorTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorMeshSelectorTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorMeshSelectorTab::~FOdysseyPainterEditorMeshSelectorTab()
{
}

FOdysseyPainterEditorMeshSelectorTab::FOdysseyPainterEditorMeshSelectorTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_MeshSelector"),
                            LOCTEXT( "OdysseyPainterEditorMeshSelectorTab", "Mesh Selector" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Mesh16" ))
    , mEditor(iEditor)
    , mMeshSelector(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorMeshSelectorTab::CreateWidget()
{
	mMeshSelector = SNew( SOdysseyMeshSelector )
        .OnMeshChanged_Raw(this, &FOdysseyPainterEditorMeshSelectorTab::OnMeshSelected );

    return mMeshSelector;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyMeshSelector>
FOdysseyPainterEditorMeshSelectorTab::MeshSelector()
{
    return mMeshSelector;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorMeshSelectorTab::OnMeshSelected( UStaticMesh* iMesh )
{
}

#undef LOCTEXT_NAMESPACE
