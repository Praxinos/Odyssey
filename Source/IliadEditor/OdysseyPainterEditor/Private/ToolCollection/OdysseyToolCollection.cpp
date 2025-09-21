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
        UE_LOG(LogTemp, Display, TEXT("IsTransient"));
        return true;
    }
    UE_LOG(LogTemp, Display, TEXT("Is NOT Transient"));
    return false;
}

void UOdysseyToolCollection::AddTool(UOdysseyPainterEditorTool* iTool)
{
    if( !iTool )
        return;

    mToolsConfig.Add( iTool );
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::RemoveToolAtIndex(int iIndex)
{
    if( iIndex >= mToolsConfig.Num() )
        return;

    mToolsConfig.RemoveAt( iIndex );
    OnCollectionChanged.Broadcast();
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

const TArray<UOdysseyPainterEditorTool*>& UOdysseyToolCollection::GetTools() const
{
    return mToolsConfig;
}
