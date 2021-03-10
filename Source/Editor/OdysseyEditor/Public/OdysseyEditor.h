// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

class FOdysseyEditorGUI;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYEDITOR_API FOdysseyEditor
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
    virtual void InitData();
    virtual void InitGUI();

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
    virtual void FillExtender(FBaseToolkit* iToolkit, TSharedPtr<FExtender>& ioExtender);
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager );
    virtual void AddEditedObject(UObject* iObject);
    virtual void RemoveEditedObject(UObject* iObject);

public:
    TArray<UObject*> mEditedObjects;

    FOnAddEditedObject mOnAddEditedObject;
    FOnRemoveEditedObject mOnRemoveEditedObject;
};
