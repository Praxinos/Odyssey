// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "StrokeEngine/OdysseyStrokeEngine.h"

#include "StrokeEngine/OdysseyBlendParametersOverrides.h"
#include "StrokeEngine/OdysseyBrushOptionsOverrides.h"
#include "ObjectEditorUtils.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyStrokeEngine::~UOdysseyStrokeEngine()
{
}

UOdysseyStrokeEngine::UOdysseyStrokeEngine(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Properties
    , Brush(nullptr)
    , Shape(iObjectInitializer.CreateDefaultSubobject<UOdysseyFreehandShape>(GetTransientPackage(), "UOdysseyStrokeEngine::Shape"))
    , BlendParameters()
    , BrushInstance(nullptr)
    , BrushOptions(iObjectInitializer.CreateDefaultSubobject<UOdysseyBrushOptions>(GetTransientPackage(), "UOdysseyStrokeEngine::BrushOptions"))

    //Internal
    , mPaintEngine(nullptr)
    , mIsPainting(false)
{
    Shape->OnPathBeginDelegate().AddUObject(this, &UOdysseyStrokeEngine::OnShapePathBegin);
    Shape->OnPathToDelegate().AddUObject(this, &UOdysseyStrokeEngine::OnShapePathTo);
    Shape->OnPathEndDelegate().AddUObject(this, &UOdysseyStrokeEngine::OnShapePathEnd);
    Shape->OnResetDelegate().AddUObject(this, &UOdysseyStrokeEngine::OnShapeReset);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tool

void
UOdysseyStrokeEngine::Initialize(FOdysseyPaintEngine* iPaintEngine)
{
    SetPaintEngine(iPaintEngine);

    //Set Default Brush
    UOdysseyPainterEditorSettings* settings = UOdysseyPainterEditorSettings::Get();
    FObjectEditorUtils::SetPropertyValue(this, "Brush", settings->BrushDefaults.DefaultBrush.LoadSynchronous());
}

void
UOdysseyStrokeEngine::Activate()
{
}

void
UOdysseyStrokeEngine::Inactivate()
{
    Flush(); //Finish everything
    Commit(); //Commit the jobs that has been done
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- PaintEngine Callbacks

void
UOdysseyStrokeEngine::OnPaintEngineBlockChanged()
{
    if (BrushInstance)
        BrushInstance->SetBlock(mPaintEngine->PaintBlock());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Shape Callbacks

void
UOdysseyStrokeEngine::OnShapePathBegin( const FOdysseyPoint& iPoint )
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    FOdysseyPoint point = iPoint;

    //StrokeBegin does not draw anything
    if (!BrushInstance->StrokeBegin())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyBrushAssetBase::StrokeBegin() from StrokeEngine"));
        return;
    }

    TArray<UOdysseyBrushAssetBase::FStep> steps = BrushInstance->StepsTo(iPoint);
    for (int i = 0; i < steps.Num(); i++)
    {
        mWorker.Push([this, step = steps[i]]()
        {
            if (!BrushInstance->StrokeStep(step, false))
                UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyBrushAssetBase::StrokeStep() from StrokeEngine"));
        });
    }
}

void
UOdysseyStrokeEngine::OnShapePathTo( const TArray<FOdysseyPoint>& iPoints )
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    for (int i = 0; i < iPoints.Num(); i++)
    {
        TArray<UOdysseyBrushAssetBase::FStep> steps = BrushInstance->StepsTo(iPoints[i]);
        for (int j = 0; j < steps.Num(); j++)
        {
            mWorker.Push([this, step = steps[j]]()
            {
                if (!BrushInstance->StrokeStep(step, false))
                    UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyBrushAssetBase::StrokeStep() from StrokeEngine"));
            });
        }
    }
}

void
UOdysseyStrokeEngine::OnShapePathEnd( const FOdysseyPoint& iPoint )
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    mWorker.Push([this]()
    {
        if (!BrushInstance->StrokeEnd())
            UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyBrushAssetBase::StrokeTo() from StrokeEngine") );
    });
}

