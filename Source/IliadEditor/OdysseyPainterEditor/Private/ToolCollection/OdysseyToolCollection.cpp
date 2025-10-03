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

UOdysseyPainterEditorTool* UOdysseyToolCollection::AddTool(UOdysseyPainterEditorTool* iTool, int32 iIndex)
{
    if( !iTool )
        return nullptr;

    if( !mToolsConfig.IsValidIndex(iIndex) )
        iIndex = INDEX_NONE;

    /*if( ContainsSimilarTool(iTool) ) // We don't add the tool if a similar one is already in the collection
        return;*/

    FName uniqueName = MakeUniqueObjectName(this, UOdysseyPainterEditorTool::StaticClass(), iTool->GetFName());
    UOdysseyPainterEditorTool* duplicate = DuplicateObject<UOdysseyPainterEditorTool>(iTool, this, uniqueName);

    Modify();

    if( iIndex == INDEX_NONE )
        mToolsConfig.Add( duplicate );
    else
        mToolsConfig.Insert( duplicate, iIndex );

    OnCollectionChanged.Broadcast();

    return duplicate;
}

void UOdysseyToolCollection::RemoveToolAtIndex(int iIndex)
{
    if( iIndex >= mToolsConfig.Num() )
        return;

    Modify();

    mToolsConfig.RemoveAt( iIndex );
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::RemoveTool(UOdysseyPainterEditorTool* iTool)
{
    if( !mToolsConfig.Contains(iTool))
        return;

    Modify();

    mToolsConfig.Remove(iTool);
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::MoveTool(int32 iFromIndex, int32 iToIndex)
{
    if (mToolsConfig.IsValidIndex(iFromIndex) && (mToolsConfig.IsValidIndex(iToIndex) || iToIndex == mToolsConfig.Num()) )
    {
        Modify();

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

int32 UOdysseyToolCollection::GetIndexOfTool(UOdysseyPainterEditorTool* iTool)
{
    int32 index = INDEX_NONE;
    mToolsConfig.Find(iTool, index);
    return index;
}

bool UOdysseyToolCollection::ContainsTool(UOdysseyPainterEditorTool* iTool)
{
    return mToolsConfig.Contains( iTool );
}

bool UOdysseyToolCollection::ContainsSimilarTool(UOdysseyPainterEditorTool* iTool)
{
    if (!iTool)
        return false;

    for (UOdysseyPainterEditorTool* tool : mToolsConfig)
    {
        if (iTool->IsSameAs(tool))
            return true;
    }

    return false;
}

const TArray<UOdysseyPainterEditorTool*> UOdysseyToolCollection::GetTools() const
{
    return mToolsConfig;
}
