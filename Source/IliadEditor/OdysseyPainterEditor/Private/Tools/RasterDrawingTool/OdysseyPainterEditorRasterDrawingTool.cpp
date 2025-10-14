// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingTool.h"
#include "OdysseyMediaRaster.h"
#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"
#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"
#include "Toolkits/BaseToolkit.h"

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
#include "OdysseyPainterEditorCommands.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationBezier.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationCatmullRom.h"
#include "FreehandShape/Interpolation/OdysseyInterpolationLine.h"
#include "FreehandShape/OdysseyFreehandShapeOverrides.h"
#include "OdysseyPainterEditorRasterSelection.h"
#include "ScopedTransaction.h"
#include "Tools/RasterDrawingTool/OdysseyPainterEditorRasterDrawingToolOverrides.h"

#include "OdysseyHUDElement.h"
#include "SOdysseySinglePropertyView.h"

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
    //Internal
    , mPaintEngine()
    , mBrushContexts(nullptr)
    , mBaseSize(0)
    , mShapeHUD(MakeShared<FOdysseyHUDElement>())
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.DrawingTool64");

    Shapes.AddShapeType(EOdysseyShapeType::kFreehand, CreateShape<UOdysseyFreehandShape>("UOdysseyPainterEditorRasterDrawingTool::FreehandShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kLine, CreateShape<UOdysseyLineShape>("UOdysseyPainterEditorRasterDrawingTool::LineShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kRectangle, CreateShape<UOdysseyRectangleShape>("UOdysseyPainterEditorRasterDrawingTool::RectangleShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kPolygon, CreateShape<UOdysseyPolygonShape>("UOdysseyPainterEditorRasterDrawingTool::PolygonShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kEllipse, CreateShape<UOdysseyEllipseShape>("UOdysseyPainterEditorRasterDrawingTool::EllipseShape"));
    Shapes.AddShapeType(EOdysseyShapeType::kBezier, CreateShape<UOdysseyBezierShape>("UOdysseyPainterEditorRasterDrawingTool::BezierShape"));

    Shapes.SetActiveShapeType(EOdysseyShapeType::kFreehand);
}

template<class T>
T*
UOdysseyPainterEditorRasterDrawingTool::CreateShape(FName iName)
{
    T* shape = CreateDefaultSubobject<T>(iName, true);

    shape->OnBegin().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnShapeBegin);
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
        if( settings->BrushDefaults.DefaultBrush.LoadSynchronous() )
            FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Brush), settings->BrushDefaults.DefaultBrush.LoadSynchronous());
    }

    Super::Activate();
}

void
UOdysseyPainterEditorRasterDrawingTool::Load()
{
    UOdysseyPainterEditorTool::Load();
    FCoreUObjectDelegates::OnObjectsReinstanced.AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnBlueprintReinstanced);

    TSharedPtr<FOdysseyPainterEditorRasterSelection> rasterSelection = GetEditor()->RasterSelection();
    rasterSelection->OnChanged().AddUObject(this, &UOdysseyPainterEditorRasterDrawingTool::OnRasterSelectionChanged);
    if (!rasterSelection->IsEmpty())
        mPaintEngine.SetMaskBlock(rasterSelection->GetBlock());

    mHUD->AddElement(rasterSelection->GetHUD());
    mHUD->AddElement(mShapeHUD);
}

void
UOdysseyPainterEditorRasterDrawingTool::Unload()
{
    Shapes.GetActiveShape()->Abort();

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

TSharedPtr<FOdysseyRasterBlock>
UOdysseyPainterEditorRasterDrawingTool::GetRasterBlockFromEditor(bool iCreate) const
{
    FOdysseyMediaProvider mediaProvider = GetEditor()->GetCurrentMediaProvider();
    if (mediaProvider.IsLocked())
        return nullptr;

    bool hasRaster = mediaProvider.HasMedia<FOdysseyMediaRaster>();
    if (!hasRaster)
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaRaster>> mediaRasters;
    if (iCreate)
        mediaRasters = mediaProvider.GetOrCreateMedias<FOdysseyMediaRaster>();
    else
        mediaRasters = mediaProvider.GetMedias<FOdysseyMediaRaster>();

    if( mediaRasters.Num() <= 0 )
        return nullptr;

    return mediaRasters[0]->GetRasterBlock();
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(true);
    if (!rasterBlock)
        return false;

    mPaintEngine.RasterBlock(rasterBlock);
    if ( BrushInstance )
        BrushInstance->SetBlock(mPaintEngine.PaintBlock());

    mSubPixelPoint = iPointInTexture;
    if (!SubPixel)
    {
        mSubPixelPoint.x = FMath::Floor(mSubPixelPoint.x) + 0.5f;
        mSubPixelPoint.y = FMath::Floor(mSubPixelPoint.y) + 0.5f;
    }

    return Shapes.GetActiveShape()->OnMouseDown(mSubPixelPoint, iKey);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey != EKeys::LeftMouseButton)
        return false;

    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        Shapes.GetActiveShape()->Abort();
        return false;
    }

    mSubPixelPoint = iPointInTexture;
    if (!SubPixel)
    {
        mSubPixelPoint.x = FMath::Floor(mSubPixelPoint.x) + 0.5f;
        mSubPixelPoint.y = FMath::Floor(mSubPixelPoint.y) + 0.5f;
    }

    return Shapes.GetActiveShape()->OnMouseUp(mSubPixelPoint, iKey);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return;

    if (BrushInstance)
        BrushInstance->StrokeMoveTo(iPointInTexture);

    mSubPixelPoint = iPointInTexture;
    if (!SubPixel)
    {
        mSubPixelPoint.x = FMath::Floor(mSubPixelPoint.x) + 0.5f;
        mSubPixelPoint.y = FMath::Floor(mSubPixelPoint.y) + 0.5f;
    }

    Shapes.GetActiveShape()->OnMouseHover(mSubPixelPoint);
}

