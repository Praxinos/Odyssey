// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyMediaRaster.h"
#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"
#include "Tools/RasterDrawingTool/Widgets/SOdysseyPainterEditorRasterDrawingToolTopTab.h"

#include "FreehandShape/OdysseyFreehandShape.h"
#include "LineShape/OdysseyLineShape.h"
#include "RectangleShape/OdysseyRectangleShape.h"
#include "PolygonShape/OdysseyPolygonShape.h"
#include "EllipseShape/OdysseyEllipseShape.h"
#include "BezierShape/OdysseyBezierShape.h"

#include "ISinglePropertyView.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "Widgets/Layout/SWrapBox.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyPainterEditorSource.h"
#include "OdysseyMediaProvider.h"
#include "Models/OdysseyPainterEditorCommands.h"

#include "OdysseyHUDElement.h"
#include "OdysseyHUDSystem.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterDrawingTool::~UOdysseyPainterEditorRasterDrawingTool()
{
}

UOdysseyPainterEditorRasterDrawingTool::UOdysseyPainterEditorRasterDrawingTool()
    : Super()
    //Properties
    , Brush(nullptr)
    , BrushInstance(nullptr)
    , BrushOptions(CreateDefaultSubobject<UOdysseyBrushOptions>("UOdysseyPainterEditorRasterDrawingTool::BrushOptions", true))
    , SelectedShape(EOdysseyShape::kFreehand)
    , SelectedShapeInstance(nullptr)
    //Internal
    , mPaintEngine()
    , mBrushContexts(nullptr)
    , mBaseSize(0)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.DrawingTool64");

    AvailableShapes.Add(EOdysseyShape::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterDrawingTool::FreehandShape"));
    AvailableShapes.Add(EOdysseyShape::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterDrawingTool::LineShape"));
    AvailableShapes.Add(EOdysseyShape::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterDrawingTool::RectangleShape"));
    AvailableShapes.Add(EOdysseyShape::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterDrawingTool::PolygonShape"));
    AvailableShapes.Add(EOdysseyShape::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterDrawingTool::EllipseShape"));
    AvailableShapes.Add(EOdysseyShape::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterDrawingTool::BezierShape"));

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

    shape->SetHUD( mHUD );

    return shape;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorRasterDrawingTool::Activate()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditor()->PaintColor()));
    
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
    UOdysseyPainterEditorTool::Load();
    //GEditor->OnBlueprintCompiled().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnBlueprintCompiled);
    FCoreUObjectDelegates::OnObjectsReinstanced.AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnBlueprintReinstanced);

    if (!GetEditor()->EditorMask().IsEmpty())
        mPaintEngine.SetMaskBlock(GetEditor()->EditorMask().GetBlock());

	/* TODO: Done in OnMouseDown(), but check if we need to do something here too or not
    mPaintEngine.RasterBlock(mToolContext->GetRasterBlock());

	if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock()); */
}

void
UOdysseyPainterEditorRasterDrawingTool::Unload()
{
	mPaintEngine.RasterBlock(nullptr);
    mPaintEngine.SetMaskBlock(nullptr);

	if ( BrushInstance )
		BrushInstance->SetBlock(nullptr);

    FCoreUObjectDelegates::OnObjectsReinstanced.RemoveAll(this);
}

bool
UOdysseyPainterEditorRasterDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;
    
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = mediaRasters[0]->GetRasterBlock();
    mPaintEngine.RasterBlock(rasterBlock);
    if ( BrushInstance )
		BrushInstance->SetBlock(mPaintEngine.PaintBlock());
        
    return SelectedShapeInstance->OnMouseDown(iPointInTexture, iKey);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;

    return SelectedShapeInstance->OnMouseUp(iPointInTexture, iKey);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return;

    if (BrushInstance)
        BrushInstance->StrokeMoveTo(iPointInTexture);

    SelectedShapeInstance->OnMouseHover(iPointInTexture);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return;

    SelectedShapeInstance->OnMouseDrag(iPointInTexture);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnKeyDown(const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;

    return SelectedShapeInstance->OnKeyDown(iKey);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnKeyUp(const FKey& iKey)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return false;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return false;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
        return false;

    return SelectedShapeInstance->OnKeyUp(iKey);
}

void
UOdysseyPainterEditorRasterDrawingTool::Tick(float iDeltaTime)
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return;
    
    //ensure we can retrieve a media
    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();
    if( mediaRasters.Num() <= 0 )
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
    FScopedTransaction transaction(LOCTEXT("raster-drawing-tool.transaction.paint-stroke", "Paint Stroke"));
    mPaintEngine.Commit(BlendParameters);
    
    FOdysseyPainterEditor* editor = GetEditor();
    TSharedPtr<FOdysseyPainterEditorSource> source = editor->GetSource();
    if (source)
        source->RecordCurrentFrameUndo();
}

void
UOdysseyPainterEditorRasterDrawingTool::BindShortcuts(FBaseToolkit* iToolkit)
{
    Super::BindShortcuts(iToolkit);

    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorToolCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorRasterDrawingTool::__VA_ARGS__ ), FCanExecuteAction() );

	MAP_ACTION(painterEditorToolCommands.RefreshBrush, RefreshBrushInstance)
    MAP_ACTION(painterEditorToolCommands.IncreaseBrushSize, AddSize, 1)
    MAP_ACTION(painterEditorToolCommands.DecreaseBrushSize, AddSize, -1)
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeNormal, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Normal )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeErase, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Erase )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeTop, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Top )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeBack, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Back )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeSub, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Sub )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeAdd, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Add )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMul, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Mul )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMin, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Min )
    MAP_ACTION(painterEditorToolCommands.SetAlphaModeMax, SetAlphaMode, ::ULIS::eAlphaMode::Alpha_Max )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeNormal, SetBlendMode, ::ULIS::eBlendMode::Blend_Normal )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeTop, SetBlendMode, ::ULIS::eBlendMode::Blend_Top )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBack, SetBlendMode, ::ULIS::eBlendMode::Blend_Back )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBehind, SetBlendMode, ::ULIS::eBlendMode::Blend_Behind )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDissolve, SetBlendMode, ::ULIS::eBlendMode::Blend_Dissolve )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeBayerDither8x8, SetBlendMode, ::ULIS::eBlendMode::Blend_BayerDither8x8 )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDarken, SetBlendMode, ::ULIS::eBlendMode::Blend_Darken )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeMultiply, SetBlendMode, ::ULIS::eBlendMode::Blend_Multiply )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColorBurn, SetBlendMode, ::ULIS::eBlendMode::Blend_ColorBurn )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearBurn, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearBurn )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDarkerColor, SetBlendMode, ::ULIS::eBlendMode::Blend_DarkerColor )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLighten, SetBlendMode, ::ULIS::eBlendMode::Blend_Lighten )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeScreen, SetBlendMode, ::ULIS::eBlendMode::Blend_Screen )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColorDodge, SetBlendMode, ::ULIS::eBlendMode::Blend_ColorDodge )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearDodge, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearDodge )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLighterColor, SetBlendMode, ::ULIS::eBlendMode::Blend_LighterColor )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeOverlay, SetBlendMode, ::ULIS::eBlendMode::Blend_Overlay )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSoftLight, SetBlendMode, ::ULIS::eBlendMode::Blend_SoftLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHardLight, SetBlendMode, ::ULIS::eBlendMode::Blend_HardLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeVividLight, SetBlendMode, ::ULIS::eBlendMode::Blend_VividLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLinearLight, SetBlendMode, ::ULIS::eBlendMode::Blend_LinearLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePinLight, SetBlendMode, ::ULIS::eBlendMode::Blend_PinLight )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHardMix, SetBlendMode, ::ULIS::eBlendMode::Blend_HardMix )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePhoenix, SetBlendMode, ::ULIS::eBlendMode::Blend_Phoenix )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeReflect, SetBlendMode, ::ULIS::eBlendMode::Blend_Reflect )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeGlow, SetBlendMode, ::ULIS::eBlendMode::Blend_Glow )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDifference, SetBlendMode, ::ULIS::eBlendMode::Blend_Difference )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeExclusion, SetBlendMode, ::ULIS::eBlendMode::Blend_Exclusion )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAdd, SetBlendMode, ::ULIS::eBlendMode::Blend_Add )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSubstract, SetBlendMode, ::ULIS::eBlendMode::Blend_Substract )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeDivide, SetBlendMode, ::ULIS::eBlendMode::Blend_Divide )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAverage, SetBlendMode, ::ULIS::eBlendMode::Blend_Average )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeNegation, SetBlendMode, ::ULIS::eBlendMode::Blend_Negation )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeHue, SetBlendMode, ::ULIS::eBlendMode::Blend_Hue )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeSaturation, SetBlendMode, ::ULIS::eBlendMode::Blend_Saturation )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeColor, SetBlendMode, ::ULIS::eBlendMode::Blend_Color )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeLuminosity, SetBlendMode, ::ULIS::eBlendMode::Blend_Luminosity )
    MAP_ACTION(painterEditorToolCommands.SetBlendModePartialDerivative, SetBlendMode, ::ULIS::eBlendMode::Blend_PartialDerivative )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeWhiteOut, SetBlendMode, ::ULIS::eBlendMode::Blend_Whiteout )
    MAP_ACTION(painterEditorToolCommands.SetBlendModeAngleCorrected, SetBlendMode, ::ULIS::eBlendMode::Blend_AngleCorrected )

    #undef MAP_ACTION
}

