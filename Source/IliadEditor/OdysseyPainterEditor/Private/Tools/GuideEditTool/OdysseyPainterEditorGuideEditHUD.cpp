// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorGuideEditHUD.h"

#include "OdysseyPainterEditorGuideVisualizer.h"
#include "OdysseyPainterEditorGuideEditTool.h"

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
    TArray<FVector2D> HandlePositions;
    if (bCanDrawHandles)
    {
        if (const FOdysseyPainterEditorGuideVisualizer* Visualizer = FOdysseyPainterEditorGuideVisualizer::Find(Guide->GetGuideType()))
            Visualizer->GetHandlePositions(*Guide, FVector2D(Params.mTextureWidth, Params.mTextureHeight), HandlePositions);
    }
    const int32 HandleCount = FMath::Min(HandlePositions.Num(), Handles.Num());

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
