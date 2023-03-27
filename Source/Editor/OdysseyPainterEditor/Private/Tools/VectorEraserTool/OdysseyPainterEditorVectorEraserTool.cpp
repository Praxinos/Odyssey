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

    mEraserHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorEraserTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD( );
    iEngine->AddHUD(&mEraserHUD);
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseDownVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture
                                                        , const FKey& iKey )
{
    mEraserHUD.SetRadius( Radius );
    mEraserHUD.BlendMask( true );

    iEngine->UseMaskImage();
    iEngine->GetBLContext()->setFillAlpha( 0.0f );
    iEngine->GetBLContext()->clearAll();
    iEngine->GetBLContext()->setFillAlpha( 1.0f );
    iEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
    iEngine->UseColorImage();

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseHoverVector( FOdysseyVectorEngine* iEngine
                                                         , FOdysseyVectorScene* iScene
                                                         , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mEraserHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/

    /*}*/
}

void
UOdysseyPainterEditorVectorEraserTool::OnMouseDragVector( FOdysseyVectorEngine* iEngine
                                                        , FOdysseyVectorScene* iScene
                                                        , const FOdysseyPoint& iPointInTexture )
{
    BLPoint pt = { 0, 0 };

    mEraserHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    iEngine->UseMaskImage();
    iEngine->GetBLContext()->setFillAlpha( 1.0f );
    iEngine->GetBLContext()->fillCircle( iPointInTexture.x, iPointInTexture.y, Radius );
    iEngine->UseColorImage();
}

bool
UOdysseyPainterEditorVectorEraserTool::OnMouseUpVector( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    ::ULIS::FRectD Roi;

    mEraserHUD.BlendMask( false );

    iEngine->UseMaskImage();
    iEngine->Erase( iScene, Roi, false );
    iEngine->UseColorImage();

    return true;
}

void
UOdysseyPainterEditorVectorEraserTool::Commit()
{

}

void
UOdysseyPainterEditorVectorEraserTool::PropertyChanged( const FName& iPropertyName )
{
    mEraserHUD.SetRadius( Radius );
}
