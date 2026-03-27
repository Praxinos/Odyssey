// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeLayerStack.generated.h"

class UArianeLayer;
class UArianeLayerDrawing;
class UArianeLayerFolder;

UCLASS()
class ARIANE_API UArianeLayerStack : public UObject
{
    GENERATED_BODY()

    DECLARE_MULTICAST_DELEGATE( FOnLayerStackChanged );
    DECLARE_MULTICAST_DELEGATE( FOnCurrentLayerChanged );

public:
    ~UArianeLayerStack();
    UArianeLayerStack();

    UArianeLayerFolder* GetRootLayerFolder();

    void RemoveSelectedLayers();
    void SelectAllLayers();
    void SelectLayer( UArianeLayer* Layer, bool bRecurse = true );
    void SelectLayers( const TArray<UArianeLayer*> LayerSelection, bool bRecurse = true );
    void ClearLayerSelection();
    const TArray<UArianeLayer*>& GetSelectedLayers();
    UArianeLayerDrawing* GetFirstSelectedDrawingLayer();
    UArianeLayerDrawing* CreateDrawingLayer( UArianeLayerFolder* ParentLayerFolder );
    UArianeLayerFolder* CreateFolderLayer( UArianeLayerFolder* ParentLayerFolder );

    FOnLayerStackChanged& OnPreLayerStackChangedDelegate();
    FOnLayerStackChanged& OnPostLayerStackChangedDelegate();

    FOnCurrentLayerChanged& OnPreCurrentLayerChangedDelegate();
    FOnCurrentLayerChanged& OnPostCurrentLayerChangedDelegate();

protected:
    void SelectLayer_Private( UArianeLayer* Layer, bool bRecurse );

protected:
    UPROPERTY()
    UArianeLayerFolder* RootLayerFolder;

    UPROPERTY()
    TArray<UArianeLayer*> SelectedLayers;

protected:
    FOnLayerStackChanged OnPreLayerStackChanged;
    FOnLayerStackChanged OnPostLayerStackChanged;

    FOnCurrentLayerChanged OnPreCurrentLayerChanged;
    FOnCurrentLayerChanged OnPostCurrentLayerChanged;
};
