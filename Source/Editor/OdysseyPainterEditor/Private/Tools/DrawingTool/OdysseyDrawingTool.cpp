// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/DrawingTool/OdysseyDrawingTool.h"

#include "Tools/DrawingTool/OdysseyBlendParametersOverrides.h"
#include "Tools/DrawingTool/OdysseyBrushOptionsOverrides.h"
#include "FreehandShape/OdysseyFreehandShape.h"
#include "Tools/DrawingTool/Widgets/SOdysseyDrawingToolOptions.h"
#include "ObjectEditorUtils.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyDrawingTool::~UOdysseyDrawingTool()
{
}

UOdysseyDrawingTool::UOdysseyDrawingTool()
    : Super()
    //Properties
    , Brush(nullptr)
    , BrushInstance(nullptr)
    , BrushOptions(CreateDefaultSubobject<UOdysseyBrushOptions>("UOdysseyDrawingTool::BrushOptions", true))
    , BlendParameters()
    , SelectedShape(EOdysseyShape::kFreehand)
    , SelectedShapeInstance(nullptr)

    //Internal
    , mPaintEngine(nullptr)
{   
    AvailableShapes.Add(EOdysseyShape::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyDrawingTool::FreehandShape"));
    SelectedShapeInstance = AvailableShapes[SelectedShape];
}

template<class T>
T*
UOdysseyDrawingTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnPathBeginDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathBegin);
    shape->OnPathToDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathTo);
    shape->OnPathEndDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathEnd);
    shape->OnPathAbortDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathAbort);
    shape->OnPathResetDelegate().AddUObject(this, &UOdysseyDrawingTool::OnShapePathReset);

    shape->AdaptStepDelegate().BindUObject(this, &UOdysseyDrawingTool::AdaptShapeStep);

    return shape;
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
UOdysseyDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    SelectedShapeInstance->OnMouseDown(iPointInTexture, iKey);
}

void
UOdysseyDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    SelectedShapeInstance->OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (BrushInstance)
        BrushInstance->StrokeMoveTo(iPointInTexture);

    SelectedShapeInstance->OnMouseHover(iPointInTexture);
}

void
UOdysseyDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    SelectedShapeInstance->OnMouseDrag(iPointInTexture);
}

void
UOdysseyDrawingTool::OnKeyDown(const FKey& iKey)
{   
    SelectedShapeInstance->OnKeyDown(iKey);
}

void
UOdysseyDrawingTool::OnKeyUp(const FKey& iKey)
{
    SelectedShapeInstance->OnKeyUp(iKey);
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
    SelectedShapeInstance->Tick(iDeltaTime);

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

void
UOdysseyDrawingTool::BindShortcuts(FBaseToolkit* iToolkit)
{
    Super::BindShortcuts(iToolkit);
}

void
UOdysseyDrawingTool::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    Super::ExtendMenu(iOwner, iMenuName);
}

TSharedPtr<SWidget>
UOdysseyDrawingTool::GetWidget()
{
    return SNew(SOdysseyDrawingToolOptions).Tool(this);
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

    TArray<UOdysseyBrushAssetBase::FStep> steps = BrushInstance->StepsTo({ iPoint });
    for (int i = 0; i < steps.Num(); i++)
    {
        mWorker.Push([this, step = steps[i]]()
        {
            if (!BrushInstance->StrokeStep(step))
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

    TArray<UOdysseyBrushAssetBase::FStep> steps = BrushInstance->StepsTo(iPoints);
    for (int j = 0; j < steps.Num(); j++)
    {
        mWorker.Push([this, step = steps[j]]()
        {
            if (!BrushInstance->StrokeStep(step))
                UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyBrushAssetBase::StrokeStep() from StrokeEngine"));
        });
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

    Flush();
    Commit();
}

void
UOdysseyDrawingTool::OnShapePathAbort()
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    BrushInstance->StrokeFlush();

    //Update immediately the changes
    mPaintEngine->Update(BlendParameters);
}

void
UOdysseyDrawingTool::OnShapePathReset()
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

float
UOdysseyDrawingTool::AdaptShapeStep(float iStep)
{
    return (iStep / 100.f) * BrushOptions->Size;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
UOdysseyDrawingTool::SetPaintEngine(FOdysseyPaintEngine* iPaintEngine)
{
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

EOdysseyShape
UOdysseyDrawingTool::GetSelectedShape() const
{
    return SelectedShape;
}

UOdysseyShape*
UOdysseyDrawingTool::GetSelectedShapeInstance()
{
    return SelectedShapeInstance;
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

    SelectedShapeInstance->ApplyOverrides(iBrushInstance->Overrides);

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

    if (propertyName == "SelectedShape")
        OnPostShapeChanged();

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

void
UOdysseyDrawingTool::OnPostShapeChanged()
{
    FObjectEditorUtils::SetPropertyValue(this, "SelectedShapeInstance", AvailableShapes[SelectedShape]);
}