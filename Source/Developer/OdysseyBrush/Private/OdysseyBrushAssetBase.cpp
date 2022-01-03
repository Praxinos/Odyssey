// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrushAssetBase.h"
#include "Engine/Texture2D.h"
#include "OdysseySurface.h"
#include "Proxies/OdysseyBrushColor.h"
#include <ULIS>
#include "ULISLoaderModule.h"

FOdysseyBrushState::FOdysseyBrushState()
{
    //dummy context
    ::ULIS::FContext::MarkEventFinished( &event );
}

void
FOdysseyBrushState::ResetEvent()
{
    event = ::ULIS::FEvent::NoOP();
}

/////////////////////////////////////////////////////
// BrushAssetBase
//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Constructor


UOdysseyBrushAssetBase::UOdysseyBrushAssetBase( const  FObjectInitializer&  ObjectInitializer )
    : Super( ObjectInitializer )
{
}


UOdysseyBrushAssetBase::~UOdysseyBrushAssetBase()
{
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Public C++ API
FOdysseyBrushState&
UOdysseyBrushAssetBase::GetState()
{
    return  state;
}

FOdysseyDrawingState*
UOdysseyBrushAssetBase::FindState( const FName& iKey )
{
    return *mStates.Find( iKey );
}

const FOdysseyDrawingState*
UOdysseyBrushAssetBase::FindState( const FName& iKey ) const
{
    return *mStates.Find( iKey );
}

void
UOdysseyBrushAssetBase::AddOrReplaceState( const FName& iKey, FOdysseyDrawingState* iState )
{
    FOdysseyDrawingState* old_state = nullptr;
    mStates.RemoveAndCopyValue( iKey, old_state );
    delete old_state;

    mStates.Add( iKey, iState );
}


const TArray< ::ULIS::FRectI >&
UOdysseyBrushAssetBase::GetInvalidRects() const
{
    return  invalid_rects;
}


void
UOdysseyBrushAssetBase::PushInvalidRect( const  ::ULIS::FRectI& iRect )
{
    invalid_rects.Add( iRect );
}


void
UOdysseyBrushAssetBase::ClearInvalidRects()
{
    invalid_rects.Empty();
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
//float
//UOdysseyBrushAssetBase::GetZ()
//{
//    return  state.point.z;
//}


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
//float
//UOdysseyBrushAssetBase::GetPitch()
//{
//    return  state.point.pitch;
//}


/** Get Pitch Normalized */
//float
//UOdysseyBrushAssetBase::GetPitchNormalized()
//{
//    return  GetPitch() / 360.f;
//}


/** Get Roll */
//float
//UOdysseyBrushAssetBase::GetRoll()
//{
//    return  state.point.roll;
//}


/** Get Roll Normalized */
//float
//UOdysseyBrushAssetBase::GetRollNormalized()
//{
//    return  GetRoll() / 360.f;
//}


/** Get Yaw */
//float
//UOdysseyBrushAssetBase::GetYaw()
//{
//    return  state.point.yaw;
//}



/** Get Yaw Normalized */
//float
//UOdysseyBrushAssetBase::GetYawNormalized()
//{
//    return  GetYaw() / 360.f;
//}


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
    return  state.point.acceleration;
}


/** Get Jolt */
FVector2D
UOdysseyBrushAssetBase::GetJolt()
{
    return  state.point.jolt;
}

/** Get Delta Position between this point and the last */
FVector2D
UOdysseyBrushAssetBase::GetDeltaPosition()
{
    return  state.point.deltaPosition;
}


/** Get Delta Time in ms between this point and the last */
int
UOdysseyBrushAssetBase::GetDeltaTime()
{
    return  state.point.deltaTime;
}

/** Returns the keyboard and mouse keys down at this point */
TArray<FKey>
UOdysseyBrushAssetBase::GetKeysDown()
{
    return  state.point.keysDown;
}


/*******************************/
/**       State Getters        */
/*******************************/
/** Get Color */
FOdysseyBrushColor
UOdysseyBrushAssetBase::GetColor()
{
    return  FOdysseyBrushColor( state.color );
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

EOdysseyColorModel
UOdysseyBrushAssetBase::GetCanvasColorModel()
{
    if (!state.target_temp_buffer)
        return EOdysseyColorModel::kRGBA;

    return OdysseyColorModelFromULISFormat(state.target_temp_buffer->Format());
}

EOdysseyChannelDepth
UOdysseyBrushAssetBase::GetCanvasChannelDepth()
{
    if (!state.target_temp_buffer)
        return EOdysseyChannelDepth::k8;

    return OdysseyChannelDepthFromULISFormat(state.target_temp_buffer->Format());
}

//static
FOdysseyBrushRect
UOdysseyBrushAssetBase::GetCanvasRect()
{
    if (!state.target_temp_buffer)
        return FOdysseyBrushRect();

    return FOdysseyBrushRect(state.target_temp_buffer->Rect());
}

/** Get Stroke Buffer*/
FOdysseyBlockProxy
UOdysseyBrushAssetBase::GetStrokeBlock( FOdysseyBrushRect Area )
{
    if (Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <=0))
        return FOdysseyBlockProxy::MakeNullProxy();

    if (!state.target_temp_buffer)
    {
        if( Area.IsInitialized() )
        {
            TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable( new ::ULIS::FBlock( Area.Width(), Area.Height(), ::ULIS::Format_RGBA8 ));
            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( ::ULIS::Format_RGBA8 );
            ::ULIS::FEvent eventClear;
            ctx.Clear( *dst,
                        ::ULIS::FRectI::Auto,
                        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
                        0,
                        nullptr,
                        &eventClear);
            ctx.Flush();
            return FOdysseyBlockProxy::MakeProxy(dst, 1, &eventClear);
        }
        return FOdysseyBlockProxy::MakeNullProxy();
    }


    ::ULIS::FBlock* src = state.target_temp_buffer;
    ::ULIS::eFormat format = src->Format();

    ::ULIS::FRectI rect = Area.IsInitialized() ? Area.GetValue() : src->Rect();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable(new ::ULIS::FBlock( rect.w, rect.h, format ));

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );

    //be sure we copy only the needed part
    ::ULIS::FRectI src_rect = rect & src->Rect();
    ::ULIS::FVec2I dst_pos(src_rect.x - rect.x, src_rect.y - rect.y);

    ::ULIS::FEvent eventCopy;
    ctx.Copy( *src, *dst, src_rect, dst_pos, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &state.event, &eventCopy);
    ctx.Flush();

    return FOdysseyBlockProxy::MakeProxy(dst, 1, &eventCopy);
}

