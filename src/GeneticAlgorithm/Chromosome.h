#ifndef GENETICALGORITHM_CHROMOSOME_H
#define GENETICALGORITHM_CHROMOSOME_H

#include "../Op.h"
#include "../GNode/Tree.h"
#include "../GNode/Node.h"
#include "Utils/GlobalCppRandomEngine.h"
#include <iostream>
#include <queue>

namespace GeneticAlgorithm {

    /* 染色体
     */
    class Chromosome {

    private:

        /** @var unsigned long 保存了此染色体的长度 */
        unsigned long lengthOfData;

        /** @var Op** 保存了此染色体中基因的信息 */
        Op** dataArray;

        /** @var bool 为true时表示计算Fitness后缓存了计算结果，可以不用重复算 */
        bool isFitnessCached = false;

        /** @var long double 缓存的上一次的适应度计算结果。需要判断isFitnessCached以确定确实缓存下来了。 */
        long double fitnessCached;

    public:

        /**
         * 创建染色体
         *
         * 入参是染色体的长度
         *
         * @param unsigned long lengthOfChromosome
         */
        Chromosome(unsigned long lengthOfChromosome) {
            if (lengthOfChromosome < 8) {
                throw "Error, lengthOfChromosome must >= 8";
            }
            this->dataArray = new Op*[lengthOfChromosome];
            for (unsigned long i = 0; i < lengthOfChromosome; i++) {
                this->dataArray[i] = nullptr;
            }
            this->lengthOfData = lengthOfChromosome;
        }

        /**
         * 删除染色体，释放内存
         */
        ~Chromosome() {
            for (unsigned long i = 0; i < this->lengthOfData; i++) {
                if (nullptr != this->dataArray) {
                    delete this->dataArray[i];
                }
            }
            delete[] this->dataArray;
        }

        /**
         * 设置给定位置的 Op 对象指针
         *
         * @param unsigned long offset 位置，大于等于0小于染色体的长度
         * @param Op* value Op 对象的指针
         * @return bool 成功返回 true
         */
        bool setGene(unsigned long offset, Op* value) {
            if (offset > this->lengthOfData - 1) {
                return false;
            }
            if (this->dataArray[offset] != value) {
                this->dataArray[offset] = value;
                this->isFitnessCached = false;
            }
            return true;
        }

        /**
         * 获取给定位置的 Op 对象的指针
         *
         * @param unsigned long offset 位置，大于等于0小于染色体的长度
         * @return Op* Op对象指针
         */
        Op* getGene(unsigned long offset) {
            if (offset > this->lengthOfData - 1) {
                throw "Error, out of range.";
            }
            return this->dataArray[offset];
        }

        /**
         * 打印调试信息
         *
         * @return void
         */
        void dump() {
            using namespace std;
            auto tree = this->buildTree();
            auto root = tree->getRoot();
            auto rootOp = root->getValue();
            rootOp->print(root);
            cout << "=" << rootOp->calculate(root) << endl;
            delete tree;
        }

        /**
         * 获取染色体长度
         *
         * @return unsigned long
         */
        unsigned long getLength() {
            return this->lengthOfData;
        }

        /**
         * 获取适应度
         *
         * @return long double
         */
        long double getFitness() {
            if (this->isFitnessCached) {
                return this->fitnessCached;
            }
            auto tree = this->buildTree();
            auto root = tree->getRoot();
            auto rootOp = root->getValue();
            auto different = 100.0L - rootOp->calculate(root);
            this->fitnessCached = 1.0L / (different * different + 1.0L);
            delete tree;
            this->isFitnessCached = true;
            return this->fitnessCached;
        }

