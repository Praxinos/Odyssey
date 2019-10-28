// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyFolderLayer.h"

#define LOCTEXT_NAMESPACE "OdysseyFolderLayer"

FOdysseyFolderLayer::FOdysseyFolderLayer(FName InName)
{
    mName = InName;

    mIsLocked = false;
    mIsVisible = true;

    mBlendingMode = ::ULIS::eBlendingMode::kNormal;

    mLayersInFolder = TArray<TSharedPtr<IOdysseyLayer>>();
}

FOdysseyFolderLayer::~FOdysseyFolderLayer()
{
    for( int i = 0; i < mLayersInFolder.Num(); i++)
    {
        mLayersInFolder[i].Reset();
    }
}

IOdysseyLayer::eType
FOdysseyFolderLayer::GetType() const
{
    return eType::kFolder;
}

::ULIS::eBlendingMode
FOdysseyFolderLayer::GetBlendingMode()
{
    return mBlendingMode;
}

void
FOdysseyFolderLayer::SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode )
{
    mBlendingMode = iBlendingMode;
}

TSharedPtr<FOdysseyBlock>
FOdysseyFolderLayer::GenerateBlockFromContent() const
{
    return nullptr;
}

void
FOdysseyFolderLayer::AppendLayer( TSharedPtr<IOdysseyLayer> iLayer )
{
    if( iLayer.IsValid() )
        mLayersInFolder.Add( iLayer );
}

void
FOdysseyFolderLayer::AddLayerAtIndex( TSharedPtr<IOdysseyLayer> InLayer, int InIndex )
{
    if( InLayer.IsValid() )
        if( InIndex < mLayersInFolder.Num() )
            mLayersInFolder.Insert(InLayer, InIndex);
}


#undef LOCTEXT_NAMESPACE
