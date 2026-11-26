// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
// Ariane Headers
#include "ArianeLayerVectorEnums.h"
// Ariane Editor Headers
#include "ArianeEditorHUD.h"
#include "ArianeEditorSubsystem.h"
// Odyssey Headers
#include "OdysseyPainterEditorColorType.h"
#include "InteractiveTool.h"
#include "InteractiveToolManager.h"
#include "UObject/ObjectSaveContext.h"
// ULIS Headers
#include <ULIS>

#include "ArianeEditor.generated.h"

class UArianeEditorTool;
class FArianeEditorViewportToolkit;
class FArianeEditorTab;
class FTabManager;
class UWorld;
class AArianePainting3DActor;
class UArianePainting3DComponent;
class UInteractiveToolManager;
class UArianeEditorToolBuilder;
class UArianeLayerVector;
class UArianeLayer;
class UArianeLayerFolder;
class UArianeLayerStack;
struct FArianePrimitive;
struct FArianeObject;

UENUM()
enum class EArianeEditorDrawingCoordinateSystem : uint8
{
    World,
    Layer,
    CommonAncestor,
    Local,
};

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
     * @brief True while the open Sequencer plays or scrubs: the drawing changes every frame
     */
    bool IsSequencerPlaying() const;

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

    void SetCurrentPainting3DComponent( UArianePainting3DComponent* InPainting3DComponent );
    EArianeDrawingOrigin GetDrawingOrigin();
    EArianeDrawingOrientation GetDrawingOrientation();
    EArianeEditorDrawingCoordinateSystem GetDrawingCoordinateSystem();
    void SetDrawingDistanceToPlane( float InDrawingDistanceToPlane );
    float GetDrawingDistanceToPlane();
    bool GetShowGrid() const;

    static FArianeClipboard& GetClipboard();

    void GroupSelectedObjects( const FName& NewGroupName );
    void UngroupSelectedGroups();
    void DeleteSelectedObjects();
    void CopySelectedObjects();
    void PasteObjects();
    void ConvertSelectedPrimitives();
    void CopySelectedLayers();
    void PasteLayers();
    void SelectAllObjects();
    UArianeLayerVector* GetCurrentVectorLayer();

protected:
    /**
     * @brief Add a tab
     * @param Tab the tab to add
     */
    void AddTab( TSharedPtr<FArianeEditorTab> Tab );

    /** Duplicates a layer and, for a folder, all its children (DuplicateObject alone would share them) */
    UArianeLayer* DuplicateLayerTree( UArianeLayer* SourceLayer, UObject* Outer );

    /** Pastes a copy of a clipboard layer tree into FosterFolder, attaching and registering each level */
    void PasteLayerTree( UArianeLayerStack* LayerStack, UArianeLayerFolder* FosterFolder, UArianeLayer* ClipboardLayer );

    /**
     * @brief Remove a tab
     * @param Tab the tab to remove
     */
    void RemoveTab( TSharedPtr<FArianeEditorTab> Tab );

    /** Init all tabs */
    void InitTabs();

    void ExtendToolbarSaveAssetButton( UToolMenu* iToolMenu );
    void ExtendToolbarToolParameters( UToolMenu* iToolMenu );
    void SetDrawingOrigin( EArianeDrawingOrigin InDrawingOrigin);
    void SetDrawingOrientation( EArianeDrawingOrientation InDrawingOrientation);
    TSharedRef<SWidget> CreateDrawingOrientationSegmentControl( const FText& InLabel );
    TSharedRef<SWidget> CreateDrawingCoordinateSystemComboBox( const FText& InLabel );
    TSharedRef<SWidget> CreateDrawingOriginComboBox( const FText& InLabel );
    TSharedRef<SWidget> CreateDrawingDistanceToPlaneSpinBox( const FText& InLabel );
    void OnDrawingOriginChanged( int32 EnumID, ESelectInfo::Type );
    const FSlateBrush* GetDrawingOrientationBackgroundBrush( EArianeDrawingOrientation InDrawingOrientation );
    void ClearPainting3DComponents();
    void OnEditorSelectionChanged( UObject* NewSelection );
    void AddToolBuilder( UArianeEditorToolBuilder* ToolBuilder );
    void OnDrawingCoordinateSystemChanged( int32, ESelectInfo::Type );
    void OnToolEnded( UInteractiveToolManager* Manager, UInteractiveTool *Tool );
    void OnObjectPreSave( UObject* SavedObject, FObjectPreSaveContext Context );
    static void ConvertPrimitives( UArianePainting3DComponent* Painting3DComponent
                                 , TArray<FArianePrimitive*>& PrimitivesToConvert );

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
    float DrawingDistanceToPlane;
    EArianeDrawingOrigin DrawingOrigin;
    EArianeDrawingOrientation DrawingOrientation;
    EArianeEditorDrawingCoordinateSystem DrawingCoordinateSystem;
    bool bShowGrid;
    FString LastActiveToolIdentifier;
};
