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
#include "FreehandShape/Interpolation/OdysseyInterpolationLine.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"
#include "PainterEditor/OdysseyPainterEditorRasterSelection.h"

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
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
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

    shape->OnInteractive().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapeInteractive);
    shape->OnCommit().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapeCommit);
    shape->OnAbort().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapeAbort);

    shape->SetHUD( mShapeHUD );

    return shape;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorRasterDrawingTool::Activate()
{
	FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Color), FOdysseyBrushColor(GetEditor()->PaintColor()));
    
    //Set Default Brush
    if(!Brush)
    {
        UOdysseyPainterEditorSettings* settings = UOdysseyPainterEditorSettings::Get();
        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Brush), settings->BrushDefaults.DefaultBrush.LoadSynchronous());
    }

    Super::Activate();
}

void
UOdysseyPainterEditorRasterDrawingTool::Load()
{
    UOdysseyPainterEditorTool::Load();
    //GEditor->OnBlueprintCompiled().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnBlueprintCompiled);
    FCoreUObjectDelegates::OnObjectsReinstanced.AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnBlueprintReinstanced);

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnRasterSelectionChanged);
    if (!rasterSelection->IsEmpty())
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());

    mHUD->AddElement(rasterSelection->GetHUD());
    mHUD->AddElement(mShapeHUD);

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
    
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().RemoveAll(this);
    mHUD->RemoveElement(rasterSelection->GetHUD());
    mHUD->RemoveElement(mShapeHUD);

	if ( BrushInstance )
		BrushInstance->SetBlock(nullptr);

    FCoreUObjectDelegates::OnObjectsReinstanced.RemoveAll(this);

    UOdysseyPainterEditorTool::Unload();
}

bool
UOdysseyPainterEditorRasterDrawingTool::IsActivable() const
{
    return GetEditor()->GetCurrentMediaProvider().HasMedia<FOdysseyMediaRaster>();
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (UOdysseyPainterEditorRasterBaseTool::OnMouseDown(iPointInTexture, iKey))
        return true;

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
    if (UOdysseyPainterEditorRasterBaseTool::OnMouseUp(iPointInTexture, iKey))
        return true;

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
UOdysseyPainterEditorRasterDrawingTool::BeginStroke( const FOdysseyPoint& iPoint )
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
UOdysseyPainterEditorRasterDrawingTool::StrokeTo( const TArray<FOdysseyPoint>& iPoints )
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
UOdysseyPainterEditorRasterDrawingTool::StrokeEnd()
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
UOdysseyPainterEditorRasterDrawingTool::StrokeAbort()
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
}

void
UOdysseyPainterEditorRasterDrawingTool::StrokeReset()
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
    mOnBrushChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterDrawingTool::AddSize(int iAmount)
{
    float value = FMath::Max(BrushOptions->Size + iAmount, 0.f);
    FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Size), value);
}

void
UOdysseyPainterEditorRasterDrawingTool::SetAlphaMode(::ULIS::eAlphaMode iAlphaMode)
{
    FOdysseyBlendParameters value = BlendParameters;
    value.AlphaMode = (EOdysseyAlphaMode)iAlphaMode;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BlendParameters), value);
    
}

void
UOdysseyPainterEditorRasterDrawingTool::SetBlendMode(::ULIS::eBlendMode iBlendMode)
{
    FOdysseyBlendParameters value = BlendParameters;
    value.BlendingMode = (EOdysseyBlendingMode)iBlendMode;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BlendParameters), value);
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

    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BrushInstance), nullptr);
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
	
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BrushInstance), brushInstance);

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
        if (blendParametersOverrides->bOverride_Opacity)
            blendParameters.Opacity = blendParametersOverrides->Opacity;

        if (blendParametersOverrides->bOverride_BlendingMode)
            blendParameters.BlendingMode = blendParametersOverrides->BlendingMode;
        
        if (blendParametersOverrides->bOverride_AlphaMode)
            blendParameters.AlphaMode = blendParametersOverrides->AlphaMode;

        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BlendParameters), blendParameters);
    }

    UOdysseyBrushOptionsOverrides* brushOptionsOverrides = Cast<UOdysseyBrushOptionsOverrides>(iBrushInstance->EditorOverrides[UOdysseyBrushOptionsOverrides::StaticClass()]);
    if (brushOptionsOverrides)
    {
        if (brushOptionsOverrides->bOverride_Size)
            FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Size), brushOptionsOverrides->Size);
        if (brushOptionsOverrides->bOverride_Flow)
            FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Flow), brushOptionsOverrides->Flow);
    }

    //PATCH: Use Step / AdaptativeStep / InterpolationType from FreehandShapeOverrides
    //But we should have a RasterDrawingToolOverrides class
    const UOdysseyFreehandShapeOverrides* freehandShapeOverrides = Cast<const UOdysseyFreehandShapeOverrides>(iBrushInstance->EditorOverrides[UOdysseyFreehandShapeOverrides::StaticClass()]);
    if (freehandShapeOverrides)
    {
        if (freehandShapeOverrides->bOverride_Step)
            FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Step), freehandShapeOverrides->Step);
        if (freehandShapeOverrides->bOverride_AdaptativeStep)
            FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, AdaptativeStep), freehandShapeOverrides->AdaptativeStep);
        if (freehandShapeOverrides->bOverride_InterpolationType)
            FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, InterpolationType), freehandShapeOverrides->InterpolationType);
    }

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
    SelectedShapeInstance->Abort();
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, SelectedShapeInstance), AvailableShapes[SelectedShape]);
    mOnShapeChanged.Broadcast();
}

