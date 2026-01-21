// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
// Ariane
#include "ArianeEditorTool.h"
#include "ArianeEditorPathDrawingTool.generated.h"

class FArianeEditor;

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
        virtual bool OnMouseDown( const FVector2D& iViewportCoords, const FKey& iKey, bool iRepeat = false ) override;
        virtual void OnMouseHover( const FVector2D& iViewportCoords ) override;
        virtual void OnMouseDrag( const FVector2D& iViewportCoords ) override;
        virtual bool OnMouseUp( const FVector2D& iViewportCoords, const FKey& iKey ) override;

    protected:
        TSharedPtr<SWidget> ExtendContextMenu( FMenuBuilder& menu );

    public:

};
