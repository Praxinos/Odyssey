// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyEditor.h"

#include "OdysseyEditorGUI.h"

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
FOdysseyEditor::Init()
{
    mEditedObjects.Add(GetPrimaryEditedObject());

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

bool
FOdysseyEditor::OnCloseRequested()
{
    return true;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Interface

void
FOdysseyEditor::FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender)
{
	return GetGUI()->FillExtender(iToolkit, ioExtender);
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
