// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideEditTool.h"

#include "OdysseyPainterEditorGuideEditHUD.h"
#include "InputCoreTypes.h"
#include "OdysseyPainterEditor.h"
#include "Shapes/OdysseyPainterEditorEllipseGuide.h"
#include "Shapes/OdysseyPainterEditorLineGuide.h"
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
    if (!Guide || !Guide->bVisible || Guide->bLocked || HandleIndex < 0 || HandleIndex > 1)
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
    const FVector2D Position(
        FMath::Clamp(TexturePosition.X / Source->Width(), 0.0, 1.0),
        FMath::Clamp(TexturePosition.Y / Source->Height(), 0.0, 1.0));
    ApplyHandlePosition(Position);
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

void UOdysseyPainterEditorGuideEditTool::ApplyHandlePosition(const FVector2D& Position)
{
    if (UOdysseyPainterEditorLineGuide* Line = Cast<UOdysseyPainterEditorLineGuide>(Guide))
    {
        if (ActiveHandle == 0) Line->Start = Position;
        else Line->End = Position;
    }
    else if (UOdysseyPainterEditorEllipseGuide* Ellipse = Cast<UOdysseyPainterEditorEllipseGuide>(Guide))
    {
        if (ActiveHandle == 0)
        {
            Ellipse->Center = Position;
        }
        else
        {
            const double CosAngle = FMath::Cos(FMath::DegreesToRadians(Ellipse->Angle));
            const double SinAngle = FMath::Sin(FMath::DegreesToRadians(Ellipse->Angle));
            const FVector2D Delta = Position - Ellipse->Center;
            const int32 Width = GetEditor()->GetSource()->Width();
            const int32 Height = GetEditor()->GetSource()->Height();
            const FVector2D DeltaPixels(Delta.X * Width, Delta.Y * Height);
            Ellipse->Radius.X = FMath::Clamp(FMath::Abs(DeltaPixels.X * CosAngle + DeltaPixels.Y * SinAngle) / Width, 0.0, 1.0);
            Ellipse->Radius.Y = FMath::Clamp(FMath::Abs(-DeltaPixels.X * SinAngle + DeltaPixels.Y * CosAngle) / Height, 0.0, 1.0);
        }
    }
}

#undef LOCTEXT_NAMESPACE
