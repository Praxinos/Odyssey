// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrushAssetBase.h"
#include "Engine/Texture2D.h"
#include "OdysseySurface.h"
#include "OdysseyBlock.h"
#include "Proxies/OdysseyBrushColor.h"
#include <ULIS3>
#include "ULISLoaderModule.h"

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


const TArray< ::ul3::FRect >&
UOdysseyBrushAssetBase::GetInvalidRects() const
{
    return  invalid_rects;
}


void
UOdysseyBrushAssetBase::PushInvalidRect( const  ::ul3::FRect& iRect )
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

    return FOdysseyBrushRect(state.target_temp_buffer->GetBlock()->Rect());
}

/** Get Stroke Buffer*/
FOdysseyBlockProxy
UOdysseyBrushAssetBase::GetStrokeBlock( FOdysseyBrushRect Area )
{
    if (Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <=0))
        return FOdysseyBlockProxy::MakeNullProxy();

    if (!state.target_temp_buffer)
    {
        if (Area.IsInitialized())
        {
            TSharedPtr<FOdysseyBlock> dst = MakeShareable(new FOdysseyBlock(Area.Width(), Area.Height(), ULIS3_FORMAT_RGBA8));
            ::ul3::ClearRaw(dst->GetBlock());
            return dst;
        }
        return FOdysseyBlockProxy::MakeNullProxy();
    }
        
	FOdysseyBlock* src = state.target_temp_buffer;
	::ul3::tFormat format = src->Format();
	
    ::ul3::FRect rect = Area.IsInitialized() ? Area.GetValue() : src->GetBlock()->Rect();
    TSharedPtr<FOdysseyBlock> dst = MakeShareable(new FOdysseyBlock(rect.w, rect.h, format));
    
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = rect.h > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	//be sure we copy only the needed part
	::ul3::FRect src_rect = rect & src->GetBlock()->Rect();
    ::ul3::FVec2I dst_pos(src_rect.x - rect.x, src_rect.y - rect.y);
    ::ul3::Copy( hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, src->GetBlock(), dst->GetBlock(), src_rect, dst_pos );

	return FOdysseyBlockProxy(dst);
}

//static
void
UOdysseyBrushAssetBase::DebugStamp()
{
    if( !state.target_temp_buffer)
        return;

    int size = ::ul3::FMaths::Max( GetSizeModifier() * GetPressure(), 1.f );

    ::ul3::FBlock debug_stamp( size, size, state.target_temp_buffer->Format() );
    ::ul3::FPixelValue color = ::ul3::Conv( state.color, ULIS3_FORMAT_RGBAF );
    color.SetAlphaF( GetFlowModifier() );

    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = size > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;
    ::ul3::Fill( hULIS.ThreadPool()
               , ULIS3_BLOCKING
               , perfIntent
               , hULIS.HostDeviceInfo()
               , ULIS3_NOCB
               , &debug_stamp
               , color
               , debug_stamp.Rect() );

    ::ul3::FRect invalidRect;
    invalidRect.x = GetX() - size / 2;
    invalidRect.y = GetY() - size / 2;
    invalidRect.w = size + 1;
    invalidRect.h = size + 1;

    ::ul3::Blend( hULIS.ThreadPool()
                , ULIS3_BLOCKING
                , perfIntent
                , hULIS.HostDeviceInfo()
                , ULIS3_NOCB
                , &debug_stamp
                , state.target_temp_buffer->GetBlock()
                , debug_stamp.Rect()
                , ::ul3::FVec2F( GetX() - size / 2, GetY() - size / 2 )
                , ULIS3_AA
                , ::ul3::BM_NORMAL
                , ::ul3::AM_NORMAL
                , 1.f );

    PushInvalidRect( invalidRect );
}

//static
void
UOdysseyBrushAssetBase::Stamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow, bool iAntiAliasing, EOdysseyBlendingMode BlendingMode, EOdysseyAlphaMode AlphaMode )
{
    if( !state.target_temp_buffer ) return;
    if( !Sample.m )     return;

	TSharedPtr<FOdysseyBlock> block = Sample.m;
    FRectF invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );    //PATCH: until ::ulis3::FRectF
    //::ul3::FRect invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );
    IULISLoaderModule& hULIS = IULISLoaderModule::Get();
    ::ul3::uint32 MT_bit = block->Height() > 256 ? ULIS3_PERF_MT : 0;
    ::ul3::uint32 perfIntent = MT_bit | ULIS3_PERF_SSE42;

	::ul3::tFormat block_format = block->Format();
	::ul3::tFormat target_format = state.target_temp_buffer->Format();
	if (block_format == target_format)
	{
		::ul3::Blend(hULIS.ThreadPool()
			, ULIS3_BLOCKING
			, perfIntent
			, hULIS.HostDeviceInfo()
			, ULIS3_NOCB
			, block->GetBlock()
			, state.target_temp_buffer->GetBlock()
			, block->GetBlock()->Rect()
			, ::ul3::FVec2F(invalidRect.x, invalidRect.y)
			, iAntiAliasing
			, static_cast<::ul3::eBlendingMode>(BlendingMode)
			, static_cast<::ul3::eAlphaMode>(AlphaMode)
			, FMath::Clamp(Flow, 0.f, 1.f));
	}
	else
	{
		FOdysseyBlock* conv = new FOdysseyBlock(block->Width(), block->Height(), target_format);
		::ul3::Conv(hULIS.ThreadPool(), ULIS3_BLOCKING, perfIntent, hULIS.HostDeviceInfo(), ULIS3_NOCB, block->GetBlock(), conv->GetBlock());
		::ul3::Blend(hULIS.ThreadPool()
			, ULIS3_BLOCKING
			, perfIntent
			, hULIS.HostDeviceInfo()
			, ULIS3_NOCB
			, conv->GetBlock()
			, state.target_temp_buffer->GetBlock()
			, conv->GetBlock()->Rect()
			, ::ul3::FVec2F(invalidRect.x, invalidRect.y)
			, iAntiAliasing
			, static_cast<::ul3::eBlendingMode>(BlendingMode)
			, static_cast<::ul3::eAlphaMode>(AlphaMode)
			, FMath::Clamp(Flow, 0.f, 1.f));

		delete conv;
	}

    ::ul3::FRect invalidRectI( FMath::FloorToInt( invalidRect.x ), FMath::FloorToInt( invalidRect.y ), FMath::CeilToInt( invalidRect.w + 2 ), FMath::CeilToInt( invalidRect.h + 2 ) );
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