//static
void
UOdysseyBrushAssetBase::DebugStamp()
{
    if( !state.target_temp_buffer)
        return;

    ::ULIS::FEvent eventInput = state.event;

    int size = ::ULIS::FMath::Max( GetSizeModifier() * GetPressure(), 1.f );

    ::ULIS::FBlock* debug_stamp = new ::ULIS::FBlock( size, size, state.target_temp_buffer->Format() );
    ::ULIS::FColor color = state.color.ToFormat( ::ULIS::Format_RGBAF );
    color.SetAlphaF( GetFlowModifier() );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( debug_stamp->Format() );
    const bool bEnableMTPolicy = true;// size > 256;
    ::ULIS::FEvent eventFill;
    ctx.Fill( *debug_stamp, color, ::ULIS::FRectI::Auto, bEnableMTPolicy ? ::ULIS::FSchedulePolicy::AsyncCacheEfficient : ::ULIS::FSchedulePolicy::MonoChunk, 1, &eventInput, &eventFill );
    ctx.Flush();

    ::ULIS::FRectI invalidRect;
    invalidRect.x = GetX() - size / 2;
    invalidRect.y = GetY() - size / 2;
    invalidRect.w = size + 1;
    invalidRect.h = size + 1;

    ::ULIS::FEvent eventBlend(
        ::ULIS::FOnEventComplete(
            [debug_stamp](const ::ULIS::FRectI& iRect)
            {
                delete debug_stamp;
            }
        )
    );
    ctx.Blend(
          *debug_stamp
        , *state.target_temp_buffer
        , ::ULIS::FRectI::Auto
        , ::ULIS::FVec2F( GetX() - size / 2, GetY() - size / 2 )
        , ::ULIS::Blend_Normal
        , ::ULIS::Alpha_Normal
        , 1.f
        , bEnableMTPolicy ? ::ULIS::FSchedulePolicy::AsyncCacheEfficient : ::ULIS::FSchedulePolicy::MonoScanlines
        , 1
        , &eventFill
        , &eventBlend
    );
    ctx.Flush();
    state.event = eventBlend;
    PushInvalidRect( invalidRect );
}

