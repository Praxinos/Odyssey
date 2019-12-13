// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBrushAssetBase.h"
#include "Engine/Texture2D.h"
#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include "OdysseyBrushContext.h"

/////////////////////////////////////////////////////
// FOdysseyBrushPoolCache


bool
FOdysseyBrushPoolCache::KeyExists( const  FString&  iKey )  const
{
    return  pool.Contains( iKey );
}


const  FOdysseyBlockProxy&
FOdysseyBrushPoolCache::Retrieve( const  FString&  iKey )  const
{
    return  *pool.Find( iKey );
}


void
FOdysseyBrushPoolCache::Store( const  FString&  iKey, const  FOdysseyBlockProxy& iValue )
{
    pool.Emplace( iKey, iValue );
}


void
FOdysseyBrushPoolCache::Cleanse()
{
    for( auto it : pool )
        delete  it.Value.m;

    pool.Empty();
}


/////////////////////////////////////////////////////
// BrushAssetBase
//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Constructor


UOdysseyBrushAssetBase::UOdysseyBrushAssetBase( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
    pools.Add( FOdysseyBrushPoolCache() );  // kStep
    pools.Add( FOdysseyBrushPoolCache() );  // kSubstroke
    pools.Add( FOdysseyBrushPoolCache() );  // kStroke
    pools.Add( FOdysseyBrushPoolCache() );  // kState
    pools.Add( FOdysseyBrushPoolCache() );  // kSuper
}


