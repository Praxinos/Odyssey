// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorColorPickerTool::~UOdysseyPainterEditorColorPickerTool()
{
}

UOdysseyPainterEditorColorPickerTool::UOdysseyPainterEditorColorPickerTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ColorPicker64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorColorPickerTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    if (iKey == EKeys::LeftMouseButton)
    {
        mIsPicking = true;
        PickColorMove(iPointInTexture);
        return true;
    }
    return false;
}

void
UOdysseyPainterEditorColorPickerTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    if (mIsPicking)
        PickColorMove(iPointInTexture);
}

bool
UOdysseyPainterEditorColorPickerTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    if (mIsPicking)
    {
        mIsPicking = false;
        PickColorUp(iPointInTexture);
        return true;
    }
    return false;
}

void
UOdysseyPainterEditorColorPickerTool::PickColorMove( const FOdysseyPoint& iPointInTexture )
{
    UTexture* texture = GetEditor()->GetSource()->DisplayTexture();

    if( ( iPointInTexture.x >= 0.0f ) && ( iPointInTexture.x < texture->GetSurfaceWidth() - 1.0f )
     && ( iPointInTexture.y >= 0.0f ) && ( iPointInTexture.y < texture->GetSurfaceHeight() - 1.0f ) )
    {
        TArray<FColor> colors;
        FRenderCommandFence fence;

        ENQUEUE_RENDER_COMMAND(GetPixel)(
            [&](FRHICommandListImmediate& RHICmdList)
            {
                RHICmdList.ReadSurfaceData(texture->GetResource()->TextureRHI, FIntRect(iPointInTexture.x, iPointInTexture.y, iPointInTexture.x + 1, iPointInTexture.y + 1), colors, FReadSurfaceDataFlags());
            }
        );

        fence.BeginFence();
        fence.Wait();

        ::ULIS::FColor ulisColor = ::ULIS::FColor::FromRGBA8( colors[0].R, colors[0].G, colors[0].B, colors[0].A );
        GetEditor()->PaintColor(ulisColor, false);
    }
}

void
UOdysseyPainterEditorColorPickerTool::PickColorUp( const FOdysseyPoint& iPointInTexture )
{
    mEditor->PaintColor(mEditor->PaintColor(), true); //Commit paintColor
}

void
UOdysseyPainterEditorColorPickerTool::Commit()
{
    mPaintEngine.Commit(FOdysseyBlendParameters());
}

FText
UOdysseyPainterEditorColorPickerTool::GetTooltip() const
{
    return LOCTEXT("color-picker-tool.tooltip", "Color Picker Tool");
}

EMouseCursor::Type
UOdysseyPainterEditorColorPickerTool::GetMouseCursor() const
{
    return EMouseCursor::EyeDropper;
}

#undef LOCTEXT_NAMESPACE