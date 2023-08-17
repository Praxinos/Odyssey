// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "PainterEditor/OdysseyPainterEditorExtension.h"
    
FOdysseyPainterEditorExtension::~FOdysseyPainterEditorExtension()
{
   
}

FOdysseyPainterEditorExtension::FOdysseyPainterEditorExtension(FOdysseyPainterEditor* iEditor)
    : mIsInstalled(false)
    , mEditor(iEditor)
{

}

void
FOdysseyPainterEditorExtension::Install()
{
    mIsInstalled = true;
}

void
FOdysseyPainterEditorExtension::Uninstall()
{
    mIsInstalled = false;
}

bool
FOdysseyPainterEditorExtension::IsInstalled() const
{
    return mIsInstalled;
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
