// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "OdysseyEditorTab.h"
#include "Templates/SharedPointer.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorGuidesTab : public FOdysseyEditorTab
{
public:
    static const FName& StaticId();
    FOdysseyPainterEditorGuidesTab(FOdysseyPainterEditor* iEditor);

protected:
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

private:
    FOdysseyPainterEditor* mEditor;
};
