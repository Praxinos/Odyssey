// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
    {
        mNodes[i]->~FOdysseyNTree();
        delete mNodes[i];
    }
    
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
        return mNodes.Last();
    }
    else
    {
        mNodes.EmplaceAt( iIndexEmplace, new FOdysseyNTree<T>( iNodeContent ) );
        mNodes[iIndexEmplace]->mParent = this;
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
    mNodes[iIndex]->~FOdysseyNTree();
    mNodes.RemoveAt( iIndex );
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
            mNodes[i]->DeleteNodeIfExist( iNodeToDelete );
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
        int index = -1;
        for( int i = 0; i < mParent->mNodes.Num(); i++ )
            if( this == mParent->mNodes[i] )
                index = i;
        
        mParent->mNodes.RemoveAt(index);

        //Place new node
        iNewPositionInTree->AddNode( this );
    

        return this;
    }
    else if( iPosition == ePosition::kAfter )
    {
        //Remove old node
        int index = -1;
        for( int i = 0; i < mParent->mNodes.Num(); i++ )
            if( this == mParent->mNodes[i] )
                index = i;
        
        mParent->mNodes.RemoveAt(index);
        
        //Place new node
        index = -1;
        for( int i = 0; i < iNewPositionInTree->mParent->mNodes.Num(); i++)
            if( iNewPositionInTree == iNewPositionInTree->mParent->mNodes[i] )
                index = i;
        
        iNewPositionInTree->mParent->AddNode( this, index + 1 );
        
        return this;
    }
    else if( iPosition == ePosition::kBefore )
    {
        //Remove old node
        int index = -1;
        for( int i = 0; i < mParent->mNodes.Num(); i++ )
            if( this == mParent->mNodes[i] )
                index = i;
        
        mParent->mNodes.RemoveAt(index);

        //Place new node
        index = -1;
        for( int i = 0; i < iNewPositionInTree->mParent->mNodes.Num(); i++)
            if( iNewPositionInTree == iNewPositionInTree->mParent->mNodes[i] )
                index = i;
        
        iNewPositionInTree->mParent->AddNode( this, index );
        
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
void
FOdysseyNTree<T>::SetNodeContent( T& iNodeContent )
{
    mNodeContent = iNodeContent;
}


template< typename T >
const TArray<FOdysseyNTree<T>*>*
FOdysseyNTree<T>::GetNodes() const
{
    return &mNodes;
}

template< typename T >
void
FOdysseyNTree<T>::DepthFirstSearchTree( TArray<T>* ioContents, bool iIncludeRoot ) const
{
    if( iIncludeRoot )
        ioContents->Add( mNodeContent );
    
    if( GetNodes()->Num() == 0 )
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
    
    if( GetNodes().Num() == 0 )
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


class IOdysseyLayer;

inline FArchive& operator<<(FArchive &Ar, FOdysseyNTree<IOdysseyLayer*>& ioSaveNTree )
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
        
        if( ioSaveNTree.mParent ) //The root is empty, we don't load it
        {
            Ar << &(ioSaveNTree.mNodeContent);
        }
        
        Ar << numNodes;
        
        for( int i = 0; i < numNodes; i++ )
        {
            FOdysseyNTree<IOdysseyLayer*>* newNode = new FOdysseyNTree<IOdysseyLayer*>(NULL);
            ioSaveNTree.AddNode( newNode );
            Ar << *newNode;
        }
    }
    
    return Ar;
}
