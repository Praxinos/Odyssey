// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Shortcuts/Global/OdysseyAnimationGlobalTimelineHeaderShortcuts.h"
#include "OdysseyPainterEditorAnimationCommands.h"
#include "OdysseyAnimation.h"

#define LOCTEXT_NAMESPACE "AnimationEditor"

namespace OdysseyAnimationGlobalTimelineHeaderShortcuts
{

bool
IsChecked_SetAnimationLeftBoundAutomatic(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return false;

    return animation->GetLeftBoundMode() == EOdysseyAnimationBoundMode::Automatic;
}

bool
IsChecked_SetAnimationLeftBoundManual(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return false;

    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.set-animation-left-bound-manual", "Set Animation Left Bound To Manual"));
    return animation->GetLeftBoundMode() == EOdysseyAnimationBoundMode::Manual;
}

bool
IsChecked_SetAnimationRightBoundAutomatic(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return false;

    return animation->GetRightBoundMode() == EOdysseyAnimationBoundMode::Automatic;
}

bool
IsChecked_SetAnimationRightBoundManual(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return false;

    return animation->GetRightBoundMode() == EOdysseyAnimationBoundMode::Manual;
}

void
Action_SetAnimationLeftBoundAutomatic(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.set-animation-left-bound-automatic", "Set Animation Left Bound To Automatic"));
    animation->SetLeftBoundMode(EOdysseyAnimationBoundMode::Automatic);
}

void
Action_SetAnimationLeftBoundManual(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.set-animation-left-bound-manual", "Set Animation Left Bound To Manual"));
    animation->SetLeftBoundMode(EOdysseyAnimationBoundMode::Manual);
}

void
Action_SetAnimationRightBoundAutomatic(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.set-animation-right-bound-automatic", "Set Animation Right Bound To Automatic"));
    animation->SetRightBoundMode(EOdysseyAnimationBoundMode::Automatic);
}

void
Action_SetAnimationRightBoundManual(TAttribute<UOdysseyAnimation*> iAnimation)
{
    UOdysseyAnimation* animation = iAnimation.Get();
    if (!animation)
        return;

    FScopedTransaction ScopedTransaction(LOCTEXT("timeline.shortcuts.set-animation-right-bound-manual", "Set Animation Right Bound To Manual"));
    animation->SetRightBoundMode(EOdysseyAnimationBoundMode::Manual);
}

void
MapActions_SetAnimationBounds(TSharedRef<FUICommandList> iCommandList, const TAttribute<UOdysseyAnimation*>& iAnimation)
{
    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationLeftBoundAutomatic,
        FExecuteAction::CreateStatic(&Action_SetAnimationLeftBoundAutomatic, iAnimation),
        FCanExecuteAction(),
        FIsActionChecked::CreateStatic(&IsChecked_SetAnimationLeftBoundAutomatic, iAnimation)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationLeftBoundManual,
        FExecuteAction::CreateStatic(&Action_SetAnimationLeftBoundManual, iAnimation),
        FCanExecuteAction(),
        FIsActionChecked::CreateStatic(&IsChecked_SetAnimationLeftBoundManual, iAnimation)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationRightBoundAutomatic,
        FExecuteAction::CreateStatic(&Action_SetAnimationRightBoundAutomatic, iAnimation),
        FCanExecuteAction(),
        FIsActionChecked::CreateStatic(&IsChecked_SetAnimationRightBoundAutomatic, iAnimation)
    );

    iCommandList->MapAction(
        FOdysseyPainterEditorAnimationCommands::Get().SetAnimationRightBoundManual,
        FExecuteAction::CreateStatic(&Action_SetAnimationRightBoundManual, iAnimation),
        FCanExecuteAction(),
        FIsActionChecked::CreateStatic(&IsChecked_SetAnimationRightBoundManual, iAnimation)
    );
}

}

#undef LOCTEXT_NAMESPACE
