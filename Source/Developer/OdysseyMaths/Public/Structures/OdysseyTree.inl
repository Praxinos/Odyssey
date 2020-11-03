// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

template< typename T >
FOdysseyNTree<T>::FOdysseyNTree( T iNodeContent )
    : mNodeContent(iNodeContent)
    , mNodes()
    , mParent( NULL )
{
}

template< typename T >
FOdysseyNTree<T>::~FOdysseyNTree()
{
    for( int i = 0; i < mNodes.Num(); i++ )
        delete mNodes[i];
    
    mNodes.Empty();
}

template< typename T >
FOdysseyNTree<T>*
FOdysseyNTree<T>::AddNode( T iNodeContent, int iIndexEmplace )
{
    if( iIndexEmplace < 0 || iIndexEmplace >= mNodes.Num() )
    {
        mNodes.Add( new FOdysseyNTree<T>( iNodeContent ) );
        mNodes.Last()->mParent = this;
        BroadcastNodeAdded(mNodes.Last());
        return mNodes.Last();
    }
    else
    {
        mNodes.EmplaceAt( iIndexEmplace, new FOdysseyNTree<T>( iNodeContent ) );
        mNodes[iIndexEmplace]->mParent = this;
        BroadcastNodeAdded(mNodes[iIndexEmplace]);
        return mNodes[iIndexEmplace];
    }
}

template< typename T >
FOdysseyNTree<T>*
FOdysseyNTree<T>::AddNode( FOdysseyNTree<T>* iNode, int iIndexEmplace )
{
    if( iIndexEmplace < 0 || iIndexEmplace >= mNodes.Num() )
    {
        mNodes.Add( iNode );
        iNode->mParent = this;
        return iNode;
    }
    else
    {
        mNodes.EmplaceAt( iIndexEmplace, iNode );
        iNode->mParent = this;
        return iNode;
    }
}


template< typename T >
void
FOdysseyNTree<T>::DeleteNode( int iIndex )
{
    checkf( iIndex >= 0 && iIndex < mNodes.Num(), TEXT("Index out of bounds in DeleteNode of FOdysseyNTree") );
    FOdysseyNTree<T>* node = mNodes[i];
    FOdysseyNTree<T>* parent = node->Parent;
    int index = node->GetIndexInParent();
    node->mParent = nullptr;
    mNodes.RemoveAt( iIndex );
    BroadcastNodeRemoved(node, parent, index);
    delete node;
}

template< typename T >
void
FOdysseyNTree<T>::DeleteNodeIfExist( FOdysseyNTree<T>* iNodeToDelete )
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


template< typename T >
FOdysseyNTree<T>*
FOdysseyNTree<T>::MoveNodeTo( FOdysseyNTree<T>* iNewPositionInTree, ePosition iPosition )
{
    checkf( iNewPositionInTree != NULL, TEXT("Null node passed in MoveNodeTo of FOdysseyNTree") );
    checkf( mParent != NULL, TEXT("Cannot move the root of the tree"));
    checkf( !( iNewPositionInTree->mParent == NULL && ( iPosition == ePosition::kBefore || iPosition == ePosition::kAfter ) ), TEXT("Cannot move a node after or before the root of the tree") );
    
    if( iPosition == ePosition::kIn )
    {
        //Remove old node
        FOdysseyNTree<T>* parent = mParent;
        int index = GetIndexInParent();
        mParent->mNodes.RemoveAt(index);

        //Place new node
        iNewPositionInTree->AddNode( this );
    
        BroadcastNodeMoved(this, parent, index);

        return this;
    }
    else if( iPosition == ePosition::kAfter )
    {
        //Remove old node
        FOdysseyNTree<T>* parent = mParent;
        int oldIndex = GetIndexInParent();
        mParent->mNodes.RemoveAt(oldIndex);
        
        //Place new node
        int index = iNewPositionInTree->GetIndexInParent();
        iNewPositionInTree->mParent->AddNode( this, index + 1 );
        BroadcastNodeMoved(this, parent, oldIndex);
        
        return this;
    }
    else if( iPosition == ePosition::kBefore )
    {
        //Remove old node
        FOdysseyNTree<T>* parent = mParent;
        int oldIndex = GetIndexInParent();
        mParent->mNodes.RemoveAt(index);

        //Place new node
        int index = iNewPositionInTree->GetIndexInParent();
        iNewPositionInTree->mParent->AddNode( this, index );

        BroadcastNodeMoved(this, parent, oldIndex);
        
        return this;
    }
    return NULL;
}

