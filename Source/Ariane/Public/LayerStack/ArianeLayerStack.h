// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"

#include "ArianeLayerStack.generated.h"

class UArianeLayer;
class UArianeLayerDrawing;
class UArianeLayerFolder;
class UArianeLayerFolder;
class UArianePainting3DComponent;

UCLASS()
class ARIANE_API UArianeLayerStack : public USceneComponent
{
    GENERATED_BODY()

    DECLARE_MULTICAST_DELEGATE( FOnLayerStackChanged );
    DECLARE_MULTICAST_DELEGATE( FOnLayerSelectionChanged );

public:
    ~UArianeLayerStack();
    UArianeLayerStack();

    UArianeLayerFolder* GetRootFolder();

    void RemoveSelectedLayers();
    void SelectAllLayers();
    void SelectLayer( UArianeLayer* Layer, bool bTriggerevent, bool bRecurse = true );
    void SelectLayers( const TArray<UArianeLayer*> LayerSelection, bool bClearSelectionFirst, bool bTriggerevent, bool bRecurse = true );
    void ClearLayerSelection( bool bTriggerEvent );
    const TArray<UArianeLayer*>& GetSelectedLayers();
    UArianeLayerDrawing* GetFirstSelectedDrawingLayer();
    UArianeLayerDrawing* CreateDrawingLayer( UArianeLayerFolder* ParentLayerFolder );
    UArianeLayerFolder* CreateFolderLayer( UArianeLayerFolder* ParentLayerFolder );
    UArianePainting3DComponent* GetPainting3DComponent();
    void OnComponentDestroyed();
    void GetLayers( TArray<UArianeLayer*>& Layers );
    void Init();
    UArianeLayer* GetCurrentLayer();

    FOnLayerStackChanged& OnPreLayerStackChangedDelegate();
    FOnLayerStackChanged& OnPostLayerStackChangedDelegate();

    FOnLayerSelectionChanged& OnPreLayerSelectionChangedDelegate();
    FOnLayerSelectionChanged& OnPostLayerSelectionChangedDelegate();

protected:
    void SelectLayer_Private( UArianeLayer* Layer, bool bRecurse );

protected:
    UPROPERTY()
    UArianeLayerFolder* RootFolder;

    UPROPERTY()
    TArray<UArianeLayer*> SelectedLayers;

protected:
    FOnLayerStackChanged OnPreLayerStackChanged;
    FOnLayerStackChanged OnPostLayerStackChanged;

    FOnLayerSelectionChanged OnPreLayerSelectionChanged;
    FOnLayerSelectionChanged OnPostLayerSelectionChanged;
};
