// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane Editor Headers
#include "ArianeEditorTool.h"
#include "PathDrawingTool/ArianeEditorPathTracer.h"
// Ariane Headers
#include "ArianePainting3DComponent.h" // for EArianePainting3DGeometryMode
#include "ArianePath.h"

// Odyssey
#include "OdysseyPainterEditorColorType.h"

#include "ArianeEditorPathDrawingTool.generated.h"

class FArianeEditor;
struct FArianePath;

UENUM(BlueprintType)
enum class EArianeEditorPathDrawingToolSegmentType : uint8
{
    Polyline = 0,
    CubicBezier = 1,
};

UCLASS()
class ARIANEEDITOR_API UArianeEditorPathDrawingTool : public UArianeEditorTool
{
GENERATED_BODY()

public:
    static FString GetStaticType() { return "ArianeEditor_PathDrawingTool"; };
    virtual FString GetType() override { return GetStaticType(); };

public:
    // Destructor
    virtual ~UArianeEditorPathDrawingTool();

    //Constructor
    UArianeEditorPathDrawingTool();

    //Mouse events overrides
    virtual bool OnMouseDown( FEditorViewportClient* iViewportClient
                            , FSceneView* View
                            , const FKey& iKey
                            , const FArianePointerState& State
                            , bool iRepeat = false ) override;
    virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                             , FSceneView* View
                             , const FArianePointerState& State ) override;
    virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                            , FSceneView* View
                            , const FKey& iKey
                            , const FArianePointerState& State ) override;
    virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                          , FSceneView* View
                          , const FKey& iKey
                          , const FArianePointerState& State ) override;
    virtual bool SupportsColorType( EOdysseyPainterEditorColorType ColorType ) override;

    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

    void Activate();
    void Inactivate();
    virtual void DrawHUD ( FCanvas* Canvas, IToolsContextRenderAPI* RenderAPI ) override;
    virtual bool GetCursor( EMouseCursor::Type& OutCursor ) override;

protected:
    virtual void ExtendContextMenu( FMenuBuilder& menu ) override;

    /**
     * @brief Add a new vertex to the current Path. Create a segment between this vertex and the previously created one.
     * @param ViewportClient
     * @param State the state of the input device (mouse or stylus)
     */
    void PlotVertex( FEditorViewportClient* ViewportClient
                   , const FArianePointerState& State
                   , bool bInteractive );
    FArianeGroup* GetParentGroup( UArianeLayerDrawing* DrawingLayer );

public:
    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool
             , meta = ( ToolTip = "Size"
                      , ClampMin = "0.01"
                      , Delta = "0.1"
                      , UIMin = "0.01" ) )
    double Size;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    bool bPressureSensitivity;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    EArianePathLineType LineType;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    EArianeEditorPathDrawingToolSegmentType SegmentType;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    bool bShowGrid;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    UMaterialInterface* MaterialInterface;


protected:
    FArianePath* EditedPath;
    FArianeEditorPathTracer PathTracer;
    FArianeSegment* PreviousSegment;
    FArianeSegment* CurrentSegment;
    EMouseCursor::Type Cursor;
};
