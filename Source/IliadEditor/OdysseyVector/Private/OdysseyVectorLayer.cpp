// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorCell.h"

FOdysseyVectorLayer::~FOdysseyVectorLayer()
{
}

FOdysseyVectorLayer::FOdysseyVectorLayer( IOdysseyVectorLayer* iLayerInterface )
    : FOdysseyVectorObject("Layer")
    , mLayerInterface( iLayerInterface )
{
    bIsSystem = true;
}

FOdysseyVectorLayer::FOdysseyVectorLayer()
    : FOdysseyVectorObject("Layer")
    , mLayerInterface( nullptr )
{
    bIsSystem = true;
}

void
FOdysseyVectorLayer::InvalidateCell( FOdysseyVectorCell* iCell )
{
    if( std::find( mInvalidatedCellList.begin()
                 , mInvalidatedCellList.end()
                 , iCell ) == mInvalidatedCellList.end() )
    {
        mInvalidatedCellList.push_back( iCell );
    }

    iCell->SetPendingRedraw( true );
}

void
FOdysseyVectorLayer::InvalidateChild( FOdysseyVectorObject* iChild
                                    , uint64 iChildInvalidationFlags )
{
    FOdysseyVectorObject::InvalidateChild( iChild, iChildInvalidationFlags );
}

FOdysseyVectorCell*
FOdysseyVectorLayer::GetCellByIndex( uint32 iIndex )
{
    return mLayerInterface->GetCellByIndex( iIndex );
}

FOdysseyVectorCell*
FOdysseyVectorLayer::GetLastCell()
{
    return mLayerInterface->GetLastCell();
}

FOdysseyVectorCell*
FOdysseyVectorLayer::GetFirstCell()
{
    return mLayerInterface->GetFirstCell();
}

bool
FOdysseyVectorLayer::Contains( FOdysseyVectorCell* iCell )
{
    return mLayerInterface->Contains( iCell );
}

uint32
FOdysseyVectorLayer::GetWidth()
{
    return mLayerInterface->GetWidth();
}

uint32
FOdysseyVectorLayer::GetHeight()
{
    return mLayerInterface->GetHeight();
}

void
FOdysseyVectorLayer::RequestRedraw( FOdysseyVectorCell *iCell, uint64 iRedrawFlags )
{
    InvalidateCell( iCell );

    RequestRedraw( iRedrawFlags );
}

void
FOdysseyVectorLayer::RequestRedraw( uint64 iRedrawFlags )
{
    // the item will be removed from the list only in non-interactive modes
    mInvalidatedCellList.remove_if( [ iRedrawFlags ] ( FOdysseyVectorCell* cell )
    {
        cell->OnRequestRedrawDelegate().Broadcast( cell->GetScene(), iRedrawFlags );

        return ( iRedrawFlags & FOdysseyVectorCell::REDRAW_INTERACTIVE ) ? false : true;
    } );
}

uint32
FOdysseyVectorLayer::RemoveChild( FOdysseyVectorObject* iChild )
{
    if( iChild->GetClass() == FOdysseyVectorCell::StaticClass() )
    {
        mInvalidatedCellList.remove ( static_cast<FOdysseyVectorCell*>( iChild ) );
    }

    return FOdysseyVectorObject::RemoveChild( iChild );
}

bool
FOdysseyVectorLayer::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorObject::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorLayer::AddSharedObject( FOdysseyVectorObject* iVectorObject )
{
    mSharedObjectList.push_back( iVectorObject );
}

void
FOdysseyVectorLayer::RemoveSharedObject( FOdysseyVectorObject* iVectorObject )
{
    mSharedObjectList.remove( iVectorObject );
}

void
FOdysseyVectorLayer::AddSharedTag( FOdysseyVectorTag* iVectorTag )
{
    // Because the Proxy can run this function at anytime, we must alos protect the access
    // to the list of shared tags
    mSharedTagMutex.lock();

    mSharedTagList.push_back( iVectorTag );

    mSharedTagMutex.unlock();
}

void
FOdysseyVectorLayer::RemoveSharedTag( FOdysseyVectorTag* iVectorTag )
{
    // Because the Proxy can run this function at anytime, we must alos protect the access
    // to the list of shared tags
    mSharedTagMutex.lock();

    mSharedTagList.remove( iVectorTag );

    mSharedTagMutex.unlock();
}

std::list<FOdysseyVectorTag*>&
FOdysseyVectorLayer::GetSharedTagList()
{
    return mSharedTagList;
}

const std::list<FOdysseyVectorTag*>&
FOdysseyVectorLayer::GetSharedTagList() const
{
    return mSharedTagList;
}

std::mutex&
FOdysseyVectorLayer::GetSharedTagMutex()
{
    return mSharedTagMutex;
}

FOdysseyVectorTag*
FOdysseyVectorLayer::GetSelectedTagByClassType( uint32 iClassType )
{
    for( FOdysseyVectorTag* tag : mSharedTagList )
    {
        if( tag->GetClass() == iClassType )
        {
            if( tag->GetOwner()->IsSelected() )
            {
                return tag;
            }
        }
    }

    return nullptr;
}

void
FOdysseyVectorLayer::GetSelectedTagByClassType( uint32 iClassType
                                                  , std::list<FOdysseyVectorTag*>& oSelectedTagList )
{
    for( FOdysseyVectorTag* tag : mSharedTagList )
    {
        if( tag->GetClass() == iClassType )
        {
            if( tag->GetOwner()->IsSelected() )
            {
                oSelectedTagList.push_back(tag);
            }
        }
    }
}

bool
FOdysseyVectorLayer::HasSharedTag( FOdysseyVectorTag* iTag )
{
    return ( std::find( mSharedTagList.begin(), mSharedTagList.end(), iTag ) == mSharedTagList.end() ) ? false : true;
}
