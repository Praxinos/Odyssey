#include "OdysseyVectorScene.h"

void FOdysseyVectorScene::Init( std::string iName )
{
    SetName( iName );
}

void
FOdysseyVectorScene::SetEngine( FOdysseyVectorEngine* iEngine )
{
    mEngine = iEngine;
}

FOdysseyVectorEngine*
FOdysseyVectorScene::GetEngine()
{
    return mEngine;
}

void
FOdysseyVectorScene::ClearSelection()
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        obj->SetIsSelected ( false );
    }

    mSelectedObjectList.clear();
}

void
FOdysseyVectorScene::Unselect( FOdysseyVectorObject* iVecObj )
{
    iVecObj->SetIsSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
FOdysseyVectorScene::Select( FOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetIsSelected( true );

        mSelectedObjectList.push_back( iVecObj );
    }
}

FOdysseyVectorObject*
FOdysseyVectorScene::CopyShape()
{
    FOdysseyVectorScene* rootCopy = new FOdysseyVectorScene();

    rootCopy->Init( Name );

    return static_cast<FOdysseyVectorObject*>(rootCopy);
}

FOdysseyVectorGroup*
FOdysseyVectorScene::GroupSelectdObjects( )
{
    FOdysseyVectorGroup* group = new FOdysseyVectorGroup();
    BLPoint averageTranslation = { 0.0f, 0.0f };
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

    group->Invalidate();


    return group;
}

std::list<FOdysseyVectorObject*>&
FOdysseyVectorScene::GetSelectedObjectList()
{
    return mSelectedObjectList;
}

void
FOdysseyVectorScene::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{

}

void
FOdysseyVectorScene::InvalidateObject( FOdysseyVectorObject* iObject )
{
    mInvalidatedObjectList.push_back( iObject );
}

void
FOdysseyVectorScene::UpdateShape( uint32 iUpdateFlags )
{
    for( std::list<FOdysseyVectorObject*>::iterator it = mInvalidatedObjectList.begin(); it != mInvalidatedObjectList.end(); ++it )
    {
        FOdysseyVectorObject *obj = (*it);

        obj->Update( iUpdateFlags );
    }

    if( ( iUpdateFlags & FOdysseyVectorObject::KEEPINVALIDATED ) == 0 )
    {
        mInvalidatedObjectList.clear();
    }
}

FOdysseyVectorObject*
FOdysseyVectorScene::GetLastSelected()
{
    return ( mSelectedObjectList.empty() == true ) ? nullptr : mSelectedObjectList.back();
}

uint32
FOdysseyVectorScene::GetType()
{
    return FOdysseyVectorObject::VECTORROOTTYPE;
}

void
FOdysseyVectorScene::RemoveSelectedObjects()
{
    for(std::list<FOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it)
    {
        FOdysseyVectorObject* selectedObject = (*it);

        // prevent nested removal
        if( selectedObject->HasSelectedParent() == false )
        {
            selectedObject->GetParent()->RemoveChild( selectedObject );
        }
    }

    ClearSelection(); // will unset set selection flag as well
}