template< typename T >
FOdysseyNTree<T>*
FOdysseyNTree<T>::FindNode( T iToFind )
{
    FOdysseyNTree<T>* node = NULL;
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
}



template< typename T >
FOdysseyNTree<T>*
FOdysseyNTree<T>::GetParent() const
{
    return mParent;
}

template< typename T >
int
FOdysseyNTree<T>::GetIndexInParent() const
{
    if( !mParent )
        return -1;
    
    for( int i = 0; i < mParent->mNodes.Num(); i++ )
        if( this == mParent->mNodes[i] )
            return i;
    
    return -1;
}


template< typename T >
T
FOdysseyNTree<T>::GetNodeContent() const
{
    return mNodeContent;
}

template< typename T >
T*
FOdysseyNTree<T>::GetNodeContentPtr() const
{
    return &mNodeContent;
}


template< typename T >
TArray<FOdysseyNTree<T>*>
FOdysseyNTree<T>::GetNodes() const
{
    return mNodes;
}

template< typename T >
void
FOdysseyNTree<T>::DepthFirstSearchTree( TArray<T>* ioContents, bool iIncludeRoot ) const
{
    if( iIncludeRoot )
        ioContents->Add( mNodeContent );
    
    if( mNodes.Num() == 0 )
        return;
    
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        mNodes[i]->DepthFirstSearchTree( ioContents, true );
    }
}


template< typename T >
void
FOdysseyNTree<T>::BreadthFirstSearchTree( TArray<T>* ioContents, bool iIncludeRoot ) const
{
    if( iIncludeRoot )
        ioContents->Add( mNodeContent );
    
    if( mNodes.Num() == 0 )
        return;
    
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        ioContents->Add( mNodes[i]->mNodeContent );
    }
    
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        mNodes[i]->BreadthFirstSearchTree( ioContents, false );
    }
}

template< typename T >
int
FOdysseyNTree<T>::GetNumberParents() const
{
    if( mParent == NULL )
        return 0;
    
    int numParents = 1;
    FOdysseyNTree<T>* parent = mParent;
    while( parent != NULL )
    {
        parent = parent->mParent;
        numParents++;
    }
    return numParents;
}

template< typename T >
bool
FOdysseyNTree<T>::HasForParent(FOdysseyNTree<T>* iParentToSearch) const
{
    FOdysseyNTree<T>* parent = mParent;
    while( parent != NULL )
    {
        if( parent == iParentToSearch )
            return true;
        
        parent = parent->mParent;
    }
    return false;
}

template< typename T >
FOdysseyNTree<T>::FOdysseyTreeNodeAdded&
BroadcastNodeAdded(FOdysseyNTree<T>* iNode)
{
    mNodeAdded.Broadcast(iNode);
    if (mParent)
    {
        mParent->BroadcastNodeAdded(iNode);
    }
}

template< typename T >
FOdysseyNTree<T>::FOdysseyTreeNodeRemoved&
BroadcastNodeRemoved(FOdysseyNTree<T>* iNode, FOdysseyNTree<T>* iParent, int iIndex)
{
    mNodeRemoved.Broadcast(iNode, iParent, iIndex);
    if (mParent)
    {
        mParent->BroadcastNodeRemoved(iNode, iParent, iIndex);
    }
}

template< typename T >
FOdysseyNTree<T>::FOdysseyTreeNodeMoved&
BroadcastNodeMoved(FOdysseyNTree<T>* iNode, FOdysseyNTree<T>* iParent, int iIndex)
{
    mNodeRemoved.Broadcast(iNode, iParent, iIndex);
    if (mParent)
    {
        mParent->BroadcastNodeRemoved(iNode, iParent, iIndex);
    }
}

template< typename T >
FOdysseyNTree<T>::FOdysseyTreeNodeAdded&
NodeAdded()
{
    return mNodeAdded;
}

template< typename T >
FOdysseyNTree<T>::FOdysseyTreeNodeRemoved&
NodeRemoved()
{
    return mNodeRemoved;
}

template< typename T >
FOdysseyNTree<T>::FOdysseyTreeNodeMoved&
NodeMoved()
{
    return mNodeMoved;
}

class IOdysseyLayer;

template< typename T >
inline FArchive& operator<<(FArchive &Ar, FOdysseyNTree<T>& ioSaveNTree )
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
            FOdysseyNTree<T> newNode = new FOdysseyNTree<T>(NULL);
            ioSaveNTree.AddNode( newNode );
            Ar << *newNode;
        }
    }
    
    return Ar;
}
