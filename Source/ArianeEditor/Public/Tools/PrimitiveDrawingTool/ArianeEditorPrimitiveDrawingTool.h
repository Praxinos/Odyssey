// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "InteractiveToolBuilder.h"
// Ariane Editor Headers
#include "ArianeEditorTool.h"
// Ariane Headers
#include "ArianePainting3DComponent.h" // for EArianePainting3DGeometryMode
#include "ArianePath.h"

// Odyssey
#include "OdysseyPainterEditorColorType.h"

#include "ArianeEditorPrimitiveDrawingTool.generated.h"

struct FArianePrimitive;

UENUM()
enum class EArianePrimitiveToolShapeType : uint8
{
    Ellipse,
    Rectangle,
    Line,
    Polygon,
};

UCLASS()
class ARIANEEDITOR_API UArianeEditorPrimitiveDrawingTool : public UArianeEditorTool
{
public:
    GENERATED_BODY()

public:
    static FString GetStaticType() { return "ArianeEditor_PrimitiveDrawingTool"; };
    virtual FString GetType() override { return GetStaticType(); };

public:
    // Destructor
    virtual ~UArianeEditorPrimitiveDrawingTool();

    //Constructor
    UArianeEditorPrimitiveDrawingTool();

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
    //virtual bool SupportsColorType( EOdysseyPainterEditorColorType ColorType ) override;

    void Activate();
    void Inactivate();
    virtual bool GetCursor( EMouseCursor::Type& OutCursor ) override;
    virtual bool OnKeyDownGlobal( const FKeyEvent& InKeyEvent ) override;
    virtual bool OnKeyUpGlobal( const FKeyEvent& InKeyEvent ) override;
    virtual void ExtendToolbar( UToolMenu* iToolMenu ) override;
    virtual void Render(IToolsContextRenderAPI* RenderAPI) override;

protected:
    //double GetLineRotationAngle( FArianeLine* iLine, const FOdysseyPoint& iPointInTexture );
    FArianeGroup* GetParentGroup( UArianeLayerDrawing* DrawingLayer );
    //void BindDelegates();
    //void UnbindDelegates();

public:
    UPROPERTY( EditAnywhere
             , Category = PrimitiveDrawingTool
             , meta = ( ToolTip  = "PrimitiveShapeType" ) )
    EArianePrimitiveToolShapeType PrimitiveShapeType;

    UPROPERTY( EditAnywhere
             , Category = PrimitiveDrawingTool
             , meta = ( ToolTip = "StrokeWidth"
                      , ClampMin = "0.01"
                      , Delta = "0.1"
                      , UIMin = "0.01" ) )
    double StrokeWidth;

    UPROPERTY( EditAnywhere
             , Category = PrimitiveDrawingTool
             , meta = ( ToolTip  = "Uniform" ) )
    bool Uniform;
    bool UniformAtKeyDown;

    UPROPERTY( EditAnywhere
             , Category = PrimitiveDrawingTool )
    EArianePathLineType LineType;

    UPROPERTY( EditAnywhere
             , Category = PrimitiveDrawingTool )
    bool bShowGrid;

    UPROPERTY( EditAnywhere
             , Category = PrimitiveDrawingTool )
    UMaterialInterface* MaterialInterface;

    //UPROPERTY( EditAnywhere
    //         , Category = "Shape" )
    //FOdysseyShapes Shapes;

    UPROPERTY( EditAnywhere
             , Category = PrimitiveDrawingTool
             , meta = ( ToolTip = "Division count"
                      , EditCondition = "PrimitiveShapeType == EArianePrimitiveToolShapeType::Polygon"
                      , EditConditionHides
                      , ClampMin = "3"
                      , UIMin = "3"
                      , Delta = "1"
                      , LinearDeltaSensitivity = "5"
                      , ClampMax = "20"
                      , UIMax = "20" ) )
    int DivisionCount;

protected:
    FArianePrimitive* Primitive;
    FVector2D MouseDown;
    uint32 RectangleNumber;
    uint32 LineNumber;
    uint32 EllipseNumber;
    uint32 PolygonNumber;
    EMouseCursor::Type Cursor;
    FVector PrimitiveCoordsAtDown;
    FVector IntersectAtDown;
};
