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

        //Mouse events
        virtual bool OnMouseDown( FEditorViewportClient* iViewportClient
                                , double iViewportX
                                , double iViewportY
                                , const FKey& iKey
                                , bool iRepeat = false ) override;
        virtual void OnMouseHover( FEditorViewportClient* iViewportClient
                                 , double iViewportX
                                 , double iViewportY ) override;
        virtual bool OnMouseDrag( FEditorViewportClient* iViewportClient
                                , double iViewportX
                                , double iViewportY ) override;
        virtual bool OnMouseUp( FEditorViewportClient* iViewportClient
                              , double iViewportX
                              , double iViewportY
                              , const FKey& iKey ) override;

    protected:
        virtual void ExtendContextMenu( FMenuBuilder& menu );
        void PlotVertex( FEditorViewportClient* iViewportClient
                       , double iViewportX
                       , double iViewportY );

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