void
UOdysseyPainterEditorRasterDrawingTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock || rasterBlock != mPaintEngine.GetRasterBlock())
    {
        Shapes.GetActiveShape()->Abort();
        return;
    }

    FOdysseyPoint point = iPointInTexture;
    if (!SubPixel)
    {
        point.x = FMath::Floor(point.x) + 0.5f;
        point.y = FMath::Floor(point.y) + 0.5f;

        if (mSubPixelPoint == point)
            return;
    }

    mSubPixelPoint = point;

    Shapes.GetActiveShape()->OnMouseDrag(point);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnKeyDown(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Shapes.GetActiveShape()->OnKeyDown(iKey);
}

bool
UOdysseyPainterEditorRasterDrawingTool::OnKeyUp(const FKey& iKey)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return false;

    return Shapes.GetActiveShape()->OnKeyUp(iKey);
}

void
UOdysseyPainterEditorRasterDrawingTool::Tick(float iDeltaTime)
{
    TSharedPtr<FOdysseyRasterBlock> rasterBlock = GetRasterBlockFromEditor(false);
    if (!rasterBlock)
        return;

    if( !BrushInstance )
        return;

    mWorker.Push([this, iDeltaTime]()
    {
        //Execute a the brush tick node before executing anything else
        BrushInstance->Tick(iDeltaTime, false);
    });

    //Ticke the shape
    Shapes.GetActiveShape()->Tick(iDeltaTime);

    mWorker.ExecuteFor(1000/60); //60fps

    BrushInstance->StrokeFlush();

    //Update the paintEngine
    mPaintEngine.Update(BlendParameters);
}

void
UOdysseyPainterEditorRasterDrawingTool::Flush()
{
    mWorker.Finish();

    if( BrushInstance )
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

void UOdysseyPainterEditorRasterDrawingTool::BindShortcuts(TSharedPtr<FUICommandList> iCommandList)
{
    Super::BindShortcuts(iCommandList);

    const FOdysseyPainterEditorCommands& painterEditorToolCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) iCommandList->MapAction( action, FExecuteAction::CreateUObject( this, &UOdysseyPainterEditorRasterDrawingTool::__VA_ARGS__ ), FCanExecuteAction() );

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
UOdysseyPainterEditorRasterDrawingTool::ExtendMenu( TSharedRef<FExtender> iExtender )
{
    Super::ExtendMenu(iExtender);
}

void
UOdysseyPainterEditorRasterDrawingTool::ExtendToolbar( UToolMenu* iToolMenu )
{
    Super::ExtendToolbar(iToolMenu);

    FToolMenuSection& section = iToolMenu->AddSection(NAME_None);
    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, BrushOptions, GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Size), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BlendParameters), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
                .OnOverridePropertyHandle_Lambda(
                    [](TSharedPtr<IPropertyHandle> iHandle)
                    {
                        return iHandle->GetChildHandle("Opacity");
                    }
                )
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, BrushOptions, GET_MEMBER_NAME_CHECKED(UOdysseyBrushOptions, Flow), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BlendParameters), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
                .OnOverridePropertyHandle_Lambda(
                    [](TSharedPtr<IPropertyHandle> iHandle)
                    {
                        return iHandle->GetChildHandle("BlendingMode");
                    }
                )
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitWidget(
            NAME_None,
            SNew(SBox)
            .Padding(10.f, 0.f, 10.f, 0.f)
            [
                SNew(SOdysseySinglePropertyView, this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BlendParameters), FSinglePropertyParams())
                .InnerPadding(10.f)
                .ValueWidthOverride(100.f)
                .OnOverridePropertyHandle_Lambda(
                    [](TSharedPtr<IPropertyHandle> iHandle)
                    {
                        return iHandle->GetChildHandle("BlendingMode");
                    }
                )
            ],
            FText()
        )
    );

    section.AddEntry(
        FToolMenuEntry::InitToolBarButton(
            NAME_None,
            FUIAction(
                FExecuteAction::CreateLambda(
                    [this]()
                    {
                        FOdysseyBlendParameters value = BlendParameters;
                        value.bEraserMode = !value.bEraserMode;
                        FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BlendParameters), value);
                    }
                ),
                FCanExecuteAction::CreateLambda([](){ return true;}),
                FIsActionChecked::CreateLambda([this](){ return BlendParameters.bEraserMode;})
            ),
            FText(),
            LOCTEXT("raster-drawing-tool.toolbar.eraser-mode.tooltip", "Toggles the tool Eraser Mode"),
            FSlateIcon("OdysseyStyle", "PainterEditor.TopBar.Eraser32"),
            EUserInterfaceActionType::ToggleButton
        )
    );
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

    UOdysseyPainterEditorRasterDrawingToolOverrides* toolOverrides = Cast<UOdysseyPainterEditorRasterDrawingToolOverrides>(iBrushInstance->EditorOverrides[UOdysseyPainterEditorRasterDrawingToolOverrides::StaticClass()]);
    if (toolOverrides)
    {
        if (toolOverrides->bOverride_Shape)
        {
            Shapes.SetActiveShapeType(toolOverrides->Shape);
        }

        if (toolOverrides->bOverride_SubPixel)
            FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, SubPixel), toolOverrides->SubPixel);
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

    Shapes.GetActiveShape()->ApplyOverrides(iBrushInstance->EditorOverrides);

    mOnApplyOverridesDelegate.Broadcast(iBrushInstance->EditorOverrides);
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- UObject Overrides

