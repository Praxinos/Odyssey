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
#include "ArianeGraph.h"
#include "ArianeLayerDrawing.generated.h"

struct FArianeObject;
struct FArianeGroup;
struct FArianeCycle;
struct FArianePath;
class UMaterialInterface;
class UArianeImage;
struct FArianeEllipse;
struct FArianeRectangle;
struct FArianeLine;
struct FArianePolygon;

class UArianeLayerDrawing;

class ARIANE_API FArianeGeometryProxy : public FPrimitiveSceneProxy
{
    public:
        ~FArianeGeometryProxy();
        FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel, UArianeLayerDrawing* DrawingLayer );

        virtual SIZE_T GetTypeHash() const override;
        virtual uint32 GetMemoryFootprint( void ) const override;

        virtual FPrimitiveViewRelevance GetViewRelevance( const FSceneView* View ) const override;
        virtual void GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                           , const FSceneViewFamily& ViewFamily
                                           , uint32 VisibilityMap
                                           , FMeshElementCollector& Collector) const override;
        void GetImageDynamicMeshElements( FMeshElementCollector& Collector
                                        , int32 ViewIndex ) const;
        virtual void DrawStaticElements( FStaticPrimitiveDrawInterface * PDI ) override;

    protected:
        UArianeLayerDrawing* DrawingLayer;
        FMaterialRelevance MaterialRelevance;
};

UCLASS()
class ARIANE_API UArianeLayerDrawing : public UArianeLayer
{
    GENERATED_BODY()


    DECLARE_MULTICAST_DELEGATE( FOnImageChanged );

public:
    ~UArianeLayerDrawing();
    UArianeLayerDrawing();

    virtual void Update( bool bInteractive ) override;

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

    //virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    UArianeImage* GetImage();
    void SetImage( UArianeImage* InImage, bool bTriggerEvent = true );
    virtual void OnRegister() override;
    void OnAssetLoaded(UObject* LoadedObject);

    virtual void GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials = false ) const override;
    virtual void OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType ) override;
    virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
    virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
    FOnImageChanged& OnPreImageChangedDelegate();
    FOnImageChanged& OnPostImageChangedDelegate();

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootObjectInvalidated();
    virtual void BeginDestroy() override;

protected:
    UPROPERTY( EditAnywhere, Instanced )
    UArianeImage* Image;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrigin DrawingOrigin;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrientation DrawingOrientation;

protected:
    FOnImageChanged OnPreImageChanged;
    FOnImageChanged OnPostImageChanged;
};
