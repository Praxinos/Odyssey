// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyToolCollection.h"

/////////////////////////////////////////////////////
// OdysseyToolCollection

UOdysseyToolCollection::UOdysseyToolCollection(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UOdysseyToolCollection::IsCollectionTransient() const
{
    if(HasAnyFlags(RF_Transient) || GetOutermost()->HasAnyPackageFlags(PKG_TransientFlags))
    {
        return true;
    }
    return false;
}

void UOdysseyToolCollection::AddTool(UOdysseyPainterEditorTool* iTool)
{
    if( !iTool )
        return;

    if( ContainsSimilarTool(iTool) ) // We don't add the tool if a similar one is already in the collection
        return;

    UOdysseyPainterEditorTool* duplicate = DuplicateObject<UOdysseyPainterEditorTool>(iTool, this);

    mToolsConfig.Add( duplicate );
    OnCollectionChanged.Broadcast();
}

bool UOdysseyToolCollection::ContainsSimilarTool(UOdysseyPainterEditorTool* iTool)
{
    if (!iTool)
        return false;

    for (UOdysseyPainterEditorTool* tool : mToolsConfig)
    {
        if (iTool->IsSameAs(tool)) // We don't add the tool if a similar one is already in the collection
            return true;
    }

    return false;
}

void UOdysseyToolCollection::RemoveToolAtIndex(int iIndex)
{
    if( iIndex >= mToolsConfig.Num() )
        return;

    mToolsConfig.RemoveAt( iIndex );
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::RemoveTool(UOdysseyPainterEditorTool* iTool)
{
    if( mToolsConfig.Contains(iTool))
        mToolsConfig.Remove(iTool);
}

void UOdysseyToolCollection::MoveTool(int32 iFromIndex, int32 iToIndex)
{
    if (mToolsConfig.IsValidIndex(iFromIndex) && mToolsConfig.IsValidIndex(iToIndex))
    {
        UOdysseyPainterEditorTool* tool = mToolsConfig[iFromIndex];
        mToolsConfig.RemoveAt(iFromIndex);

        // adjust index if removal shifted the array
        if (iFromIndex < iToIndex)
        {
            iToIndex--;
        }

        mToolsConfig.Insert(tool, iToIndex);

        OnCollectionChanged.Broadcast();
    }
}

bool UOdysseyToolCollection::ContainsTool(UOdysseyPainterEditorTool* iTool)
{
    return mToolsConfig.Contains( iTool );
}

const TArray<UOdysseyPainterEditorTool*> UOdysseyToolCollection::GetTools() const
{
    return mToolsConfig;
}
