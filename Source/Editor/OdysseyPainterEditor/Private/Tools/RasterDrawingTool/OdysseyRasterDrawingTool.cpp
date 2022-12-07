// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyRasterDrawingTool.h"

#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"
#include "FreehandShape/OdysseyFreehandShape.h"
#include "Tools/RasterDrawingTool/Widgets/SOdysseyRasterDrawingToolOptions.h"
#include "ObjectEditorUtils.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyRasterDrawingTool::~UOdysseyRasterDrawingTool()
{
}

UOdysseyRasterDrawingTool::UOdysseyRasterDrawingTool()
    : Super()
    //Properties
    , Brush(nullptr)
    , BrushInstance(nullptr)
    , BrushOptions(CreateDefaultSubobject<UOdysseyBrushOptions>("UOdysseyRasterDrawingTool::BrushOptions", true))
    , BlendParameters()
    , SelectedShape(EOdysseyShape::kFreehand)
    , SelectedShapeInstance(nullptr)

    //Internal
    , mPaintEngine(nullptr)
    , mIsDrawingLocked(false)
{
    AvailableShapes.Add(EOdysseyShape::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyRasterDrawingTool::FreehandShape"));
    SelectedShapeInstance = AvailableShapes[SelectedShape];
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.DrawingTool64");
}

template<class T>
T*
UOdysseyRasterDrawingTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnPathBeginDelegate().AddUObject(this, &UOdysseyRasterDrawingTool::OnShapePathBegin);
    shape->OnPathToDelegate().AddUObject(this, &UOdysseyRasterDrawingTool::OnShapePathTo);
    shape->OnPathEndDelegate().AddUObject(this, &UOdysseyRasterDrawingTool::OnShapePathEnd);
    shape->OnPathAbortDelegate().AddUObject(this, &UOdysseyRasterDrawingTool::OnShapePathAbort);
    shape->OnPathResetDelegate().AddUObject(this, &UOdysseyRasterDrawingTool::OnShapePathReset);

    shape->AdaptStepDelegate().BindUObject(this, &UOdysseyRasterDrawingTool::AdaptShapeStep);

    return shape;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tool

void
UOdysseyRasterDrawingTool::Initialize(FOdysseyPaintEngine* iPaintEngine)
{
    SetPaintEngine(iPaintEngine);

    //Set Default Brush
    UOdysseyPainterEditorSettings* settings = UOdysseyPainterEditorSettings::Get();
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Brush", settings->BrushDefaults.DefaultBrush.LoadSynchronous());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyTool overrides

void
UOdysseyRasterDrawingTool::Activate()
{

}

void
UOdysseyRasterDrawingTool::Inactivate()
{
    Flush(); //Finish everything
    Commit(); //Commit the jobs that has been done
}

bool
UOdysseyRasterDrawingTool::CanDraw()
{
    // Check if everything is alright
    if (!BrushInstance && BrushInstance->GetBlock())
        return false;

    return !mIsDrawingLocked;
}

bool
UOdysseyRasterDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!CanDraw())
        return false;

    return SelectedShapeInstance->OnMouseDown(iPointInTexture, iKey);
}

bool
UOdysseyRasterDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!CanDraw())
        return false;

    return SelectedShapeInstance->OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyRasterDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (!CanDraw())
        return;

    if (BrushInstance)
        BrushInstance->StrokeMoveTo(iPointInTexture);

    SelectedShapeInstance->OnMouseHover(iPointInTexture);
}

void
UOdysseyRasterDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!CanDraw())
        return;

    SelectedShapeInstance->OnMouseDrag(iPointInTexture);
}

bool
UOdysseyRasterDrawingTool::OnKeyDown(const FKey& iKey)
{
    if (!CanDraw())
        return false;

    return SelectedShapeInstance->OnKeyDown(iKey);
}

bool
UOdysseyRasterDrawingTool::OnKeyUp(const FKey& iKey)
{
    if (!CanDraw())
        return false;

    return SelectedShapeInstance->OnKeyUp(iKey);
}

