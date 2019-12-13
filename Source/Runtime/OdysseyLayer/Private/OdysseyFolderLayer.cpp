// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFolderLayer.h"

#define LOCTEXT_NAMESPACE "OdysseyFolderLayer"

//---

FOdysseyFolderLayer::~FOdysseyFolderLayer()
{
    for( int i = 0; i < mLayersInFolder.Num(); i++ )
    {
        mLayersInFolder[i].Reset();
    }
}

FOdysseyFolderLayer::FOdysseyFolderLayer( const FName& iName )
    : IOdysseyLayer( iName )
    , mLayersInFolder()
    , mBlendingMode( ::ULIS::eBlendingMode::kNormal )
{
}

//---

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

//---

TSharedPtr<FOdysseyBlock>
FOdysseyFolderLayer::GenerateBlockFromContent() const
{
    return nullptr;
}

//---

void
FOdysseyFolderLayer::AppendLayer( TSharedPtr<IOdysseyLayer> iLayer )
{
    if( !iLayer.IsValid() )
        return;

    mLayersInFolder.Add( iLayer );
}

void
FOdysseyFolderLayer::AddLayerAtIndex( TSharedPtr<IOdysseyLayer> iLayer, int iIndex )
{
    if( !iLayer.IsValid() )
        return;

    if( iIndex < mLayersInFolder.Num() )
        mLayersInFolder.Insert( iLayer, iIndex );

    //TODO: there is no way to know if it's really added outside this function ?!
}

//---

#undef LOCTEXT_NAMESPACE
