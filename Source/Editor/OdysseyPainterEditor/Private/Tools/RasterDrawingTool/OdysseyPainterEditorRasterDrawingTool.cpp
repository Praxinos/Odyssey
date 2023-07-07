// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"

#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"
#include "FreehandShape/OdysseyFreehandShape.h"
#include "ObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorRasterDrawingTool"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterDrawingTool::~UOdysseyPainterEditorRasterDrawingTool()
{
    //GEditor->OnBlueprintCompiled().RemoveAll(this);
}

UOdysseyPainterEditorRasterDrawingTool::UOdysseyPainterEditorRasterDrawingTool()
    : Super()
    //Properties
    , mBaseSize(0)
    , Brush(nullptr)
    , BrushInstance(nullptr)
    , BrushOptions(CreateDefaultSubobject<UOdysseyBrushOptions>("UOdysseyPainterEditorRasterDrawingTool::BrushOptions", true))
    , SelectedShape(EOdysseyShape::kFreehand)
    , SelectedShapeInstance(nullptr)
    //Internal
    , mPaintEngine()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.DrawingTool64");
    //GEditor->OnBlueprintCompiled().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnBlueprintCompiled);

    AvailableShapes.Add(EOdysseyShape::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterDrawingTool::FreehandShape"));
    SelectedShapeInstance = AvailableShapes[SelectedShape];
}

template<class T>
T*
UOdysseyPainterEditorRasterDrawingTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnPathBeginDelegate().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapePathBegin);
    shape->OnPathToDelegate().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapePathTo);
    shape->OnPathEndDelegate().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapePathEnd);
    shape->OnPathAbortDelegate().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapePathAbort);
    shape->OnPathResetDelegate().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapePathReset);

    shape->AdaptStepDelegate().BindUObject(this, &UOdysseyPainterEditorRasterDrawingTool::AdaptShapeStep);

    return shape;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorRasterDrawingTool::Activate()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(mToolContext->GetEditor()->PaintColor()));
    
    //Set Default Brush
    if(!Brush)
    {
        UOdysseyPainterEditorSettings* settings = UOdysseyPainterEditorSettings::Get();
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "Brush", settings->BrushDefaults.DefaultBrush.LoadSynchronous());
    }

    Super::Activate();
}

void
UOdysseyPainterEditorRasterDrawingTool::Load()
{
	mPaintEngine.RasterBlock(mToolContext->GetRasterBlock());

	if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock());
}

void
UOdysseyPainterEditorRasterDrawingTool::Unload()
{
	mPaintEngine.RasterBlock(nullptr);

	if ( BrushInstance )
		BrushInstance->SetBlock(nullptr);
}

bool
UOdysseyPainterEditorRasterDrawingTool::IsActivable() const
{
    return !!mToolContext->GetRasterBlock();
}

bool
UOdysseyPainterEditorRasterDrawingTool::CanDraw()
{
    if (!BrushInstance)
        return false;

    if (!mToolContext->GetRasterBlock() && !mToolContext->CanProvideRasterBlockOnDemand())
        return false;

    if (mToolContext->IsRasterBlockReadOnly())
        return false;

    return true;
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!CanDraw())
        return false;

    if (!mToolContext->GetRasterBlock())
        mToolContext->OnProvideRasterBlockDelegate().Execute();

    return SelectedShapeInstance->OnMouseDown(iPointInTexture, iKey);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!CanDraw())
        return false;

    return SelectedShapeInstance->OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    if (!CanDraw())
        return;

    if (BrushInstance)
        BrushInstance->StrokeMoveTo(iPointInTexture);

    SelectedShapeInstance->OnMouseHover(iPointInTexture);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!CanDraw())
        return;

    SelectedShapeInstance->OnMouseDrag(iPointInTexture);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnKeyDown(const FKey& iKey)
{
    if (!CanDraw())
        return false;

    return SelectedShapeInstance->OnKeyDown(iKey);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnKeyUp(const FKey& iKey)
{
    if (!CanDraw())
        return false;

    return SelectedShapeInstance->OnKeyUp(iKey);
}

void
UOdysseyPainterEditorRasterDrawingTool::Tick(float iDeltaTime)
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

    mWorker.ExecuteFor(1000/60); //60fps

    BrushInstance->StrokeFlush();

    //Update the paintEngine
    mPaintEngine.Update(BlendParameters);
}

void
UOdysseyPainterEditorRasterDrawingTool::Flush()
{
    mWorker.Finish();
    BrushInstance->StrokeFlush();
}

