// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/Tab/SOdysseyPainterEditorToolCollection.h"

#include "OdysseyPainterEditor.h"

SOdysseyPainterEditorToolCollection::~SOdysseyPainterEditorToolCollection()
{}

SOdysseyPainterEditorToolCollection::SOdysseyPainterEditorToolCollection()
{}

void
SOdysseyPainterEditorToolCollection::Construct(const FArguments& InArgs)
{
    mEditor = InArgs._Editor;

    FOdysseyPainterEditor* editor = mEditor.Get();

    if (!editor)
        return;
}
