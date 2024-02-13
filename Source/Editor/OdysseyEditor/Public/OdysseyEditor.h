// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "ToolMenuOwner.h"
#include "OdysseyEditorTab.h"
#include "OdysseyEditorShortcuts.h"

class FOdysseyEditorGUI;
class FTabManager;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYEDITOR_API FOdysseyEditor
    : public FGCObject //Allows us to register External UObject in Garbage Collector
    , public FTickableEditorObject //Allows us to react to Tick events
    , public TSharedFromThis<FOdysseyEditor>
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnAddEditedObject, UObject*);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRemoveEditedObject, UObject*);

public:
    // Construction / Destruction
    virtual ~FOdysseyEditor();
    FOdysseyEditor(const FName& iId, const FText& iName, UObject* iEditedObject);

public:
    //Non Overridable Methods
    const FName& GetId() const;
    UObject* GetEditedObject() const;

    void AddEditedObject(UObject* iObject);
    void RemoveEditedObject(UObject* iObject);

    void AddTab(TSharedRef<FOdysseyEditorTab> iTab);
    template<class T> TSharedPtr<T> FindTab() const;
    const TArray<TSharedPtr<FOdysseyEditorTab>>& GetTabs() const;
    void InitTabs();
    void CloseAllTabs();

    void RegisterTabSpawners( const TSharedRef<FTabManager>& iTabManager );
    void UnregisterTabSpawners( const TSharedRef<FTabManager>& iTabManager );

    FOnAddEditedObject& OnAddEditedObjectDelegate();
    FOnRemoveEditedObject& OnRemoveEditedObjectDelegate();

    FOdysseyEditorShortcuts& GetShortcuts();

public:
    // Interface
    virtual void Initialize() = 0;
    virtual TSharedRef<FTabManager::FLayout> CreateLayout() = 0;
    virtual void BuildModeLayout(TSharedPtr<FAssetEditorModeUILayer> iModeUILayerPtr);

public:
    // Overridable Methods
    virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName );
    virtual void BindShortcuts(FBaseToolkit* iToolkit);
    virtual bool OnCloseRequested();
    virtual void OnClose();
    virtual TArray<UObject*> GetAdditionalEditedObjects();

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

    // FTickableEditorObject implementation
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT( FOdysseyEditor, STATGROUP_Tickables); }

public:
    FName mId;
    FText mName;
    UObject* mEditedObject;
    TArray<TSharedPtr<FOdysseyEditorTab>> mTabs;
    TArray<UObject*> mAdditionalEditedObjects;
    FString mTabsSaveFilename;

    FOnAddEditedObject mOnAddEditedObject;
    FOnRemoveEditedObject mOnRemoveEditedObject;

    FOdysseyEditorShortcuts mShortcuts;
};

template<class T>
TSharedPtr<T>
FOdysseyEditor::FindTab() const
{
    const FName& id = T::StaticId();
    for (const TSharedPtr<FOdysseyEditorTab> tab : mTabs)
    {
        if (tab->GetId() == id)
            return StaticCastSharedPtr<T>(tab);
    }
    return nullptr;
}