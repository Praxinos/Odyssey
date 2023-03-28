// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathSmoothTool/OdysseyPainterEditorVectorPathSmoothTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathSmoothTool::~UOdysseyPainterEditorVectorPathSmoothTool()
{
}

UOdysseyPainterEditorVectorPathSmoothTool::UOdysseyPainterEditorVectorPathSmoothTool()
    : Radius(20.0f)
    , mPickingHUD()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathSmoothTool64");

    mPickedPointArray.reserve( 50 );

    mPickingHUD.SetRadius( Radius );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathSmoothTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD(&mPickingHUD);
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture
                                                      , const FKey& iKey )
{
    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHover( FOdysseyVectorEngine* iEngine
                                                       , FOdysseyVectorScene* iScene
                                                       , const FOdysseyPoint& iPointInTexture )
{
    double diameter = Radius * 2.0f;
    ::ULIS::FRectI rect = { (int)iPointInTexture.x - (int)Radius
                          , (int)iPointInTexture.y - (int)Radius
                          , (int)diameter
                          , (int)diameter };

    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
/*
    if( rect.x < 0 ) rect.x = 0;
    if( rect.y < 0 ) rect.y = 0;

    rect = rect & layerStack->GetSurface()->Block()->Rect();

    if( rect.Area() )
    {*/
    /*}*/
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                      , FOdysseyVectorScene* iScene
                                                      , const FOdysseyPoint& iPointInTexture )
{
    mPickingHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

    mPickedPointArray.clear();

    iEngine->PickPoints( iScene
                       , iPointInTexture.x
                       , iPointInTexture.y
                       , Radius
                       , mPickedPointArray
                       , FOdysseyVectorPath::PICK_POINT );

    for( int i = 0; i < mPickedPointArray.size(); i++ )
    {
        FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>(mPickedPointArray[i]);

        if( cubicVertex->GetSegmentCount() == 2 )
        {
            cubicVertex->SmoothSegments( true, true );
        }
    }

    iScene->Update( 0 );
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                    , FOdysseyVectorScene* iScene
                                                    , const FOdysseyPoint& iPointInTexture
                                                    , const FKey& iKey )
{

    return false;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::Commit()
{

}

void
UOdysseyPainterEditorVectorPathSmoothTool::PropertyChanged( const FName& iPropertyName )
{
    mPickingHUD.SetRadius( Radius );
}
