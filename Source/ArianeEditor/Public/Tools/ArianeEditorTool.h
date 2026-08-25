// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Framework/MultiBox/MultiBoxExtender.h"
#include "InteractiveTool.h"
#include "BaseBehaviors/BehaviorTargetInterfaces.h"
// Ariane Editor
#include "ArianePointerState.h"
#include "ArianeEditorHUD.h"
// OdysseyHeaders
#include "OdysseyPainterEditorColorType.h"
#include "StylusInputHandler.h"
// Common
#include <chrono>

#include "ArianeEditorTool.generated.h"

class FArianeEditor;
struct FArianeVertex;
struct FArianeSegment;
struct FArianeSegmentCubic;
struct FArianeObject;
struct FArianePath;
struct FArianePoint;
struct FArianeHandleSegment;
class FSceneView;
class FStylusState;
struct FSlateBrush;
class UArianeLayer;
class UArianeLayerDrawing;
class FArianeEditorToolInputProcessor;

UCLASS(Abstract)
class ARIANEEDITOR_API UArianeEditorTool : public UInteractiveTool
                                         , public IClickDragBehaviorTarget
                                         , public IHoverBehaviorTarget
                                         , public FOdysseyStylusInputHandler
{
    GENERATED_BODY()

public:
    enum class eStylusEventFence
    {
        kNone,
        kStylusUp,
        kStylusDown,
    };

    struct FPointQuadTreeEntry
    {
        FArianePoint* Point;
        FVector2D HUDPosition;

        FPointQuadTreeEntry( FArianePoint* InPoint, const FVector2D& InHUDPosition )
        {
            Point = InPoint;
            HUDPosition = InHUDPosition;
        }
    };

    struct ARIANEEDITOR_API FPointQuadTree
    {
        public:
           ~FPointQuadTree();
           FPointQuadTree( const FIntRect& InRect
                         , uint32 MaxPointsPerQuad
                         , TArray<FPointQuadTreeEntry>& PointQuadTreeEntries
                         , uint32 Depth
                         , uint32 MaxDepth );

            void Build( uint32 MaxPointsPerQuad
                      , TArray<FPointQuadTreeEntry>& ParentPointQuadTreeEntries
                      , uint32 Depth
                      , uint32 MaxDepth );
            //void Draw( BLContext* iBLContext
            //         , FOdysseyVectorGroupPaint* iScene
            //         , uint64 iFlags );
            void PickPoints( const FVector2D& HUDPosition
                           , double SelectionRadius
                           , TArray<FArianePoint*>& OutPickedPoints );
        private:
            TArray<FPointQuadTreeEntry> PointQuadTreeEntries;
            FPointQuadTree* Children[4];
            FIntRect Rect;
    };

    struct FPickingFlags
    {
        public:
            FPickingFlags& SetPathVertex()        { PathVertex         = true; return *this; };
            FPickingFlags& SetPathSegment()       { PathSegment        = true; return *this; };
            FPickingFlags& SetPathSegmentHandle() { PathSegmentHandle  = true; return *this; };
            FPickingFlags& SetPathVertexHandle()  { PathVertexHandle   = true; return *this; };

        public:
            bool PathVertex         : 1 = 0;
            bool PathSegment        : 1 = 0;
            bool PathSegmentHandle  : 1 = 0;
            bool PathVertexHandle   : 1 = 0;
    };

public:
    // Destructor
    virtual ~UArianeEditorTool();

    //Constructor
    UArianeEditorTool();

public:
    virtual FString GetType() { return FString(); };

public:
    // UInteractiveTool::Setup
    virtual void Setup() override;
    // UInteractiveTool::Shutdown
    virtual void Shutdown( EToolShutdownType ShutdownType ) override;

    // Implements IClickDragBehaviorTarget::CanBeginClickSequence
    virtual FInputRayHit CanBeginClickDragSequence(const FInputDeviceRay& PressPos) override;
    // Implements IClickDragBehaviorTarget::OnClickPress
    virtual void OnClickPress(const FInputDeviceRay& PressPos) override;
    // Implements IClickDragBehaviorTarget::OnClickDrag
    virtual void OnClickDrag(const FInputDeviceRay& DragPos) override;
    // Implements IClickDragBehaviorTarget::OnClickRelease
    virtual void OnClickRelease(const FInputDeviceRay& ReleasePos) override;
    // Implements IClickDragBehaviorTarget::OnTerminateDragSequence
    virtual void OnTerminateDragSequence()override;

    // Implements IHoverBehaviorTarget::FInputRayHit
    virtual FInputRayHit BeginHoverSequenceHitTest(const FInputDeviceRay& PressPos) override;
    // Implements IHoverBehaviorTarget::OnBeginHover
    virtual void OnBeginHover(const FInputDeviceRay& DevicePos) override;
    // Implements IHoverBehaviorTarget::OnUpdateHover
    virtual bool OnUpdateHover(const FInputDeviceRay& DevicePos) override;
    // Implements IHoverBehaviorTarget::OnEndHover
    virtual void OnEndHover() override;

    // Implements UInteractiveTool::Render for rendering in 3D space
    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

    //Mouse events
    virtual bool OnMouseDown( FEditorViewportClient* iViewportClient
                            , FSceneView* View
                            , const FKey& iKey
                            , const FArianePointerState& State
                            , bool iRepeat = false );
    virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                             , FSceneView* View
                             , const FArianePointerState& State );
    virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                            , FSceneView* View
                            , const FKey& iKey
                            , const FArianePointerState& State );
    virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                          , FSceneView* View
                          , const FKey& iKey
                          , const FArianePointerState& State );
    virtual bool OnMouseClick( FEditorViewportClient* iViewportClient
                             , FSceneView* View
                             , const FKey& iKey
                             , const FArianePointerState& State );

    virtual bool OnMouseEnter( FEditorViewportClient* ViewportClient,
                               FViewport* Viewport,
                               int32 x,
                               int32 y );

    virtual bool OnMouseLeave( FEditorViewportClient* ViewportClient,
                               FViewport* Viewport );

    // Implements UInteractiveTool::DrawHUD for overlay drawing in 2D space
    virtual void DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI ) override;

    /** Get the tool's tooltip */
    virtual FText GetTooltip() const;
    /** Is the tool activable ? */
    virtual bool IsActivable() const;
    /** Is the tool activated ? */
    virtual bool IsActivated() const;

    /** Callback for when the tool is activated */
    virtual void Activate();
    /** Callback for when the tool is inactivated */
    virtual void Inactivate();

    virtual bool SupportsColorType( EOdysseyPainterEditorColorType ColorType );

    /**
     * @brief Set the editor
     * @param InEditor
     */
    virtual void Init( FArianeEditor* InEditor );

    /**
     * @brief Get the current mouse cursor to display
     * @return the current mouse cursor to display
     */
    virtual bool GetCursor( EMouseCursor::Type& OutCursor );


    bool ProcessKeyUpGlobal(const FKeyEvent& InKeyEvent);
    bool ProcessKeyDownGlobal(const FKeyEvent& InKeyEvent);

    // For global key press events
    virtual bool OnKeyUpGlobal(const FKeyEvent& InKeyEvent);
    virtual bool OnKeyDownGlobal(const FKeyEvent& InKeyEvent);

