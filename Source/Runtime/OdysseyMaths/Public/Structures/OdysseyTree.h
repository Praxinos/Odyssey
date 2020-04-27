// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

enum class ePosition : char
{
    kIn,
    kBefore,
    kAfter,
    kInvalid
};

template< typename T >
class FOdysseyNTree
{
//Construction Destruction
public:
    FOdysseyNTree( T iNodeContent );
    ~FOdysseyNTree();
    
public:
    FOdysseyNTree<T>* AddNode( T iNodeContent, int iIndexEmplace = -1 );
    FOdysseyNTree<T>* AddNode( FOdysseyNTree<T>* iNode, int iIndexEmplace = -1 );

    void DeleteNode( int iIndex );
    void DeleteNodeIfExist( FOdysseyNTree<T>* iNodeToDelete );
    FOdysseyNTree<T>* MoveNodeTo( FOdysseyNTree<T>* iNewPositionInTree, ePosition iPosition = ePosition::kAfter );
    
    //Find the first node (depthSearch) which content is iToFind
    FOdysseyNTree<T>* FindNode( T iToFind );
    
    FOdysseyNTree<T>* GetParent() const;
    int GetIndexInParent() const;
    T GetNodeContent() const;
    T* GetNodeContentPtr() const;
    void SetNodeContent( T& iNodeContent );
    
    const TArray<FOdysseyNTree*>* GetNodes() const;
    
    void DepthFirstSearchTree( TArray<T>* ioContents, bool iIncludeRoot = true ) const;
    void BreadthFirstSearchTree( TArray<T>* ioContents, bool iIncludeRoot = true ) const;
    
    int GetNumberParents() const;
    
    //Check if iParentToSearch is a parent (direct or indirect) of the currentNode
    bool HasForParent( FOdysseyNTree<T>* iParentToSearch ) const;

    // Overloads for save in archive
    friend FArchive& operator<<(FArchive &Ar, FOdysseyNTree<IOdysseyLayer*>& ioSaveNTree );

private:
    T mNodeContent;
    TArray<FOdysseyNTree*> mNodes;
    FOdysseyNTree* mParent;
    
};

//Implementation
#include "OdysseyTree.inl"
