// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Math/Color.h"
#include "OdysseyStrokePoint.h"
#include "OdysseyBrushPreferencesOverrides.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushPivot.h"
#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushCache.h"
#include <ULIS_CCOLOR>
#include <ULIS_MATHS>
#include <ULIS_BLENDINGMODES>
#include "OdysseyBrushAssetBase.generated.h"

class UTexture2D;
class FOdysseyBlock;

/////////////////////////////////////////////////////
// FOdysseyBrushState
struct  FOdysseyBrushState
{
    FOdysseyBlock*                          target_temp_buffer;
    FOdysseyStrokePoint                     point;
    ::ULIS::CColor                          color;
    float                                   size_modifier;
    float                                   opacity_modifier;
    float                                   flow_modifier;
    ::ULIS::eBlendingMode                   blendingMode_modifier;
    ::ULIS::eAlphaMode                      alphaMode_modifier;
    float                                   step;
    float                                   smoothing_strength;
    int                                     currentPointIndex;
    const  TArray< FOdysseyStrokePoint >*   currentStroke;
};


/////////////////////////////////////////////////////
// FOdysseyBrushPoolCache
class ODYSSEYBRUSH_API FOdysseyBrushPoolCache
{
public:
    // Public API
    bool                        KeyExists(  const  FString&  iKey )  const;
    const  FOdysseyBlockProxy&  Retrieve(   const  FString&  iKey )  const;
    void                        Store(      const  FString&  iKey, const  FOdysseyBlockProxy&  iValue );
    void                        Cleanse();

private:
    // Private Data Members
    TMap< FString, FOdysseyBlockProxy >  pool;
};


/////////////////////////////////////////////////////
// BrushAssetBase
/**
 * BrushAssetBase
 * Base class for OdysseyBrush Blueprint.
 */
UCLASS(Abstract, hideCategories=(Object), Blueprintable)
class ODYSSEYBRUSH_API UOdysseyBrushAssetBase : public UObject
{
    GENERATED_UCLASS_BODY()

public:
    // Construction / Destruction
    ~UOdysseyBrushAssetBase();

public:
    // Public C++ API
    FOdysseyBrushState&             GetState();
    const TArray< ::ULIS::FRect >&  GetInvalidRects() const;
    void                            PushInvalidRect( const  ::ULIS::FRect& iRect );
    void                            ClearInvalidRects();

    bool                            KeyExistsInPool(    ECacheLevel iLevel, const  FString&  iKey )  const;
    void                            StoreInPool(        ECacheLevel iLevel, const  FString&  iKey, const  FOdysseyBlockProxy&  iValue );
    FOdysseyBlockProxy              RetrieveInPool(     ECacheLevel iLevel, const  FString&  iKey )  const;
    void                            CleansePool(        ECacheLevel iLevel );
    void                            CleansePools();

public:
    // OdysseyBrushBlueprint Getters

    /*******************************/
    /** State Stroke Point Getters */
    /*******************************/

    /** Get X */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetX();

    /** Get Y */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetY();

    /** Get Z */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetZ();

    /** Get Pressure */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetPressure();

    /** Get Altitude */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAltitude();

    /** Get Altitude Normalized */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAltitudeNormalized();

    /** Get Azimuth */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAzimuth();

    /** Get Azimuth Normalized */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAzimuthNormalized();

    /** Get Twist */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetTwist();

    /** Get Twist Normalized */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetTwistNormalized();

    /** Get Pitch */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetPitch();

    /** Get Pitch Normalized */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetPitchNormalized();

    /** Get Roll */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetRoll();

    /** Get Roll Normalized */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetRollNormalized();

    /** Get Yaw */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetYaw();

    /** Get Yaw Normalized */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetYawNormalized();

    /** Get distance travelled */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetDistanceAlongStroke();

    /** Get Direction Vector Tangent */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetDirectionVectorTangent();

    /** Get Direction Vector Normal */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetDirectionVectorNormal();

    /** Get Direction Angle Tangent as Degrees */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetDirectionAngleTangentDeg();

    /** Get Direction Angle Normal as Degrees */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetDirectionAngleNormalDeg();

    /** Get Speed */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetSpeed();

    /** Get Acceleration */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetAcceleration();

    /** Get Jolt */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetJolt();

    /*******************************/
    /**       State Getters        */
    /*******************************/

    /** Get Color */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FOdysseyBrushColor  GetColor();

    /** Get Size Modifier */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetSizeModifier();

    /** Get Opacity Modifier */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetOpacityModifier();

    /** Get Flow Modifier */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetFlowModifier();

    /** Get Step */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetStep();

    /** Get Smoothing Strength */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetSmoothingStrength();

    /** Get Current Stroke Point Index */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    int  GetCurrentStrokePointIndex();

    /** Get Current Canvas Width */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    int  GetCanvasWidth();

    /** Get Current Canvas Height */
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    int  GetCanvasHeight();

public:
    // Odyssey Brush Native events
    UFUNCTION(BlueprintNativeEvent)
    void OnSelected();

    UFUNCTION(BlueprintNativeEvent)
    void OnTick();

    UFUNCTION(BlueprintNativeEvent)
    void OnStep();

    UFUNCTION(BlueprintNativeEvent)
    void OnStateChanged();

    UFUNCTION(BlueprintNativeEvent)
    void OnStrokeBegin();

    UFUNCTION(BlueprintNativeEvent)
    void OnStrokeEnd();

public:
    // Odyssey Brush Public Driving Methods
    /* Run the stamp action */
    void ExecuteSelected();

    /* Run the stamp action */
    void ExecuteTick();

    /* Run the step action */
    void ExecuteStep();

    /* Run the state change action */
    void ExecuteStateChanged();

    /* Run the stroke begin action */
    void ExecuteStrokeBegin();

    /* Run the stroke end action */
    void ExecuteStrokeEnd();

private:
    // Private Members Data
    FOdysseyBrushState                      state;
    TArray< ::ULIS::FRect >                 invalid_rects;
    TArray< FOdysseyBrushPoolCache >        pools;

public:
    UPROPERTY(EditAnywhere,Category="Overrides")
    FOdysseyBrushPreferencesOverrides       Preferences;
};
