// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrushAssetBase.h"

#include "Engine/Texture2D.h"
#include "ObjectEditorUtils.h"
#include "OdysseySurface.h"
#include "Proxies/OdysseyBrushColor.h"
#include <ULIS>
#include "ULISLoaderModule.h"

//static
FOdysseyBrushOverride*
FOdysseyBrushOverride::Instance()
{
    static FOdysseyBrushOverride* instance = nullptr;
    if (!instance)
        instance = new FOdysseyBrushOverride();

    return instance;
}

//static
void
FOdysseyBrushOverride::Register(UClass* iClass)
{
    Instance()->mClasses.Add(iClass);
}

//static
TArray<UClass*>
FOdysseyBrushOverride::GetClasses()
{
    return Instance()->mClasses;
}

/////////////////////////////////////////////////////
// BrushAssetBase
//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------------- Constructor

UOdysseyBrushAssetBase::~UOdysseyBrushAssetBase()
{
}

UOdysseyBrushAssetBase::UOdysseyBrushAssetBase()
    //Properties
    : BrushOptions(CreateDefaultSubobject<UOdysseyBrushOptions>("UOdysseyBrushAssetBase::BrushOptions"))

    //Internal
    , mIsDrawing(false)
{
    ::ULIS::FContext::MarkEventFinished(&mEvent);
}

void
UOdysseyBrushAssetBase::PostLoad()
{
    Super::PostLoad();

    if (HasAnyFlags(RF_ArchetypeObject))
    {
        //Remove all overrides which failed to load (can happen in our case when removing overrides classes)
        TArray<FName> keysToRemove;
        for (auto Elem : Overrides)
        {
            if (!Elem.Value)
                keysToRemove.Add(Elem.Key);
        }

        for (auto& key : keysToRemove)
        {
            Overrides.Remove(key);
        }

        //Add missing classes (can happen in our case when adding overrides classes, in existing brushes)
        for (auto overrideClass : FOdysseyBrushOverride::GetClasses())
        {
            bool containsClass = Overrides.Contains(overrideClass->GetFName());
            if (!containsClass)
            {
                Overrides.Add(overrideClass->GetFName(), NewObject<UObject>(this, overrideClass, overrideClass->GetFName() ));
            }
        }
    }
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Brush Context API

void
UOdysseyBrushAssetBase::AddContext(FOdysseyBrushContext* iContext)
{
    mContexts.Add(iContext);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Stroke API


void
UOdysseyBrushAssetBase::StrokeMoveTo( const FOdysseyPoint& iPoint )
{
    mPoint = iPoint;
}

bool
UOdysseyBrushAssetBase::StrokeBegin()
{
    if (mIsDrawing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyBrushAssetBase::Begin() : StrokeEngine has already begun") );
        return false;
    }

    mIsDrawing = true;

    //Reset the stroke before drawing
    StrokeReset();

    return true;
}

bool
UOdysseyBrushAssetBase::StrokeTo( const TArray< FOdysseyPoint >& iPoints )
{
    TArray<UOdysseyBrushAssetBase::FStep> steps = StepsTo(iPoints);
    for (int i = 0; i < steps.Num(); i++)
    {
        if (!StrokeStep(steps[i]))
            return false;
    }

    return true;
}

TArray<UOdysseyBrushAssetBase::FStep>
UOdysseyBrushAssetBase::StepsTo(const TArray< FOdysseyPoint >& iPoints)
{
    TArray<FStep> steps;

    steps.Add(FStep(iPoints[0], (uint32)eStepType::kSubStrokeBegin));
    for(int i = 1; i < iPoints.Num(); i++)
    {
        steps.Add(FStep(iPoints[i], (uint32)eStepType::kNone ));
    }
    steps.Last().mType |= (uint32)eStepType::kSubStrokeEnd;

    return steps;
}

void
UOdysseyBrushAssetBase::StrokeFlush()
{
    //Finish any drawing
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_RGBA8);
    ctx.Finish();

    //Invalidate mEditedBlock
    if (mEditedBlock)
        mEditedBlock->Dirty(mInvalidRects.GetData(), mInvalidRects.Num());
    mInvalidRects.Empty();
}

bool
UOdysseyBrushAssetBase::StrokeStep(const FStep& iStep)
{
    // Check if everything is alright
    if (!mIsDrawing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyBrushAssetBase::To() before UOdysseyBrushAssetBase::Begin()"));
        return false;
    }

    mPoint = iStep.mPoint;
    mPointIndex++;


    //Call OnStrokeBegin node if first point
    if (mPointIndex == 0)
    {
        FEditorScriptExecutionGuard ScriptGuard;
        OnStrokeBegin();
    }

    //Call OnSubStrokeBegin node
    if (iStep.mType & (uint32)eStepType::kSubStrokeBegin)
    {
        FEditorScriptExecutionGuard ScriptGuard;
        OnSubStrokeBegin();
    }

    //Always call OnStep node 
    {
        FEditorScriptExecutionGuard ScriptGuard;
        OnStep();
    }

    //Call OnSubStrokeBegin node
    if (iStep.mType & (uint32)eStepType::kSubStrokeEnd)
    {
        FEditorScriptExecutionGuard ScriptGuard;
        OnSubStrokeEnd();
    }

    return true;
}

bool
UOdysseyBrushAssetBase::StrokeEnd()
{
    // Check if everything is alright
    if (!mIsDrawing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyBrushAssetBase::End() before UOdysseyBrushAssetBase::Begin()") );
        return false;
    }

    //Call the OnStrokeEnd node
    {
        FEditorScriptExecutionGuard ScriptGuard;
        OnStrokeEnd();
    }

    StrokeFlush();

    mIsDrawing = false;

    return true;
}

bool
UOdysseyBrushAssetBase::StrokeAbort()
{
    // Check if everything is alright
    if (!mIsDrawing)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyBrushAssetBase::Abort() before UOdysseyBrushAssetBase::Begin()") );
        return false;
    }
    
    mIsDrawing = false;

    return true;
}