public:
    // UInteractiveTool
    virtual void OnTick(float DeltaTime) override;

private:
    /** FOdysseyStylusInputHandler Overrides and utilities */
    virtual void OnPacket(const UE::StylusInput::FStylusInputPacket& iPacket, UE::StylusInput::IStylusInputInstance* iInstance) override;
    void StartStylusInputRecord(const FKey& iMouseButton);
    void StopStylusInputRecord();
    void ReadStylusInput(eStylusEventFence iUntilEventType = eStylusEventFence::kNone);
    FArianePointerState StylusPacketToArianePointerState(const UE::StylusInput::FStylusInputPacket& iPacket);

protected:
    void PopupContextMenu();
    TSharedPtr<SWidget> CreateContextMenu();
    virtual void ExtendContextMenu( FMenuBuilder& menu );

    /**
     * @brief Get a layer's drawing plane
     * @param ViewportClient
     * @param DrawingLayer the layer from get the plane from
     */
    FPlane GetDrawingPlane( FEditorViewportClient* ViewportClient, UArianeLayerDrawing* DrawingLayer );

    /**
     * @brief Get the active viewport client
     * @return the active viewport client
     */
    FEditorViewportClient* GetActiveViewportClient();

    /**
     * @brief Draw the layer orientation grid
     * @param RenderAPI
     * @param DrawingLayer
     */
    void DrawLayerOrientationGrid( IToolsContextRenderAPI* RenderAPI, UArianeLayerDrawing* DrawingLayer );

    /**
     * @brief Get the current layer
     * @return the current layer if any Painting3DActor and a layer are selected
     */
    UArianeLayer* GetCurrentLayer();

    /** Is the tool allowed to act on the viewport ? */
    virtual bool CanDraw();

   /**
     * @brief converts screen position to HUD position (i.e with DPI scaling)
     * @param ViewportClient
     * @param ScreenPosition
     */
    static FVector2D ScreenToHUD( FEditorViewportClient* ViewportClient, const FVector2D& ScreenPosition );

    static bool WorldToPixel( FSceneView* View, const FVector& WorldPosition, FVector2D& OutHUDPosition );
    static bool WorldToHUD( FEditorViewportClient* ViewportClient
                          , FSceneView* View
                          , const FVector& WorldPosition
                          , FVector2D& OutHUDPosition );
