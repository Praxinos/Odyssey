// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorFunctionLibrary.h"

#include "OdysseyBrushAssetBase.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "BrushContext/OdysseyPainterEditorBrushContext.h"

//---

//static
float
UOdysseyPainterEditorFunctionLibrary::GetViewportZoom( UOdysseyBrushAssetBase* BrushInstance )
{
    if (!BrushInstance)
        return 100.f;

    FOdysseyPainterEditorBrushContext* context = BrushInstance->GetContext<FOdysseyPainterEditorBrushContext>("FOdysseyPainterEditorBrushContext");
    if (!context)
        return 100.f;

    //---

    return context->Zoom();
}

//static
float
UOdysseyPainterEditorFunctionLibrary::GetViewportRotation( UOdysseyBrushAssetBase* BrushInstance )
{
    if (!BrushInstance)
        return 0.f;

    FOdysseyPainterEditorBrushContext* context = BrushInstance->GetContext<FOdysseyPainterEditorBrushContext>("FOdysseyPainterEditorBrushContext");
    if (!context)
        return 0.f;

    //---

    return context->Rotation();
}

//static
FVector2D
UOdysseyPainterEditorFunctionLibrary::GetViewportPan( UOdysseyBrushAssetBase* BrushInstance )
{
    if (!BrushInstance)
        return FVector2D::ZeroVector;

    FOdysseyPainterEditorBrushContext* context = BrushInstance->GetContext<FOdysseyPainterEditorBrushContext>("FOdysseyPainterEditorBrushContext");
    if (!context)
        return FVector2D::ZeroVector;

    //---

    return context->Pan();
}

//static
float
UOdysseyPainterEditorFunctionLibrary::GetStep( UOdysseyBrushAssetBase* BrushInstance )
{
    if (!BrushInstance)
        return 0.f;

    FOdysseyPainterEditorBrushContext* context = BrushInstance->GetContext<FOdysseyPainterEditorBrushContext>("FOdysseyPainterEditorBrushContext");
    if (context)
        return context->GetStep();

    //---

    return 0.f;
}
