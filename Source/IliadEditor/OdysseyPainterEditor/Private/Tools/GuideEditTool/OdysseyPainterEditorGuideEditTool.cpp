// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideEditTool.h"

#include "OdysseyPainterEditorGuideEditHUD.h"
#include "InputCoreTypes.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorGuideVisualizer.h"
#include "OdysseyPainterEditorSource.h"
#include "ScopedTransaction.h"

#define LOCTEXT_NAMESPACE "PainterEditorGuideEditTool"

UOdysseyPainterEditorGuideEditTool::UOdysseyPainterEditorGuideEditTool()
{
    mIsTemporaryTool = true;
    TSharedRef<FOdysseyPainterEditorGuideEditHUD> GuideEditHUD = MakeShared<FOdysseyPainterEditorGuideEditHUD>(this);
    GuideEditHUD->BindToolCallbacks();
    mHUD->AddElement(GuideEditHUD);
}

void UOdysseyPainterEditorGuideEditTool::SetGuide(UOdysseyPainterEditorGuide* InGuide)
{
    Guide = InGuide;
    ActiveHandle = INDEX_NONE;
}


UOdysseyPainterEditorGuide* UOdysseyPainterEditorGuideEditTool::GetGuide() const
{
    return Guide;
}

bool UOdysseyPainterEditorGuideEditTool::IsActivable() const
{
    return Guide && Guide->bVisible && !Guide->bLocked && GetEditor() && GetEditor()->GetSource().IsValid() && Super::IsActivable();
}

void UOdysseyPainterEditorGuideEditTool::Inactivate()
{
    ActiveHandle = INDEX_NONE;
    Transaction.Reset();
    Super::Inactivate();
}

FText UOdysseyPainterEditorGuideEditTool::GetTooltip() const
{
    return LOCTEXT("edit-guide-tooltip", "Edit the selected guide in the viewport");
}

void UOdysseyPainterEditorGuideEditTool::BeginHandleDrag(int32 HandleIndex)
{
    if (!Guide || !Guide->bVisible || Guide->bLocked || HandleIndex < 0 || HandleIndex >= FOdysseyPainterEditorGuideEditHUD::MaxHandles)
        return;

    ActiveHandle = HandleIndex;
    Transaction = MakeUnique<FScopedTransaction>(LOCTEXT("move-guide-handle", "Move Guide Handle"));
    Guide->Modify();
}

void UOdysseyPainterEditorGuideEditTool::DragHandle(const FVector2D& TexturePosition)
{
    if (ActiveHandle == INDEX_NONE || !Guide || !GetEditor())
        return;
    const TSharedPtr<FOdysseyPainterEditorSource> Source = GetEditor()->GetSource();
    if (!Source.IsValid() || Source->Width() <= 0 || Source->Height() <= 0)
        return;
    const FOdysseyPainterEditorGuideVisualizer* Visualizer = FOdysseyPainterEditorGuideVisualizer::Find(Guide->GetGuideType());
    if (!Visualizer)
        return;
    Visualizer->SetHandlePosition(*Guide, ActiveHandle, TexturePosition, FVector2D(Source->Width(), Source->Height()));
}

void UOdysseyPainterEditorGuideEditTool::EndHandleDrag()
{
    if (ActiveHandle != INDEX_NONE && Guide)
    {
        Guide->MarkPackageDirty();
        Guide->PostEditChange();
    }
    ActiveHandle = INDEX_NONE;
    Transaction.Reset();
}

bool UOdysseyPainterEditorGuideEditTool::OnKeyDown(const FKey& Key)
{
    if (Key == EKeys::Enter || Key == EKeys::Escape)
    {
        if (GetEditor())
            GetEditor()->InactivateTemporaryTool();
        return true;
    }
    return false;
}

#undef LOCTEXT_NAMESPACE
