// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"

class UArianeEditorTool;
class FArianeEditorViewportToolkit;
class FArianeEditorTab;
class FTabManager;
class UWorld;

/**
 * Base class for a Painting Editor
 */
class ARIANEEDITOR_API FArianeEditor
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{
public:
    DECLARE_MULTICAST_DELEGATE( FOnPreChangeCurrentTool );
    DECLARE_MULTICAST_DELEGATE( FOnPostChangeCurrentTool );

public:
    ~FArianeEditor();
    FArianeEditor( FArianeEditorViewportToolkit* iToolkit );

    /**
        * @brief Add a painting3D Component to specified actors
        * @param Actors an array of actors
        */
    void AddPainting3DComponent( const TArray<class AActor *> Actors );

    /**
        * @brief Add a painting3D Actor (Actor+Component)
        */
    void AddPainting3DActor();

    /**
        * @brief Close all Tabs
        */
    void CloseAllTabs();

    /** Init */
    void Init();

    /**
        * @brief Get the currentl tool
        * @return the current tool
        */
    UArianeEditorTool* GetCurrentTool();

    const FName& GetId() const;

    /**
        * @brief Get all tabs
        * @return an array of tabs
        */
    const TArray<TSharedPtr<FArianeEditorTab>>& GetTabs() const;

    /**
        * @brief Get the toolkit
        * @return the toolkit
        */
    FArianeEditorViewportToolkit* GetToolkit();

    /**
        * @brief Get the tools
        * @return an array of all the tools
        */
    const TArray<UArianeEditorTool*>& GetTools();

    /**
        * @brief Get the World
        * @return The World
        */
    UWorld* GetWorld();

    /**
        * @brief Register tab spawners
        * @param TabManager the tab manager
        */
    void RegisterTabSpawners();

    /**
        * @brief Set the current tool
        * @param Tool the tool to set as the current tool
        */

    void SetCurrentTool( UArianeEditorTool* Tool );

    /**
        * @brief Unregister tab spawners
        * @param TabManager the tab manager
        */
    void UnregisterTabSpawners();


    // FGCObject overrides
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

    // Delegates
    FOnPreChangeCurrentTool& OnPreChangeCurrentToolDelegate();
    FOnPostChangeCurrentTool& OnPostChangeCurrentToolDelegate();

protected:
    /**
        * @brief Add a tool
        * @param Tool the tool to add
        */
    void AddTool( UArianeEditorTool* Tool );

    /**
        * @brief Removee a tool
        * @param Tool the tool to remove
        */
    void RemoveTool( UArianeEditorTool* Tool );

    /** Init all tools */
    void InitTools();

    /**
        * @brief Add a tab
        * @param Tab the tab to add
        */
    void AddTab( TSharedPtr<FArianeEditorTab> Tab );

    /**
        * @brief Remove a tab
        * @param Tab the tab to remove
        */
    void RemoveTab( TSharedPtr<FArianeEditorTab> Tab );

    /** Init all tabs */
    void InitTabs();


protected:
    FOnPreChangeCurrentTool OnPreChangeCurrentTool;
    FOnPostChangeCurrentTool OnPostChangeCurrentTool;

    FArianeEditorViewportToolkit* Toolkit;
    FName Name;
    TArray<UArianeEditorTool*> Tools;
    TArray<TSharedPtr<FArianeEditorTab>> Tabs;
    UArianeEditorTool* CurrentTool;
};
