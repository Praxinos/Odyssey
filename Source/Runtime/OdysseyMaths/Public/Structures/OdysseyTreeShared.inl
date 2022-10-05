// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

template< typename tDerived >
FOdysseyNTreeShared<tDerived>::~FOdysseyNTreeShared()
{   
    mChildren.Empty();
}

template< typename tDerived >
FOdysseyNTreeShared<tDerived>::FOdysseyNTreeShared()
    : mChildren()
    , mParent()
{
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::AddChild( TSharedPtr<tDerived> iChild, int iIndexEmplace )
{
	TSharedPtr<FOdysseyNTreeShared<tDerived>> parent = this->AsShared();
    
    TSharedPtr<tDerived> oldParent = iChild->GetParent();
    if (oldParent != nullptr)
        oldParent->RemoveChild(iChild->GetIndexInParent());

    iChild->mParent = TWeakPtr<tDerived>(StaticCastSharedPtr<tDerived>(parent));
    if( iIndexEmplace < 0 || iIndexEmplace >= mChildren.Num() )
    {
        mChildren.Add( iChild );
    }
    else
    {
        mChildren.EmplaceAt( iIndexEmplace, iChild );
    }
    iChild->OnParentChanged(oldParent);
    BroadcastChildAdded(iChild);
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::RemoveChild( int iIndex )
{
    checkf( iIndex >= 0 && iIndex < mChildren.Num(), TEXT("Index out of bounds in RemoveChild of FOdysseyNTreeShared") );
    TSharedPtr<tDerived> child = mChildren[iIndex];
    TSharedPtr<tDerived> oldParent = child->GetParent();

    child->mParent.Reset();
    mChildren.RemoveAt( iIndex );

    child->OnParentChanged(oldParent);
    BroadcastChildRemoved(child, oldParent, iIndex);
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::MoveTo( TSharedPtr<tDerived> iNode, ePosition iPosition )
{
    checkf( iNode != nullptr, TEXT("Null node passed in MoveTo of FOdysseyNTreeShared") );
    checkf( iNode->mParent != nullptr || ( iPosition != ePosition::kBefore && iPosition != ePosition::kAfter ), TEXT("Cannot move a node after or before the root of the tree") );
    
	TSharedPtr<FOdysseyNTreeShared<tDerived>> selfRef = this->AsShared();
	TSharedPtr<tDerived> self = StaticCastSharedPtr<tDerived>(selfRef);

    if( iPosition == ePosition::kIn )
    {
        //Place new node
        iNode->AddChild(self);
    }
    else if( iPosition == ePosition::kAfter )
    {   
        //Place new node
        iNode->GetParent()->AddChild(self, iNode->GetIndexInParent() + 1 );
    }
    else if( iPosition == ePosition::kBefore )
    {
        //Place new node
        iNode->GetParent()->AddChild(self, iNode->GetIndexInParent() );
    }
}

template< typename tDerived >
TSharedPtr<tDerived>
FOdysseyNTreeShared<tDerived>::GetParent() const
{
    return mParent.Pin();
}

template< typename tDerived >
int
FOdysseyNTreeShared<tDerived>::GetIndexInParent() const
{
    if( mParent == nullptr)
        return -1;
    
	TSharedPtr<tDerived> parent = mParent.Pin();

    for( int i = 0; i < parent->mChildren.Num(); i++ )
        if( this == parent->mChildren[i].Get() )
            return i;
    
    return -1;
}

template< typename tDerived >
TArray<TSharedPtr<tDerived>>
FOdysseyNTreeShared<tDerived>::GetChildren() const
{
    return mChildren;
}

template< typename tDerived >
TSharedPtr<tDerived>
FOdysseyNTreeShared<tDerived>::GetChild(int iIndex) const
{
    return mChildren[iIndex];
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::DepthFirstSearchTree( TArray<TSharedPtr<tDerived>>* ioContents, bool iIncludeRoot )
{
	if (iIncludeRoot)
	{
		TSharedPtr<FOdysseyNTreeShared<tDerived>> selfRef = this->AsShared();
		TSharedPtr<tDerived> self = StaticCastSharedPtr<tDerived>(selfRef);
		ioContents->Add(self);
	}
    
    if( mChildren.Num() == 0 )
        return;
    
    for( int i = 0; i < mChildren.Num(); i++ )
    {
        mChildren[i]->DepthFirstSearchTree( ioContents, true );
    }
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::BreadthFirstSearchTree( TArray<TSharedPtr<tDerived>>* ioContents, bool iIncludeRoot )
{
	if (iIncludeRoot)
	{
		TSharedPtr<FOdysseyNTreeShared<tDerived>> selfRef = this->AsShared();
		TSharedPtr<tDerived> self = StaticCastSharedPtr<tDerived>(selfRef);
		ioContents->Add(self);
	}
    
    if( mChildren.Num() == 0 )
        return;
    
    for( int i = 0; i < mChildren.Num(); i++ )
    {
        ioContents->Add( mChildren[i] );
    }
    
    for( int i = 0; i < mChildren.Num(); i++ )
    {
        mChildren[i]->BreadthFirstSearchTree( ioContents, false );
    }
}

template< typename tDerived >
int
FOdysseyNTreeShared<tDerived>::GetNumberParents() const
{
    int numParents = 0;
    if( mParent == NULL )
        return numParents;
    
    TSharedPtr<tDerived> parent = mParent.Pin();
    while( parent != NULL )
    {
        parent = parent->mParent.Pin();
        numParents++;
    }
    return numParents;
}

template< typename tDerived >
bool
FOdysseyNTreeShared<tDerived>::HasForParent(TSharedPtr<tDerived> iParentToSearch) const
{
    TSharedPtr<tDerived> parent = mParent.Pin();
    while( parent != NULL )
    {
        if( parent == iParentToSearch )
            return true;
        
        parent = parent->mParent.Pin();
    }
    return false;
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::BroadcastChildAdded(TSharedPtr<tDerived> iChild)
{
    mOnChildAddedDelegate.Broadcast(iChild);
    TSharedPtr<tDerived> parent = mParent.Pin();
    while( parent != NULL )
    {   
        parent->mOnChildAddedDelegate.Broadcast(iChild);
        parent = parent->mParent.Pin();
    }
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::BroadcastChildRemoved(TSharedPtr<tDerived> iChild, TSharedPtr<tDerived> iParent, int iIndex)
{
    mOnChildRemovedDelegate.Broadcast(iChild, iParent, iIndex);

    TSharedPtr<tDerived> parent = mParent.Pin();
    while( parent != NULL )
    {   
        parent->mOnChildRemovedDelegate.Broadcast(iChild, iParent, iIndex);
        parent = parent->mParent.Pin();
    }
}

template< typename tDerived >
typename FOdysseyNTreeShared<tDerived>::FOnChildAdded&
FOdysseyNTreeShared<tDerived>::OnChildAddedDelegate()
{
    return mOnChildAddedDelegate;
}

template< typename tDerived >
typename FOdysseyNTreeShared<tDerived>::FOnChildRemoved&
FOdysseyNTreeShared<tDerived>::OnChildRemovedDelegate()
{
    return mOnChildRemovedDelegate;
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::OnParentChanged(TSharedPtr<tDerived> iOldParent)
{
}
