#ifndef GNODE_TREE_H
#define GNODE_TREE_H

#include "Graph.h"

namespace GNode {

    template<class T>
    class Tree: public Graph<T> {

    public:

        Tree(): Graph<T>() {
            this->root->setDoubleLink(false);
        }

        Tree(T rootValue): Graph<T>(rootValue) {
            this->root->setDoubleLink(false);
        }

        Tree(T rootValue, void (*deleteCallbackFunction)(T)): Graph<T>(rootValue, deleteCallbackFunction) {
            this->root->setDoubleLink(false);
        }

        Node<T>* create(T val) {
            auto node = new Node<T>(val);
            node->setDoubleLink(false);
            this->nodes.insert(node);
            return node;
        }

    };

}

#endif
