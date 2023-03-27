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
UOdysseyPainterEditorColorPickerTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
}

bool
UOdysseyPainterEditorColorPickerTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorColorPickerTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                     , FOdysseyVectorScene* iScene
                                                     , const FOdysseyPoint& iPointInTexture
                                                     , const FKey& iKey )
{
    FColor color = iEngine->GetColorImagePixelValue( iPointInTexture.x, iPointInTexture.y );
    ::ULIS::FColor ulisColor = ::ULIS::FColor::RGBA8( color.R, color.G, color.B, color.A );

    GetEditorAs<FOdysseyPainterEditor>()->PaintColor().SetValue( ulisColor );

    return false;
}

void
UOdysseyPainterEditorColorPickerTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}
