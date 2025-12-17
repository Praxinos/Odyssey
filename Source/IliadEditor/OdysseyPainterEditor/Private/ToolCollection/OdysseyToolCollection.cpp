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
    if (HasAnyFlags(RF_Transient) || GetOutermost() == GetTransientPackage())
    {
        return true;
    }

    return false;
}

UOdysseyPainterEditorToolConfiguration* UOdysseyToolCollection::AddToolConfiguration(UClass* iToolClass, FToolPropertySnapshot& iSnapshotConfig, FSlateBrush& iIcon, int32 iIndex)
{
    if( !iToolClass )
        return nullptr;

    if( !mToolsConfig.IsValidIndex( iIndex ) )
        iIndex = INDEX_NONE;

    UOdysseyPainterEditorToolConfiguration* toolConfig = NewObject<UOdysseyPainterEditorToolConfiguration>(this);
    toolConfig->mToolClass = iToolClass;
    toolConfig->mSnapshot = iSnapshotConfig;
    toolConfig->mIcon = iIcon;

    Modify();

    if( iIndex == INDEX_NONE )
        mToolsConfig.Add( toolConfig );
    else
        mToolsConfig.Insert( toolConfig, iIndex );

    OnCollectionChanged.Broadcast();

    return toolConfig;
}

void UOdysseyToolCollection::RemoveToolConfigurationAtIndex(int iIndex)
{
    if( iIndex >= mToolsConfig.Num() )
        return;

    Modify();

    mToolsConfig.RemoveAt( iIndex );
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::RemoveToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig)
{
    if( !mToolsConfig.Contains(iToolConfig))
        return;

    Modify();

    mToolsConfig.Remove(iToolConfig);
    OnCollectionChanged.Broadcast();
}

void UOdysseyToolCollection::MoveToolConfiguration(int32 iFromIndex, int32 iToIndex)
{
    if (mToolsConfig.IsValidIndex(iFromIndex) && (mToolsConfig.IsValidIndex(iToIndex) || iToIndex == mToolsConfig.Num()) )
    {
        Modify();

        UOdysseyPainterEditorToolConfiguration* toolConfig = mToolsConfig[iFromIndex];
        mToolsConfig.RemoveAt(iFromIndex);

        // adjust index if removal shifted the array
        if (iFromIndex < iToIndex)
        {
            iToIndex--;
        }

        mToolsConfig.Insert(toolConfig, iToIndex);

        OnCollectionChanged.Broadcast();
    }
}

int32 UOdysseyToolCollection::GetIndexOfToolConfiguration(UOdysseyPainterEditorToolConfiguration* iToolConfig)
{
    int32 index = INDEX_NONE;
    mToolsConfig.Find(iToolConfig, index);
    return index;
}

bool UOdysseyToolCollection::ContainsSimilarToolConfiguration(UClass* iToolClass, FToolPropertySnapshot& iSnapshotConfig)
{
    check( iToolClass );

    for (UOdysseyPainterEditorToolConfiguration* toolConfig : mToolsConfig)
    {
        if( iSnapshotConfig == toolConfig->mSnapshot )
            return true;
    }

    return false;
}

const TArray<UOdysseyPainterEditorToolConfiguration*> UOdysseyToolCollection::GetToolConfigurations() const
{
    return mToolsConfig;
}