        /**
         * 与另一个染色体交叉，返回新的染色体
         *
         * @param Chromosome* another 另一个染色体对象
         * @return Chromosome* 新的染色体对象，需要手动释放内存
         */
        Chromosome* crossover(Chromosome* another) {
            using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
            unsigned long beginOfTail = this->lengthOfData / 2 - 1;
            if (another->getLength() != this->lengthOfData) {
                throw "Length not equals!";
            }
            std::uniform_int_distribution<unsigned long> crossoverSplitDistribution(1, beginOfTail - 1);
            auto offset = crossoverSplitDistribution(GlobalCppRandomEngine::engine);
            auto newChromosome = new Chromosome(this->lengthOfData);
            for (unsigned long i = 0; i < offset; i++) {
                newChromosome->setGene(i, Op::createLike(this->dataArray[i]));
            }
            for (unsigned long i = offset; i < beginOfTail; i++) {
                newChromosome->setGene(i, Op::createLike(another->getGene(i)));
            }
            long double mixValue = 0.0, min = 0.0, max = 0.0;
            for (unsigned long i = beginOfTail; i < this->lengthOfData; i++) {
                min = this->getGene(i)->getMin();
                max = this->getGene(i)->getMax();
                mixValue = (this->getGene(i)->getValue() + another->getGene(i)->getValue()) / 2.0;
                newChromosome->setGene(i, new Op(Op::OP_NUMBER, mixValue, min, max));
            }
            return newChromosome;
        }

        /**
         * 以一定的概率r变异
         *
         * @param long double r
         * @return void
         */
        void mutation(long double r) {
            using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
            unsigned long beginOfTail = this->lengthOfData / 2 - 1;
            if (r <= 0.0) {
                return;
            }
            Op* oldGene;
            std::uniform_real_distribution<long double> p(0.0, 1.0);
            for (unsigned long i = 0; i < this->lengthOfData; i++) {
                if (p(GlobalCppRandomEngine::engine) <= r) {
                    this->isFitnessCached = false;
                    oldGene = this->dataArray[i];
                    if (i < beginOfTail) {
                        this->dataArray[i] = Op::getRandomOptionOp();
                    } else if (nullptr != oldGene) {
                        this->dataArray[i] = Op::getRandomNumberOp(oldGene->getMin(), oldGene->getMax());
                    } else {
                        this->dataArray[i] = Op::getRandomNumberOp();
                    }
                    if (nullptr != oldGene) {
                        delete oldGene;
                    }
                }
            }
        }

    private:

        /**
         * 根据染色体的信息，构造其对应的语法树
         *
         * @return Tree<Op*>*
         */
        GNode::Tree<Op*>* buildTree() {
            using namespace GNode;
            using namespace std;
            if (nullptr == this->dataArray[0]) {
                throw "Error, nullptr == this->dataArray[0], in Chromosome::buildTree().";
            }
            if (Op::END == this->dataArray[0]->getTypeValue()) {
                return new Tree<Op*>(new Op(Op::OP_NUMBER, 0.0L), [](Op* o) {delete o;});
            }
            queue<Node<Op*>*> hungryQueue;
            Node<Op*>* workingNode;
            Node<Op*>* childNode;
            Op* childNodeOp;
            int emptyChildNumber = 0;
            unsigned long offset = 1;
            unsigned long beginOfTail = this->lengthOfData / 2 - 1;
            auto tree = new Tree<Op*>(this->dataArray[0]);
            hungryQueue.push(tree->getRoot());
            while (!hungryQueue.empty()) {
                workingNode = hungryQueue.front();
                hungryQueue.pop();
                emptyChildNumber = Op::OP_OPERATION == workingNode->getValue()->getOpType() ? 2 : 0;
                while (emptyChildNumber > 0) {
                    childNodeOp = this->dataArray[offset];
                    if (Op::OP_OPERATION == childNodeOp->getOpType()) {
                        if (Op::END != childNodeOp->getTypeValue()) {
                            childNode = tree->create(childNodeOp);
                            hungryQueue.push(childNode);
                        } else {
                            offset = beginOfTail;
                            childNodeOp = this->dataArray[beginOfTail];
                            childNode = tree->create(childNodeOp);
                        }
                    } else {
                        childNode = tree->create(childNodeOp);
                    }
                    childNodeOp->setOpAttribute(2 == emptyChildNumber ? Op::OP_ATTR_LEFT : Op::OP_ATTR_RIGHT);
                    workingNode->add(childNode);
                    offset++;
                    emptyChildNumber--;
                    if (offset >= this->lengthOfData) {
                        throw "Error, out of size, in Chromosome::buildTree().";
                    }
                }
            }
            return tree;
        }

    };

}

#endif