void
UOdysseyPainterEditorRasterDrawingTool::Commit()
{
    mPaintEngine.Commit(BlendParameters);
}

void
UOdysseyPainterEditorRasterDrawingTool::BindShortcuts(FBaseToolkit* iToolkit)
{
    Super::BindShortcuts(iToolkit);

    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorToolCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorRasterDrawingTool::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorToolCommands.RefreshBrush, RefreshBrushInstance )

    #undef MAP_ACTION
}

void
UOdysseyPainterEditorRasterDrawingTool::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    Super::ExtendMenu(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Shape Callbacks

void
UOdysseyPainterEditorRasterDrawingTool::OnShapePathBegin( const FOdysseyPoint& iPoint )
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

    GEditor->BeginTransaction(TEXT("PaintEngine"), LOCTEXT("OnPaintStroke", "Paint Stroke"), nullptr);

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
UOdysseyPainterEditorRasterDrawingTool::OnShapePathTo( const TArray<FOdysseyPoint>& iPoints )
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
UOdysseyPainterEditorRasterDrawingTool::OnShapePathEnd( const FOdysseyPoint& iPoint )
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

    GEditor->EndTransaction();
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapePathAbort()
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    BrushInstance->StrokeFlush();

    //Update immediately the changes
    mPaintEngine.Update(BlendParameters);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapePathReset()
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }

    mWorker.Clear();

    BrushInstance->StrokeReset();
    
    //Reset the PaintEngine as if didn't draw on it
    mPaintEngine.Abort();
}

