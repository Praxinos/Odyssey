// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditorPathDrawingTool.generated.h"

class FArianeEditor;
struct FArianePath;

UCLASS()
class ARIANEEDITOR_API UArianeEditorPathDrawingTool : public UArianeEditorTool
{
    GENERATED_BODY()

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
                                , const FArianePointerState& State ) override;
        virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                              , const FKey& iKey
                              , const FArianePointerState& State ) override;

    protected:
        virtual void ExtendContextMenu( FMenuBuilder& menu ) override;

        /**
         * @brief Create a new vertex
         * @param ViewportClient
         * @param State the state of the input device (mouse or stylus)
         */
        void PlotVertex( FEditorViewportClient* ViewportClient
                       , const FArianePointerState& State );

    public:
        UPROPERTY( EditAnywhere
                 , Category=PathDrawingTool
                 , meta = ( ToolTip = "Radius"
                          , ClampMin = "0.0"
                          , UIMin = "0.0" ) )
        double Radius;

    protected:
        FArianePath* EditedPath;
};
