// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"
    
FOdysseyAnimationEditorExtension::~FOdysseyAnimationEditorExtension()
{
   
}

FOdysseyAnimationEditorExtension::FOdysseyAnimationEditorExtension(FOdysseyPainterEditor* iEditor)
    : FOdysseyPainterEditorExtension(iEditor)
{

}

void
FOdysseyAnimationEditorExtension::Install()
{
    FOdysseyPainterEditorExtension::Install();
}

void
FOdysseyAnimationEditorExtension::Uninstall()
{
    FOdysseyPainterEditorExtension::Uninstall();
}
