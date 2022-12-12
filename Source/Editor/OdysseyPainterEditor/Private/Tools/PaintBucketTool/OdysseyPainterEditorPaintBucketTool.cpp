// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
UOdysseyPainterEditorPaintBucketTool::~UOdysseyPainterEditorPaintBucketTool()
{
}

UOdysseyPainterEditorPaintBucketTool::UOdysseyPainterEditorPaintBucketTool()
{
    Icon = *FOdysseyStyle::GetBrush( "PainterEditor.ToolsTab.PaintBucket64");
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------- OdysseyPainterEditorTool overrides

void
UOdysseyPainterEditorPaintBucketTool::Activate()
{
	//FOdysseyObjectEditorUtils::SetPropertyValue(BrushOptions, "Color", FOdysseyBrushColor(GetEditorAs<FOdysseyPainterEditor>()->PaintColor()));
}

bool
UOdysseyPainterEditorPaintBucketTool::CanDraw()
{
    return IsActivable();
}

bool
UOdysseyPainterEditorPaintBucketTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (!CanDraw())
        return false;

    if (!mPaintEngine.PaintBlock())
        return false;

	//Do the fill
	::ULIS::FBlock* paintBlock = mPaintEngine.PaintBlock();
	::ULIS::FColor color = GetEditorAs<FOdysseyPainterEditor>()->PaintColor().GetValue();
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
UOdysseyPainterEditorPaintBucketTool::Commit()
{
	mPaintEngine.Commit(FOdysseyBlendParameters());
}
