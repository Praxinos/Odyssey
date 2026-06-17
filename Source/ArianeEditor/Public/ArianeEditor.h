// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
// Odyssey Headers
#include "OdysseyPainterEditorColorType.h"
#include "InteractiveTool.h"
// ULIS Headers
#include <ULIS>

#include "ArianeEditorHUD.h"

class UArianeEditorTool;
class FArianeEditorViewportToolkit;
class FArianeEditorTab;
class FTabManager;
class UWorld;
class AArianePainting3DActor;
class UArianePainting3DComponent;
class UInteractiveToolManager;
class UArianeEditorToolBuilder;

/**
 * Base class for a Painting Editor
 */
class ARIANEEDITOR_API FArianeEditor
    : public FGCObject //Allows us to register External UObject in Garbage Collector
    , public TSharedFromThis<FArianeEditor>
{
public:
    DECLARE_MULTICAST_DELEGATE( FOnCurrentToolChanged );
    DECLARE_MULTICAST_DELEGATE( FOn3DPaintingComponentSelectionChanged );

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
     * @return the newly created Painting 3D Actor
     */
    AArianePainting3DActor* AddPainting3DActor();

    /**
     * @brief Close all Tabs
     */
    void CloseAllTabs();

    /** Init */
    void Init();

    /**
     * @brief Get the color type
     * @return the color type, either Raw or Indexed
     */
    EOdysseyPainterEditorColorType GetColorType();

    /**
     * @brief Get the current tool
     * @return the current tool
     */
    UArianeEditorTool* GetCurrentTool();

    /**
     * @brief Get the current mouse cursor to display
     * @return the current mouse cursor to display
     */
    bool GetCursor( EMouseCursor::Type& OutCursor );

    const FName& GetId() const;

    /**
     * @brief Get the current paint color
     * @return the current paint color
     */
    ::ULIS::FColor GetPaintColor();

    /**
     * @brief Get a tool by type
     * @return the tool matching the type
     */
    UArianeEditorTool* GetTool( const FString& ToolType );

    /**
     * @brief Get all tools
     * @return an array of all the tools
     */
    const TArray<UArianeEditorTool*>& GetTools();

    /**
     * @brief Get all tabs
     * @return an array of tabs
     */
    const TArray<TSharedPtr<FArianeEditorTab>>& GetTabs() const;

    /**
     * @brief Get the Tool Manager
     * @return the Tool Manager
     */
    UInteractiveToolManager* GetToolManager();

    /**
     * @brief Get the toolkit
     * @return the toolkit
     */
    FArianeEditorViewportToolkit* GetToolkit();

    /**
     * @brief Get the World
     * @return The World
     */
    UWorld* GetWorld();

    /**
     * @brief Check if the current tool is of the type passed as an argument
     * @param ToolType the tool type
     */
    bool IsCurrentTool( const FString& ToolType );

    /** Called once the UI is inited **/
    void PostInit();

    /**
     * @brief Register tab spawners
     * @param TabManager the tab manager
     */
    void RegisterTabSpawners();

    /**
     * @brief Set the color type
     * @param InColorType the color type, either Raw or Indexed
     */
    void SetColorType( EOdysseyPainterEditorColorType& InColorType );

    /**
     * @brief Set the current tool
     * @param Tool the tool to set as the current tool
     * @param PreviousToolShutdownType how to terminate the previous tool
     * @param TriggerEvent call Pre/Post CurrentToolChanged delegate
     */
    void SetCurrentTool( UArianeEditorTool* Tool
                       , EToolShutdownType PreviousToolShutdownType
                       , bool TriggerEvent );

    /**
     * @brief Set the current tool
     * @param ToolType the tool type to set as the current tool
     * @param PreviousToolShutdownType how to terminate the previous tool
     * @param TriggerEvent call Pre/Post CurrentToolChanged delegate
     */
    void SetCurrentTool( const FString& ToolType
                       , EToolShutdownType PreviousToolShutdownType
                       , bool TriggerEvent  );

    /**
     * @brief Set the current paint color
     * @param InPaintcolor the paint color to use as the current
     */
    void SetPaintColor( const ::ULIS::FColor& InPaintcolor );

    /**
     * @brief Ticks
     * @param DeltaTime the time that has passed since the last call
     */
    void Tick( float DeltaTime );

    /**
     * @brief Unregister tab spawners
     * @param TabManager the tab manager
     */
    void UnregisterTabSpawners();

    // FGCObject overrides
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

    // Delegates
    FOnCurrentToolChanged& OnPreCurrentToolChangedDelegate();
    FOnCurrentToolChanged& OnPostCurrentToolChangedDelegate();
    FOn3DPaintingComponentSelectionChanged& OnPre3DPaintingComponentSelectionChangedDelegate();
    FOn3DPaintingComponentSelectionChanged& OnPost3DPaintingComponentSelectionChangedDelegate();
    FSimpleDelegate& OnRegenerateToolbarAndMenusDelegate();

    void ExtendLevelEditorToolbar( UToolMenu* iToolbar );
    UArianePainting3DComponent* GetCurrentPainting3DComponent();

    /** Register all tools */
    void RegisterTools();
    /** Unresgister all tools */
    void UnregisterTools();

    const FArianeEditorHUD::FDrawingFlags& GetHUDDrawingFlags();

protected:
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

    void ExtendToolbarSaveAssetButton( UToolMenu* iToolMenu );
    void ExtendToolbarToolParameters( UToolMenu* iToolMenu );
    void ClearPainting3DComponents();
    void OnEditorSelectionChanged( UObject* NewSelection );
    void AddToolBuilder( UArianeEditorToolBuilder* ToolBuilder );


protected:
    FSimpleDelegate OnRegenerateToolbarAndMenus;

    FOnCurrentToolChanged OnPreCurrentToolChanged;
    FOnCurrentToolChanged OnPostCurrentToolChanged;

    FOn3DPaintingComponentSelectionChanged OnPre3DPaintingComponentSelectionChanged;
    FOn3DPaintingComponentSelectionChanged OnPost3DPaintingComponentSelectionChanged;

    FArianeEditorViewportToolkit* Toolkit;
    FName Name;
    FName ToolbarMenuName;
    TArray<UArianeEditorTool*> Tools;
    TArray<TSharedPtr<FArianeEditorTab>> Tabs;
    //UArianeEditorTool* CurrentTool;
    TWeakObjectPtr<UArianePainting3DComponent> CurrentPainting3DComponent;
    ::ULIS::FColor PaintColor;
    EOdysseyPainterEditorColorType ColorType;
    FArianeEditorHUD::FDrawingFlags HUDDrawingFlags;
};
