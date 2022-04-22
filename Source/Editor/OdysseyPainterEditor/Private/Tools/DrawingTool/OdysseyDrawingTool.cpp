// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/DrawingTool/OdysseyDrawingTool.h"

#include "Tools/DrawingTool/OdysseyBlendParametersOverrides.h"
#include "Tools/DrawingTool/OdysseyBrushOptionsOverrides.h"
#include "ObjectEditorUtils.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyDrawingTool::~UOdysseyDrawingTool()
{
}

UOdysseyDrawingTool::UOdysseyDrawingTool(const FObjectInitializer& iObjectInitializer)
    : Super(iObjectInitializer)
    //Properties
    , Brush(nullptr)
    , Shape(iObjectInitializer.CreateDefaultSubobject<UOdysseyFreehandShape>(GetTransientPackage(), "UOdysseyDrawingTool::Shape"))
    , BlendParameters()
    , BrushInstance(nullptr)
    , BrushOptions(iObjectInitializer.CreateDefaultSubobject<UOdysseyBrushOptions>(GetTransientPackage(), "UOdysseyDrawingTool::BrushOptions"))

    //Internal
    , mPaintEngine(nullptr)
    , mIsPainting(false)
{
    Shape->OnPathBeginDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathBegin);
    Shape->OnPathToDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathTo);
    Shape->OnPathEndDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathEnd);
    Shape->OnResetDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapeReset);
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tool

void
UOdysseyDrawingTool::Initialize(FOdysseyPaintEngine* iPaintEngine)
{
    SetPaintEngine(iPaintEngine);

    //Set Default Brush
    UOdysseyPainterEditorSettings* settings = UOdysseyPainterEditorSettings::Get();
    FObjectEditorUtils::SetPropertyValue(this, "Brush", settings->BrushDefaults.DefaultBrush.LoadSynchronous());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyTool overrides

void
UOdysseyDrawingTool::Activate()
{

}

void
UOdysseyDrawingTool::Inactivate()
{
    Flush(); //Finish everything
    Commit(); //Commit the jobs that has been done
}

void
UOdysseyDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInViewport, const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    Begin( iPointInTexture );
}

void
UOdysseyDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInViewport, const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    End();
}

void
UOdysseyDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInViewport, const FOdysseyPoint& iPointInTexture)
{
    if (BrushInstance)
        BrushInstance->StrokeMoveTo(iPointInTexture);
}

void
UOdysseyDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInViewport, const FOdysseyPoint& iPointInTexture)
{
    To( iPointInTexture );
}

void
UOdysseyDrawingTool::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::Escape)
    {
        Abort();
        return;
    }
}

void
UOdysseyDrawingTool::OnKeyUp(const FKey& iKey)
{

}

