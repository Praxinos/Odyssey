// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

template< typename tDerived >
FOdysseyNTreeShared<tDerived>::~FOdysseyNTreeShared()
{   
    mNodes.Empty();
}

template< typename tDerived >
FOdysseyNTreeShared<tDerived>::FOdysseyNTreeShared()
    : mNodes()
    , mParent()
{
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::AddNode( TSharedPtr<tDerived> iNode, int iIndexEmplace )
{
	TSharedPtr<FOdysseyNTreeShared<tDerived>> parent = this->AsShared();
    iNode->mParent = TWeakPtr<tDerived>(StaticCastSharedPtr<tDerived>(parent));
    if( iIndexEmplace < 0 || iIndexEmplace >= mNodes.Num() )
    {
        mNodes.Add( iNode );
    }
    else
    {
        mNodes.EmplaceAt( iIndexEmplace, iNode );
    }
    BroadcastNodeAdded(iNode);
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::DeleteNode( int iIndex )
{
    checkf( iIndex >= 0 && iIndex < mNodes.Num(), TEXT("Index out of bounds in DeleteNode of FOdysseyNTreeShared") );
    TSharedPtr<tDerived> node = mNodes[iIndex];
    TSharedPtr<tDerived> parent = node->mParent.Pin();
    node->mParent.Reset();
    mNodes.RemoveAt( iIndex );
    BroadcastNodeRemoved(node, parent, iIndex);
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::DeleteNodeIfExist( TSharedPtr<tDerived> iNodeToDelete )
{
    //Can't delete the root of the tree, so we never check mNodeContent
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        if( mNodes[i] == iNodeToDelete )
        {
            DeleteNode( i );
        }
        else
        {
            mNodes[i]->DeleteNodeIfExist( iNodeToDelete );
        }
    }
}


template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::MoveNodeTo( TSharedPtr<tDerived> iNode, ePosition iPosition )
{
    checkf( iNode != nullptr, TEXT("Null node passed in MoveNodeTo of FOdysseyNTreeShared") );
    checkf( iNode->mParent != nullptr || ( iPosition != ePosition::kBefore && iPosition != ePosition::kAfter ), TEXT("Cannot move a node after or before the root of the tree") );
    
    //Remove old node
    TSharedPtr<tDerived> oldParent = mParent.Pin();
    int oldIndex = GetIndexInParent();
    if (oldParent != nullptr)
    {
        oldParent->DeleteNode(oldIndex);
    }

	
	TSharedPtr<FOdysseyNTreeShared<tDerived>> selfRef = this->AsShared();
	TSharedPtr<tDerived> self = StaticCastSharedPtr<tDerived>(selfRef);

    if( iPosition == ePosition::kIn )
    {
        //Place new node
        iNode->AddNode(self);
        BroadcastNodeMoved(self, oldParent, oldIndex);
    }
    else if( iPosition == ePosition::kAfter )
    {   
        //Place new node
        int index = iNode->GetIndexInParent();
        iNode->mParent.Pin()->AddNode(self, index + 1 );
        BroadcastNodeMoved(self, oldParent, oldIndex);
    }
    else if( iPosition == ePosition::kBefore )
    {
        //Place new node
        int index = iNode->GetIndexInParent();
        iNode->mParent.Pin()->AddNode(self, index );
        BroadcastNodeMoved(self, oldParent, oldIndex);
    }
}

/* template< typename tDerived >
FOdysseyNTreeShared<tDerived>*
FOdysseyNTreeShared<tDerived>::FindNode( T iToFind )
{
    FOdysseyNTreeShared<tDerived>* node = NULL;
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        if( iToFind == mNodes[i]->mNodeContent )
            return mNodes[i];
        else
            node = mNodes[i]->FindNode(iToFind);
        
        if( node != NULL )
            return node;
    }
    return NULL;
} */



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

    for( int i = 0; i < parent->mNodes.Num(); i++ )
        if( this == parent->mNodes[i].Get() )
            return i;
    
    return -1;
}


/* template< typename tDerived >
T
FOdysseyNTreeShared<tDerived>::GetNodeContent() const
{
    return mNodeContent;
}

template< typename tDerived >
T*
FOdysseyNTreeShared<tDerived>::GetNodeContentPtr() const
{
    return &mNodeContent;
} */


template< typename tDerived >
TArray<TSharedPtr<tDerived>>
FOdysseyNTreeShared<tDerived>::GetNodes() const
{
    return mNodes;
}

template< typename tDerived >
TSharedPtr<tDerived>
FOdysseyNTreeShared<tDerived>::GetNode(int iIndex) const
{
    return mNodes[iIndex];
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
    
    if( mNodes.Num() == 0 )
        return;
    
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        mNodes[i]->DepthFirstSearchTree( ioContents, true );
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
    
    if( mNodes.Num() == 0 )
        return;
    
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        ioContents->Add( mNodes[i] );
    }
    
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        mNodes[i]->BreadthFirstSearchTree( ioContents, false );
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
FOdysseyNTreeShared<tDerived>::BroadcastNodeAdded(TSharedPtr<tDerived> iNode)
{
    mNodeAdded.Broadcast(iNode);
    TSharedPtr<tDerived> parent = mParent.Pin();
    while( parent != NULL )
    {   
        parent->mNodeAdded.Broadcast(iNode);
        parent = parent->mParent.Pin();
    }
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::BroadcastNodeRemoved(TSharedPtr<tDerived> iNode, TSharedPtr<tDerived> iParent, int iIndex)
{
    mNodeRemoved.Broadcast(iNode, iParent, iIndex);

    TSharedPtr<tDerived> parent = mParent.Pin();
    while( parent != NULL )
    {   
        parent->mNodeRemoved.Broadcast(iNode, iParent, iIndex);
        parent = parent->mParent.Pin();
    }
}

template< typename tDerived >
void
FOdysseyNTreeShared<tDerived>::BroadcastNodeMoved(TSharedPtr<tDerived> iNode, TSharedPtr<tDerived> iParent, int iIndex)
{
    mNodeMoved.Broadcast(iNode, iParent, iIndex);
	TSharedPtr<tDerived> parent = mParent.Pin();
    while( parent != NULL )
    {   
        parent->mNodeMoved.Broadcast(iNode, iParent, iIndex);
        parent = parent->mParent.Pin();
    }
}

template< typename tDerived >
typename FOdysseyNTreeShared<tDerived>::FOdysseyTreeNodeAdded&
FOdysseyNTreeShared<tDerived>::NodeAdded()
{
    return mNodeAdded;
}

template< typename tDerived >
typename FOdysseyNTreeShared<tDerived>::FOdysseyTreeNodeRemoved&
FOdysseyNTreeShared<tDerived>::NodeRemoved()
{
    return mNodeRemoved;
}

template< typename tDerived >
typename FOdysseyNTreeShared<tDerived>::FOdysseyTreeNodeMoved&
FOdysseyNTreeShared<tDerived>::NodeMoved()
{
    return mNodeMoved;
}

/* class IOdysseyLayer;

template< typename tDerived >
inline FArchive& operator<<(FArchive &Ar, FOdysseyNTreeShared<tDerived>& ioSaveNTree )
{
    if( Ar.IsSaving() )
    {
        int numNodes = ioSaveNTree.mNodes.Num();
        Ar << &(ioSaveNTree.mNodeContent);
        Ar << numNodes;
        for( int i = 0; i < numNodes; i++ )
            Ar << (*ioSaveNTree.mNodes[i]);
    }
    else if( Ar.IsLoading() )
    {
        int numNodes;
        Ar << ioSaveNTree.mNodeContent;
        Ar << numNodes;
        for( int i = 0; i < numNodes; i++ )
        {
            FOdysseyNTreeShared<tDerived> newNode = new FOdysseyNTreeShared<tDerived>(NULL);
            ioSaveNTree.AddNode( newNode );
            Ar << *newNode;
        }
    }
    
    return Ar;
} */
