// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
#include "Proxies/OdysseyBrushRect.h"
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

    void                            AddOrReplaceState( const FName& iKey, FOdysseyDrawingState* iState );
    FOdysseyDrawingState*           FindState( const FName& iKey ); //TODO: maybe not needed ?
    const FOdysseyDrawingState*     FindState( const FName& iKey ) const;

public:
    // OdysseyBrushBlueprint Getters

    /*******************************/
    /** State Stroke Point Getters */
    /*******************************/

    //Gets the X axis of the stylus or mouse on ILIAD canvas.
    UFUNCTION( BlueprintPure, Category="Odyssey|Canvas" )
    float  GetX();

    //Gets the Y axis of the stylus or mouse on ILIAD canvas.
    UFUNCTION( BlueprintPure, Category="Odyssey|Canvas" )
    float  GetY();

    /** Gets Z */
    //Won't work.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Canvas" )
    //float  GetZ();

    //Gets stylus pressure on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns normalized Float (0 = no pressure | 1 = max pressure).
    UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Pressure", KeyWords = "Tablet") )
    float  GetPressure();

    //Gets stylus altitude on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns Float (angle).
    UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Altitude", KeyWords = "Tablet Tilt") )
    float  GetAltitude();

    //Gets stylus altitude on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns normalized Float (0.5 = 45° | 1 = 90°).
    UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Altitude Normalized", KeyWords = "Tablet") )
    float  GetAltitudeNormalized();

    //Gets stylus azimtuh on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns Float (angle).
    UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Azimuth", KeyWords = "Tablet") )
    float  GetAzimuth();

    //Gets stylus azimuth on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Returns normalized Float (0.5 = 180° | 1 = 360°).
    UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Azimuth Normalized", KeyWords = "Tablet" ) )
    float  GetAzimuthNormalized();

    //Gets stylus twist on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Only works with specific stylus !
    //Returns Float (angle).
    UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Twist", KeyWords = "Tablet" ) )
    float  GetTwist();

    //Gets stylus twist on the tablet (make sure Tablet drivers are activated in Preferences > ILIAD Stylus Input).
    //Only works with specific stylus !
    //Returns normalize Float (0.5 = 180° | 1 = 360°).
    UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Twist Normalized", KeyWords = "Tablet" ) )
    float  GetTwistNormalized();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Pitch", KeyWords = "Tablet") )
    //float  GetPitch();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Pitch Normalized", KeyWords = "Tablet") )
    //float  GetPitchNormalized();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Roll", KeyWords = "Tablet") )
    //float  GetRoll();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Roll Normalized", KeyWords = "Tablet") )
    //float  GetRollNormalized();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Yaw", KeyWords = "Tablet") )
    //float  GetYaw();

    //Won't work.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Input", meta = ( DisplayName = "Get Stylus Yaw Normalized", KeyWords = "Tablet") )
    //float  GetYawNormalized();

    //Gets distance travelled along the stroke.
    //Returns a Float in pixels.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    float  GetDistanceAlongStroke();

    //Gets Direction Vector Tangent.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    FVector2D GetDirectionVectorTangent();

    //Gets Direction Vector Normal.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    FVector2D GetDirectionVectorNormal();

    //Gets Direction Angle Tangent as Degrees.
    //Returns an angle as a Float.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    float  GetDirectionAngleTangentDeg();

    //Gets Direction Angle Normal as Degrees
    //Returns an angle as a float.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    float  GetDirectionAngleNormalDeg();

    //Gets Speed.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    FVector2D GetSpeed();

    //Gets Acceleration.
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    FVector2D GetAcceleration();

    //Gets Jolt .
    //Returns a Vector 2D.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    FVector2D GetJolt();

    //Gets Delta Position between this event and the last.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    FVector2D GetDeltaPosition();

    //Gets Delta Time in micro seconds between this event and the last.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    int GetDeltaTime();

    //Returns the keyboard and mouse keys down at this point.
    UFUNCTION(BlueprintPure,Category="Odyssey|Input")
    TArray<FKey> GetKeysDown();


    /*******************************/
    /**       State Getters        */
    /*******************************/

    //Gets color from Color Selector and Color Sliders.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers", meta = ( DisplayName = "Get Color Modifier" ) )
    FOdysseyBrushColor  GetColor();

    //Gets size from Top Bar.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers" )
    float  GetSizeModifier();

    //Gets opacity from Top Bar.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers" )
    float  GetOpacityModifier();

    //Gets flow from Top Bar.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers" )
    float  GetFlowModifier();

    //Gets step from Stroke Options.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers", meta = ( DisplayName = "Get Step Modifier" ) )
    float  GetStep();

    //Gets smoothing strength from Stroke Options.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers", meta = ( DisplayName = "Get Smoothing Strength Modifier" ) )
    float  GetSmoothingStrength();

    //Gets index of each stamp applied in the stroke.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    int  GetCurrentStrokePointIndex();

    //Gets width of the Texture asset currently modified.
    UFUNCTION( BlueprintPure, Category="Odyssey|Canvas" )
    int  GetCanvasWidth();

    //Gets height of the Texture asset currently modified.
    UFUNCTION( BlueprintPure, Category="Odyssey|Canvas" )
    int  GetCanvasHeight();

    //Get the Canvas Color Model
    UFUNCTION(BlueprintPure, Category="Odyssey|Canvas")
    EOdysseyColorModel GetCanvasColorModel();

    //Get the Canvas Channel Depth
    UFUNCTION(BlueprintPure, Category="Odyssey|Canvas")
    EOdysseyChannelDepth GetCanvasChannelDepth();

    //Get the Canvas Rectangle
    UFUNCTION(BlueprintPure, Category="Odyssey|Canvas", meta = ( DisplayName="Get Canvas Rectangle" ))
    FOdysseyBrushRect GetCanvasRect();

    //Gets Stroke Block.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Stroke")
    FOdysseyBlockProxy GetStrokeBlock( FOdysseyBrushRect Area );

public:
    // Odyssey Brush Stamps    

    //Debug Stamp, stamps a basic shape with the current modifiers parameters. Is also pressure sensitive.
    UFUNCTION( BlueprintCallable, Category="Odyssey|Stamps", meta = (HideSelfPin) )
    void  DebugStamp();

    //Node that reunites all the necessary information to create a stamp.
    //Must be connected to an Event (i.e. "Event on Step").
    //Requires 3 mandatory input to work : Odyssey Block Reference and X|Y coordinates.
    UFUNCTION( BlueprintCallable, Category="Odyssey|Stamps", meta = ( DefaultToSelf="Target", HideSelfPin, HidePin="Target") )
    void  Stamp( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, UPARAM(DisplayName="Handle Position") FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f, bool AntiAliasing = false, EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal );

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

    //Event is triggered when a subdivision of the stroke stroke begins on the canvas.
    UFUNCTION(BlueprintNativeEvent)
    void OnSubStrokeBegin();

    //Event is triggered when a subdivision of the stroke ends (when the click or stylus is dropped)
    UFUNCTION(BlueprintNativeEvent)
    void OnSubStrokeEnd();

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

    /* Run the stroke begin action */
    void ExecuteSubStrokeBegin();

    /* Run the stroke end action */
    void ExecuteSubStrokeEnd();

private:
    // Private Members Data
    FOdysseyBrushState                      state;
    TMap< FName, FOdysseyDrawingState* >    mStates;
    TArray< ::ul3::FRect >                  invalid_rects;

public:
    UPROPERTY(EditAnywhere,Category="Overrides")
    FOdysseyBrushPreferencesOverrides       Preferences;
};
