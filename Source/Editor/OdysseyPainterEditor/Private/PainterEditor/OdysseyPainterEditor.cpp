// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditor.h"

FOdysseyPainterEditor::~FOdysseyPainterEditor()
{

}

FOdysseyPainterEditor::FOdysseyPainterEditor()
{

}

void
FOdysseyPainterEditor::Init()
{

}

TSharedPtr<FOdysseyPainterEditorToolkit>
FOdysseyPainterEditor::GetToolkit()
{
    return mToolkit.Pin();
}

void
FOdysseyPainterEditor::SetToolkit(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit)
{
    mToolkit = iToolkit;
}

void
FOdysseyPainterEditor::OnToolkitInitialized()
{

}
