// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianeLayer.h"
#include "ArianeLayerVectorEnums.h"
#include "ArianeCoreEnums.h"
#include "ArianeGraph.h"
#include "ArianeMeshRenderItem.h"
#include "ArianeLayerVector.generated.h"

struct FArianeObject;
struct FArianeGroup;
struct FArianeCycle;
struct FArianePath;
class UMaterialInterface;
class UArianeDrawing;
struct FArianeEllipse;
struct FArianeRectangle;
struct FArianeLine;
struct FArianePolygon;

class UArianeLayerVector;

// See the comment at the top of ArianeLayerVector.cpp
extern TAutoConsoleVariable<bool> CVarShowArianeNormals;

class ARIANE_API FArianeGeometryProxy : public FPrimitiveSceneProxy
{
    public:
        ~FArianeGeometryProxy();
        FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                            , UArianeLayerVector* VectorLayer
                            , const TArray<FArianeMeshRenderItem>& InMeshItems );

        // A material that can't be used on a static mesh is replaced by the default surface material
        static UMaterialInterface* GetMeshCompatibleMaterial( UMaterialInterface* MaterialInterface );

        // Enqueued by UArianeLayerVector::EnqueueRenderItem(s)
        void SetMeshItem_RenderThread( int32 Index, const FArianeMeshRenderItem& Item );
        void SetMeshItems_RenderThread( TArray<FArianeMeshRenderItem>&& InMeshItems );

        virtual SIZE_T GetTypeHash() const override;
        virtual uint32 GetMemoryFootprint( void ) const override;

        virtual FPrimitiveViewRelevance GetViewRelevance( const FSceneView* View ) const override;
        virtual void GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                           , const FSceneViewFamily& ViewFamily
                                           , uint32 VisibilityMap
                                           , FMeshElementCollector& Collector) const override;
        virtual void DrawStaticElements( FStaticPrimitiveDrawInterface * PDI ) override;

    protected:
        void DrawMeshItem( const FArianeMeshRenderItem& Item
                         , const FSceneViewFamily& ViewFamily
                         , int32 ViewIndex
                         , FMeshElementCollector& Collector ) const;

    protected:
        // Render thread only: copy of the drawing's items
        TArray<FArianeMeshRenderItem> MeshItems;
        // Layer settings captured here: a change recreates the proxy (MarkRenderStateDirty)
        // UArianeLayer::IsVisible: includes the parent folders
        bool bLayerVisible;
        bool bCastShadows;
        FMaterialRelevance MaterialRelevance;
};

UCLASS()
class ARIANE_API UArianeLayerVector : public UArianeLayer
{
    GENERATED_BODY()


    DECLARE_MULTICAST_DELEGATE( FOnDrawingChanged );

public:
    ~UArianeLayerVector();
    UArianeLayerVector();

    virtual void Update( bool bInteractive ) override;

    /**
     * @brief Get the drawing origin
     * @return the drawing origin
     */
    EArianeDrawingOrigin GetDrawingOrigin();

    /**
     * @brief Set the drawing origin
     * @param the drawing origin type
     */
    void SetDrawingOrigin( EArianeDrawingOrigin InDrawingOrigin );

    /**
     * @brief Get the drawing orientation
     * @return the drawing orientation
     */
    EArianeDrawingOrientation GetDrawingOrientation();

    /**
     * @brief Set the drawing orientation
     * @param InDrawingOrientation the drawing orientation
     */
    void SetDrawingOrientation( EArianeDrawingOrientation InDrawingOrientation );

    //virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    UArianeDrawing* GetDrawing();
    void SetDrawing( UArianeDrawing* InDrawing, bool bTriggerEvent = true );
    virtual void OnRegister() override;

    virtual void GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials = false ) const override;
    virtual void OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType ) override;
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    FOnDrawingChanged& OnPreDrawingChangedDelegate();
    FOnDrawingChanged& OnPostDrawingChangedDelegate();
    virtual void Serialize(FArchive& Ar ) override;
    virtual void PreSave(FObjectPreSaveContext Context) override;
    virtual void PostLoad() override;
    bool GetOverrideDrawingSettings() const;
    void SetOverrideDrawingSettings(bool bInOverrideDrawingSettings);
    void SetDrawingDistanceToPlane( float InDrawingDistanceToPlane );
    float GetDrawingDistanceToPlane();

    // Game thread. No proxy: nothing to do, the next CreateSceneProxy copies the whole array
    void EnqueueRenderItem( int32 Index, const FArianeMeshRenderItem& Item );
    void EnqueueRenderItems( const TArray<FArianeMeshRenderItem>& Items );
    // The settings stored in every item changed (normals, editor HUD color): the displayed drawings rebuild their items
    static void RebuildAllRenderItems();

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootObjectInvalidated();
    virtual void PropertyChanged( const FName& iPropertyName
                                , const FName& iMemberPropertyName
                                , const FName& iCategory ) override;

protected:
    UPROPERTY( EditAnywhere, Instanced )
    UArianeDrawing* Drawing;

    UPROPERTY( EditAnywhere, Category = VectorLayerOptions )
    bool bOverrideDrawingSettings;

    UPROPERTY( EditAnywhere, Category = VectorLayerOptions
             , meta = ( ToolTip = "Drawing distance to plane"
                      , EditCondition = "bOverrideDrawingSettings == true"
                      , EditConditionHides ) )
    float DrawingDistanceToPlane;

    UPROPERTY( EditAnywhere, Category = VectorLayerOptions
             , meta = ( ToolTip = "Drawing Origin"
                      , EditCondition = "bOverrideDrawingSettings == true"
                      , EditConditionHides ) )
    EArianeDrawingOrigin DrawingOrigin;

    UPROPERTY( EditAnywhere, Category = VectorLayerOptions
             , meta = ( ToolTip = "Drawing Orientation"
                      , EditCondition = "bOverrideDrawingSettings == true"
                      , EditConditionHides ) )
    EArianeDrawingOrientation DrawingOrientation;

protected:
    FOnDrawingChanged OnPreDrawingChanged;
    FOnDrawingChanged OnPostDrawingChanged;
};
