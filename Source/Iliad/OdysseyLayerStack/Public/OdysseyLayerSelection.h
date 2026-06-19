// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019
#pragma once

#include "CoreMinimal.h"

//TODO: Should belong to an editor module
#if WITH_EDITOR

class UOdysseyLayerStack;
class UOdysseyLayer;

class ODYSSEYLAYERSTACK_API FOdysseyLayerSelection
{
    public:
        ~FOdysseyLayerSelection();
        FOdysseyLayerSelection();
        FOdysseyLayerSelection( UOdysseyLayerStack* iLayerStack );

    public:
        bool IsSelected( UOdysseyLayer* iLayer );
        void Select( UOdysseyLayer* iLayer );
        void DeselectAll();
        void Deselect( UOdysseyLayer* iLayer );
        void BindHierarchyChanged();
        void UnbindHierarchyChanged();
        void OnHierarchyChanged( UOdysseyLayerStack* iLayerStack );

    private:
        UOdysseyLayerStack* mLayerStack;
        TArray<UOdysseyLayer*> mSelectedLayers;
};

#endif
