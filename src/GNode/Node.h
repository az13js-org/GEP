#ifndef GNODE_NODE_H
#define GNODE_NODE_H

#include <set>

namespace GNode {

    template<class T>
    class Node {

    public:

        Node() {
        }

        Node(T val) {
            value = val;
        }

        ~Node() {
        }

        void setDoubleLink(bool enable) {
            useDoubleLink = enable;
        }

        T getValue() {
            return value;
        }

        void add(Node<T>* child) {
            if (nodes.count(child) != 0) {
                return;
            }
            nodes.insert(child);
            if (useDoubleLink) {
                child->add(this);
            }
        }

        std::set<Node<T>*> getNodes() {
            return nodes;
        }

        void remove(Node<T>* node) {
            if (nodes.count(node) == 0) {
                return;
            }
            nodes.erase(node);
            if (useDoubleLink) {
                node->remove(this);
            }
        }

    private:

        T value = 0;

        std::set<Node<T>*> nodes;

        bool useDoubleLink = true;

    };

}

#endif
