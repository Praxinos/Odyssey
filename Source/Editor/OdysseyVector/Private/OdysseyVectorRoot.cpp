#include "OdysseyVectorRoot.h"

void UOdysseyVectorRoot::Init( std::string iName )
{
    SetName( iName );
}

void
UOdysseyVectorRoot::SetEngine( FOdysseyVectorEngine* iEngine )
{
    mEngine = iEngine;
}

FOdysseyVectorEngine*
UOdysseyVectorRoot::GetEngine()
{
    return mEngine;
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
UOdysseyVectorRoot::Unselect( UOdysseyVectorObject* iVecObj )
{
    iVecObj->SetIsSelected( false );

    mSelectedObjectList.remove( iVecObj );
}

void
UOdysseyVectorRoot::Select( UOdysseyVectorObject* iVecObj )
{
    if( std::find( mSelectedObjectList.begin(), mSelectedObjectList.end(), iVecObj ) == mSelectedObjectList.end() )
    {
        iVecObj->SetIsSelected( true );

        mSelectedObjectList.push_back( iVecObj );
    }
}

UOdysseyVectorObject*
UOdysseyVectorRoot::CopyShape()
{
    UOdysseyVectorRoot* rootCopy = NewObject<UOdysseyVectorRoot>();

    rootCopy->Init( Name );

    return Cast<UOdysseyVectorObject>(rootCopy);
}

UOdysseyVectorGroup*
UOdysseyVectorRoot::GroupSelectdObjects( )
{
    UOdysseyVectorGroup* group = NewObject<UOdysseyVectorGroup>();
    BLPoint averageTranslation = { 0.0f, 0.0f };
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

std::list<UOdysseyVectorObject*>&
UOdysseyVectorRoot::GetSelectedObjectList()
{
    return mSelectedObjectList;
}

void
UOdysseyVectorRoot::DrawShape( ::ULIS::FRectD& iRoi, uint64 iFlags )
{

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

UOdysseyVectorObject*
UOdysseyVectorRoot::GetLastSelected()
{
    if ( mSelectedObjectList.empty() == true )
    {
        return nullptr;
    }

    return mSelectedObjectList.back();
}

uint32
UOdysseyVectorRoot::GetType()
{
    return UOdysseyVectorObject::VECTORROOTTYPE;
}

void
UOdysseyVectorRoot::RemoveSelectedObjects()
{
    for(std::list<UOdysseyVectorObject*>::iterator it = mSelectedObjectList.begin(); it != mSelectedObjectList.end(); ++it)
    {
        UOdysseyVectorObject* selectedObject = (*it);

        // prevent nested removal
        if( selectedObject->HasSelectedParent() == false )
        {
            selectedObject->GetParent()->RemoveChild( selectedObject );
        }
    }

    ClearSelection(); // will unset set selection flag as well
}
