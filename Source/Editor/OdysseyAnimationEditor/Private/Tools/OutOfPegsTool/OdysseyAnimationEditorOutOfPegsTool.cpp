// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"

#include "UObject/OdysseyObjectEditorUtils.h"

UOdysseyAnimationEditorOutOfPegsTool::~UOdysseyAnimationEditorOutOfPegsTool()
{

}

UOdysseyAnimationEditorOutOfPegsTool::UOdysseyAnimationEditorOutOfPegsTool()
{

}

bool
UOdysseyAnimationEditorOutOfPegsTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return true;
}

bool
UOdysseyAnimationEditorOutOfPegsTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return true;
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyAnimationEditorOutOfPegsTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

EMouseCursor::Type
UOdysseyAnimationEditorOutOfPegsTool::GetMouseCursor() const
{
    return UOdysseyPainterEditorTool::GetMouseCursor();
}

TSharedRef<SWidget>
UOdysseyAnimationEditorOutOfPegsTool::CreateTopTabWidget()
{
    return SNullWidget::NullWidget;
}

void
UOdysseyAnimationEditorOutOfPegsTool::PostEditChangeProperty( FPropertyChangedEvent& iEvent)
{
    Super::PostEditChangeProperty(iEvent);
    
    FName propertyName = iEvent.GetPropertyName();
    FName memberPropertyName = iEvent.MemberProperty->GetFName();
    
    if (   memberPropertyName == TEXT("Pan")
        || propertyName == TEXT("Rotation")
        || propertyName == TEXT("Zoom") )
    {
        mCell->SetOutOfPegs(Pan, Rotation, Zoom, iEvent.ChangeType & EPropertyChangeType::Interactive);
    }
}

TSharedPtr<FOdysseyAnimationCell>
UOdysseyAnimationEditorOutOfPegsTool::GetCell() const
{
    return mCell;
}

void
UOdysseyAnimationEditorOutOfPegsTool::SetCell(TSharedPtr<FOdysseyAnimationCell> iCell)
{
    if (iCell == mCell)
        return;

    if (mCell)
    {
        mCell->OnOutOfPegsChanged().RemoveAll(this);
    }

    mCell = iCell;

    if (!mCell)
        return;

    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Pan", mCell->OutOfPegsPan());
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Rotation", mCell->OutOfPegsRotation());
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegsZoom());

    mCell->OnOutOfPegsChanged().AddUObject(this, &UOdysseyAnimationEditorOutOfPegsTool::OnCellOutOfPegsChanged);
}

void
UOdysseyAnimationEditorOutOfPegsTool::OnCellOutOfPegsChanged(bool iIsInteractive)
{
    EPropertyChangeType::Type changeType = iIsInteractive ? EPropertyChangeType::Interactive : EPropertyChangeType::Unspecified;
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Pan", mCell->OutOfPegsPan(), changeType);
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Rotation", mCell->OutOfPegsRotation(), changeType);
    FOdysseyObjectEditorUtils::SetPropertyValue(this, "Zoom", mCell->OutOfPegsZoom(), changeType);
}