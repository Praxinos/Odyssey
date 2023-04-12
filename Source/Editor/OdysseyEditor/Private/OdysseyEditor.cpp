// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyEditor.h"

#include "OdysseyEditorGUI.h"
#include "ToolMenus.h"
#include "ToolMenuOwner.h"

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

    InitData(iEditedObject);
    InitGUI();
}

void
FOdysseyEditor::InitData(UObject* iEditedObject)
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
    GEditor->UndoTransaction(true);
}

void
FOdysseyEditor::Redo()
{
    GEditor->RedoTransaction();
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
FOdysseyEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	GetGUI()->ExtendMenu( iOwner, iMenuName );
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
    
}

FString
FOdysseyEditor::GetReferencerName() const
{
    return "FOdysseyEditor";
}

//--------------------------------------------------------------------------------------
//------------------------------------------------- FTickableEditorObject implementation

void
FOdysseyEditor::Tick(float DeltaTime)
{
    
}