#ifndef OP_H
#define OP_H

#include "GNode/Node.h"
#include "GeneticAlgorithm/Utils/GlobalCppRandomEngine.h"
#include <random>
#include <cmath>
#include <iostream>

class Op {

public:

    static const int ADD; // 加
    static const int SUB; // 减
    static const int PRO; // 乘
    static const int DES; // 除
    static const int SIN; // 正弦运算
    static const int END; // 停止操作

    static const int OP_OPERATION; // 运算符
    static const int OP_NUMBER; // 数字
    static const int OP_VARIABLE; // 自变量类型

    static const int OP_ATTR_LEFT; // 是左侧的数字或自变量
    static const int OP_ATTR_RIGHT; // 是右侧的数字或自变量

private:

    int opType; // OP_OPERATION or OP_NUMBER

    long double opNumber; // value of op if OP_NUMBER

    int opTypeNumber; // op type value if OP_OPERATION

    int numberLeftOrRight; // 是左侧的还是右侧的

    long double opNumberMin = 0.0;

    long double opNumberMax = 1.0;

public:

    static Op* getRandomNumberOp(long double min = 0.0, long double max = 1.0) {
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

    static Op* getRandomOp(long double min = 0.0, long double max = 1.0) {
        using namespace std;
        using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
        uniform_int_distribution<int> selectOpDistribution(1, 3); // 3 种，数字，操作，变量
        auto selectResult = selectOpDistribution(GlobalCppRandomEngine::engine);
        if (1 == selectResult) {
            return getRandomNumberOp(min, max);
        }
        if (2 == selectResult) {
            return getRandomOptionOp();
        }
        return new Op(OP_VARIABLE, (min + max) / 2, min, max); // 变量Op和数字Op比较类似
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
        return opType; // OP_OPERATION, OP_VARIABLE, OP_NUMBER
    }

    long double getValue() {
        return opNumber;
    }

    int getTypeValue() {
        return opTypeNumber; // ADD, SUB, PRO, DES, SIN, END
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

    /**
     * 返回当前的 Op 需要的子 Op 数目
     *
     * @return int
     */
    int getOperandTotal() {
        if (OP_NUMBER == opType) {
            return 0;
        }
        if (OP_VARIABLE == opType) {
            return 0;
        }
        if (OP_OPERATION == opType && SIN == opTypeNumber) {
            return 1;
        }
        return 2;
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
        if (OP_VARIABLE == opType) {
            cout << "x";
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
        if (SIN == opTypeNumber) {
            if (nullptr == left) {
                cout << "?";
                return;
            }
            cout << "sin(";
            left->print(nodeLeft);
            cout << ")";
            return;
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

    /**
     * 执行表达式计算
     *
     * @param Node<Op*>* node 节点
     * @param long double variable 自变量，可选，默认是0
     * @return long double
     */
    long double calculate(GNode::Node<Op*>* node, long double variable = 0.0) {
        using namespace std;
        if (OP_NUMBER == opType) {
            return opNumber;
        }
        if (OP_VARIABLE == opType) {
            return variable;
        }
        long double left = 0;
        long double right = 0;
        for (auto e : node->getNodes()) {
            if (OP_ATTR_LEFT == e->getValue()->getOpAttribute()) {
                left = e->getValue()->calculate(e, variable);
            } else {
                right = e->getValue()->calculate(e, variable);
            }
        }
        if (SIN == opTypeNumber) {
            return sin(left);
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
const int Op::SIN = 5;
const int Op::END = 6;

const int Op::OP_OPERATION = -1;
const int Op::OP_NUMBER = -2;
const int Op::OP_VARIABLE = -3;

const int Op::OP_ATTR_LEFT = -4;
const int Op::OP_ATTR_RIGHT = -5;

#endif
