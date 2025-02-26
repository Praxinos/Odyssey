// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyBlockClipboardData.h"

FOdysseyBlockClipboardData::FOdysseyBlockClipboardData()
    : IOdysseyClipboardData(StaticId())
{

}

FOdysseyBlockClipboardData::FOdysseyBlockClipboardData( TSharedPtr<::ULIS::FBlock> iBlock )
    : IOdysseyClipboardData(StaticId())
{
    Copy(iBlock);
}

const FGuid&
FOdysseyBlockClipboardData::StaticId()
{
    static FGuid id = FGuid::NewGuid();
    return id;
}

TSharedPtr<::ULIS::FBlock> FOdysseyBlockClipboardData::GetBlock() const
{
    return mBlockCopy;
}

void
FOdysseyBlockClipboardData::Copy( TSharedPtr<::ULIS::FBlock> iBlock )
{
    mBlockCopy = iBlock;
}


void FOdysseyBlockClipboardData::AddReferencedObjects(FReferenceCollector& Collector)
{

}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

FString
FOdysseyBlockClipboardData::GetReferencerName() const
{
    return "FOdysseyBlockClipboardData";
}