/* Gary

    virtual bool OnMouseClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey );
    virtual bool OnMouseDoubleClick(const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey);
    // For global key press events
    virtual bool OnKeyUpGlobal(const FKeyEvent& InKeyEvent);
    virtual bool OnKeyDownGlobal(const FKeyEvent& InKeyEvent);
*/

public:
    virtual void ExtendToolbar( UToolMenu* iToolMenu );

/* Gary
    virtual void BindShortcuts(TSharedPtr<FUICommandList> iCommandList);
    virtual void ExtendMenu( TSharedRef<FExtender> iExtender );
    virtual TSharedPtr<FOdysseyHUDElement> GetHUD();
*/

protected:
    virtual void PropertyChanged(const FName& iPropertyName);
    virtual void PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive);
    virtual void PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive);

protected:
    // UObject overrides
    virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent) override;
    virtual void PostTransacted(const FTransactionObjectEvent& iTransactionEvent) override;
    virtual void PostInitProperties() override;
    virtual void PostDuplicate(EDuplicateMode::Type DuplicateMode) override;

protected:
    // Temp
    void FlushStylusInput();
    void ListenStylusInput();
    void IgnoreStylusInput();

    // Picking
protected:
    static void MapPath( FEditorViewportClient* ViewportClient
                       , FSceneView* View
                       , FArianePath* Path
                       , const FIntRect& Rect
                       , TArray<FPointQuadTreeEntry>& OutPointQuadTreeEntries );
    static void MapPoints( FEditorViewportClient* ViewportClient
                         , FSceneView* View
                         , FArianeObject* Object
                         , const FIntRect& Rect
                         , TArray<FPointQuadTreeEntry>& OutPointQuadTreeEntries );

    void MakePointQuadTree( FEditorViewportClient* ViewportClient
                          , FSceneView* View
                          , TArray<UArianeLayerDrawing*> DrawingLayers
                          , bool bFocusedObjectsOnly );

    static void GetVertexHandlePositions( FArianeVertex* Vertex, FVector OutVertexHandlePositions[2] );
    bool PickPathPoints( FEditorViewportClient* ViewportClient
                       , FSceneView* View
                       , FArianePath* Path
                       , double ViewportX
                       , double ViewportY
                       , double PickingRadius
                       , TArray<FArianeVertex*>& OutPickedVertices
                       , TArray<FArianeHandleSegment*>& OutPickedHandles
                       , const FPickingFlags& PickingFlags );
    FVector2D WorldToPlane( const FVector& WorldPosition, const FPlane& ProjectionPlane );

    // HUD