void
UOdysseyPainterEditorRasterDrawingTool::PropertyChanged(const FName& iPropertyName)
{
    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Brush))
        BrushChanged();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BrushInstance))
        return;
        //BrushInstanceChanged();

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, SelectedShape))
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

void
UOdysseyPainterEditorRasterDrawingTool::OnRasterSelectionChanged()
{
    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    if (rasterSelection->IsEmpty())
    {
        mPaintEngine.SetMaskBlock(nullptr);
    }
    else
    {
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());
    }
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapeInteractive(const TArray<FOdysseyPoint>& iPoints)
{
    if ( !SelectedShapeInstance->IsProgressive() )
        return;

    for (const FOdysseyPoint& point : iPoints)
    {
        TArray<FOdysseyPoint> interpolatedPoints = InterpolateTo(point);

        if (interpolatedPoints.IsEmpty())
            continue;

        if (mIsFirstPoint)
        {
            BeginStroke( interpolatedPoints[0] );
            interpolatedPoints.RemoveAt(0);
            mIsFirstPoint = false;
        }

        if (interpolatedPoints.IsEmpty())
            continue;

        StrokeTo(interpolatedPoints);
    }
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapeCommit(const TArray<FOdysseyPoint>& iPoints, bool iReset)
{
    if (iReset)
    {
        ResetInterpolation();
        StrokeReset();
        mIsFirstPoint = true;

        for (const FOdysseyPoint& point : iPoints)
        {
            TArray<FOdysseyPoint> interpolatedPoints = InterpolateTo(point);

            if ( interpolatedPoints.IsEmpty() )
                continue;

            if ( mIsFirstPoint )
            {
                BeginStroke(interpolatedPoints[0]);
                interpolatedPoints.RemoveAt(0);
                mIsFirstPoint = false;
            }

            if ( interpolatedPoints.IsEmpty() )
                continue;

            StrokeTo(interpolatedPoints);
        }
    }

    StrokeEnd();

    Flush();
    Commit();

    mIsFirstPoint = true;
    ResetInterpolation();
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapeAbort()
{
    ResetInterpolation();
    StrokeAbort();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - Interpolation

TArray<FOdysseyPoint>
UOdysseyPainterEditorRasterDrawingTool::InterpolateTo(const FOdysseyPoint& iPoint)
{
    if (!mInterpolator)
    {
        if (SelectedShape == EOdysseyShape::kFreehand )
        {
            switch(InterpolationType)
            {
                case EOdysseyInterpolationType::kCatmullRom: mInterpolator = MakeShared<FOdysseyInterpolationCatmullRom>(); break;
                case EOdysseyInterpolationType::kBezier: mInterpolator = MakeShared<FOdysseyInterpolationBezier>(); break;
                case EOdysseyInterpolationType::kLine: mInterpolator = MakeShared<FOdysseyInterpolationLine>(); break;

                default: break;
            }
        }
        else
        {
            mInterpolator = MakeShared<FOdysseyInterpolationLine>();
        }

        if (AdaptativeStep)
        {
            mInterpolator->SetStep( FMath::Max( 1.f, AdaptShapeStep(Step) ) );
        }
        else
        {
            mInterpolator->SetStep(Step);
        }
        
        mInterpolator->AddPoint( iPoint );
        mLastPoint = iPoint;
        return { iPoint };
    }

    //If the Interpolator is ready to produce points do it, otherwise.... don't (Thanks Captain Obvious)
    while( !mInterpolator->IsReady() )
    {
        //Add Point to Interpolator
        mInterpolator->AddPoint( iPoint );
    }

    TArray<FOdysseyPoint> newPoints = mInterpolator->ComputePoints();

    for( int i = 0; i < newPoints.Num(); ++i )
    {
        newPoints[i].ComputeRelativeParameters(mLastPoint, true);
        mLastPoint = newPoints[i];
    }

    return newPoints;
}

void
UOdysseyPainterEditorRasterDrawingTool::ResetInterpolation()
{
    mInterpolator = nullptr;
    mLastPoint = FOdysseyPoint();
}

#undef LOCTEXT_NAMESPACE
