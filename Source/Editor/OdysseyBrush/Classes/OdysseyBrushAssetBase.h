// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "InputCoreTypes.h" 
#include "Math/Color.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyBrushContext.h"
#include "OdysseyBrushOptions.h"
#include "Proxies/OdysseyBrushColor.h"
#include "Proxies/OdysseyBrushPivot.h"
#include "Proxies/OdysseyBrushBlock.h"
#include "Proxies/OdysseyBrushRect.h"
#include <ULIS>
#include "OdysseyBrushAssetBase.generated.h"

//singleton
class ODYSSEYBRUSH_API FOdysseyBrushOverride
{
private:
    static FOdysseyBrushOverride* Instance();

public:
    static void Register(UClass* iClass);
    static TArray<UClass*> GetClasses();

private:
    TArray<UClass*> mClasses;
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
    GENERATED_BODY()

public:
    enum class eStepType
    {
        kNone = 0,
        kSubStrokeBegin = 1 << 0,
        kSubStrokeEnd = 1 << 1
    };

    struct FStep
    {
        FStep(const FOdysseyPoint& iPoint, uint32 iType)
            : mPoint(iPoint)
            , mType(iType)
        {}

        FOdysseyPoint mPoint;
        uint32 mType;
    };

public:
    // Construction / Destruction
    ~UOdysseyBrushAssetBase();
    UOdysseyBrushAssetBase();

public:
    /** Getter for World Pointer, this workaround may be unsafe but allows us to use Blueprint Function Libraries withing Odyssey Brushes. It is always NULL in a brush context. */
    virtual UWorld* GetWorld() const override final { return  nullptr; }

    virtual void PostLoad();
    
public:
    // Paint Engine Stroke API

    // Moves the current position to iPoint
    // Does NOT draw, it just moves instantly the current position to the given point
    // Any new painting action will then begin from this position (even Tick will be able to draw according to this position)
    void StrokeMoveTo(const FOdysseyPoint& iPoint);

    //Begins a stroke
    bool StrokeBegin();

    //Continues the stroke by interpolating from the last position to iPoint
    bool StrokeTo( const TArray< FOdysseyPoint >& iPoints );

    //Continues the stroke by interpolating from the last position to iPoint
    TArray<FStep> StepsTo( const TArray< FOdysseyPoint >& iPoints );

    //Executes a single step at iPoint
    bool StrokeStep( const FStep& iStep );

    //Aborts the stroke
    bool StrokeAbort();

    //Ends the stroke
    bool StrokeEnd();

    // Resets the Stroke without stopping the drawing process
    void StrokeReset();

    //Flushes the drawing in EditedBlock (calling Dirty with invalid rects and finishing ULIS drawings)
    void StrokeFlush();

public:
    // Setters

    // Sets the Stroke Options the Paint Engine will use to draw a stroke
    void SetBrushOptions( UOdysseyBrushOptions* iBrushOptions );

    // Sets the block on which the brush is drawing
    void SetBlock(::ULIS::FBlock* mEditedBlock);

public:
    // Getters

    //Returns the BrushOptions used by the Brush
    UOdysseyBrushOptions* GetBrushOptions();

public:
    //Context Management
    void AddContext(FOdysseyBrushContext* iContext);
    template<class T> T* GetContext(FString iName);
    
public:
    // Tick

    // Called once for each engine tick
	void Tick(float DeltaTime, bool iShouldFlush);

private:
    // Internal - Tick API

    // Executes the brush Tick node immediately
    void ExecuteTick();

private:
    // Internal - Callbacks

    //Called when the brushoptions properties are changed
    void OnBrushOptionsChanged();

private:
    // Internal

    // Computes the interpolation from the interpolator, and relative parameters for all generated points
    TArray< FOdysseyPoint > ComputeInterpolation();

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

    //Gets opacity from Top Bar.
    //UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers" )
    // float  GetOpacityModifier();

    //Gets the editor's currently selected color (in the color wheel for example).
    UFUNCTION( BlueprintPure, Category = "Odyssey|Modifiers", meta = (DisplayName = "Get Color Modifier") )
    FOdysseyBrushColor GetColor();

    //Gets size from Top Bar.
    UFUNCTION(BlueprintPure, Category = "Odyssey|Modifiers" )
    float GetSizeModifier();

    //Gets flow from Top Bar.
    UFUNCTION( BlueprintPure, Category="Odyssey|Modifiers" )
    float GetFlowModifier();

