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
    bool ret = false;

    bool hasVector = GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (hasVector)
    {
        //Should be done in OnMouseUpVector directly
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = GetEditor()->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
        if (mediaVectors.Num() > 0)
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();
            ret = UOdysseyPainterEditorColorPickerTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
        }
    }
    else
    {
        UTexture* texture = GetEditor()->GetSource()->DisplayTexture();

        TArray<FLinearColor> colors;
        ENQUEUE_RENDER_COMMAND(GetPixel)(
            [&](FRHICommandListImmediate& RHICmdList)
            {
                RHICmdList.ReadSurfaceData(texture->GetResource()->TextureRHI, FIntRect(iPointInTexture.x, iPointInTexture.y, iPointInTexture.x + 1, iPointInTexture.y + 1), colors, FReadSurfaceDataFlags());
            }
        );
        
        FRenderCommandFence fence;
        fence.BeginFence();
        fence.Wait();

        ::ULIS::FColor ulisColor = ::ULIS::FColor::RGBAF( colors[0].R, colors[0].G, colors[0].B, colors[0].A );
        GetEditor()->PaintColor(ulisColor, true);
    }

    return ret;
}

bool
UOdysseyPainterEditorColorPickerTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    FColor color = iEngine->GetColorImagePixelValue( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FColor ulisColor = ::ULIS::FColor::RGBA8( color.R, color.G, color.B, color.A );

    GetEditor()->PaintColor(ulisColor, true);

    return false;
}

void
UOdysseyPainterEditorColorPickerTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}