void
UOdysseyStrokeEngine::OnShapeReset()
{

    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    mWorker.Clear();

    BrushInstance->StrokeReset();
    
    //Reset the PaintEngine as if didn't draw on it
    if (mPaintEngine)
        mPaintEngine->Reset();
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Stroke API

bool
UOdysseyStrokeEngine::Begin( const FOdysseyPoint& iPoint )
{
    // Check if everything is alright
    if (mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyStrokeEngine::Begin() while Painting") );
        return false;
    }

    mIsPainting = true;

    //Make the shape begin
    if (!Shape->Begin(iPoint))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyFreehandShape::Begin() from StrokeEngine") );
        return false;
    }

    return true;
}

bool
UOdysseyStrokeEngine::To( const FOdysseyPoint& iPoint )
{
    // Check if everything is alright
    if (!mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyStrokeEngine::To() before UOdysseyStrokeEngine::Begin()") );
        return false;
    }

    //Make the shape begin
    if (!Shape->To(iPoint))
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyFreehandShape::To() from StrokeEngine") );
        return false;
    }

    return true;
}

bool
UOdysseyStrokeEngine::End()
{
    // Check if everything is alright
    if (!mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyStrokeEngine::End() before UOdysseyStrokeEngine::Begin()") );
        return false;
    }
    
    //End the Stroke
    if (!Shape->End())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyFreehandShape::End() from StrokeEngine") );
        return false;
    }

    Flush();
    Commit();

    //Stop the painting
    mIsPainting = false;

    return true;
}

bool
UOdysseyStrokeEngine::Abort()
{
    // Check if everything is alright
    if (!mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyStrokeEngine::Abort() before UOdysseyStrokeEngine::Begin()") );
        return false;
    }
    
    //End the Stroke
    if (!Shape->Abort())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyFreehandShape::Abort() from StrokeEngine") );
        return false;
    }

    BrushInstance->StrokeFlush();

    //Update immediately the changes
    mPaintEngine->Update(BlendParameters);

    mIsPainting = false;
    return true;
}

void
UOdysseyStrokeEngine::Flush()
{
    mWorker.Finish();
    BrushInstance->StrokeFlush();
}

