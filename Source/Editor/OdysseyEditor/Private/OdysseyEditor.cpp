// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyEditor.h"

#include "OdysseyEditorGUI.h"
#include "ToolMenus.h"

/////////////////////////////////////////////////////
// FOdysseyEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyEditor::~FOdysseyEditor()
{
}

FOdysseyEditor::FOdysseyEditor()
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyEditor::Initialize(UObject* iEditedObject)
{
    if (iEditedObject)
        mEditedObjects.Add(iEditedObject);

    InitData();
    InitGUI();
}

void
FOdysseyEditor::InitData()
{
}

void
FOdysseyEditor::InitGUI()
{
    GetGUI()->Init();
}
//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

void
FOdysseyEditor::Undo()
{

}

void
FOdysseyEditor::Redo()
{

}

void
FOdysseyEditor::ClearUndo()
{
    
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

FOdysseyEditor::FOnAddEditedObject&
FOdysseyEditor::OnAddEditedObjectDelegate()
{
    return mOnAddEditedObject;
}

FOdysseyEditor::FOnRemoveEditedObject&
FOdysseyEditor::OnRemoveEditedObjectDelegate()
{
    return mOnRemoveEditedObject;
}

TSharedRef<FTabManager::FLayout>
FOdysseyEditor::GetLayout()
{
    return GetGUI()->GetLayout();
}

TArray<UObject*>
FOdysseyEditor::GetEditedObjects()
{
    return mEditedObjects;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Listeners

void
FOdysseyEditor::OnToolkitInitialized(FBaseToolkit* iToolkit)
{
    GetGUI()->OnToolkitInitialized(iToolkit);
}

void
FOdysseyEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	GetGUI()->BindShortcuts(iToolkit);
}

bool
FOdysseyEditor::OnCloseRequested()
{
    return true;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Interface

void
FOdysseyEditor::ExtendMenu( FName iMenuName )
{
	GetGUI()->ExtendMenu( iMenuName );
    UToolMenus::Get()->RefreshAllWidgets();
}

void
FOdysseyEditor::UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager )
{
    GetGUI()->UnregisterTabSpawners(iTabManager);
}

void
FOdysseyEditor::AddEditedObject(UObject* iObject)
{
    mEditedObjects.Add(iObject);
    mOnAddEditedObject.Broadcast(iObject);
}

void
FOdysseyEditor::RemoveEditedObject(UObject* iObject)
{
    mEditedObjects.Remove(iObject);
    mOnRemoveEditedObject.Broadcast(iObject);
}
