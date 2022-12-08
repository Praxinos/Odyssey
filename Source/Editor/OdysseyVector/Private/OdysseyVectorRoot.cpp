#include "OdysseyVectorRoot.h"

void UOdysseyVectorRoot::Init( std::string iName )
{
    SetName( iName );
}

void
UOdysseyVectorRoot::ClearSelection()
{
    for( std::list<UOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);

        obj->SetIsSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
UOdysseyVectorRoot::Select( UOdysseyVectorObject& iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), &iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj.SetIsSelected( true );

        mSelectedObjectList.push_back( &iVecObj );
    }
}

UOdysseyVectorObject*
UOdysseyVectorRoot::CopyShape()
{
    UOdysseyVectorRoot* rootCopy = NewObject<UOdysseyVectorRoot>();

    rootCopy->Init( mName );

    return Cast<UOdysseyVectorObject>(rootCopy);
}

void
UOdysseyVectorRoot::Select( double iX, double iY, double iRadius )
{
    UOdysseyVectorObject* pickedObject = RecursiveSelect( *this, iX, iY, iRadius );

    if ( pickedObject )
    {
        if( pickedObject->GetClass() == UOdysseyVectorLoop::StaticClass() )
        {
            pickedObject = pickedObject->GetParent();
        }

        Select ( *pickedObject );
    }
}

UOdysseyVectorGroup*
UOdysseyVectorRoot::GroupSelectdObjects( )
{
    UOdysseyVectorGroup* group = NewObject<UOdysseyVectorGroup>();
    BLPoint averageTranslation = { 0.0f, 0.0f };
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    // We don't use the mSelectedObjectList because we want to keep the same order
    // and we work on a copy to be able to delete the objects while iterating
    std::list<UOdysseyVectorObject*> objectList = mChildrenList;

    if ( mSelectedObjectList.size() )
    {
        for( std::list<UOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            UOdysseyVectorObject *obj = (*it);
            BLPoint origin = obj->GetWorldMatrix().mapPoint( 0.0f, 0.0f );

            averageTranslation.x += origin.x;
            averageTranslation.y += origin.y;
        }

        averageTranslation.x /= mSelectedObjectList.size();
        averageTranslation.y /= mSelectedObjectList.size();

        averageTranslation = this->GetInverseWorldMatrix().mapPoint ( averageTranslation.x, averageTranslation.y );
    }

    AppendChild ( group );

    group->Translate( averageTranslation.x, averageTranslation.y );
    group->UpdateMatrix();

    while( objectList.size () )
    {
        UOdysseyVectorObject *obj = objectList.back();

        if ( obj->IsSelected() == true )
        {
            obj->GetParent()->RemoveChild( obj );

            group->PrependChild( obj );
        }

        objectList.pop_back();
    }

    group->Update();


    return group;
}

void
UOdysseyVectorRoot::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    for( std::list<UOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);
        ::ULIS::FRectD bbox = obj->GetBBox( false );

        blctx.save();
        blctx.setMatrix( obj->GetWorldMatrix() );
        blctx.strokeRect( bbox.x, bbox.y, bbox.w, bbox.h );
        blctx.restore();
    }
}

void
UOdysseyVectorRoot::InvalidateObject( UOdysseyVectorObject* iObject )
{
    mInvalidatedObjectList.push_back( iObject );
}

void
UOdysseyVectorRoot::UpdateShape()
{
    for( std::list<UOdysseyVectorObject*>::iterator it = mInvalidatedObjectList.begin(); it != mInvalidatedObjectList.end(); ++it )
    {
        UOdysseyVectorObject *obj = (*it);

        obj->Update();
    }

    mInvalidatedObjectList.clear();
}

void
UOdysseyVectorRoot::Bucket( double iX, double iY, uint32 iFillColor )
{
    UOdysseyVectorObject* pickedObject = RecursiveSelect( *this, iX, iY, 1.0f );

    if ( pickedObject )
    {
        pickedObject->SetFilled(true);
        pickedObject->SetFillColor( iFillColor );
    }
}

UOdysseyVectorObject*
UOdysseyVectorRoot::GetLastSelected()
{
    if ( mSelectedObjectList.empty() == true )
    {
        return nullptr;
    }

    return mSelectedObjectList.back();
}

UOdysseyVectorObject*
UOdysseyVectorRoot::RecursiveSelect( UOdysseyVectorObject& iObj, double iX, double iY, double iRadius )
{
    BLMatrix2D inverseLocalMatrix;
    BLPoint localCoords;
    BLPoint localSize;
    double localRadius;
    UOdysseyVectorObject* pickedObject = nullptr;

    BLMatrix2D::invert( inverseLocalMatrix, iObj.GetLocalMatrix() );

    localCoords = inverseLocalMatrix.mapPoint( iX, iY );
    localSize   = inverseLocalMatrix.mapPoint( iX + iRadius, 0.0f );

    localRadius = localSize.x - localCoords.x;

    for( std::list<UOdysseyVectorObject*>::iterator it = iObj.GetChildrenList().begin(); it != iObj.GetChildrenList().end(); ++it )
    {
        UOdysseyVectorObject *child = (*it);
        UOdysseyVectorObject* pickedChild = nullptr;

        pickedChild = RecursiveSelect( *child, localCoords.x, localCoords.y, localRadius );

        if ( pickedChild )
        {
            pickedObject = pickedChild;
        }
    }

    return ( pickedObject ) ? pickedObject : iObj.Pick( localCoords.x, localCoords.y, localRadius );
}
