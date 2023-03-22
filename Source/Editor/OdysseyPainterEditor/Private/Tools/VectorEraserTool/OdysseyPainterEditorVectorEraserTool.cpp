// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorEraserTool/OdysseyPainterEditorVectorEraserTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorEraserTool::~UOdysseyPainterEditorVectorEraserTool()
{

}

UOdysseyPainterEditorVectorEraserTool::UOdysseyPainterEditorVectorEraserTool()
    : Radius( 20.0f )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Eraser64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorEraserTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD( );
        vectorEngine->AddHUD( &mEraserHUD );

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorEraserTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mEraserHUD.SetRadius( Radius );
        mEraserHUD.BlendMask( true );

        vectorEngine->UseMaskImage();
        vectorEngine->GetBLContext()->setFillAlpha( 0.0f );
        vectorEngine->GetBLContext()->clearAll();
        vectorEngine->GetBLContext()->setFillAlpha( 1.0f );
        vectorEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
        vectorEngine->UseColorImage();
    }

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        double diameter = Radius * 2.0f;
        ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                              , (int)iPointInTexture.y - (int)Radius
                              , (int)diameter
                              , (int)diameter };

        mEraserHUD.SetRadius( Radius );
        mEraserHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
        if( rect.x < 0 ) rect.x = 0;
        if( rect.y < 0 ) rect.y = 0;

        rect = rect & layerStack->GetSurface()->Block()->Rect();

        if( rect.Area() )
        {*/
            currentVectorLayer->RenderImageChanged( /*{ rect },*/ true );
        /*}*/
    }
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        BLPoint pt = { 0, 0 };

        mEraserHUD.SetRadius( Radius );
        mEraserHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

        vectorEngine->UseMaskImage();
        vectorEngine->GetBLContext()->setFillAlpha( 1.0f );
        vectorEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
        vectorEngine->UseColorImage();

        currentVectorLayer->RenderImageChanged(true);

        /*vectorEngine->GetBLContext()->blitImage(pt,*vectorEngine->GetBLMask());*/
    }
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        ::ULIS::FRectD Roi;

        mEraserHUD.BlendMask( false );

        vectorEngine->UseMaskImage();
        vectorEngine->Erase( currentVectorLayer->GetScene(), Roi, false );
        vectorEngine->UseColorImage();

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorEraserTool::Commit()
{

}