    //Gets index of each stamp applied in the stroke.
    UFUNCTION( BlueprintPure, Category="Odyssey|Stroke" )
    int  GetCurrentStrokePointIndex();

public:
    // Canvas Nodes

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

    //TODO: GetCanvas Transform

    //Gets Stroke Block.
    UFUNCTION( BlueprintPure, Category = "Odyssey|Stroke")
    FOdysseyBlockProxy GetStrokeBlock( FOdysseyBrushRect Area );

    //TODO: GetLayerStack...()

public:
    /*******************************/
    /**   Odyssey Brush Stamps    **/
    /*******************************/

    //Debug Stamp, stamps a basic shape with the current modifiers parameters. Is also pressure sensitive.
    UFUNCTION( BlueprintCallable, Category="Odyssey|Stamps", meta = (HideSelfPin) )
    void  DebugStamp();

    //Node that reunites all the necessary information to create a stamp.
    //Must be connected to an Event (i.e. "Event on Step").
    //Requires 3 mandatory input to work : Odyssey Block Reference and X|Y coordinates.
    UFUNCTION( BlueprintCallable, Category="Odyssey|Stamps", meta = ( DefaultToSelf="Target", HideSelfPin, HidePin="Target") )
    void  Stamp( UPARAM(DisplayName="Block") FOdysseyBlockProxy Sample, UPARAM(DisplayName="Handle Position") FOdysseyPivot Pivot, float X, float Y, float Flow = 1.f, bool AntiAliasing = false, EOdysseyBlendingMode BlendingMode = EOdysseyBlendingMode::kNormal, EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal );

public:
    /*********************************/
    /** Odyssey Brush Native Events **/
    /*********************************/
    
    //Event is triggered when the brush is loaded.
    UFUNCTION(BlueprintNativeEvent)
    void OnSelected(); //OnInit

    UFUNCTION(BlueprintNativeEvent)
    void OnTick();
    
    //Event is triggered at each step of the stroke.
    UFUNCTION(BlueprintNativeEvent)
    void OnStep();

    //Event is triggered when anything is changed in ILIAD interface (variables or modifiers).
    UFUNCTION(BlueprintNativeEvent)
    void OnStateChanged(); //OnContextChanged

    //Event is triggered when stroke begins on the canvas (after clicking or touching the tablet with the stylus tip).
    UFUNCTION(BlueprintNativeEvent)
    void OnStrokeBegin(); //OnBeginPaint

    //Event is triggered when the stroke ends (when the click or stylus is dropped)
    UFUNCTION(BlueprintNativeEvent)
    void OnStrokeEnd(); //OnEndPaint

    //Event is triggered when a subdivision of the stroke stroke begins on the canvas.
    UFUNCTION(BlueprintNativeEvent)
    void OnSubStrokeBegin(); //OnBeginSubPaint

    //Event is triggered when a subdivision of the stroke ends (when the click or stylus is dropped)
    UFUNCTION(BlueprintNativeEvent)
    void OnSubStrokeEnd(); //OnEndSubPaint

public:
    // Odyssey Brush Public Driving Methods
    /* Run the stamp action */
    void ExecuteSelected();

    /* Run the state change action */
    void ExecuteStateChanged();

    virtual void Serialize (FArchive& Ar);

public:
    //PROPERTIES
    UPROPERTY(Transient, NonTransactional)
    UOdysseyBrushOptions*                   BrushOptions;

#if WITH_EDITORONLY_DATA
    UPROPERTY(VisibleDefaultsOnly, Instanced, Category="Overrides")
    TMap<FName, UObject*>                        Overrides;
#endif


    // UPROPERTY(VisibleDefaultsOnly, Instanced)
    // UObject*                                OverrideTest;

private:
    TArray<FOdysseyBrushContext*>           mContexts;

    // External
    ::ULIS::FBlock*                         mEditedBlock;

    //Internal
    TArray< FOdysseyPoint >                 mStroke;
    FOdysseyPoint                           mPoint; //The point used to draw
    FOdysseyPoint                           mPreviousPoint;
    uint64                                  mPointIndex; //The index in mStroke of the point used to draw
    bool                                    mIsDrawing;
    TArray<::ULIS::FRectI>                  mInvalidRects;
    ::ULIS::FEvent                          mEvent;
};

template<class T> T*
UOdysseyBrushAssetBase::GetContext(FString iName)
{
    for (int i = 0; i < mContexts.Num(); i++)
    {
        if (mContexts[i]->Name() != iName)
            continue;

        return static_cast<T*>(mContexts[i]);
    }

    return nullptr;
}