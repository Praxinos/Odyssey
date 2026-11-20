// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "OdysseyHUDElement.h"
#include "Templates/SharedPointer.h"

class FOdysseyPainterEditor;
class FOdysseyHUDLine;
class FOdysseyHUDEllipse;
class UOdysseyPainterEditorGuide;

/** Draws all visible guides for the editor's current asset. */
class FOdysseyPainterEditorGuideOverlay : public FOdysseyHUDElement
{
public:
    FOdysseyPainterEditorGuideOverlay(FOdysseyPainterEditor* InEditor);

protected:
    virtual void DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& Params) override;

private:
    struct FGuideHUDShape
    {
        TSharedPtr<FOdysseyHUDLine> Line;
        TSharedPtr<FOdysseyHUDEllipse> Ellipse;
    };

    FOdysseyPainterEditor* Editor;
    TMap<TWeakObjectPtr<UOdysseyPainterEditorGuide>, FGuideHUDShape> GuideShapes;
};
