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
FOdysseyNTree<T>*
FOdysseyNTree<T>::MoveNodeTo( FOdysseyNTree* iNewPositionInTree, ePosition iPosition )
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
T
FOdysseyNTree<T>::GetNodeContent()
{
    return mNodeContent;
}

template< typename T >
T*
FOdysseyNTree<T>::GetNodeContentPtr()
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
TArray<FOdysseyNTree<T>*>
FOdysseyNTree<T>::GetNodes()
{
    return mNodes;
}

template< typename T >
void
FOdysseyNTree<T>::DepthFirstSearchTree( TArray<T>* ioContents )
{
    ioContents->Add( mNodeContent );
    
    if( GetNodes().Num() == 0 )
        return;
    
    for( int i = 0; i < mNodes.Num(); i++ )
    {
        mNodes[i]->DepthFirstSearchTree( ioContents );
    }
}
