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


UCLASS()
class ARIANE_API UArianeLayerDrawing : public UArianeLayer
{
    GENERATED_BODY()

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
    virtual void OnRegister() override;

protected:
    void BindDelegates();
    void UnbindDelegates();
    void OnRootObjectInvalidated();

protected:
    UPROPERTY( EditAnywhere, Instanced, Interp )
    UArianeImage* Image;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrigin DrawingOrigin;

    UPROPERTY( EditAnywhere )
    EArianeLayerDrawingOrientation DrawingOrientation;
};
