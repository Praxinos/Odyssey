// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"

#include "OdysseyMediaVector.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorColorPickerTool::~UOdysseyPainterEditorColorPickerTool()
{
}

UOdysseyPainterEditorColorPickerTool::UOdysseyPainterEditorColorPickerTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.ColorPickerTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

bool
UOdysseyPainterEditorColorPickerTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
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
        GetEditor()->PaintColor(ulisColor, true);
    }

    return false;
}

void
UOdysseyPainterEditorColorPickerTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}
