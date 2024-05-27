// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterSelectionTool/OdysseyPainterEditorRasterSelectionTool.h"
#include "GeomTools.h"
#include "OdysseyHUDPolygon.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyHUDSystem.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorRasterSelectionTool::~UOdysseyPainterEditorRasterSelectionTool()
{
    //We give back what we don't own
    mEditor = nullptr;
    mHUD = nullptr;
    //--

    if (mSelectionBlock)
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }
}

UOdysseyPainterEditorRasterSelectionTool::UOdysseyPainterEditorRasterSelectionTool():
    mCurrentSelection( nullptr ),
    mIsSelectionAreaSet(false),
    mToolSelectionArea(nullptr),
    mPaintEngine(),
    mSelectionBlock(nullptr)
{
    Icon = *FOdysseyStyle::GetBrush("PainterEditor.ToolsTab.Lasso32");
}

void UOdysseyPainterEditorRasterSelectionTool::Init(TSharedPtr<FOdysseyHUDElement> iHUD, FOdysseyPainterEditor* iEditor, bool iUniform)
{
    mHUD = iHUD;
    mEditor = iEditor;
    Uniform = iUniform;
}

bool UOdysseyPainterEditorRasterSelectionTool::IsActivable() const
{
    return true;
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if( mCurrentSelection )
    {
        delete mCurrentSelection;
        mCurrentSelection = nullptr;
    }

    TArray<FVector2D> areaPoints;
    FOdysseyHUDPolygon* selection = new FOdysseyHUDPolygon(FName("CurrentSelection"), areaPoints);
    mHUD->AddElement(selection);

    switch (SelectionShape)
    {
    case EOdysseySelectionShape::Rectangle:
        mCurrentSelection = new FOdysseyPainterEditorRasterRectangleSelection(selection->GetPoints());
        break;
    case EOdysseySelectionShape::Freehand:
        mCurrentSelection = new FOdysseyPainterEditorRasterFreehandSelection(selection->GetPoints());
        break;
    case EOdysseySelectionShape::Ellipse:
        mCurrentSelection = new FOdysseyPainterEditorRasterEllipseSelection(selection->GetPoints());
        break;
    default:
        break;
    }

    if( mCurrentSelection->OnMouseDown( iPointInTexture, iKey ) )
    {
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
}

void UOdysseyPainterEditorRasterSelectionTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    mCurrentSelection->OnMouseDrag(iPointInTexture);
}

bool UOdysseyPainterEditorRasterSelectionTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    //Merge/replace/substract HUD of selection to already existing selection if it exists
    mCurrentSelection->OnMouseUp( iPointInTexture, iKey );

    mHUD->RemoveElementByKey( "RasterSelection" );
    mHUD->ChangeElementKeyTo( "CurrentSelection", "RasterSelection" );

    mEditor->HUDSystem()->ClearHUDSurface();

    if (mCurrentSelection)
    {
        delete mCurrentSelection;
        mCurrentSelection = nullptr;
    }
   
    return true;
}

bool UOdysseyPainterEditorRasterSelectionTool::OnKeyDown(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    return false;
}

bool UOdysseyPainterEditorRasterSelectionTool::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
        return true;
    }
    else if (iKey == EKeys::Enter || iKey == EKeys::SpaceBar || iKey == EKeys::Escape)
    {
        ClearSelection();
        return true;
    }

    return false;
}

void UOdysseyPainterEditorRasterSelectionTool::Load()
{
}

void UOdysseyPainterEditorRasterSelectionTool::Unload()
{
    ClearSelection();
}

bool UOdysseyPainterEditorRasterSelectionTool::IsSelectionAreaSet()
{
    return mIsSelectionAreaSet;
}

bool UOdysseyPainterEditorRasterSelectionTool::IsInSelectionArea(FVector2D iPoint)
{
    if( !mToolSelectionArea || mToolSelectionArea->GetPoints().Num() == 0 )
        return false;

    return FGeomTools2D::IsPointInPolygon(iPoint, mToolSelectionArea->GetPoints()); 
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> UOdysseyPainterEditorRasterSelectionTool::GetSelectionBlock()
{
    return mSelectionBlock;
}

::ULIS::FRectI UOdysseyPainterEditorRasterSelectionTool::GetSelectionAreaBoundingRect()
{
    if (mToolSelectionArea && mToolSelectionArea->GetPoints().Num() != 0)
    {
        int minX = mToolSelectionArea->GetPoints()[0].X;
        int maxX = mToolSelectionArea->GetPoints()[0].X;
        int minY = mToolSelectionArea->GetPoints()[0].Y;
        int maxY = mToolSelectionArea->GetPoints()[0].Y;
        for (int i = 1; i < mToolSelectionArea->GetPoints().Num(); i++)
        {
            minX = FMath::Min(minX, mToolSelectionArea->GetPoints()[i].X);
            maxX = FMath::Max(maxX, mToolSelectionArea->GetPoints()[i].X);
            minY = FMath::Min(minY, mToolSelectionArea->GetPoints()[i].Y);
            maxY = FMath::Max(maxY, mToolSelectionArea->GetPoints()[i].Y);
        }
        return ::ULIS::FRectI::FromMinMax(minX, minY, maxX, maxY);
    }
    return ::ULIS::FRectI::FromXYWH(0, 0, 0, 0);
}

bool UOdysseyPainterEditorRasterSelectionTool::IsSelectionValid(::ULIS::FRectI iSelectionArea)
{
    if (iSelectionArea.w > 8192 || iSelectionArea.h > 8192) //Unreal limitations + very slow in ULIS at these sizes
        return false;

    return true;
}

void UOdysseyPainterEditorRasterSelectionTool::ClearSelection()
{
    mHUD->EmptyHUDElements();
    mIsSelectionAreaSet = false;
    if( mSelectionBlock )
    {
        mSelectionBlock.Reset();
        mSelectionBlock = nullptr;
    }
    mToolSelectionArea = nullptr;
    mEditor->HUDSystem()->ClearHUDSurface();
}

void UOdysseyPainterEditorRasterSelectionTool::ClearBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    if (!iBlock)
        return;

    ::ULIS::eFormat format = iBlock->Format();
    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
    ::ULIS::FEvent clearEvent;

    ctx.Clear(
        *iBlock,
        iBlock->Rect(),
        ::ULIS::FSchedulePolicy::AsyncCacheEfficient,
        0,
        nullptr,
        &clearEvent);

    ctx.Finish();
}
