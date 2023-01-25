// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "ToolMenuOwner.h"

class FOdysseyEditorGUI;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYEDITOR_API FOdysseyEditor
    : public FGCObject //Allows us to register External UObject in Garbage Collector
    , public FTickableEditorObject //Allows us to react to Tick events
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddEditedObject, UObject*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoveEditedObject, UObject*);

public:
    // Construction / Destruction
    virtual ~FOdysseyEditor();
    FOdysseyEditor();

public:
    // Initialization
    void Initialize(UObject* iEditedObject);

protected:
    // Protected Initialization

    // Seperating Init from InitGUI allows us to seperate Data and GUI initialization also in derived classes
    virtual void InitData(UObject* iEditedObject);
    virtual void InitGUI();

public:
    // Undo
    virtual void Undo();
    virtual void Redo();
    virtual void ClearUndo();

public:
    // Getters
    FOnAddEditedObject& OnAddEditedObjectDelegate();
    FOnRemoveEditedObject& OnRemoveEditedObjectDelegate();
    virtual TSharedRef<FTabManager::FLayout> GetLayout();
    virtual TArray<UObject*> GetEditedObjects();
    virtual FOdysseyEditorGUI* GetGUI() = 0;

public:
    // Listeners
    virtual void OnToolkitInitialized(FBaseToolkit* iToolkit);
    virtual bool OnCloseRequested();

public:
    // Interface
    virtual void BindShortcuts(FBaseToolkit* iToolkit);
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager );
    virtual void AddEditedObject(UObject* iObject);
    virtual void RemoveEditedObject(UObject* iObject);

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

    // FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( FOdysseyEditor, STATGROUP_Tickables); }

public:
    TArray<UObject*> mEditedObjects;

    FOnAddEditedObject mOnAddEditedObject;
    FOnRemoveEditedObject mOnRemoveEditedObject;
};
