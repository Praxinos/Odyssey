#include "OdysseyVectorRoot.h"

FOdysseyVectorRoot::~FOdysseyVectorRoot()
{
}

FOdysseyVectorRoot::FOdysseyVectorRoot( std::string iName )
    : FOdysseyVectorObject( iName )
{

}

void
FOdysseyVectorRoot::ClearSelection()
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        obj->SetIsSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
FOdysseyVectorRoot::Select( FOdysseyVectorObject& iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), &iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj.SetIsSelected( true );

        mSelectedObjectList.push_back( &iVecObj );
    }
}

FOdysseyVectorObject*
FOdysseyVectorRoot::CopyShape()
{
    return new FOdysseyVectorRoot( mName );
}

void
FOdysseyVectorRoot::Select( double iX, double iY, double iRadius )
{
    FOdysseyVectorObject* pickedObject = RecursiveSelect( *this, iX, iY, iRadius );

    if ( pickedObject )
    {
        if( typeid ( *pickedObject ) == typeid ( FOdysseyVectorLoop ) )
        {
            pickedObject = pickedObject->GetParent();
        }

        Select ( *pickedObject );
    }
}

FOdysseyVectorGroup*
FOdysseyVectorRoot::GroupSelectdObjects( )
{
    FOdysseyVectorGroup* group = new FOdysseyVectorGroup();
    BLPoint averageTranslation = { 0.0f, 0.0f };
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();
    // We don't use the mSelectedObjectList because we want to keep the same order
    // and we work on a copy to be able to delete the objects while iterating
    std::list<FOdysseyVectorObject*> objectList = mChildrenList;

    if ( mSelectedObjectList.size() )
    {
        for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
        {
            FOdysseyVectorObject *obj = (*it);
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
        FOdysseyVectorObject *obj = objectList.back();

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
FOdysseyVectorRoot::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);
        ::ULIS::FRectD bbox = obj->GetBBox( false );

        blctx.save();
        blctx.setMatrix( obj->GetWorldMatrix() );
        blctx.strokeRect( bbox.x, bbox.y, bbox.w, bbox.h );
        blctx.restore();
    }
}

void
FOdysseyVectorRoot::InvalidateObject( FOdysseyVectorObject* iObject )
{
    mInvalidatedObjectList.push_back( iObject );
}

void
FOdysseyVectorRoot::UpdateShape()
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mInvalidatedObjectList.begin(); it != mInvalidatedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        obj->Update();
    }

    mInvalidatedObjectList.clear();
}

void
FOdysseyVectorRoot::Bucket( double iX, double iY, uint32 iFillColor )
{
    FOdysseyVectorObject* pickedObject = RecursiveSelect( *this, iX, iY, 1.0f );

    if ( pickedObject )
    {
        pickedObject->SetFilled(true);
        pickedObject->SetFillColor( iFillColor );
    }
}

FOdysseyVectorObject*
FOdysseyVectorRoot::GetLastSelected()
{
    if ( mSelectedObjectList.empty() == true )
    {
        return nullptr;
    }

    return mSelectedObjectList.back();
}

FOdysseyVectorObject*
FOdysseyVectorRoot::RecursiveSelect( FOdysseyVectorObject& iObj, double iX, double iY, double iRadius )
{
    BLMatrix2D inverseLocalMatrix;
    BLPoint localCoords;
    BLPoint localSize;
    double localRadius;
    FOdysseyVectorObject* pickedObject = nullptr;

    BLMatrix2D::invert( inverseLocalMatrix, iObj.GetLocalMatrix() );

    localCoords = inverseLocalMatrix.mapPoint( iX, iY );
    localSize   = inverseLocalMatrix.mapPoint( iX + iRadius, 0.0f );

    localRadius = localSize.x - localCoords.x;

    for( std::list<FOdysseyVectorObject*>::iterator it = iObj.GetChildrenList().begin(); it != iObj.GetChildrenList().end(); ++it )
    {
        FOdysseyVectorObject *child = (*it);
        FOdysseyVectorObject* pickedChild = nullptr;

        pickedChild = RecursiveSelect( *child, localCoords.x, localCoords.y, localRadius );

        if ( pickedChild )
        {
            pickedObject = pickedChild;
        }
    }

    return ( pickedObject ) ? pickedObject : iObj.Pick( localCoords.x, localCoords.y, localRadius );
}
