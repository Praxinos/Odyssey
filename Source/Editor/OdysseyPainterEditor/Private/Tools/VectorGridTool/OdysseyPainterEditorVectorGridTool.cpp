// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "LayerStack/OdysseyTextureLayer.h"
#include "LayerStack/OdysseyTextureLayerStack.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "TextureEditor/OdysseyTextureEditor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorVectorGridTool::~UOdysseyPainterEditorVectorGridTool()
{
}

UOdysseyPainterEditorVectorGridTool::UOdysseyPainterEditorVectorGridTool()
    : DivisionsX( 4 )
    , DivisionsY( 4 )
    , PickingRadius( 10.0f )
    , mMultipleSelectionMode( false )
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.Grid64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorVectorGridTool::Activate()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        mGridHUD.MakeGrid( currentVectorLayer->GetScene(), DivisionsX, DivisionsY );

        mGridNodeArray.clear();

        vectorEngine->ClearHUD( );
        vectorEngine->AddHUD( &mGridHUD );

        currentVectorLayer->RenderImageChanged(false);
    }
}

bool
UOdysseyPainterEditorVectorGridTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        for( int i = 0; i < iPointInTexture.keysDown.Num(); i++ )
        {
            if( ( iPointInTexture.keysDown[i] == EKeys::LeftShift ) || ( iPointInTexture.keysDown[i] == EKeys::RightShift ) )
            {
                mMultipleSelectionMode = true;
            }
        }

        // multiple selection mode
        if ( mMultipleSelectionMode == true )
        {
            mMultipleSelectionMode = true;

            mGridHUD.StartSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }
        else
        {
            FGridNode* gridNode = mGridHUD.PickNode( iPointInTexture.x, iPointInTexture.y, PickingRadius );

            mGridNodeArray.clear();

            if( gridNode )
            {
                mGridNodeArray.push_back( gridNode );
            }
        }

        currentVectorLayer->RenderImageChanged(false);
    }

    return true;
}

void
UOdysseyPainterEditorVectorGridTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();

        if( mMultipleSelectionMode == true )
        {
            mGridHUD.DragSelectionRectangle( iPointInTexture.x, iPointInTexture.y );
        }
        else
        {
            FSelectionBox& selectionBox = mGridHUD.GetSelectionBox();
            BLPoint spaceDif = selectionBox.space->GetInverseWorldMatrix().mapVector( iPointInTexture.deltaPosition.X
                                                                                    , iPointInTexture.deltaPosition.Y );

            for( int i = 0; i < mGridNodeArray.size(); i++ )
            {
                mGridNodeArray[i]->Set( mGridNodeArray[i]->GetX() + spaceDif.x, mGridNodeArray[i]->GetY() + spaceDif.y );
            }

            mGridHUD.Deform();

            currentVectorLayer->GetScene()->Update( FOdysseyVectorObject::FREQUENTUPDATES
                                                  | FOdysseyVectorObject::KEEPINVALIDATED );
        }

        currentVectorLayer->RenderImageChanged(true);
        //RedrawCurrentLayer( { /*beforeBBox | selectedObject->GetBBox( true )*/{ 0, 0, 0, 0 } } );
    }
}

bool
UOdysseyPainterEditorVectorGridTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        if( mMultipleSelectionMode == true )
        {
            mGridNodeArray.clear();

            mGridHUD.EndSelectionRectangle( mGridNodeArray );
        }

        currentVectorLayer->GetScene()->Update( 0 );

        mMultipleSelectionMode = false;

        currentVectorLayer->RenderImageChanged(false);

        return true;
    }

    return false;
}

void
UOdysseyPainterEditorVectorGridTool::Commit()
{

}

void
UOdysseyPainterEditorVectorGridTool::PropertyChanged( const FName& iPropertyName )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayer* currentLayer = Cast<UOdysseyTextureLayer>(layerStack->CurrentLayer.Get());

    if( currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass() )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(currentLayer);

        mGridHUD.MakeGrid( currentVectorLayer->GetScene(), DivisionsX, DivisionsY );

        currentVectorLayer->RenderImageChanged(false);
    }
}

void
UOdysseyPainterEditorVectorGridTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    PropertyChanged(PropertyChangedEvent.GetPropertyName());
}
