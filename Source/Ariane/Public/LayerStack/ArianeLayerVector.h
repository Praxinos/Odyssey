// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019


#pragma once

// Unreal headers
#include "CoreMinimal.h"
#include "StructUtils/InstancedStruct.h"
// Ariane Headers
#include "ArianeID.h"
#include "ArianeLayer.h"
#include "ArianeLayerVectorEnums.h"
#include "ArianeCoreEnums.h"
#include "ArianeGraph.h"
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

class ARIANE_API FArianeGeometryProxy : public FPrimitiveSceneProxy
{
    public:
        ~FArianeGeometryProxy();
        FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel, UArianeLayerVector* VectorLayer );

        virtual SIZE_T GetTypeHash() const override;
        virtual uint32 GetMemoryFootprint( void ) const override;

        virtual FPrimitiveViewRelevance GetViewRelevance( const FSceneView* View ) const override;
        virtual void GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                           , const FSceneViewFamily& ViewFamily
                                           , uint32 VisibilityMap
                                           , FMeshElementCollector& Collector) const override;
        void GetDrawingDynamicMeshElements( const FSceneViewFamily& ViewFamily
                                          , FMeshElementCollector& Collector
                                          , int32 ViewIndex ) const;
        virtual void DrawStaticElements( FStaticPrimitiveDrawInterface * PDI ) override;

    protected:
        UArianeLayerVector* VectorLayer;
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
    void OnAssetLoaded(UObject* LoadedObject);

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

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootObjectInvalidated();
    virtual void BeginDestroy() override;
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
