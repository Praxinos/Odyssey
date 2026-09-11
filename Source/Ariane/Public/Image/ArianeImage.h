// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianeLayer.h"
#include "ArianeCoreEnums.h"
#include "ArianeGraph.h"
#include "ArianeImage.generated.h"

struct FArianeObject;
struct FArianeGroup;
struct FArianeCycle;
struct FArianePath;
class UMaterialInterface;
struct FArianeEllipse;
struct FArianeRectangle;
struct FArianeLine;
struct FArianePolygon;
class UArianeLayerDrawing;
struct FArianeImageKeyData;

UCLASS()
class ARIANE_API UArianeImage : public UObject
{
    GENERATED_BODY()

public:
    ~UArianeImage();
    UArianeImage();

    virtual void Update( bool bInteractive );
    virtual void PostEditUndo() override;
    virtual void PostLoad() override;

    /**
     * @brief Get the top-most vector object
     * @return the top-most vector object
     */
    FArianeGroup* GetRootGroup();

    /**
     * @brief Allocate a new path (in a FInstancedStruct)
     * @param InMaterialInterface a material interface or nullptr to use the default one
     * @param AllocationModel
     * @return the new path
     */
    FArianePath* AllocPath( UMaterialInterface* InMaterialInterface
                          , const FName& InName
                          , EArianeAllocationModel AllocationModel );

    /**
     * @brief Allocate a new basic object (in a FInstancedStruct)
     * @param AllocationModel
     * @return the new object
     */
    FArianeObject* AllocObject( const FName& InName, EArianeAllocationModel AllocationModel );

    /**
     * @brief Allocate a new group (in a FInstancedStruct)
     * @param AllocationModel
     * @return the new group
     */
    FArianeGroup* AllocGroup( const FName& InName, EArianeAllocationModel AllocationModel );


    /**
     * @brief Get all instanced objects
     * @return an array of all instanced objects
     */
    const TArray<FInstancedStruct>& GetInstancedObjects() const;

    TArray<FInstancedStruct>& GetInstancedObjects();

    /**
     * @brief Get an instanced object by its ID
     * @return the desired object
     */
    FArianeObject* GetObject( const FGuid& InGuid );

    /**
     * @brief Delete (deallocate) an instanced object
     * @param the object to deallocate
     */
    void DeleteInstancedObject( FArianeObject* Object );

    /** Empty the whole object tree and create a new one **/
    void ResetHierarchy();

    // debug
    void PrintPointers();

    void IncrementMaterial( UMaterialInterface* MaterialInterface );
    void DecrementMaterial( UMaterialInterface* MaterialInterface );

    /**
     * @brief Get used materials
     * @OutUsedMaterials output array
     * @bEmptyFirst empty the output array first.
     */
    void GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials = false ) const;

    void AppendUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials ) const;
    void ClearObjectSelection();
    void SelectObject( FArianeObject* ObjectToSelect );
    const TArray<FArianeObject*>& GetSelectedObjects() const;
    TArray<FArianeObject*>& GetSelectedObjects();
    void AppendSelectedTrees( TArray<FArianeObject*>& SelectedTrees );
    void GetSelectedTrees( TArray<FArianeObject*>& SelectedTrees );
    void UnselectObject( FArianeObject* ObjectToSelect );
    void InvalidateCache();
    FArianeEllipse* AllocEllipse( UMaterialInterface* InMaterialInterface
                                , const FName& InName
                                , double RadiusX
                                , double RadiusY
                                , double StrokeWidth
                                , EArianeAllocationModel AllocationModel );
    FArianeRectangle* AllocRectangle( UMaterialInterface* InMaterialInterface
                                    , const FName& InName
                                    , double Width
                                    , double Height
                                    , double StrokeWidth
                                    , EArianeAllocationModel AllocationModel );
    FArianeLine* AllocLine( UMaterialInterface* InMaterialInterface
                          , const FName& InName
                          , const FVector& StartPoint
                          , const FVector& EndPoint
                          , double StrokeWidth
                          , EArianeAllocationModel AllocationModel );
    FArianePolygon* AllocPolygon( UMaterialInterface* InMaterialInterface
                                , const FName& InName
                                , uint32 CornerCount
                                , double Radius
                                , double StrokeWidth
                                , EArianeAllocationModel AllocationModel );
    FArianeCycle* AllocCycle( UMaterialInterface* InMaterialInterface
                            , const FName& InName
                            , EArianeAllocationModel AllocationModel );
    virtual void BeginDestroy() override;
    //virtual void InitializeComponent() override;

    void SetDrawingLayer( TWeakObjectPtr<UArianeLayerDrawing> InDrawingLayer );
    TWeakObjectPtr<UArianeLayerDrawing> GetDrawingLayer();
    void OnRegisterLayer();
    void Animate( const FArianeImageKeyData* KeyData, const FArianeImageKeyData* NextKeyData, float T );

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootObjectInvalidated();


public:
    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedObjects;

public:
    mutable FCriticalSection InstancedObjectsAccessRW;

protected:
    UPROPERTY( EditAnywhere )
    mutable FArianeObjectID RootGroupID;

    TWeakObjectPtr<UArianeLayerDrawing> DrawingLayer;

protected:
    // counter to determine how many times the material is used (i.e by how many objects)
    TMap<UMaterialInterface*, uint32> UsedMaterials;

    // Temp
    TArray<FArianeObject*> SelectedObjects;
};