void
UOdysseyRasterDrawingTool::Tick(float iDeltaTime)
{
    if (!CanDraw())
        return;

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
UOdysseyRasterDrawingTool::Flush()
{
    mWorker.Finish();
    BrushInstance->StrokeFlush();
}

void
UOdysseyRasterDrawingTool::Commit()
{
    if (mPaintEngine)
        mPaintEngine->Commit(BlendParameters);
}

void
UOdysseyRasterDrawingTool::BindShortcuts(FBaseToolkit* iToolkit)
{
    Super::BindShortcuts(iToolkit);

    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorToolCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyRasterDrawingTool::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorToolCommands.RefreshBrush, RefreshBrushInstance )

    #undef MAP_ACTION
}

void
UOdysseyRasterDrawingTool::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    Super::ExtendMenu(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- PaintEngine Callbacks

void
UOdysseyRasterDrawingTool::OnPaintEngineBlockChanged()
{
    if (BrushInstance)
        BrushInstance->SetBlock(mPaintEngine->PaintBlock());
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Shape Callbacks

void
UOdysseyRasterDrawingTool::OnShapePathBegin( const FOdysseyPoint& iPoint )
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    //StrokeBegin does not draw anything
    if (!BrushInstance->StrokeBegin())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to call UOdysseyBrushAssetBase::StrokeBegin() from StrokeEngine"));
        return;
    }

    TArray<FOdysseyPoint> points = { iPoint };
    if (mAdaptShapePointsDelegate.IsBound())
        points = mAdaptShapePointsDelegate.Execute({iPoint});

    TArray<UOdysseyBrushAssetBase::FStep> steps = BrushInstance->StepsTo(points);
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
UOdysseyRasterDrawingTool::OnShapePathTo( const TArray<FOdysseyPoint>& iPoints )
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    TArray<FOdysseyPoint> points = mAdaptShapePointsDelegate.IsBound() ? mAdaptShapePointsDelegate.Execute(iPoints) : iPoints;

    TArray<UOdysseyBrushAssetBase::FStep> steps = BrushInstance->StepsTo(points);
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
UOdysseyRasterDrawingTool::OnShapePathEnd( const FOdysseyPoint& iPoint )
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
UOdysseyRasterDrawingTool::OnShapePathAbort()
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
UOdysseyRasterDrawingTool::OnShapePathReset()
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
UOdysseyRasterDrawingTool::AdaptShapeStep(float iStep)
{
    return (iStep / 100.f) * BrushOptions->Size;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
UOdysseyRasterDrawingTool::SetPaintEngine(FOdysseyPaintEngine* iPaintEngine)
{
    if (mPaintEngine)
        mPaintEngine->OnBlockChangedDelegate().RemoveAll(this);

    mPaintEngine = iPaintEngine;

    if (!mPaintEngine)
        return;
        
    mPaintEngine->OnBlockChangedDelegate().AddUObject(this, &UOdysseyRasterDrawingTool::OnPaintEngineBlockChanged);

    if (BrushInstance)
        BrushInstance->SetBlock(mPaintEngine->PaintBlock());
}

void
UOdysseyRasterDrawingTool::SetBrushContexts(TArray<FOdysseyBrushContext*> iContexts)
{
    mBrushContexts = iContexts;
}

// Set wether the tool can draw or not
void
UOdysseyRasterDrawingTool::IsDrawingLocked(bool iValue)
{
    if( mIsDrawingLocked == iValue )
        return;

    mIsDrawingLocked = iValue;

    if( mIsDrawingLocked )
    {
        //Flush and commit
        Flush();
        Commit();
    }
}

void
UOdysseyRasterDrawingTool::RefreshBrushInstance()
{
    DestroyBrushInstance();
    CreateBrushInstance(true);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyBrush*
UOdysseyRasterDrawingTool::GetBrush()
{
	return Brush;
}

UOdysseyBrushAssetBase*
UOdysseyRasterDrawingTool::GetBrushInstance()
{
    return BrushInstance; 
}

// Returns the BlendParameters
FOdysseyBlendParameters
UOdysseyRasterDrawingTool::GetBlendParameters() const
{
    return BlendParameters;
}

// Returns the BrushOptions
UOdysseyBrushOptions*
UOdysseyRasterDrawingTool::GetBrushOptions()
{
    return BrushOptions;
}

EOdysseyShape
UOdysseyRasterDrawingTool::GetSelectedShape() const
{
    return SelectedShape;
}

UOdysseyShape*
UOdysseyRasterDrawingTool::GetSelectedShapeInstance()
{
    return SelectedShapeInstance;
}

UOdysseyRasterDrawingTool::FOnApplyOverrides&
UOdysseyRasterDrawingTool::OnApplyOverridesDelegate()
{
    return mOnApplyOverridesDelegate;
}


UOdysseyRasterDrawingTool::FAdaptShapePoints&
UOdysseyRasterDrawingTool::AdaptShapePointsDelegate()
{
    return mAdaptShapePointsDelegate;
}

bool
UOdysseyRasterDrawingTool::IsDrawingLocked()
{
    return mIsDrawingLocked;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - BrushInstance

void
UOdysseyRasterDrawingTool::DestroyBrushInstance()
{
	if (!BrushInstance)
		return;

    FOdysseyObjectEditorUtils::SetPropertyValue(this, "BrushInstance", nullptr);
}

void
UOdysseyRasterDrawingTool::CreateBrushInstance(bool iApplyOverrides)
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

    FOdysseyObjectEditorUtils::SetPropertyValue(this, "BrushInstance", brushInstance);
    
    brushInstance->ExecuteSelected();
    brushInstance->ExecuteStateChanged();
}

void
UOdysseyRasterDrawingTool::OnBrushCompiled(UBlueprint* iBlueprint)
{
	DestroyBrushInstance();
	CreateBrushInstance(false);
}

void
UOdysseyRasterDrawingTool::ApplyOverrides(UOdysseyBrushAssetBase* iBrushInstance)
{
    if (BrushInstance)
        UE_LOG(LogTemp, Warning, TEXT("ApplyOverrides whould only called when no BrushInstance is active, to avoid calling ExecuteStateChanged at each value change") );

    UOdysseyBlendParametersOverrides* blendParametersOverrides = Cast<UOdysseyBlendParametersOverrides>(iBrushInstance->Overrides["OdysseyBlendParametersOverrides"]);
    if (blendParametersOverrides)
    {
        FOdysseyBlendParameters blendParameters = GetBlendParameters();
        blendParametersOverrides->Override(blendParameters);
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "BlendParameters", blendParameters);
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
UOdysseyRasterDrawingTool::PreEditChange(FEditPropertyChain& PropertyAboutToChange)
{
    Super::PreEditChange(PropertyAboutToChange);
    //Nothing to do
}

void
UOdysseyRasterDrawingTool::PreEditChange(FProperty* PropertyAboutToChange)
{
    Super::PreEditChange(PropertyAboutToChange);

    FString propertyName = PropertyAboutToChange->GetNameCPP();
    if (propertyName == "Brush")
        OnPreBrushChanged();
}

void
UOdysseyRasterDrawingTool::PostEditChangeChainProperty( struct FPropertyChangedChainEvent & PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    if (!BrushInstance)
        return;

    BrushInstance->ExecuteStateChanged();
}

void
UOdysseyRasterDrawingTool::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent)
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
UOdysseyRasterDrawingTool::OnPreBrushChanged()
{
    //Destroy the brushInstance
    DestroyBrushInstance();

    //Unbind OnCompiled delegate
    if (Brush)
        Brush->OnCompiled().RemoveAll(this);
}

void
UOdysseyRasterDrawingTool::OnPostBrushChanged()
{
    if (!Brush)
        return;

    //Bind OnCompiled delegate
    Brush->OnCompiled().AddUObject(this, &UOdysseyRasterDrawingTool::OnBrushCompiled);

    //Create the BrushInstance to use for drawing
    CreateBrushInstance(true);
}

void
UOdysseyRasterDrawingTool::OnPostShapeChanged()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedShapeInstance", AvailableShapes[SelectedShape]);
}