//static
void
UOdysseyBrushAssetBase::Stamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow, bool iAntiAliasing, EOdysseyBlendingMode BlendingMode, EOdysseyAlphaMode AlphaMode )
{
    if( !state.target_temp_buffer )
        return;
        
    if( !Sample.IsValid() )
        return;

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = Sample.GetBlock();
    FRectF invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );    //PATCH: until ::ulis3::FRectF
    //::ULIS::FRectI invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );
    ::ULIS::eFormat block_format = block->Format();
    ::ULIS::eFormat target_format = state.target_temp_buffer->Format();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(target_format);
    const bool bEnableMTPolicy = true; //block->Height() > 256;
    ::ULIS::FSchedulePolicy policy = bEnableMTPolicy ? ::ULIS::FSchedulePolicy::AsyncCacheEfficient : ::ULIS::FSchedulePolicy::MonoScanlines;

    //should wait on that before anything
    ::ULIS::FEvent stateEvent = state.event;
    ::ULIS::FEvent eventInputs[] = { stateEvent, Sample.GetEvent() };

    ::ULIS::FBlock* src = block.Get();

    ::ULIS::FEvent eventConv;
    if( block_format == target_format )
    {
        src = new ::ULIS::FBlock(block->Width(), block->Height(), target_format);
        ctx.ConvertFormat( *block, *src, ::ULIS::FRectI::Auto, ::ULIS::FVec2I( 0 ), policy, 2, eventInputs, &eventConv );
    }
    else
    {
        ctx.Dummy_OP(2, eventInputs, &eventConv);
    }

    ::ULIS::FEvent eventBlend = ::ULIS::FEvent(
        ::ULIS::FOnEventComplete(
            //Also, passing block as a copy maintains it alive until evenBlend finishes, it is very important
            [block, src]( const ::ULIS::FRectI& iRect )
            {
                if (src != block.Get())
                    delete  src;
            }
        )
    );

    if (iAntiAliasing)
    {
        ctx.BlendAA(
              *src
            , *state.target_temp_buffer
            , block->Rect()
            , ::ULIS::FVec2F(invalidRect.x, invalidRect.y)
            , ::ULIS::eBlendMode( BlendingMode )
            , ::ULIS::eAlphaMode( AlphaMode )
            , FMath::Clamp(Flow, 0.f, 1.f)
            , policy
            , 1
            , &eventConv
            , &eventBlend
        );
    }
    else
    {
        ctx.Blend(
              *src
            , *state.target_temp_buffer
            , block->Rect()
            , ::ULIS::FVec2F(invalidRect.x, invalidRect.y)
            , ::ULIS::eBlendMode( BlendingMode )
            , ::ULIS::eAlphaMode( AlphaMode )
            , FMath::Clamp(Flow, 0.f, 1.f)
            , policy
            , 1
            , &eventConv
            , &eventBlend
        );
    }

    ctx.Flush();

    state.event = eventBlend;

    ::ULIS::FRectI invalidRectI( FMath::FloorToInt( invalidRect.x ), FMath::FloorToInt( invalidRect.y ), FMath::CeilToInt( invalidRect.w + 2 ), FMath::CeilToInt( invalidRect.h + 2 ) );
    PushInvalidRect( invalidRectI );
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

void
UOdysseyBrushAssetBase::OnSubStrokeBegin_Implementation()
{
}

void
UOdysseyBrushAssetBase::OnSubStrokeEnd_Implementation()
{
}


//--------------------------------------------------------------------------------------
//------------------------------------------------- Odyssey Brush Public Driving Methods
void
UOdysseyBrushAssetBase::ExecuteSelected()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnSelected();
}


void
UOdysseyBrushAssetBase::ExecuteTick()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnTick();
}


void
UOdysseyBrushAssetBase::ExecuteStep()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnStep();
}


void
UOdysseyBrushAssetBase::ExecuteStateChanged()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnStateChanged();
}


void
UOdysseyBrushAssetBase::ExecuteStrokeBegin()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnStrokeBegin();
}


void
UOdysseyBrushAssetBase::ExecuteStrokeEnd()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnStrokeEnd();
}


void
UOdysseyBrushAssetBase::ExecuteSubStrokeBegin()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnSubStrokeBegin();
}


void
UOdysseyBrushAssetBase::ExecuteSubStrokeEnd()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnSubStrokeEnd();
}