void
UOdysseyBrushAssetBase::StrokeReset()
{
    //Invalidate current point index (don't reset mPoint, as we could need it when drawing from OnTick event)
    mPointIndex = -1;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tick
    
void
UOdysseyBrushAssetBase::Tick(float DeltaTime, bool iShouldFlush)
{
    //Execute a the brush tick node before executing anything else 
    ExecuteTick();

    if (iShouldFlush)
        StrokeFlush();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------ Internal - Tick API

void
UOdysseyBrushAssetBase::ExecuteTick()
{
    {
        FEditorScriptExecutionGuard ScriptGuard;
        OnTick();
    }
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------- Internal - Callbacks

void
UOdysseyBrushAssetBase::OnBrushOptionsChanged()
{
    ExecuteStateChanged();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
UOdysseyBrushAssetBase::SetBrushOptions(UOdysseyBrushOptions* iBrushOptions )
{
    if (BrushOptions)
        BrushOptions->OnPropertyChangedDelegate().RemoveAll(this);

    BrushOptions = iBrushOptions;

    if (BrushOptions)
        BrushOptions->OnPropertyChangedDelegate().AddUObject(this, &UOdysseyBrushAssetBase::OnBrushOptionsChanged);
}

void
UOdysseyBrushAssetBase::SetBlock(::ULIS::FBlock* iBlock)
{
    mEditedBlock = iBlock;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters
    
UOdysseyBrushOptions*
UOdysseyBrushAssetBase::GetBrushOptions()
{
    return BrushOptions;
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
    return  mPoint.x;
}


/** Get Y */
float
UOdysseyBrushAssetBase::GetY()
{
    return  mPoint.y;
}


/** Get Z */
//float
//UOdysseyBrushAssetBase::GetZ()
//{
//    return  mPoint.z;
//}


/** Get Pressure */
float
UOdysseyBrushAssetBase::GetPressure()
{
    return  mPoint.pressure;
}


/** Get Altitude */
float
UOdysseyBrushAssetBase::GetAltitude()
{
    return  mPoint.altitude;
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
    return  mPoint.azimuth;
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
    return  mPoint.twist;
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
//    return  mPoint.pitch;
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
//    return  mPoint.roll;
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
//    return  mPoint.yaw;
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
    return  mPoint.distance_travelled;
}


/** Get Direction Vector Tangent */
FVector2D
UOdysseyBrushAssetBase::GetDirectionVectorTangent()
{
    return  mPoint.direction_vector_tangent;
}


/** Get Direction Vector Normal */
FVector2D
UOdysseyBrushAssetBase::GetDirectionVectorNormal()
{
    return  mPoint.direction_vector_normal;
}


/** Get Direction Angle Tangent as Degrees */
float
UOdysseyBrushAssetBase::GetDirectionAngleTangentDeg()
{
    return  mPoint.direction_angle_deg_tangent;
}


/** Get Direction Angle Normal as Degrees */
float
UOdysseyBrushAssetBase::GetDirectionAngleNormalDeg()
{
    return  mPoint.direction_angle_deg_normal;
}


/** Get Speed */
FVector2D
UOdysseyBrushAssetBase::GetSpeed()
{
    return  mPoint.speed;
}


/** Get Acceleration */
FVector2D
UOdysseyBrushAssetBase::GetAcceleration()
{
    return  mPoint.acceleration;
}


/** Get Jolt */
FVector2D
UOdysseyBrushAssetBase::GetJolt()
{
    return  mPoint.jolt;
}

/** Get Delta Position between this point and the last */
FVector2D
UOdysseyBrushAssetBase::GetDeltaPosition()
{
    return  mPoint.deltaPosition;
}


/** Get Delta Time in ms between this point and the last */
int
UOdysseyBrushAssetBase::GetDeltaTime()
{
    return  mPoint.deltaTime;
}

/** Returns the keyboard and mouse keys down at this point */
TArray<FKey>
UOdysseyBrushAssetBase::GetKeysDown()
{
    return  mPoint.keysDown;
}

/*******************************/
/**       State Getters        */
/*******************************/

FOdysseyBrushColor
UOdysseyBrushAssetBase::GetColor()
{
    return  BrushOptions->Color;
}

float
UOdysseyBrushAssetBase::GetSizeModifier()
{
    return  BrushOptions->Size;
}

float
UOdysseyBrushAssetBase::GetFlowModifier()
{
    return  BrushOptions->Flow / 100.f;
}


/** Get Current Stroke Point Index */
int
UOdysseyBrushAssetBase::GetCurrentStrokePointIndex()
{
    return  mPointIndex;
}


/** Get Current Canvas Width*/
int
UOdysseyBrushAssetBase::GetCanvasWidth()
{
    if( mEditedBlock )
        return  mEditedBlock->Width();
    else
        return  0;
}


/** Get Current Canvas Height*/
int
UOdysseyBrushAssetBase::GetCanvasHeight()
{
    if(mEditedBlock)
        return  mEditedBlock->Height();
    else
        return  0;
}

EOdysseyColorModel
UOdysseyBrushAssetBase::GetCanvasColorModel()
{
    if (!mEditedBlock)
        return EOdysseyColorModel::kRGBA;

    return OdysseyColorModelFromULISFormat(mEditedBlock->Format());
}

EOdysseyChannelDepth
UOdysseyBrushAssetBase::GetCanvasChannelDepth()
{
    if (!mEditedBlock)
        return EOdysseyChannelDepth::k8;

    return OdysseyChannelDepthFromULISFormat(mEditedBlock->Format());
}

//static
FOdysseyBrushRect
UOdysseyBrushAssetBase::GetCanvasRect()
{
    if (!mEditedBlock)
        return FOdysseyBrushRect();

    return FOdysseyBrushRect(mEditedBlock->Rect());
}

/** Get Stroke Buffer*/
FOdysseyBlockProxy
UOdysseyBrushAssetBase::GetStrokeBlock( FOdysseyBrushRect Area )
{
    if (Area.IsInitialized() && (Area.Width() <= 0 || Area.Height() <=0))
        return FOdysseyBlockProxy::MakeNullProxy();

    if (!mEditedBlock)
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

    ::ULIS::FBlock* src = mEditedBlock;
    ::ULIS::eFormat format = src->Format();

    ::ULIS::FRectI rect = Area.IsInitialized() ? Area.GetValue() : src->Rect();
    TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> dst = MakeShareable(new ::ULIS::FBlock( rect.w, rect.h, format ));

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( format );

    //be sure we copy only the needed part
    ::ULIS::FRectI src_rect = rect & src->Rect();
    ::ULIS::FVec2I dst_pos(src_rect.x - rect.x, src_rect.y - rect.y);

    ::ULIS::FEvent eventCopy;
    ctx.Copy( *src, *dst, src_rect, dst_pos, ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 1, &mEvent, &eventCopy);
    ctx.Flush();

    return FOdysseyBlockProxy::MakeProxy(dst, 1, &eventCopy);
}

//static
void
UOdysseyBrushAssetBase::DebugStamp()
{
    if( !mEditedBlock)
        return;

    int size = ::ULIS::FMath::Max( 20.f * GetPressure(), 1.f );

    ::ULIS::FBlock* debug_stamp = new ::ULIS::FBlock( size, size, mEditedBlock->Format() );
    ::ULIS::FColor color = ::ULIS::FColor::RGB(255, 128, 255);
    color.SetAlphaF( 1.0f );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( debug_stamp->Format() );
    const bool bEnableMTPolicy = true;// size > 256;
    ::ULIS::FEvent eventFill;
    ctx.Fill( *debug_stamp, color, ::ULIS::FRectI::Auto, bEnableMTPolicy ? ::ULIS::FSchedulePolicy::AsyncCacheEfficient : ::ULIS::FSchedulePolicy::MonoChunk, 1, &mEvent, &eventFill );
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
        , *mEditedBlock
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

    mEvent = eventBlend;

    invalidRect = invalidRect & mEditedBlock->Rect();
    invalidRect.Sanitize();
    if (invalidRect.w > 0 && invalidRect.h > 0)
        mInvalidRects.Add(invalidRect);
}

//static
void
UOdysseyBrushAssetBase::Stamp( FOdysseyBlockProxy Sample, FOdysseyPivot Pivot, float X, float Y, float Flow, bool iAntiAliasing, EOdysseyBlendingMode BlendingMode, EOdysseyAlphaMode AlphaMode )
{
    if( !mEditedBlock)
        return;

    if( !Sample.IsValid() )
        return;

    TSharedPtr< ::ULIS::FBlock, ESPMode::ThreadSafe > block = Sample.GetBlock();
    FRectF invalidRect = ComputeRectWithPivot( block, Pivot, X, Y );
    ::ULIS::eFormat block_format = block->Format();
    ::ULIS::eFormat target_format = mEditedBlock->Format();

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(target_format);
    //should wait on that before anything
    // Critical event dependency, potential other blend + all events leading to stamp being ready for current blend
    ::ULIS::FEvent eventInputs[] = { mEvent, Sample.GetEvent() };
    ::ULIS::FBlock* src = block.Get();
    ::ULIS::FEvent eventConv;
    if( block_format != target_format )
    {
        src = new ::ULIS::FBlock(block->Width(), block->Height(), target_format);
        ctx.ConvertFormat( *block, *src, ::ULIS::FRectI::Auto, ::ULIS::FVec2I( 0 ), ::ULIS::FSchedulePolicy::AsyncCacheEfficient, 2, eventInputs, &eventConv );
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
            , *mEditedBlock
            , block->Rect()
            , ::ULIS::FVec2F(invalidRect.x, invalidRect.y)
            , ::ULIS::eBlendMode( BlendingMode )
            , ::ULIS::eAlphaMode( AlphaMode )
            , FMath::Clamp(Flow, 0.f, 1.f)
            , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            , 1
            , &eventConv
            , &eventBlend
        );
    }
    else
    {
        ctx.Blend(
              *src
            , *mEditedBlock
            , block->Rect()
            , ::ULIS::FVec2F(invalidRect.x, invalidRect.y)
            , ::ULIS::eBlendMode( BlendingMode )
            , ::ULIS::eAlphaMode( AlphaMode )
            , FMath::Clamp(Flow, 0.f, 1.f)
            , ::ULIS::FSchedulePolicy::AsyncCacheEfficient
            , 1
            , &eventConv
            , &eventBlend
        );
    }

    ctx.Flush();

    mEvent = eventBlend;

    ::ULIS::FRectI invalidRectI( FMath::FloorToInt( invalidRect.x ), FMath::FloorToInt( invalidRect.y ), FMath::CeilToInt( invalidRect.w + 2 ), FMath::CeilToInt( invalidRect.h + 2 ) );
    invalidRectI = invalidRectI & mEditedBlock->Rect();
    if (invalidRectI.w > 0 && invalidRectI.h > 0)
        mInvalidRects.Add(invalidRectI);
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
UOdysseyBrushAssetBase::ExecuteStateChanged()
{
    FEditorScriptExecutionGuard ScriptGuard;
    OnStateChanged();
}

void UOdysseyBrushAssetBase::Serialize (FArchive& Ar)
{
	Super::Serialize(Ar);

    //Serialize overrides
    /* if (Ar->IsLoading())
    {
        UClass* ObjectClass = nullptr;
        Ar << ObjectClass;
    }

    if (Ar->IsSaving())
    {

    } */
}