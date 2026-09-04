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

class ARIANE_API FArianeGeometryProxy : public FPrimitiveSceneProxy
{
    public:
        ~FArianeGeometryProxy();
        FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel, UArianeImage* InImage );

        virtual SIZE_T GetTypeHash() const override;
        virtual uint32 GetMemoryFootprint( void ) const override;

        virtual FPrimitiveViewRelevance GetViewRelevance( const FSceneView* View ) const override;
        virtual void GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                           , const FSceneViewFamily& ViewFamily
                                           , uint32 VisibilityMap
                                           , FMeshElementCollector& Collector) const override;
        void GetImageDynamicMeshElements( FMeshElementCollector& Collector
                                        , int32 ViewIndex ) const;
        void InitVertexFactory();
        virtual void DrawStaticElements( FStaticPrimitiveDrawInterface * PDI ) override;

    protected:
        UArianeImage* Image;
        FMaterialRelevance MaterialRelevance;
};


UCLASS( BlueprintType, Blueprintable, EditInlineNew, meta=(BlueprintSpawnableComponent) )
class ARIANE_API UArianeImage : public UMeshComponent
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
    virtual void GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials = false ) const override;

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
    virtual void OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType ) override;
    virtual void BeginDestroy() override;
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    //virtual void InitializeComponent() override;
    void OnAssetLoaded(UObject* LoadedObject);
    UArianeLayerDrawing* GetDrawingLayer();
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

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

protected:
    TMap<UMaterialInterface*, uint32> UsedMaterials;

    // Temp
    TArray<FArianeObject*> SelectedObjects;
};
