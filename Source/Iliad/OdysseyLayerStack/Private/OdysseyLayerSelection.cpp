// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyLayerSelection.h"

//TODO: Should belong to an editor module
#if WITH_EDITOR

#include "OdysseyLayerStack.h"
#include "OdysseyLayer.h"

FOdysseyLayerSelection::~FOdysseyLayerSelection()
{
    if (mLayerStack)
    {
        UnbindHierarchyChanged();
    }
}

FOdysseyLayerSelection::FOdysseyLayerSelection()
    : mLayerStack(nullptr)
{

}

FOdysseyLayerSelection::FOdysseyLayerSelection(UOdysseyLayerStack* iLayerStack)
    : mLayerStack(iLayerStack)
{
    if (mLayerStack)
    {
        BindHierarchyChanged();
    }
}

bool
FOdysseyLayerSelection::IsSelected( UOdysseyLayer* iLayer )
{
    return ( mSelectedLayers.Find( iLayer ) == INDEX_NONE ) ? false : true;
}

void
FOdysseyLayerSelection::Select( UOdysseyLayer* iLayer )
{
    mSelectedLayers.AddUnique( iLayer );
}

void
FOdysseyLayerSelection::DeselectAll()
{
    mSelectedLayers.Empty();
}

void
FOdysseyLayerSelection::Deselect( UOdysseyLayer* iLayer )
{
    mSelectedLayers.Remove( iLayer );
}

void
FOdysseyLayerSelection::BindHierarchyChanged()
{
    UOdysseyLayerStack::OnHierarchyChanged().AddRaw( this, &FOdysseyLayerSelection::OnHierarchyChanged );
}

void
FOdysseyLayerSelection::UnbindHierarchyChanged()
{
    UOdysseyLayerStack::OnHierarchyChanged().RemoveAll( this );
}

void
FOdysseyLayerSelection::OnHierarchyChanged( UOdysseyLayerStack* iLayerStack )
{
    if (!mLayerStack)
        return;

    if (iLayerStack != mLayerStack)
        return;

    TArray<UOdysseyLayer*> updatedLayerSelection;

    // check if the layer still exists in the list of layers that belong to the layer stack
    for( UOdysseyLayer* layer : mSelectedLayers )
    {
        if( iLayerStack->GetLayers().Find( layer ) != INDEX_NONE )
        {
            updatedLayerSelection.Push( layer );
        }
    }

    mSelectedLayers = updatedLayerSelection;
}

#endif