void
UOdysseyStrokeEngine::Commit()
{
    if (mPaintEngine)
        mPaintEngine->Commit(BlendParameters);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------------- Tick API
    
void
UOdysseyStrokeEngine::Tick(float iDeltaTime)
{
    // Check if everything is alright
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyStrokeEngine::Tick() without a brush instance") );
        return;
    }

    mWorker.Push([this, iDeltaTime]()
    {
        //Execute a the brush tick node before executing anything else
        BrushInstance->Tick(iDeltaTime, false);
    });

    //Ticke the shape
    Shape->Tick(iDeltaTime);

    //Update the paintEngine
    mWorker.ExecuteFor(1000/60); //60fps

    BrushInstance->StrokeFlush();

    if (mPaintEngine)
        mPaintEngine->Update(BlendParameters);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
UOdysseyStrokeEngine::SetPaintEngine(FOdysseyPaintEngine* iPaintEngine)
{
    if (mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Can't change StrokeEngine PaintEngine while Painting") );
        return;
    }

    if (mPaintEngine)
        mPaintEngine->OnBlockChangedDelegate().RemoveAll(this);

    mPaintEngine = iPaintEngine;

    if (!mPaintEngine)
        return;
        
    mPaintEngine->OnBlockChangedDelegate().AddUObject(this, &UOdysseyStrokeEngine::OnPaintEngineBlockChanged);

    if (BrushInstance)
        BrushInstance->SetBlock(mPaintEngine->PaintBlock());
}

void
UOdysseyStrokeEngine::SetBrushContexts(TArray<FOdysseyBrushContext*> iContexts)
{
    mBrushContexts = iContexts;
}

void
UOdysseyStrokeEngine::RefreshBrushInstance()
{
    DestroyBrushInstance();
    CreateBrushInstance(true);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyBrush*
UOdysseyStrokeEngine::GetBrush()
{
	return Brush;
}

UOdysseyBrushAssetBase*
UOdysseyStrokeEngine::GetBrushInstance()
{
    return BrushInstance; 
}

// Returns the BlendParameters
FOdysseyBlendParameters
UOdysseyStrokeEngine::GetBlendParameters() const
{
    return BlendParameters;
}



// Returns the BrushOptions
UOdysseyBrushOptions*
UOdysseyStrokeEngine::GetBrushOptions()
{
    return BrushOptions;
}

UOdysseyStrokeEngine::FOnApplyOverrides&
UOdysseyStrokeEngine::OnApplyOverridesDelegate()
{
    return mOnApplyOverridesDelegate;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - BrushInstance

void
UOdysseyStrokeEngine::DestroyBrushInstance()
{
	if (!BrushInstance)
		return;

    FObjectEditorUtils::SetPropertyValue(this, "BrushInstance", nullptr);
}

void
UOdysseyStrokeEngine::CreateBrushInstance(bool iApplyOverrides)
{
	if (!Brush)
		return;

	UOdysseyBrushAssetBase* brushInstance = NewObject< UOdysseyBrushAssetBase >(GetTransientPackage(), Brush->GeneratedClass);
	
    brushInstance->SetBrushOptions(BrushOptions); //Share BrushOptions between every selected brushes
    //Set BrushContexts
    for (int i = 0; i < mBrushContexts.Num(); i++)
    {
        brushInstance->AddContext(mBrushContexts[i]); //Set the brush context so that context nodes can be used
    }
    brushInstance->SetBlock(mPaintEngine->PaintBlock());

	//Apply Overrides before setting the brushInstance in the strokeEngine properties
    if (iApplyOverrides)
	    ApplyOverrides(brushInstance);

    FObjectEditorUtils::SetPropertyValue(this, "BrushInstance", brushInstance);
    
    brushInstance->ExecuteSelected();
    brushInstance->ExecuteStateChanged();
}

void
UOdysseyStrokeEngine::OnBrushCompiled(UBlueprint* iBlueprint)
{
	DestroyBrushInstance();
	CreateBrushInstance(false);
}

void
UOdysseyStrokeEngine::ApplyOverrides(UOdysseyBrushAssetBase* iBrushInstance)
{
    if (BrushInstance)
        UE_LOG(LogTemp, Warning, TEXT("ApplyOverrides whould only called when no BrushInstance is active, to avoid calling ExecuteStateChanged at each value change") );

    UOdysseyBlendParametersOverrides* blendParametersOverrides = Cast<UOdysseyBlendParametersOverrides>(iBrushInstance->Overrides["OdysseyBlendParametersOverrides"]);
    if (blendParametersOverrides)
    {
        FOdysseyBlendParameters blendParameters = GetBlendParameters();
        blendParametersOverrides->Override(blendParameters);
        FObjectEditorUtils::SetPropertyValue(this, "BlendParameters", blendParameters);
    }

    UOdysseyBrushOptionsOverrides* brushOptionsOverrides = Cast<UOdysseyBrushOptionsOverrides>(iBrushInstance->Overrides["OdysseyBrushOptionsOverrides"]);
    if (brushOptionsOverrides)
        brushOptionsOverrides->Override(BrushOptions);

    Shape->ApplyOverrides(iBrushInstance->Overrides);

    mOnApplyOverridesDelegate.Broadcast(iBrushInstance->Overrides);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- UObject Overrides

void
UOdysseyStrokeEngine::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
    Super::PreEditChange(PropertyAboutToChange);
    //Nothing to do
}

void
UOdysseyStrokeEngine::PreEditChange(FProperty* PropertyAboutToChange)
{
    Super::PreEditChange(PropertyAboutToChange);

    FString propertyName = PropertyAboutToChange->GetNameCPP();
    if (propertyName == "Brush")
        OnPreBrushChanged();
}

void
UOdysseyStrokeEngine::PostEditChangeChainProperty( struct FPropertyChangedChainEvent & PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    if (!BrushInstance)
        return;

    BrushInstance->ExecuteStateChanged();
}

void
UOdysseyStrokeEngine::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    FName propertyName = PropertyChangedEvent.GetPropertyName();

    if (propertyName == "BrushInstance")
        return;

    if (propertyName == "Brush")
    {
        OnPostBrushChanged();
        return;
    }

    if (BrushInstance)
        BrushInstance->ExecuteStateChanged();
}

void
UOdysseyStrokeEngine::OnPreBrushChanged()
{
    //Destroy the brushInstance
    DestroyBrushInstance();

    //Unbind OnCompiled delegate
    if (Brush)
        Brush->OnCompiled().RemoveAll(this);
}

void
UOdysseyStrokeEngine::OnPostBrushChanged()
{
    if (!Brush)
        return;

    //Bind OnCompiled delegate
    Brush->OnCompiled().AddUObject(this, &UOdysseyStrokeEngine::OnBrushCompiled);

    //Create the BrushInstance to use for drawing
    CreateBrushInstance(true);
}