void
UOdysseyDrawingTool::Tick(float iDeltaTime)
{
    // Check if everything is alright
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyDrawingTool::Tick() without a brush instance") );
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

void
UOdysseyDrawingTool::Flush()
{
    mWorker.Finish();
    BrushInstance->StrokeFlush();
}

void
UOdysseyDrawingTool::Commit()
{
    if (mPaintEngine)
        mPaintEngine->Commit(BlendParameters);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- PaintEngine Callbacks

void
UOdysseyDrawingTool::OnPaintEngineBlockChanged()
{
    if (BrushInstance)
        BrushInstance->SetBlock(mPaintEngine->PaintBlock());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Shape Callbacks

void
UOdysseyDrawingTool::OnShapePathBegin( const FOdysseyPoint& iPoint )
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
UOdysseyDrawingTool::OnShapePathTo( const TArray<FOdysseyPoint>& iPoints )
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
UOdysseyDrawingTool::OnShapePathEnd( const FOdysseyPoint& iPoint )
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
UOdysseyDrawingTool::OnShapeReset()
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
UOdysseyDrawingTool::Begin( const FOdysseyPoint& iPoint )
{
    // Check if everything is alright
    if (mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyDrawingTool::Begin() while Painting") );
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
UOdysseyDrawingTool::To( const FOdysseyPoint& iPoint )
{
    // Check if everything is alright
    if (!mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyDrawingTool::To() before UOdysseyDrawingTool::Begin()") );
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
UOdysseyDrawingTool::End()
{
    // Check if everything is alright
    if (!mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyDrawingTool::End() before UOdysseyDrawingTool::Begin()") );
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
UOdysseyDrawingTool::Abort()
{
    // Check if everything is alright
    if (!mIsPainting)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot call UOdysseyDrawingTool::Abort() before UOdysseyDrawingTool::Begin()") );
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
UOdysseyDrawingTool::SetPaintEngine(FOdysseyPaintEngine* iPaintEngine)
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
        
    mPaintEngine->OnBlockChangedDelegate().AddUObject(this, &UOdysseyDrawingTool::OnPaintEngineBlockChanged);

    if (BrushInstance)
        BrushInstance->SetBlock(mPaintEngine->PaintBlock());
}

void
UOdysseyDrawingTool::SetBrushContexts(TArray<FOdysseyBrushContext*> iContexts)
{
    mBrushContexts = iContexts;
}

void
UOdysseyDrawingTool::RefreshBrushInstance()
{
    DestroyBrushInstance();
    CreateBrushInstance(true);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyBrush*
UOdysseyDrawingTool::GetBrush()
{
	return Brush;
}

UOdysseyBrushAssetBase*
UOdysseyDrawingTool::GetBrushInstance()
{
    return BrushInstance; 
}

// Returns the BlendParameters
FOdysseyBlendParameters
UOdysseyDrawingTool::GetBlendParameters() const
{
    return BlendParameters;
}



// Returns the BrushOptions
UOdysseyBrushOptions*
UOdysseyDrawingTool::GetBrushOptions()
{
    return BrushOptions;
}

UOdysseyDrawingTool::FOnApplyOverrides&
UOdysseyDrawingTool::OnApplyOverridesDelegate()
{
    return mOnApplyOverridesDelegate;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - BrushInstance

void
UOdysseyDrawingTool::DestroyBrushInstance()
{
	if (!BrushInstance)
		return;

    FObjectEditorUtils::SetPropertyValue(this, "BrushInstance", nullptr);
}

void
UOdysseyDrawingTool::CreateBrushInstance(bool iApplyOverrides)
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
UOdysseyDrawingTool::OnBrushCompiled(UBlueprint* iBlueprint)
{
	DestroyBrushInstance();
	CreateBrushInstance(false);
}

void
UOdysseyDrawingTool::ApplyOverrides(UOdysseyBrushAssetBase* iBrushInstance)
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
UOdysseyDrawingTool::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
    Super::PreEditChange(PropertyAboutToChange);
    //Nothing to do
}

void
UOdysseyDrawingTool::PreEditChange(FProperty* PropertyAboutToChange)
{
    Super::PreEditChange(PropertyAboutToChange);

    FString propertyName = PropertyAboutToChange->GetNameCPP();
    if (propertyName == "Brush")
        OnPreBrushChanged();
}

void
UOdysseyDrawingTool::PostEditChangeChainProperty( struct FPropertyChangedChainEvent & PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    if (!BrushInstance)
        return;

    BrushInstance->ExecuteStateChanged();
}

void
UOdysseyDrawingTool::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent)
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
UOdysseyDrawingTool::OnPreBrushChanged()
{
    //Destroy the brushInstance
    DestroyBrushInstance();

    //Unbind OnCompiled delegate
    if (Brush)
        Brush->OnCompiled().RemoveAll(this);
}

void
UOdysseyDrawingTool::OnPostBrushChanged()
{
    if (!Brush)
        return;

    //Bind OnCompiled delegate
    Brush->OnCompiled().AddUObject(this, &UOdysseyDrawingTool::OnBrushCompiled);

    //Create the BrushInstance to use for drawing
    CreateBrushInstance(true);
}