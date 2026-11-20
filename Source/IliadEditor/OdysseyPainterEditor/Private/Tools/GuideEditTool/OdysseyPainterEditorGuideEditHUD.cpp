// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideEditHUD.h"

#include "OdysseyPainterEditorGuideEditTool.h"
#include "Shapes/OdysseyPainterEditorEllipseGuide.h"
#include "Shapes/OdysseyPainterEditorLineGuide.h"

FOdysseyPainterEditorGuideEditHUD::FOdysseyPainterEditorGuideEditHUD(UOdysseyPainterEditorGuideEditTool* InTool)
    : Tool(InTool)
    , FirstHandle(MakeShared<FOdysseyHUDHandle>(FVector2D::ZeroVector))
    , SecondHandle(MakeShared<FOdysseyHUDHandle>(FVector2D::ZeroVector))
{
    AddElement(FirstHandle);
    AddElement(SecondHandle);
}

void FOdysseyPainterEditorGuideEditHUD::BindToolCallbacks()
{
    BindHandleCallbacks(FirstHandle, 0);
    BindHandleCallbacks(SecondHandle, 1);
}

void FOdysseyPainterEditorGuideEditHUD::BindHandleCallbacks(const TSharedPtr<FOdysseyHUDHandle>& Handle, int32 HandleIndex)
{
    const TWeakPtr<FOdysseyPainterEditorGuideEditHUD> WeakHUD = StaticCastSharedRef<FOdysseyPainterEditorGuideEditHUD>(AsShared());
    const TWeakPtr<FOdysseyHUDHandle> WeakHandle = Handle;

    Handle->OnDragBegin().AddLambda([WeakHUD, HandleIndex]()
    {
        TSharedPtr<FOdysseyPainterEditorGuideEditHUD> HUD = WeakHUD.Pin();
        UOdysseyPainterEditorGuideEditTool* ToolObject = HUD.IsValid() ? HUD->Tool.Get() : nullptr;
        if (ToolObject)
            ToolObject->BeginHandleDrag(HandleIndex);
    });

    Handle->OnDragged().AddLambda([WeakHUD, WeakHandle]()
    {
        TSharedPtr<FOdysseyPainterEditorGuideEditHUD> HUD = WeakHUD.Pin();
        TSharedPtr<FOdysseyHUDHandle> DraggedHandle = WeakHandle.Pin();
        UOdysseyPainterEditorGuideEditTool* ToolObject = HUD.IsValid() ? HUD->Tool.Get() : nullptr;
        if (ToolObject && DraggedHandle.IsValid())
            ToolObject->DragHandle(DraggedHandle->GetPosition());
    });

    Handle->OnDragEnd().AddLambda([WeakHUD]()
    {
        TSharedPtr<FOdysseyPainterEditorGuideEditHUD> HUD = WeakHUD.Pin();
        UOdysseyPainterEditorGuideEditTool* ToolObject = HUD.IsValid() ? HUD->Tool.Get() : nullptr;
        if (ToolObject)
            ToolObject->EndHandleDrag();
    });
}

void FOdysseyPainterEditorGuideEditHUD::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& Params)
{
    UOdysseyPainterEditorGuideEditTool* ToolObject = Tool.Get();
    UOdysseyPainterEditorGuide* Guide = ToolObject ? ToolObject->GetGuide() : nullptr;
    const bool bCanDrawHandles = Guide && Guide->bVisible && Params.mCanvas && Params.mTextureWidth > 0 && Params.mTextureHeight > 0;
    FirstHandle->SetIsVisible(TAttribute<bool>(bCanDrawHandles));
    SecondHandle->SetIsVisible(TAttribute<bool>(bCanDrawHandles));
    FirstHandle->IsInteractable(bCanDrawHandles && !Guide->bLocked);
    SecondHandle->IsInteractable(bCanDrawHandles && !Guide->bLocked);
    if (!bCanDrawHandles)
        return;

    FLinearColor HandleColor = Guide->Color;
    HandleColor.A *= Guide->Opacity;
    FirstHandle->SetColor(HandleColor);
    SecondHandle->SetColor(HandleColor);

    if (UOdysseyPainterEditorLineGuide* Line = Cast<UOdysseyPainterEditorLineGuide>(Guide))
    {
        FirstHandle->SetPosition(FVector2D(Line->Start.X * Params.mTextureWidth, Line->Start.Y * Params.mTextureHeight));
        SecondHandle->SetPosition(FVector2D(Line->End.X * Params.mTextureWidth, Line->End.Y * Params.mTextureHeight));
    }
    else if (UOdysseyPainterEditorEllipseGuide* Ellipse = Cast<UOdysseyPainterEditorEllipseGuide>(Guide))
    {
        FirstHandle->SetPosition(FVector2D(Ellipse->Center.X * Params.mTextureWidth, Ellipse->Center.Y * Params.mTextureHeight));
        const double CosAngle = FMath::Cos(FMath::DegreesToRadians(Ellipse->Angle));
        const double SinAngle = FMath::Sin(FMath::DegreesToRadians(Ellipse->Angle));
        SecondHandle->SetPosition(FVector2D(Ellipse->Center.X * Params.mTextureWidth + Ellipse->Radius.X * Params.mTextureWidth * CosAngle,
            Ellipse->Center.Y * Params.mTextureHeight + Ellipse->Radius.X * Params.mTextureWidth * SinAngle));
    }
}
