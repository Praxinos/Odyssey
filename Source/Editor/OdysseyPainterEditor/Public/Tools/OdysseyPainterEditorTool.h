// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyTool.h"

#include "OdysseyPainterEditorTool.generated.h"

UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorTool : public UOdysseyTool
{
    GENERATED_BODY()

public:
    // Destructor
    virtual ~UOdysseyPainterEditorTool();

    //Constructor
    UOdysseyPainterEditorTool();

public:
    void SetEditor(class FOdysseyPainterEditor* iEditor);
    template<class T> T* GetEditorAs() const { return static_cast<T*>(mEditor); };

private:
    class FOdysseyPainterEditor*              mEditor;
};