void
UOdysseyPainterEditorRasterDrawingTool::BrushChanged()
{
    //Destroy the brushInstance
    DestroyBrushInstance();

    if (!Brush)
        return;

    //Create the BrushInstance to use for drawing
    CreateBrushInstance(true);
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

void UOdysseyPainterEditorRasterDrawingTool::PropertyChanged(const FName& iPropertyName, const FName& iMemberPropertyName, bool iIsInteractive)
{
    Super::PropertyChanged(iPropertyName);

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Brush))
        BrushChanged();
}

void
UOdysseyPainterEditorRasterDrawingTool::PostPropertyChanged(const FName& iPropertyName, bool iIsInteractive)
{
    Super::PostPropertyChanged(iPropertyName, iIsInteractive);

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, BrushInstance))
        return;

    if (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, Brush))
    {
        mOnBrushChanged.Broadcast();
    }

    if ( BrushInstance )
    {
        BrushInstance->ExecuteStateChanged();
    }
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
UOdysseyPainterEditorRasterDrawingTool::OnShapeBegin()
{
    mTransaction = MakeShared<FScopedTransaction>(LOCTEXT("raster-drawing-tool.transaction.paint-stroke", "Paint Stroke"));
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapeInteractive(const TArray<FOdysseyPoint>& iPoints)
{
    if ( !Shapes.GetActiveShape()->IsProgressive() )
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
        //Finish pending work
        Flush();

        //Stop the brush stroke process
        //We don't need it to finish, as we will start over
        BrushInstance->StrokeAbort();

        //Reset the PaintEngine
        mPaintEngine.Abort();

        //Reset the interpolation process
        ResetInterpolation();

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
    mTransaction = nullptr; //Finish the undo transaction

    mIsFirstPoint = true;
    ResetInterpolation();
}

void
UOdysseyPainterEditorRasterDrawingTool::OnShapeAbort()
{
    ResetInterpolation();

    Flush(); //Finish pending work

    BrushInstance->StrokeAbort(); //Stop stroke process
    mIsFirstPoint = true;
    //Reset the PaintEngine as if didn't draw on it
    mPaintEngine.Abort();
    //Update immediately the changes
    mPaintEngine.Update(BlendParameters);

    if(mTransaction)
        mTransaction->Cancel();
    mTransaction = nullptr; //Finish the undo transaction
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- Internal - Interpolation

TArray<FOdysseyPoint>
UOdysseyPainterEditorRasterDrawingTool::InterpolateTo(const FOdysseyPoint& iPoint)
{
    if (!mInterpolator)
    {
        if (Shapes.GetActiveShapeType() == EOdysseyShapeType::kFreehand )
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

void
UOdysseyPainterEditorRasterDrawingTool::SubPixelBlueprintSetter(bool Value)
{
    FOdysseyObjectEditorUtils::SetPropertyValue(this, GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorRasterDrawingTool, SubPixel), Value);
}

#undef LOCTEXT_NAMESPACE
