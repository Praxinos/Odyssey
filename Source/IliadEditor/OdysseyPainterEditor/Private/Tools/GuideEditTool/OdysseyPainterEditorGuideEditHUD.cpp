// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideEditHUD.h"

#include "OdysseyPainterEditorGuideEditTool.h"
#include "Shapes/OdysseyPainterEditorEllipseGuide.h"
#include "Shapes/OdysseyPainterEditorLineGuide.h"

FOdysseyPainterEditorGuideEditHUD::FOdysseyPainterEditorGuideEditHUD(UOdysseyPainterEditorGuideEditTool* InTool)
    : Tool(InTool)
{
    for (int32 HandleIndex = 0; HandleIndex < MaxHandles; ++HandleIndex)
    {
        TSharedPtr<FOdysseyHUDHandle> Handle = MakeShared<FOdysseyHUDHandle>(FVector2D::ZeroVector);
        Handles.Add(Handle);
        AddElement(Handle);
    }
}

void FOdysseyPainterEditorGuideEditHUD::BindToolCallbacks()
{
    for (int32 HandleIndex = 0; HandleIndex < Handles.Num(); ++HandleIndex)
        BindHandleCallbacks(Handles[HandleIndex], HandleIndex);
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

    // Handle positions in texture pixels; only the first HandleCount handles are used by the current guide.
    FVector2D HandlePositions[MaxHandles];
    int32 HandleCount = 0;
    if (bCanDrawHandles)
    {
        const double Width = Params.mTextureWidth;
        const double Height = Params.mTextureHeight;
        if (UOdysseyPainterEditorLineGuide* Line = Cast<UOdysseyPainterEditorLineGuide>(Guide))
        {
            HandlePositions[HandleCount++] = FVector2D(Line->Start.X * Width, Line->Start.Y * Height);
            HandlePositions[HandleCount++] = FVector2D(Line->End.X * Width, Line->End.Y * Height);
        }
        else if (UOdysseyPainterEditorEllipseGuide* Ellipse = Cast<UOdysseyPainterEditorEllipseGuide>(Guide))
        {
            const FVector2D Center(Ellipse->Center.X * Width, Ellipse->Center.Y * Height);
            const double AngleRadians = FMath::DegreesToRadians(Ellipse->Angle);
            const FVector2D AxisX(FMath::Cos(AngleRadians), FMath::Sin(AngleRadians));
            const FVector2D AxisY(-AxisX.Y, AxisX.X);
            HandlePositions[HandleCount++] = Center;
            HandlePositions[HandleCount++] = Center + AxisX * (Ellipse->Radius.X * Width);
            HandlePositions[HandleCount++] = Center + AxisY * (Ellipse->Radius.Y * Height);
        }
    }

    FLinearColor HandleColor = Guide ? Guide->Color : FLinearColor::White;
    if (Guide)
        HandleColor.A *= Guide->Opacity;

    for (int32 HandleIndex = 0; HandleIndex < Handles.Num(); ++HandleIndex)
    {
        const TSharedPtr<FOdysseyHUDHandle>& Handle = Handles[HandleIndex];
        const bool bUsed = HandleIndex < HandleCount;
        Handle->SetIsVisible(TAttribute<bool>(bUsed));
        Handle->IsInteractable(bUsed && !Guide->bLocked);
        if (!bUsed)
            continue;

        Handle->SetColor(HandleColor);
        Handle->SetPosition(HandlePositions[HandleIndex]);
    }
}
