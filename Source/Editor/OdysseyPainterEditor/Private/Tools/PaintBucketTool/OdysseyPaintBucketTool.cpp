// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPaintBucketTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPaintBucketTool::~UOdysseyPaintBucketTool()
{
}

UOdysseyPaintBucketTool::UOdysseyPaintBucketTool()
    : Super()
    //Internal
    , mPaintEngine(nullptr)
    , mIsDrawingLocked(false)
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------- Tool

void
UOdysseyPaintBucketTool::Initialize(FOdysseyPaintEngine* iPaintEngine)
{
    SetPaintEngine(iPaintEngine);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyTool overrides

void
UOdysseyPaintBucketTool::Activate()
{

}

void
UOdysseyPaintBucketTool::Inactivate()
{
    Flush(); //Finish everything
    Commit(); //Commit the jobs that has been done
}

bool
UOdysseyPaintBucketTool::CanDraw()
{
    return !mIsDrawingLocked;
}

bool
UOdysseyPaintBucketTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!CanDraw())
        return false;

    //TODO: FLush and commit courrent tool before filling
    if (!mPaintEngine || !mPaintEngine->PaintBlock())
        return false;

	//Do the fill
	::ULIS::FBlock* paintBlock = mPaintEngine->PaintBlock();
	::ULIS::FColor color = ::ULIS::FColor::Red; //TODO: should be the selected drawing color
	::ULIS::FRectI rect = paintBlock->Rect();
	::ULIS::eFormat format = paintBlock->Format();

	::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(format);
	ctx.Fill(*paintBlock, color);
	ctx.Finish();
	paintBlock->Dirty();

    Commit();

    return true;
}

void
UOdysseyPaintBucketTool::Commit()
{
    if (mPaintEngine)
        mPaintEngine->Commit(FOdysseyBlendParameters());
}

void
UOdysseyPaintBucketTool::BindShortcuts(FBaseToolkit* iToolkit)
{
    Super::BindShortcuts(iToolkit);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
UOdysseyPaintBucketTool::SetPaintEngine(FOdysseyPaintEngine* iPaintEngine)
{
    mPaintEngine = iPaintEngine;
}

// Set wether the tool can draw or not
void
UOdysseyPaintBucketTool::IsDrawingLocked(bool iValue)
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

bool
UOdysseyPaintBucketTool::IsDrawingLocked()
{
    return mIsDrawingLocked;
}
