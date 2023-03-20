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
    , mPushHUD()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathSmoothTool64");

    mPickedPointArray.reserve( 50 );
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathSmoothTool::Activate()
{
    UOdysseyTextureLayerImageVector* currentVectorLayer = GetCurrentLayerImageVector();

    if(currentVectorLayer)
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        vectorEngine->ClearHUD();
        vectorEngine->AddHUD(&mPushHUD);

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return true;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
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

        mPushHUD.SetRadius( Radius );
        mPushHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );
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
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        mPushHUD.SetPosition( iPointInTexture.x, iPointInTexture.y );

        mPickedPointArray.clear();

        currentVectorLayer->GetEngine()->PickPoints( currentVectorLayer->GetScene()
                                                   , iPointInTexture.x
                                                   , iPointInTexture.y
                                                   , Radius
                                                   , mPickedPointArray
                                                   , FOdysseyVectorPath::PICK_POINT );

        for( int i = 0; i < mPickedPointArray.size(); i++ )
        {
            FOdysseyVectorVertexCubic* cubicVertex = static_cast<FOdysseyVectorVertexCubic*>(mPickedPointArray[i]);

            cubicVertex->SmoothSegments( true, true );
        }

        currentVectorLayer->GetScene()->Update( 0 );

        currentVectorLayer->RenderImageChanged(/* { rect }, */true);
    }
}

bool
UOdysseyPainterEditorVectorPathSmoothTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{

    return false;
}

void
UOdysseyPainterEditorVectorPathSmoothTool::Commit()
{

}
