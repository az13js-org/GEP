#ifndef OP_H
#define OP_H

#include "GNode/Node.h"
#include "GeneticAlgorithm/Utils/GlobalCppRandomEngine.h"
#include <random>
#include <iostream>

class Op {

public:

    static const int ADD; // 加
    static const int SUB; // 减
    static const int PRO; // 乘
    static const int DES; // 除
    static const int END; // 停止操作

    static const int OP_OPERATION; // 运算符
    static const int OP_NUMBER; // 数字

    static const int OP_ATTR_LEFT; // 是左侧的数字
    static const int OP_ATTR_RIGHT; // 是右侧的数字

private:

    int opType; // OP_OPERATION or OP_NUMBER

    long double opNumber; // value of op if OP_NUMBER

    int opTypeNumber; // op type value if OP_OPERATION

    int numberLeftOrRight; // 是左侧的还是右侧的

    long double opNumberMin = 0.0;

    long double opNumberMax = 1.0;

public:

    static Op* getRandomNumberOp(long double min, long double max) {
        using namespace std;
        using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
        uniform_real_distribution<long double> realDistribution(min, max);
        return new Op(Op::OP_NUMBER, realDistribution(GlobalCppRandomEngine::engine), min, max);
    }

    static Op* getRandomOptionOp() {
        using namespace std;
        using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
        uniform_int_distribution<int> opTypeDistribution(ADD, END);
        return new Op(Op::OP_OPERATION, opTypeDistribution(GlobalCppRandomEngine::engine));
    }

    static Op* getRandomOp(long double min, long double max) {
        using namespace std;
        using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
        bernoulli_distribution boolDistribution(0.5);
        if (boolDistribution(GlobalCppRandomEngine::engine)) { // is OP_NUMBER
            return getRandomNumberOp(min, max);
        }
        return getRandomOptionOp();
    }

    static Op* createLike(Op* source) {
        if (OP_OPERATION == source->getOpType()) {
            return new Op(source->getOpType(), source->getTypeValue());
        }
        return new Op(source->getOpType(), source->getValue(), source->getMin(), source->getMax());
    }

    Op(int opOpType, long double number, long double min, long double max) {
        opType = opOpType;
        opNumber = number;
        opNumberMin = min;
        opNumberMax = max;
    }

    Op(int opOpType, long double number) {
        opType = opOpType;
        opNumber = number;
    }

    Op(int opOpType, int number) {
        opType = opOpType;
        opTypeNumber = number;
    }

    int getOpType() {
        return opType;
    }

    long double getValue() {
        return opNumber;
    }

    int getTypeValue() {
        return opTypeNumber;
    }

    void setOpAttribute(int opNumberAttribute) {
        numberLeftOrRight = opNumberAttribute;
    }

    int getOpAttribute() {
        return numberLeftOrRight;
    }

    long double getMin() {
        return opNumberMin;
    }

    long double getMax() {
        return opNumberMax;
    }

    void print(GNode::Node<Op*>* node) {
        using namespace std;
        using namespace GNode;
        if (OP_NUMBER == opType) {
            if (opNumber < 0) {
                cout << "(";
            }
            cout << opNumber;
            if (opNumber < 0) {
                cout << ")";
            }
            return;
        }
        Op* left = nullptr;
        Op* right = nullptr;
        Node<Op*>* nodeLeft = nullptr;
        Node<Op*>* nodeRight = nullptr;
        for (auto e : node->getNodes()) {
            if (OP_ATTR_LEFT == e->getValue()->getOpAttribute()) {
                left = e->getValue();
                nodeLeft = e;
            }
            if (OP_ATTR_RIGHT == e->getValue()->getOpAttribute()) {
                right = e->getValue();
                nodeRight = e;
            }
        }
        if (nullptr == left || nullptr == right) {
            cout << "?";
            return;
        }
        cout << "(";
        left->print(nodeLeft);
        if (ADD == opTypeNumber) {
            cout << "+";
        }
        if (SUB == opTypeNumber) {
            cout << "-";
        }
        if (PRO == opTypeNumber) {
            cout << "*";
        }
        if (DES == opTypeNumber) {
            cout << "/";
        }
        right->print(nodeRight);
        cout << ")";
    }

    long double calculate(GNode::Node<Op*>* node) {
        using namespace std;
        if (OP_NUMBER == opType) {
            return opNumber;
        }
        long double left = 0;
        long double right = 0;
        for (auto e : node->getNodes()) {
            if (OP_ATTR_LEFT == e->getValue()->getOpAttribute()) {
                left = e->getValue()->calculate(e);
            } else {
                right = e->getValue()->calculate(e);
            }
        }
        if (ADD == opTypeNumber) {
            return left + right;
        }
        if (SUB == opTypeNumber) {
            return left - right;
        }
        if (PRO == opTypeNumber) {
            return left * right;
        }
        if (right < 1E-18) {
            return 0;
        }
        return left / right;
    }

};

const int Op::ADD = 1;
const int Op::SUB = 2;
const int Op::PRO = 3;
const int Op::DES = 4;
const int Op::END = 5;

const int Op::OP_OPERATION = -1;
const int Op::OP_NUMBER = -2;

const int Op::OP_ATTR_LEFT = -3;
const int Op::OP_ATTR_RIGHT = -4;

#endif