float
UOdysseyPainterEditorRasterDrawingTool::AdaptShapeStep(float iStep)
{
    return (iStep / 100.f) * BrushOptions->Size;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
UOdysseyPainterEditorRasterDrawingTool::SetBrushContexts(TArray<FOdysseyBrushContext*> iContexts)
{
    mBrushContexts = iContexts;
}

void
UOdysseyPainterEditorRasterDrawingTool::RefreshBrushInstance()
{
    DestroyBrushInstance();
    CreateBrushInstance(true);
}

void
UOdysseyPainterEditorRasterDrawingTool::SetBaseSize(float iValue)
{
    mBaseSize = iValue;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyBrush*
UOdysseyPainterEditorRasterDrawingTool::GetBrush() const
{
	return Brush;
}

UOdysseyBrushAssetBase*
UOdysseyPainterEditorRasterDrawingTool::GetBrushInstance() const
{
    return BrushInstance; 
}

// Returns the BlendParameters
FOdysseyBlendParameters
UOdysseyPainterEditorRasterDrawingTool::GetBlendParameters() const
{
    return BlendParameters;
}

// Returns the BrushOptions
UOdysseyBrushOptions*
UOdysseyPainterEditorRasterDrawingTool::GetBrushOptions() const
{
    return BrushOptions;
}

EOdysseyShape
UOdysseyPainterEditorRasterDrawingTool::GetSelectedShape() const
{
    return SelectedShape;
}

UOdysseyShape*
UOdysseyPainterEditorRasterDrawingTool::GetSelectedShapeInstance() const
{
    return SelectedShapeInstance;
}

UOdysseyPainterEditorRasterDrawingTool::FOnApplyOverrides&
UOdysseyPainterEditorRasterDrawingTool::OnApplyOverridesDelegate()
{
    return mOnApplyOverridesDelegate;
}

UOdysseyPainterEditorRasterDrawingTool::FOnDestroyBrushInstance&
UOdysseyPainterEditorRasterDrawingTool::OnDestroyBrushInstance()
{
    return mOnDestroyBrushInstance;
}

UOdysseyPainterEditorRasterDrawingTool::FOnCreatedBrushInstance&
UOdysseyPainterEditorRasterDrawingTool::OnCreatedBrushInstance()
{
    return mOnCreatedBrushInstance;
}

UOdysseyPainterEditorRasterDrawingTool::FAdaptShapePoints&
UOdysseyPainterEditorRasterDrawingTool::AdaptShapePointsDelegate()
{
    return mAdaptShapePointsDelegate;
}

FSimpleMulticastDelegate&
UOdysseyPainterEditorRasterDrawingTool::OnBrushChanged()
{
    return mOnBrushChanged;
}

FSimpleMulticastDelegate&
UOdysseyPainterEditorRasterDrawingTool::OnShapeChanged()
{
    return mOnShapeChanged;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - BrushInstance

void
UOdysseyPainterEditorRasterDrawingTool::DestroyBrushInstance()
{
	if (!BrushInstance)
		return;

    FOdysseyObjectEditorUtils::SetPropertyValue(this, "BrushInstance", nullptr);
}

void
UOdysseyPainterEditorRasterDrawingTool::CreateBrushInstance(bool iApplyOverrides)
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
    brushInstance->SetBlock(mPaintEngine.PaintBlock());

	//Apply Overrides before setting the brushInstance in the strokeEngine properties
    if (iApplyOverrides)
	    ApplyOverrides(brushInstance);

    FOdysseyObjectEditorUtils::SetPropertyValue(this, "BrushInstance", brushInstance);
    
    brushInstance->ExecuteSelected();
    brushInstance->ExecuteStateChanged();

    mOnCreatedBrushInstance.Broadcast(BrushInstance);
}

/* void
UOdysseyPainterEditorRasterDrawingTool::OnBlueprintCompiled(UBlueprint* iBlueprint)
{
    if (iBlueprint == Brush)
    {
        DestroyBrushInstance();
        CreateBrushInstance(false);
    }
} */

void
UOdysseyPainterEditorRasterDrawingTool::ApplyOverrides(UOdysseyBrushAssetBase* iBrushInstance)
{
    if (BrushInstance)
        UE_LOG(LogTemp, Warning, TEXT("ApplyOverrides whould only called when no BrushInstance is active, to avoid calling ExecuteStateChanged at each value change") );

    UOdysseyBlendParametersOverrides* blendParametersOverrides = Cast<UOdysseyBlendParametersOverrides>(iBrushInstance->EditorOverrides[UOdysseyBlendParametersOverrides::StaticClass()]);
    if (blendParametersOverrides)
    {
        FOdysseyBlendParameters blendParameters = GetBlendParameters();
        blendParametersOverrides->Override(blendParameters);
        FOdysseyObjectEditorUtils::SetPropertyValue(this, "BlendParameters", blendParameters);
    }

    UOdysseyBrushOptionsOverrides* brushOptionsOverrides = Cast<UOdysseyBrushOptionsOverrides>(iBrushInstance->EditorOverrides[UOdysseyBrushOptionsOverrides::StaticClass()]);
    if (brushOptionsOverrides)
        brushOptionsOverrides->Override(BrushOptions);

    SelectedShapeInstance->ApplyOverrides(iBrushInstance->EditorOverrides);

    mOnApplyOverridesDelegate.Broadcast(iBrushInstance->EditorOverrides);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- UObject Overrides

void
UOdysseyPainterEditorRasterDrawingTool::BrushChanged()
{
    if (!Brush)
        return;
        
    //Destroy the brushInstance
    DestroyBrushInstance();

    //Create the BrushInstance to use for drawing
    CreateBrushInstance(true);

    mOnBrushChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterDrawingTool::SelectedShapeChanged()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedShapeInstance", AvailableShapes[SelectedShape]);
    mOnShapeChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterDrawingTool::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == "Brush")
        BrushChanged();

    if (iPropertyName == "BrushInstance")
        return;
        //BrushInstanceChanged();

    if (iPropertyName == "SelectedShape")
        SelectedShapeChanged();

    if ( BrushInstance )
        BrushInstance->ExecuteStateChanged();
}

/* void
UOdysseyPainterEditorRasterDrawingTool::PreEditChange(FProperty* PropertyAboutToChange)
{
    Super::PreEditChange(PropertyAboutToChange);

    FString propertyName = PropertyAboutToChange->GetNameCPP();
    if (propertyName == "Brush")
        OnPreBrushChanged();
}

void
UOdysseyPainterEditorRasterDrawingTool::PostEditChangeChainProperty( struct FPropertyChangedChainEvent & PropertyChangedEvent)
{
    Super::PostEditChangeChainProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    if (!BrushInstance)
        return;

    BrushInstance->ExecuteStateChanged();
}

void
UOdysseyPainterEditorRasterDrawingTool::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent)
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
UOdysseyPainterEditorRasterDrawingTool::OnPreBrushChanged()
{
    //Destroy the brushInstance
    DestroyBrushInstance();

    //Unbind OnCompiled delegate
    if (Brush)
        Brush->OnCompiled().RemoveAll(this);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnPostBrushChanged()
{
    if (!Brush)
        return;

    //Bind OnCompiled delegate
    Brush->OnCompiled().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnBrushCompiled);

    //Create the BrushInstance to use for drawing
    CreateBrushInstance(true);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnPostShapeChanged()
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "SelectedShapeInstance", AvailableShapes[SelectedShape]);
}
*/

#undef LOCTEXT_NAMESPACE