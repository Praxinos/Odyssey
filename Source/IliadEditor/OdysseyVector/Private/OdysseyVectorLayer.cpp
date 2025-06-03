// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyVectorLayer.h"
#include "OdysseyVectorTag.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorCell.h"
#include "HUD/OdysseyVectorHUD.h"

FOdysseyVectorLayer::~FOdysseyVectorLayer()
{
}

FOdysseyVectorLayer::FOdysseyVectorLayer( IOdysseyVectorLayer* iLayerInterface )
    : FOdysseyVectorObject("Layer")
    , mLayerInterface( iLayerInterface )
{
    bSystem = true;
}

FOdysseyVectorLayer::FOdysseyVectorLayer()
    : FOdysseyVectorObject("Layer")
    , mLayerInterface( nullptr )
{
    bSystem = true;
}

std::list<FOdysseyVectorCell*>&
FOdysseyVectorLayer::GetInvalidateCellList()
{
    return mInvalidatedCellList;
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

FOdysseyVectorLayer::FNotifyDelegate&
FOdysseyVectorLayer::OnNotifyDelegate()
{
    return mOnNotifyDelegate;
}

std::list<IOdysseyVectorHUD*>&
FOdysseyVectorLayer::GetHUDList()
{
    return mHUDList;
}

void
FOdysseyVectorLayer::AddHUD( IOdysseyVectorHUD* iHUDObject )
{
    LockDrawing();

    GetHUDList().push_back( iHUDObject );

    UnlockDrawing();
}

void
FOdysseyVectorLayer::RemoveHUD( IOdysseyVectorHUD* iHUDObject )
{
    LockDrawing();

    GetHUDList().remove( iHUDObject );

    UnlockDrawing();
}

void
FOdysseyVectorLayer::ClearHUD()
{
    LockDrawing();

    GetHUDList().clear();

    UnlockDrawing();
}

void
FOdysseyVectorLayer::ResetHUD( FOdysseyVectorGroupPaint* iScene )
{
    LockDrawing();

    for( IOdysseyVectorHUD *hud : GetHUDList() )
    {
        hud->SetScene( iScene );
        hud->Reset();
    }

    UnlockDrawing();
}

// static
void
FOdysseyVectorLayer::Notify( uint64 iNotifyFlags )
{
    OnNotifyDelegate().Broadcast( this, iNotifyFlags );
}

void
FOdysseyVectorLayer::RequestRedraw( FOdysseyVectorCell *iCell, uint64 iRedrawFlags )
{
    if( iCell )
    {
        InvalidateCell( iCell );
    }

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