protected:
    void DrawLineHUD( FCanvas* Canvas
                    , FEditorViewportClient* ViewportClient
                    , FSceneView* View
                    , const FVector2D& HUDCoordsP0
                    , const FVector2D& HUDCoordsP1
                    , const FLinearColor& Color
                    , float Thickness );
    void DraweOutlinedLineHUD( FCanvas* Canvas
                             , FEditorViewportClient* ViewportClient
                             , FSceneView* View
                             , const FVector2D& HUDCoordsP0
                             , const FVector2D& HUDCoordsP1
                             , const FLinearColor& Color
                             , float Thickness );
    void DrawVertexHUD( FCanvas* Canvas
                      , FEditorViewportClient* ViewportClient
                      , FSceneView* View
                      , FArianeVertex* Vertex
                      , const FLinearColor& FgColor
                      , const FLinearColor& BgColor
                      , const FLinearColor& HcColor
                      , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags );
    void DrawCubicSegmentHUD( FCanvas* Canvas
                            , FEditorViewportClient* ViewportClient
                            , FSceneView* View
                            , FArianeSegmentCubic* CubicSegment
                            , const FLinearColor& FgColor
                            , const FLinearColor& BgColor
                            , const FLinearColor& HcColor
                            , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags );
    void DrawSegmentHUD( FCanvas* Canvas
                       , FEditorViewportClient* ViewportClient
                       , FSceneView* View
                       , FArianeSegment* CubicSegment
                       , const FLinearColor& FgColor
                       , const FLinearColor& BgColor
                       , const FLinearColor& HcColor
                       , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags );
    void DrawPathHUD( FCanvas* Canvas
                    , FEditorViewportClient* ViewportClient
                    , FSceneView* View
                    , FArianePath* Path
                    , const FLinearColor& FgColor
                    , const FLinearColor& BgColor
                    , const FLinearColor& HcColor
                    , const FArianeEditorHUD::FDrawingFlags& HUDDrawingFlags );

    void DrawCircleHUD( FCanvas* Canvas
                      , FEditorViewportClient* ViewportClient
                      , FSceneView* View
                      , const FVector2D& HUDCoords
                      , double Radius
                      , const FLinearColor& Color
                      , float Thickness );
    FLinearColor GetForegroundColor();
    FLinearColor GetBackgroundColor();
    FLinearColor GetHighlightColor();

public:
    FArianeEditor* GetEditor() const;

protected:
    /** Patch (kinda): Stylus state and time of last event, so that we can have a little control for differentiating mouse and stylus events */
    std::chrono::steady_clock::time_point   StylusLastEventTime;

    bool bIsRecordingStylus = false;
    bool bIsStylusDown = false;
    bool bIsFocused = false;

protected:
    TSharedPtr<FArianeEditorToolInputProcessor> InputProcessor;
    FArianeEditor* Editor;
    TSharedPtr<FUICommandList> CommandList;
    bool bHasContextMenu;
    bool bInited;
    FKey PressedKey;
    // Temp
    FPointQuadTree* PointQuadTree;

// HUD Tiles
protected:
    UTexture* VertexTexture;
    UTexture* VertexContourTexture;
    UTexture* HandleTexture;
    UTexture* LineOutlinedTexture;

public:
    const FSlateBrush* Icon;
};

// define bitwise op
//ENUM_CLASS_FLAGS(FArianeHUD::EModeFlags)
