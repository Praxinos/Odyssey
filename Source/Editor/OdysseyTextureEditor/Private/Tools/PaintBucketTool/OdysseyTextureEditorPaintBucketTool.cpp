// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyTextureEditorPaintBucketTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorPaintBucketTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorPaintBucketTool::~UOdysseyTextureEditorPaintBucketTool()
{
}

UOdysseyTextureEditorPaintBucketTool::UOdysseyTextureEditorPaintBucketTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorPaintBucketTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyTextureEditorPaintBucketTool::OnCurrentLayerChanged );
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorPaintBucketTool::Load()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentLayerRaster )
    {
	    TSharedPtr<FOdysseyRasterBlock> rasterBlock = currentLayerRaster->GetRasterBlock();
	    mPaintEngine.RasterBlock(rasterBlock);

	    //Should be managed by the tool
	    mPaintEngine.OnPreUpdateDelegate().BindUObject(this, &UOdysseyTextureEditorPaintBucketTool::OnPaintEnginePreUpdate);
    }

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorPaintBucketTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyTextureEditorPaintBucketTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();

    Unload();
}

void
UOdysseyTextureEditorPaintBucketTool::Unload()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

        if( currentLayerRaster )
        {
	        mPaintEngine.OnPreUpdateDelegate().Unbind();

            //Cleanup
	        mPaintEngine.RasterBlock(nullptr);
        }

        if( currentVectorLayer )
        {
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

            UOdysseyPainterEditorPaintBucketTool::UnloadVector( vectorEngine, vectorScene );
        }
    }
}

bool
UOdysseyTextureEditorPaintBucketTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return      currentLayer->GetClass() == UOdysseyTextureLayerImageRaster::StaticClass()
            ||  currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass();
}

void
UOdysseyTextureEditorPaintBucketTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	//ensure iLayerstack is the one the tool is working on
	UOdysseyTextureLayerStack* layerstack = GetEditorAs<FOdysseyTextureEditor>()->LayerStack();
	if ( !iLayerStack || !layerstack || layerstack != iLayerStack )
		return;

	//If not activable => Inactivate
	if (!IsActivable())
	{
		Inactivate(); //close the tool
		return;
	}

    // We have to redraw all layers in order to draw all layers without the HUD.
    // This will be removed when we'll have a dedicated HUD layer.
    TArray<UOdysseyLayer*> layers = iLayerStack->GetLayers();
    for( int i = 0; i < layers.Num(); i++ )
    {
        UOdysseyTextureLayer* textureLayer = static_cast<UOdysseyTextureLayer*>(layers[i]);
        textureLayer->RenderImageChanged(false);
    }

	//Reload the tool to edit the new layer
	Unload();
	Load();
}

bool
UOdysseyTextureEditorPaintBucketTool::OnKeyDown( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorPaintBucketTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorPaintBucketTool::OnKeyUp( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorPaintBucketTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorPaintBucketTool::OnMouseDown( const FOdysseyPoint& iPointInTexture
                                                 , const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    UOdysseyTextureLayerImageRaster* currentRasterLayer = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentRasterLayer )
    {
        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDownRaster( currentRasterLayer->GetRasterBlock()->GetBlock(), iPointInTexture, iKey );
    }

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

void
UOdysseyTextureEditorPaintBucketTool::OnMouseHover( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorPaintBucketTool::OnMouseHoverVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

void
UOdysseyTextureEditorPaintBucketTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorPaintBucketTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

bool
UOdysseyTextureEditorPaintBucketTool::OnMouseUp( const FOdysseyPoint& iPointInTexture
                                               , const FKey& iKey )
{
    FOdysseyTextureEditor* textureEditor =  GetEditorAs<FOdysseyTextureEditor>();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorPaintBucketTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );

        if( iKey == EKeys::RightMouseButton )
        {
            FOdysseyVectorObject* selectedObject = vectorScene->GetLastSelected();
            FOdysseyTextureEditorPaintBucketToolContextMenu* contextMenu = textureEditor->GetGUI()->GetPaintBucketToolContextMenu().Get();

            if( selectedObject )
            {
                if( selectedObject->GetClass() == FOdysseyVectorGroupPaint::StaticClass() )
                {
                    FOdysseyVectorGroupPaint* paintGroup = static_cast<FOdysseyVectorGroupPaint*>(selectedObject);

                    if( paintGroup->GetSelectedBucket() )
                    {
                        FSlateApplication::Get().PushMenu( textureEditor->GetGUI()->GetViewportTab().Get()->Widget().ToSharedRef(),
                                                           FWidgetPath(),
                                                           contextMenu->Widget().ToSharedRef(),
                                                           FSlateApplication::Get().GetCursorPos(),
                                                           FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu) );
                    }
                }
            }
        }
    }

    return ret;
}

void
UOdysseyTextureEditorPaintBucketTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    //Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    // redraw
    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName() );

        //currentVectorLayer->RenderImageChanged(false);
    }
}

FOdysseyBlendParameters
UOdysseyTextureEditorPaintBucketTool::OnPaintEnginePreUpdate(const FOdysseyBlendParameters& iBlendParameters)
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageRaster* currentLayerRaster = Cast<UOdysseyTextureLayerImageRaster>(layerStack->CurrentLayer.Get());
	FOdysseyBlendParameters blendParameters = iBlendParameters;

	if (!currentLayerRaster)
		return blendParameters;

	if ( currentLayerRaster->IsAlphaLocked )
		blendParameters.AlphaMode = EOdysseyAlphaMode(::ULIS::Alpha_Back);

	return blendParameters;
}

#undef LOCTEXT_NAMESPACE
