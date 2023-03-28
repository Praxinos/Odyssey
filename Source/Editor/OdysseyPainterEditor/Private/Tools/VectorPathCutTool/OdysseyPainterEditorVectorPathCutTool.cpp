// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPathCutTool/OdysseyPainterEditorVectorPathCutTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorPathCutTool::~UOdysseyPainterEditorVectorPathCutTool()
{
}

UOdysseyPainterEditorVectorPathCutTool::UOdysseyPainterEditorVectorPathCutTool()
    : mCubicPathHUD( FOdysseyVectorHUDPathCubic::VIEW_PATH | FOdysseyVectorHUDPathCubic::VIEW_POINT )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PathCutTool64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorPathCutTool::Activate( FOdysseyVectorEngine* iEngine, FOdysseyVectorScene* iScene )
{
    iEngine->ClearHUD();
    iEngine->AddHUD(&mCubicPathHUD);
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseDown( FOdysseyVectorEngine* iEngine
                                                   , FOdysseyVectorScene* iScene
                                                   , const FOdysseyPoint& iPointInTexture
                                                   , const FKey& iKey )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();

    mLineHUD.SetP0( iPointInTexture.x, iPointInTexture.y );
    mLineHUD.SetP1( iPointInTexture.x, iPointInTexture.y );

    iEngine->AddHUD( &mLineHUD );

    if ( selectedObject )
    {
        BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        mStartCutAt.x = localCoords.x;
        mStartCutAt.y = localCoords.y;
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::OnMouseDrag( FOdysseyVectorEngine* iEngine
                                                   , FOdysseyVectorScene* iScene
                                                   , const FOdysseyPoint& iPointInTexture )
{
    ::ULIS::FVec2D& p0 = mLineHUD.GetP0();
    ::ULIS::FVec2D& p1 = mLineHUD.GetP1();
    ::ULIS::FRectI rect = ::ULIS::FRectI::FromMinMax( ::ULIS::FMath::Min(p0.x, p1.x)
                                                    , ::ULIS::FMath::Min(p0.y, p1.y)
                                                    , ::ULIS::FMath::Max(p0.x, p1.x)
                                                    , ::ULIS::FMath::Max(p0.y, p1.y) );

    mLineHUD.SetP1( iPointInTexture.x, iPointInTexture.y );
}

bool
UOdysseyPainterEditorVectorPathCutTool::OnMouseUp( FOdysseyVectorEngine* iEngine
                                                 , FOdysseyVectorScene* iScene
                                                 , const FOdysseyPoint& iPointInTexture
                                                 , const FKey& iKey )
{
    FOdysseyVectorObject* selectedObject = iScene->GetLastSelected();
    ::ULIS::FVec2D endCutAt;

    iEngine->RemoveHUD( &mLineHUD );

    if ( selectedObject )
    {
        BLPoint localCoords = selectedObject->GetInverseWorldMatrix().mapPoint( iPointInTexture.x, iPointInTexture.y );

        endCutAt.x = localCoords.x;
        endCutAt.y = localCoords.y;

        if( selectedObject->GetClass() == FOdysseyVectorPathCubic::StaticClass() )
        {
            FOdysseyVectorPathCubic *cubicPath = static_cast<FOdysseyVectorPathCubic*>(selectedObject);
            std::vector<FOdysseyVectorVertexCubic*> oNewVertexArray;
            std::vector<FOdysseyVectorSegmentCubic*> oNewSegmentArray;
            std::vector<FOdysseyVectorSegmentCubic*> oOldSegmentArray;

            // crashes if I don't reserve. Why that ?
            oNewVertexArray.reserve(50);
            oNewSegmentArray.reserve(50);
            oOldSegmentArray.reserve(50);

            cubicPath->Cut( mStartCutAt, endCutAt, oNewVertexArray, oNewSegmentArray, oOldSegmentArray );
            cubicPath->Invalidate();
        }

        iScene->Update( 0 );
    }

    return true;
}

void
UOdysseyPainterEditorVectorPathCutTool::Commit()
{

}
