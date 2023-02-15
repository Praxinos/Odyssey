// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/ColorPickerTool/OdysseyPainterEditorColorPickerTool.h"

#include "OdysseyRasterBlock.h"
#include "HUD/OdysseyVectorHUDBucket.h"

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

void
UOdysseyPainterEditorColorPickerTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorColorPickerTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorColorPickerTool::OnMouseUpVector( UOdysseyTextureLayerImageVector& currentVectorLayer
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    ::ULIS::FColor color = currentVectorLayer.GetEngine()->GetColorImagePixelValue( iPointInTexture.x, iPointInTexture.y );

    GetEditorAs<FOdysseyPainterEditor>()->PaintColor().SetValue( color );

    return false;
}

bool
UOdysseyPainterEditorColorPickerTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        return OnMouseUpVector( *currentVectorLayer, iPointInTexture, iKey );
    }

    return false;
}

void
UOdysseyPainterEditorColorPickerTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}
