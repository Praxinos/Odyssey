// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "OdysseyHUDElement.h"
#include "OdysseyHUDHandle.h"

class UOdysseyPainterEditorGuideEditTool;

/** HUD element that draws editable handles for the guide edit tool's current guide. */
class FOdysseyPainterEditorGuideEditHUD : public FOdysseyHUDElement
{
public:
    explicit FOdysseyPainterEditorGuideEditHUD(UOdysseyPainterEditorGuideEditTool* InTool);
    void BindToolCallbacks();

protected:
    virtual void DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& Params) override;

private:
    void BindHandleCallbacks(const TSharedPtr<FOdysseyHUDHandle>& Handle, int32 HandleIndex);

    TWeakObjectPtr<UOdysseyPainterEditorGuideEditTool> Tool;
    TSharedPtr<FOdysseyHUDHandle> FirstHandle;
    TSharedPtr<FOdysseyHUDHandle> SecondHandle;
};
