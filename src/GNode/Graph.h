#ifndef GNODE_GRAPH_H
#define GNODE_GRAPH_H

#include "Node.h"
#include <set>
#include <list>
#include <queue>

namespace GNode {

    template<class T>
    class Graph {

    public:

        Graph() {
            root = new Node<T>();
            nodes.insert(root);
        }

        Graph(T rootValue) {
            root = new Node<T>(rootValue);
            nodes.insert(root);
        }

        Graph(T rootValue, void (*deleteCallbackFunction)(T)) {
            root = new Node<T>(rootValue);
            nodes.insert(root);
            deleteCallback = deleteCallbackFunction;
        }

        ~Graph() {
            for (auto e : nodes) {
                if (nullptr != deleteCallback) {
                    deleteCallback(e->getValue());
                }
                delete e;
            }
            nodes.clear();
        }

        Node<T>* getRoot() {
            return root;
        }

        Node<T>* create(T val) {
            auto node = new Node<T>(val);
            nodes.insert(node);
            return node;
        }

        std::set<Node<T>*> getNodes() {
            return nodes;
        }

        void gc() {
            using namespace std;
            set<Node<T>*> searched;
            list<Node<T>*> removeNode;
            queue<Node<T>*> queryQueue;
            auto queryNode = root;

            // 从根节点开始遍历整张图，查找所有节点
            queryQueue.push(root);
            while (!queryQueue.empty()) {
                queryNode = queryQueue.front();
                queryQueue.pop();
                searched.insert(queryNode);
                for (auto e : queryNode->getNodes()) {
                    if (searched.count(e) == 0) {
                        queryQueue.push(e);
                    }
                }
            }

            // 查找不到的节点就是不可达的节点，删除不可达节点
            for (auto e : nodes) {
                if (searched.count(e) == 0) {
                    // 不能直接在这里进行 nodes.erase(e) 操作！
                    removeNode.emplace_back(e);
                    if (nullptr != deleteCallback) {
                        deleteCallback(e->getValue());
                    }
                    delete e;
                }
            }

            // 从图的节点集合中移除删除掉的节点
            for (auto e : removeNode) {
                nodes.erase(e);
            }
        }

    protected:

        Node<T>* root;

        std::set<Node<T>*> nodes;

        void (*deleteCallback)(T) = nullptr;

    };

}

#endif
