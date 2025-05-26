// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorExtension.h"

FOdysseyPainterEditorExtension::~FOdysseyPainterEditorExtension()
{

}

FOdysseyPainterEditorExtension::FOdysseyPainterEditorExtension(FOdysseyPainterEditor* iEditor)
    : mEditor(iEditor)
{

}

void
FOdysseyPainterEditorExtension::Initialize()
{

}

void
FOdysseyPainterEditorExtension::Finalize()
{

}

void
FOdysseyPainterEditorExtension::ExtendMenu( TSharedRef<FExtender> iExtender )
{

}

void
FOdysseyPainterEditorExtension::BindShortcuts(FBaseToolkit* iToolkit)
{

}

void
FOdysseyPainterEditorExtension::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{

}

FOdysseyPainterEditor*
FOdysseyPainterEditorExtension::GetEditor() const
{
    return mEditor;
}

void
FOdysseyPainterEditorExtension::AddReferencedObjects(FReferenceCollector& Collector)
{

}

FString
FOdysseyPainterEditorExtension::GetReferencerName() const
{
    return "FOdysseyPainterEditorExtension";
}
