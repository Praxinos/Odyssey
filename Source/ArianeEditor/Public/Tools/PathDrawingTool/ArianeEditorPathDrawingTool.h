// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditorPathDrawingToolBuilder.h" // that way only this header needs to be included by files using this tool
#include "ArianePainting3DComponent.h" // for EArianePainting3DGeometryMode
#include "ArianePath.h"
// Odyssey
#include "OdysseyPainterEditorColorType.h"

#include "ArianeEditorPathDrawingTool.generated.h"

class FArianeEditor;
struct FArianePath;

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
                            , const FKey& iKey
                            , const FArianePointerState& State
                            , bool iRepeat = false ) override;
    virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                             , const FArianePointerState& State ) override;
    virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                            , const FKey& iKey
                            , const FArianePointerState& State ) override;
    virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                          , const FKey& iKey
                          , const FArianePointerState& State ) override;
    virtual bool SupportsColorType( EOdysseyPainterEditorColorType ColorType ) override;

    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

    void Activate();
    void Inactivate();

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

    /**
     * @brief Get a layer's drawing plane
     * @param ViewportClient
     * @param DrawingLayer the layer from get the plane from
     */
    FVector4 GetDrawingPlane( FEditorViewportClient* ViewportClient
                            , UArianeLayerDrawing* DrawingLayer );

public:
    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool
             , meta = ( ToolTip = "Size"
                      , ClampMin = "0.0"
                      , Delta = "0.1"
                      , UIMin = "0.0" ) )
    double Size;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    bool bPressureSensitivity;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    EArianePathLineType LineType;

    UPROPERTY( EditAnywhere
             , Category = PathDrawingTool )
    bool bShowGrid;

    UPROPERTY( EditAnywhere )
    UMaterialInterface* MaterialInterface;

protected:
    FArianePath* EditedPath;
};
