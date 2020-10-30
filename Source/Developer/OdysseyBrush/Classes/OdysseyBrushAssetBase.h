// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "InputCoreTypes.h" 
#include "Math/Color.h"
#include "OdysseyStrokePoint.h"
#include "OdysseyDrawingState.h"
#include "OdysseyBrushPreferencesOverrides.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushPivot.h"
#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushCache.h"
#include <ULIS3>
#include "OdysseyBrushAssetBase.generated.h"

class FOdysseyBlock;

/////////////////////////////////////////////////////
// FOdysseyBrushState
struct  FOdysseyBrushState
{
    FOdysseyBrushState()
    {}

    FOdysseyBlock*                          target_temp_buffer;
    FOdysseyStrokePoint                     point;
    ::ul3::FPixelValue                      color;
    float                                   size_modifier;
    float                                   opacity_modifier;
    float                                   flow_modifier;
    ::ul3::eBlendingMode                    blendingMode_modifier;
    ::ul3::eAlphaMode                       alphaMode_modifier;
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
    /** Getter for World Pointer, this workaround may be unsafe but allows us to use Blueprint Function Libraries withing Odyssey Brushes. It is always NULL in a brush context. */
    virtual UWorld* GetWorld() const override final { return  nullptr; }

    // Public C++ API
    FOdysseyBrushState&             GetState();
    const TArray< ::ul3::FRect >&   GetInvalidRects() const;
    void                            PushInvalidRect( const  ::ul3::FRect& iRect );
    void                            ClearInvalidRects();

    bool                            KeyExistsInPool(    ECacheLevel iLevel, const  FString&  iKey )  const;
    void                            StoreInPool(        ECacheLevel iLevel, const  FString&  iKey, const  FOdysseyBlockProxy&  iValue );
    FOdysseyBlockProxy              RetrieveInPool(     ECacheLevel iLevel, const  FString&  iKey )  const;
    void                            CleansePool(        ECacheLevel iLevel );
    void                            CleansePools();

    void                            AddOrReplaceState( const FName& iKey, FOdysseyDrawingState* iState );
    FOdysseyDrawingState*           FindState( const FName& iKey ); //TODO: maybe not needed ?
    const FOdysseyDrawingState*     FindState( const FName& iKey ) const;

public:
    // OdysseyBrushBlueprint Getters

    /*******************************/
    /** State Stroke Point Getters */
    /*******************************/

    //Gets the X axis of the stylus or mouse on ILIAD canvas.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetX();

    //Gets the Y axis of the stylus or mouse on ILIAD canvas.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetY();

    /** Gets Z */
    //Won't work.
    //UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    //float  GetZ();

    //Gets stylus pressure on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns normalized Float (0 = no pressure | 1 = max pressure).
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetPressure();

    //Gets stylus altitude on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns Float (angle).
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAltitude();

    //Gets stylus altitude on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns normalized Float (0.5 = 45° | 1 = 90°).
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAltitudeNormalized();

    //Gets stylus azimtuh on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns Float (angle).
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAzimuth();

    //Gets stylus azimuth on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns normalized Float (0.5 = 180° | 1 = 360°).
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetAzimuthNormalized();

    //Gets stylus twist on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Only works with specific stylus !
    //Returns Float (angle).
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetTwist();

    //Gets stylus twist on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Only works with specific stylus !
    //Returns normalize Float (0.5 = 180° | 1 = 360°).
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetTwistNormalized();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    //float  GetPitch();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    //float  GetPitchNormalized();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    //float  GetRoll();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    //float  GetRollNormalized();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    //float  GetYaw();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    //float  GetYawNormalized();

    //Gets distance travelled along the stroke.
    //Returns a Float in pixels.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetDistanceAlongStroke();

    //Gets Direction Vector Tangent.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetDirectionVectorTangent();

    //Gets Direction Vector Normal.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetDirectionVectorNormal();

    //Gets Direction Angle Tangent as Degrees.
    //Returns an angle as a Float.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetDirectionAngleTangentDeg();

    //Gets Direction Angle Normal as Degrees
    //Returns an angle as a float.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetDirectionAngleNormalDeg();

    //Gets Speed.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetSpeed();

    //Gets Acceleration.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetAcceleration();

    //Gets Jolt .
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetJolt();

    //Gets Delta Position between this event and the last.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FVector2D GetDeltaPosition();

    //Gets Delta Time in micro seconds between this event and the last.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    int GetDeltaTime();

    //Returns the keyboard and mouse keys down at this point.
    UFUNCTION(BlueprintPure,Category="OdysseyBrush")
    TArray<FKey> GetKeysDown();


    /*******************************/
    /**       State Getters        */
    /*******************************/

    //Gets color from Color Selector and Color Sliders.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    FOdysseyBrushColor  GetColor();

    //Gets size from Top Bar.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetSizeModifier();

    //Gets opacity from Top Bar.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetOpacityModifier();

    //Gets flow from Top Bar.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetFlowModifier();

    //Gets step from Stroke Options.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetStep();

    //Gets smoothing strength from Stroke Options.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    float  GetSmoothingStrength();

    //Gets index of each stamp applied in the stroke.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    int  GetCurrentStrokePointIndex();

    //Gets width of the Texture asset currently modified.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    int  GetCanvasWidth();

    //Gets height of the Texture asset currently modified.
    UFUNCTION( BlueprintPure, Category="OdysseyBrush" )
    int  GetCanvasHeight();

    //Gets Stroke Block.
    UFUNCTION( BlueprintCallable, Category = "OdysseyBrush")
    FOdysseyBlockProxy GetStrokeBlock( int X, int Y, int Width, int Height );

public:
    // Odyssey Brush Native events
    
    //Event is triggered when the brush is loaded.
    UFUNCTION(BlueprintNativeEvent)
    void OnSelected();

    UFUNCTION(BlueprintNativeEvent)
    void OnTick();
    
    //Event is triggered at each step of the stroke.
    UFUNCTION(BlueprintNativeEvent)
    void OnStep();

    //Event is triggered when anything is changed in ILIAD interface (variables or modifiers).
    UFUNCTION(BlueprintNativeEvent)
    void OnStateChanged();

    //Event is triggered when stroke begins on the canvas (after clicking or touching the tablet with the stylus tip).
    UFUNCTION(BlueprintNativeEvent)
    void OnStrokeBegin();

    //Event is triggered when the stroke ends (when the click or stylus is dropped)
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
    TMap< FName, FOdysseyDrawingState* >    mStates;
    TArray< ::ul3::FRect >                  invalid_rects;
    TArray< FOdysseyBrushPoolCache >        pools;

public:
    UPROPERTY(EditAnywhere,Category="Overrides")
    FOdysseyBrushPreferencesOverrides       Preferences;
};