void
UOdysseyPainterEditorRasterDrawingTool::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    Super::ExtendMenu(iOwner, iMenuName);
}

TSharedRef<SWidget>
UOdysseyPainterEditorRasterDrawingTool::CreateTopTabWidget()
{
    return SNew(SOdysseyPainterEditorRasterDrawingToolTopTab, this);
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

    mHUD->EmptyElements();
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
    BrushInstance->StrokeAbort();

    //Update immediately the changes
    mPaintEngine.Update(BlendParameters);

    mHUD->EmptyElements();
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapePathReset()
{
    if (!BrushInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot draw whithout a brush instance") );
        return;
    }


    //mWorker.Clear();
    mWorker.Finish();
    BrushInstance->StrokeFlush();
    BrushInstance->StrokeAbort();
    
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
UOdysseyPainterEditorRasterDrawingTool::SetBrushContexts(TArray<FOdysseyBrushContext*>* iContexts)
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
UOdysseyPainterEditorRasterDrawingTool::AddSize(int iAmount)
{
    float value = FMath::Max(BrushOptions->Size + iAmount, 0.f);
    FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Size", value);
}

void
UOdysseyPainterEditorRasterDrawingTool::SetAlphaMode(::ULIS::eAlphaMode iAlphaMode)
{
    FOdysseyBlendParameters value = BlendParameters;
    value.AlphaMode = (EOdysseyAlphaMode)iAlphaMode;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "BlendParameters", value);
    
}

void
UOdysseyPainterEditorRasterDrawingTool::SetBlendMode(::ULIS::eBlendMode iBlendMode)
{
    FOdysseyBlendParameters value = BlendParameters;
    value.BlendingMode = (EOdysseyBlendingMode)iBlendMode;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "BlendParameters", value);
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
	//Apply Overrides before setting the brushInstance in the strokeEngine properties
    if (iApplyOverrides)
	    ApplyOverrides(brushInstance);
	
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "BrushInstance", brushInstance);

    ConfigureBrushInstance(brushInstance);

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
UOdysseyPainterEditorRasterDrawingTool::ConfigureBrushInstance(UOdysseyBrushAssetBase* iBrushInstance)
{
    iBrushInstance->SetBrushOptions(BrushOptions); //Share BrushOptions between every selected brushes
    //Set BrushContexts
    iBrushInstance->SetContexts(mBrushContexts); //Set the brush context so that context nodes can be used
    iBrushInstance->SetBlock(mPaintEngine.PaintBlock());
    iBrushInstance->ExecuteSelected();
    iBrushInstance->ExecuteStateChanged();
}

void
UOdysseyPainterEditorRasterDrawingTool::OnBlueprintReinstanced(const FCoreUObjectDelegates::FReplacementObjectMap& iObjectMap)
{
    TArray<UObject*> newObjects;
    iObjectMap.GenerateValueArray(newObjects);

    if (newObjects.Contains(BrushInstance))
    {
        ConfigureBrushInstance(BrushInstance);
    }
}

void
UOdysseyPainterEditorRasterDrawingTool::SelectedShapeChanged()
{
    SelectedShapeInstance->AbortShape();
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

EMouseCursor::Type
UOdysseyPainterEditorRasterDrawingTool::GetMouseCursor() const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return EMouseCursor::SlashedCircle;

    return UOdysseyPainterEditorTool::GetMouseCursor();
}

FText
UOdysseyPainterEditorRasterDrawingTool::GetTooltip() const
{
    return LOCTEXT("raster-drawing-tool.tooltip", "Drawing Tool");
}

#undef LOCTEXT_NAMESPACE
