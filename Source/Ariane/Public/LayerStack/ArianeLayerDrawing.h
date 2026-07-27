// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianeLayer.h"
#include "ArianeLayerDrawingEnums.h"
#include "ArianeCoreEnums.h"

#include "ArianeLayerDrawing.generated.h"

struct FArianeObject;
struct FArianeGroup;
struct FArianePath;
class UMaterialInterface;
struct FArianeEllipse;
struct FArianeRectangle;
struct FArianeLine;
struct FArianePolygon;

UCLASS()
class ARIANE_API UArianeLayerDrawing : public UArianeLayer
{
    GENERATED_BODY()

public:
    ~UArianeLayerDrawing();
    UArianeLayerDrawing();

    virtual void Update( bool bInteractive ) override;
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

    /**
     * @brief Get the drawing origin
     * @return the drawing origin
     */
    EArianeLayerDrawingOrigin GetDrawingOrigin();

    /**
     * @brief Set the drawing origin
     * @param the drawing origin type
     */
    void SetDrawingOrigin( EArianeLayerDrawingOrigin InDrawingOrigin );

    /**
     * @brief Get the drawing orientation
     * @return the drawing orientation
     */
    EArianeLayerDrawingOrientation GetDrawingOrientation();

    /**
     * @brief Set the drawing orientation
     * @param InDrawingOrientation the drawing orientation
     */
    void SetDrawingOrientation( EArianeLayerDrawingOrientation InDrawingOrientation );

    void IncrementMaterial( UMaterialInterface* MaterialInterface );
    void DecrementMaterial( UMaterialInterface* MaterialInterface );

    /**
     * @brief Get used materials
     * @OutUsedMaterials output array
     * @bEmptyFirst empty the output array first.
     */
    void GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials );

    void AppendUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials );
    void ClearObjectSelection();
    void SelectObject( FArianeObject* ObjectToSelect );
    const TArray<FArianeObject*>& GetSelectedObjects() const;
    TArray<FArianeObject*>& GetSelectedObjects();
    void AppendSelectedTrees( TArray<FArianeObject*>& SelectedTrees );
    void GetSelectedTrees( TArray<FArianeObject*>& SelectedTrees );
    void UnselectObject( FArianeObject* ObjectToSelect );
    void InvalidateCache();
    FArianeEllipse* AllocEllipse( const FName& InName
                                , double RadiusX
                                , double RadiusY
                                , double StrokeWidth
                                , EArianeAllocationModel AllocationModel );
    FArianeRectangle* AllocRectangle( const FName& InName
                                    , double Width
                                    , double Height
                                    , double StrokeWidth
                                    , EArianeAllocationModel AllocationModel );
    FArianeLine* AllocLine( const FName& InName
                          , const FVector& StartPoint
                          , const FVector& EndPoint
                          , double StrokeWidth
                          , EArianeAllocationModel AllocationModel );
    FArianePolygon* AllocPolygon( const FName& InName
                                , uint32 CornerCount
                                , double Radius
                                , double StrokeWidth
                                , EArianeAllocationModel AllocationModel );
    virtual void OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType ) override;

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootObjectInvalidated();
    void UpdateBounds();

public:
    UPROPERTY( EditAnywhere )
    TArray<FInstancedStruct> InstancedObjects;

public:
    mutable FCriticalSection InstancedObjectsAccessRW;

protected:
    UPROPERTY( EditAnywhere )
    FArianeObjectID RootGroupID;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrigin DrawingOrigin;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrientation DrawingOrientation;

protected:
    TMap<UMaterialInterface*, uint32> UsedMaterials;

    // Temp
    TArray<FArianeObject*> SelectedObjects;
};
