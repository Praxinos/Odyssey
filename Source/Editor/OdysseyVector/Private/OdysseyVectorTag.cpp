#include "OdysseyVectorTag.h"

FOdysseyVectorTag::~FOdysseyVectorTag()
{
}

FOdysseyVectorTag::FOdysseyVectorTag( FOdysseyVectorObject* iOwnerObject )
    : mOwner( iOwnerObject )
    , mFlags( 0 )
{
}

FOdysseyVectorObject*
FOdysseyVectorTag::GetOwner()
{
    return mOwner;
}

void
FOdysseyVectorTag::SetSelected( bool iSelected )
{
    if( iSelected )
    {
        mFlags |= SELECTED;
    }
    else
    {
        mFlags &= (~SELECTED);
    }
}

bool
FOdysseyVectorTag::IsSelected()
{
    return ( mFlags & SELECTED ) ? true : false;
}
