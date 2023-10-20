// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/Cells/OdysseyAnimationCell.h"
#include "LayerStack/Cells/OdysseyAnimationCellExport.h"
#include "LayerStack/Cells/OdysseyAnimationCellImport.h"

#define LOCTEXT_NAMESPACE "FOdysseyAnimationCell"

/* FOdysseyAnimationCell::FOnLengthChanged&
FOdysseyAnimationCell::OnLengthChanged()
{
    static FOnLengthChanged onLengthChanged;
    return onLengthChanged;
} */

FOdysseyAnimationCell::~FOdysseyAnimationCell()
{
}

FOdysseyAnimationCell::FOdysseyAnimationCell(int iLength, UOdysseyAnimationLayer* iLayer)
    : mLength(iLength)
    , mLayer(iLayer)
{
}

UOdysseyAnimationLayer*
FOdysseyAnimationCell::GetLayer() const
{
    return mLayer;
}

int
FOdysseyAnimationCell::GetLength() const
{
    return mLength;
}

void
FOdysseyAnimationCell::PostDuplicate()
{

}

void
FOdysseyAnimationCell::Serialize(FArchive& Ar)
{
    if( Ar.IsSaving() )
    {
        FOdysseyAnimationCellExport::Write( this, Ar );
    }

    if( Ar.IsLoading() )
    {
        if (!FOdysseyAnimationCellImport::Read( this, Ar ))
        {
            //Old Style No Chunk Loading
            Ar << mLength;
        }
    }
}

FOdysseyMediaProvider
FOdysseyAnimationCell::GetMediaProvider(uint32 iFrameIndex) const
{
    return FOdysseyMediaProvider();
}

TSharedPtr<FOdysseyAnimationCell>
FOdysseyAnimationCell::CreateCellFromFrame(uint32 iFrameIndex) const
{
    return nullptr;
}

#undef LOCTEXT_NAMESPACE