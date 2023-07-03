// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyTextureEditorVectorPickTool.h"
#include "LayerStack/OdysseyTextureLayerImageVector.h"
#include "PainterEditor/OdysseyPainterEditorViewportTab.h"

#define LOCTEXT_NAMESPACE "OdysseyTextureEditorVectorPickTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyTextureEditorVectorPickTool::~UOdysseyTextureEditorVectorPickTool()
{
}

UOdysseyTextureEditorVectorPickTool::UOdysseyTextureEditorVectorPickTool()
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyTextureEditorVectorPickTool::Activate()
{
    UOdysseyTextureLayerStack::OnCurrentLayerChanged().AddUObject( this, &UOdysseyTextureEditorVectorPickTool::OnCurrentLayerChanged );
    Load();
    Super::Activate();
}

void
UOdysseyTextureEditorVectorPickTool::Load()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    UTexture2D* texture = layerStack->GetTexture();

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorVectorPickTool::LoadVector( vectorEngine, vectorScene );
    }
}

void
UOdysseyTextureEditorVectorPickTool::Inactivate()
{
	UOdysseyTextureLayerStack::OnCurrentLayerChanged().RemoveAll(this);
    Super::Inactivate();

    Unload();
}

void
UOdysseyTextureEditorVectorPickTool::Unload()
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());

    // layerStack might be NULL when closing the program
    if( layerStack )
    {
        UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

        if( currentVectorLayer )
        {
            FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
            FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

            UOdysseyPainterEditorVectorPickTool::UnloadVector( vectorEngine, vectorScene );
        }
    }
}

bool
UOdysseyTextureEditorVectorPickTool::IsActivable() const
{
    if (!Super::IsActivable())
        return false; 

    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    if (!layerStack)
        return false;

    UOdysseyLayer* currentLayer = layerStack->CurrentLayer.Get();
    if (!currentLayer)
        return false;

    return currentLayer->GetClass() == UOdysseyTextureLayerImageVector::StaticClass();
}

void
UOdysseyTextureEditorVectorPickTool::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
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
UOdysseyTextureEditorVectorPickTool::OnKeyDown( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorPickTool::OnKeyDownVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorVectorPickTool::OnKeyUp( const FKey& iKey )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorPickTool::OnKeyUpVector( vectorEngine, vectorScene, iKey );
    }

    return ret;
}

bool
UOdysseyTextureEditorVectorPickTool::OnMouseDown( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    FOdysseyTextureEditor* textureEditor =  GetEditorAs<FOdysseyTextureEditor>();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(textureEditor->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorPickTool::OnMouseDownVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

void
UOdysseyTextureEditorVectorPickTool::OnMouseDrag( const FOdysseyPoint& iPointInTexture )
{
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(GetEditorAs<FOdysseyTextureEditor>()->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());

    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        UOdysseyPainterEditorVectorPickTool::OnMouseDragVector( vectorEngine, vectorScene, iPointInTexture );
    }
}

bool
UOdysseyTextureEditorVectorPickTool::OnMouseUp( const FOdysseyPoint& iPointInTexture, const FKey& iKey )
{
    FOdysseyTextureEditor* textureEditor =  GetEditorAs<FOdysseyTextureEditor>();
    UOdysseyTextureLayerStack* layerStack = Cast<UOdysseyTextureLayerStack>(textureEditor->LayerStack());
    UOdysseyTextureLayerImageVector* currentVectorLayer = Cast<UOdysseyTextureLayerImageVector>(layerStack->CurrentLayer.Get());
    bool ret = false;

  /*if(FSlateApplication::Get().GetModifierKeys().IsControlDown())
  {*/
    if( iKey == EKeys::RightMouseButton )
    {
        TSharedPtr<SWidget> objectMenu = textureEditor->GetGUI()->GetVectorPickToolObjectContextMenu()->Widget();
        TSharedPtr<SWidget> vertexMenu = textureEditor->GetGUI()->GetVectorPickToolVertexContextMenu()->Widget();
        TSharedPtr<SWidget> contextMenu = ( EditionMode == EOdysseyVectorEditionMode::Object ) ? objectMenu : vertexMenu;


        FSlateApplication::Get().PushMenu(
        //textureEditor->GetGUI()->GetViewportTab().Get()->GetViewport().Get()->GetViewportWidget().ToSharedRef(),
        textureEditor->GetGUI()->GetViewportTab().Get()->Widget().ToSharedRef(),
        FWidgetPath(),
        contextMenu.ToSharedRef(),
        FSlateApplication::Get().GetCursorPos(),
        FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
        );
    }
  /*}*/
  else
    if( currentVectorLayer )
    {
        FOdysseyVectorEngine* vectorEngine = currentVectorLayer->GetEngine();
        FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

        ret = UOdysseyPainterEditorVectorPickTool::OnMouseUpVector( vectorEngine, vectorScene, iPointInTexture, iKey );
    }

    return ret;
}

void
UOdysseyTextureEditorVectorPickTool::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
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

        PropertyChangedVector( vectorEngine, vectorScene, PropertyChangedEvent.GetPropertyName());
    }
}

#undef LOCTEXT_NAMESPACE