UOdysseyBrushAssetBase::~UOdysseyBrushAssetBase()
{
    CleansePools();
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public C++ API
FOdysseyBrushState&
UOdysseyBrushAssetBase::GetState()
{
    return  state;
}


const TArray< ::ULIS::FRect >&
UOdysseyBrushAssetBase::GetInvalidRects() const
{
    return  invalid_rects;
}


void
UOdysseyBrushAssetBase::PushInvalidRect( const  ::ULIS::FRect& iRect )
{
    invalid_rects.Add( iRect );
}


void
UOdysseyBrushAssetBase::ClearInvalidRects()
{
    invalid_rects.Empty();
}


bool
UOdysseyBrushAssetBase::KeyExistsInPool(  ECacheLevel iLevel, const  FString&  iKey )  const
{
    return  pools[ (int)iLevel ].KeyExists( iKey );
}


void
UOdysseyBrushAssetBase::StoreInPool(      ECacheLevel iLevel, const  FString&  iKey, const  FOdysseyBlockProxy&  iValue )
{
    if( KeyExistsInPool( iLevel, iKey ) )
       return;

    pools[ (int)iLevel ].Store( iKey, iValue );
}


FOdysseyBlockProxy
UOdysseyBrushAssetBase::RetrieveInPool(   ECacheLevel iLevel, const  FString&  iKey )  const
{
    if( !KeyExistsInPool( iLevel, iKey ) )
       return  FOdysseyBlockProxy::MakeNullProxy();

    return  pools[ (int)iLevel ].Retrieve( iKey );
}


void
UOdysseyBrushAssetBase::CleansePool( ECacheLevel iLevel )
{
    pools[ (int)iLevel ].Cleanse();
}


void
UOdysseyBrushAssetBase::CleansePools()
{
    for( int i = 0; i < pools.Num(); ++i )
        pools[ i ].Cleanse();
}


//--------------------------------------------------------------------------------------
//-------------------------------------------------------- OdysseyBrushBlueprint Getters
/*******************************/
/** State Stroke Point Getters */
/*******************************/
/** Get X */
float
UOdysseyBrushAssetBase::GetX()
{
    return  state.point.x;
}


/** Get Y */
float
UOdysseyBrushAssetBase::GetY()
{
    return  state.point.y;
}


/** Get Z */
float
UOdysseyBrushAssetBase::GetZ()
{
    return  state.point.z;
}


/** Get Pressure */
float
UOdysseyBrushAssetBase::GetPressure()
{
    return  state.point.pressure;
}


/** Get Altitude */
float
UOdysseyBrushAssetBase::GetAltitude()
{
    return  state.point.altitude;
}


/** Get Altitude Normalized */
float
UOdysseyBrushAssetBase::GetAltitudeNormalized()
{
    return  GetAltitude() / 90.f;
}


/** Get Azimuth */
float
UOdysseyBrushAssetBase::GetAzimuth()
{
    return  state.point.azimuth;
}


/** Get Azimuth Normalized */
float
UOdysseyBrushAssetBase::GetAzimuthNormalized()
{
    return  GetAzimuth() / 360.f;
}


/** Get Twist */
float
UOdysseyBrushAssetBase::GetTwist()
{
    return  state.point.twist;
}



/** Get Twist Normalized */
float
UOdysseyBrushAssetBase::GetTwistNormalized()
{
    return  GetTwist() / 360.f;
}


/** Get Pitch */
float
UOdysseyBrushAssetBase::GetPitch()
{
    return  state.point.pitch;
}


/** Get Pitch Normalized */
float
UOdysseyBrushAssetBase::GetPitchNormalized()
{
    return  GetPitch() / 360.f;
}


/** Get Roll */
float
UOdysseyBrushAssetBase::GetRoll()
{
    return  state.point.roll;
}


/** Get Roll Normalized */
float
UOdysseyBrushAssetBase::GetRollNormalized()
{
    return  GetRoll() / 360.f;
}


/** Get Yaw */
float
UOdysseyBrushAssetBase::GetYaw()
{
    return  state.point.yaw;
}



/** Get Yaw Normalized */
float
UOdysseyBrushAssetBase::GetYawNormalized()
{
    return  GetYaw() / 360.f;
}


/** Get distance travelled */
float
UOdysseyBrushAssetBase::GetDistanceAlongStroke()
{
    return  state.point.distance_travelled;
}


/** Get Direction Vector Tangent */
FVector2D
UOdysseyBrushAssetBase::GetDirectionVectorTangent()
{
    return  state.point.direction_vector_tangent;
}


/** Get Direction Vector Normal */
FVector2D
UOdysseyBrushAssetBase::GetDirectionVectorNormal()
{
    return  state.point.direction_vector_normal;
}


/** Get Direction Angle Tangent as Degrees */
float
UOdysseyBrushAssetBase::GetDirectionAngleTangentDeg()
{
    return  state.point.direction_angle_deg_tangent;
}


/** Get Direction Angle Normal as Degrees */
float
UOdysseyBrushAssetBase::GetDirectionAngleNormalDeg()
{
    return  state.point.direction_angle_deg_normal;
}


/** Get Speed */
FVector2D
UOdysseyBrushAssetBase::GetSpeed()
{
    return  state.point.speed;
}


/** Get Acceleration */
FVector2D
UOdysseyBrushAssetBase::GetAcceleration()
{
    return  state.point.speed;
}


/** Get Jolt */
FVector2D
UOdysseyBrushAssetBase::GetJolt()
{
    return  state.point.jolt;
}



/*******************************/
/**       State Getters        */
/*******************************/
/** Get Color */
FOdysseyBrushColor
UOdysseyBrushAssetBase::GetColor()
{
    FOdysseyBrushColor col;
    col.m = state.color;
    return  col;
}



/** Get Size Modifier */
float
UOdysseyBrushAssetBase::GetSizeModifier()
{
    return  state.size_modifier;
}


/** Get Opacity Modifier */
float
UOdysseyBrushAssetBase::GetOpacityModifier()
{
    return  state.opacity_modifier;
}


/** Get Flow Modifier */
float
UOdysseyBrushAssetBase::GetFlowModifier()
{
    return  state.flow_modifier;
}


/** Get Step */
float
UOdysseyBrushAssetBase::GetStep()
{
    return  state.step;
}


/** Get Smoothing Strength */
float
UOdysseyBrushAssetBase::GetSmoothingStrength()
{
    return  state.smoothing_strength;
}


/** Get Current Stroke Point Index */
int
UOdysseyBrushAssetBase::GetCurrentStrokePointIndex()
{
    return  state.currentPointIndex;
}


/** Get Current Canvas Width*/
int
UOdysseyBrushAssetBase::GetCanvasWidth()
{
    if( state.target_temp_buffer )
        return  state.target_temp_buffer->Width();
    else
        return  0;
}


/** Get Current Canvas Height*/
int
UOdysseyBrushAssetBase::GetCanvasHeight()
{
    if( state.target_temp_buffer )
        return  state.target_temp_buffer->Height();
    else
        return  0;
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------- Odyssey Brush Native events
void
UOdysseyBrushAssetBase::OnSelected_Implementation()
{
}


void
UOdysseyBrushAssetBase::OnTick_Implementation()
{
}


void
UOdysseyBrushAssetBase::OnStep_Implementation()
{
}

void
UOdysseyBrushAssetBase::OnStateChanged_Implementation()
{
}

void
UOdysseyBrushAssetBase::OnStrokeBegin_Implementation()
{
}

void
UOdysseyBrushAssetBase::OnStrokeEnd_Implementation()
{
}


//--------------------------------------------------------------------------------------
//------------------------------------------------- Odyssey Brush Public Driving Methods
void
UOdysseyBrushAssetBase::ExecuteSelected()
{
    FOdysseyBrushContext::Instance()->SetContext( this );

    FEditorScriptExecutionGuard ScriptGuard;
    OnSelected();

    FOdysseyBrushContext::Instance()->ClearContext();
}


void
UOdysseyBrushAssetBase::ExecuteTick()
{
    FOdysseyBrushContext::Instance()->SetContext( this );

    FEditorScriptExecutionGuard ScriptGuard;
    OnTick();

    FOdysseyBrushContext::Instance()->ClearContext();
}


void
UOdysseyBrushAssetBase::ExecuteStep()
{
    FOdysseyBrushContext::Instance()->SetContext( this );

    FEditorScriptExecutionGuard ScriptGuard;
    OnStep();

    FOdysseyBrushContext::Instance()->ClearContext();
}


void
UOdysseyBrushAssetBase::ExecuteStateChanged()
{
    FOdysseyBrushContext::Instance()->SetContext( this );

    FEditorScriptExecutionGuard ScriptGuard;
    OnStateChanged();

    FOdysseyBrushContext::Instance()->ClearContext();
}


void
UOdysseyBrushAssetBase::ExecuteStrokeBegin()
{
    FOdysseyBrushContext::Instance()->SetContext( this );

    FEditorScriptExecutionGuard ScriptGuard;
    OnStrokeBegin();

    FOdysseyBrushContext::Instance()->ClearContext();
}


void
UOdysseyBrushAssetBase::ExecuteStrokeEnd()
{
    FOdysseyBrushContext::Instance()->SetContext( this );

    FEditorScriptExecutionGuard ScriptGuard;
    OnStrokeEnd();

    FOdysseyBrushContext::Instance()->ClearContext();
}

