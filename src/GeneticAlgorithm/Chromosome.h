#ifndef GENETICALGORITHM_CHROMOSOME_H
#define GENETICALGORITHM_CHROMOSOME_H

#include "../Dataset.h"
#include "Utils/GlobalCppRandomEngine.h"
#include <iostream>
#include <cmath>

namespace GeneticAlgorithm {

    /* 染色体
     */
    class Chromosome {

    private:

        /** @var unsigned long 保存了此染色体的长度 */
        unsigned long lengthOfData;

        /** @var long double* 保存了此染色体中基因的信息 */
        long double* dataArray;

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
            if (lengthOfChromosome < 3) {
                throw "Error, lengthOfChromosome must >= 3";
            }
            this->dataArray = new long double[lengthOfChromosome];
            this->lengthOfData = lengthOfChromosome;
        }

        /**
         * 删除染色体，释放内存
         */
        ~Chromosome() {
            delete[] this->dataArray;
        }

        /**
         * 设置给定位置基因的值
         *
         * @param unsigned long offset 位置，大于等于0小于染色体的长度
         * @param long double value 值
         * @return bool 成功返回 true
         */
        bool setGene(unsigned long offset, long double value) {
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
         * 获取给定位置的值
         *
         * @param unsigned long offset 位置，大于等于0小于染色体的长度
         * @return long double
         */
        long double getGene(unsigned long offset) {
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
            unsigned long unitNumber = this->lengthOfData / 3;
            unsigned long offsetBase = 0;
            long double k, a, b;
            for (unsigned long i = 0; i < unitNumber; i++) {
                offsetBase = 3 * i;
                k = this->dataArray[offsetBase];
                a = this->dataArray[offsetBase + 1];
                b = this->dataArray[offsetBase + 2];
                if (k >= 0 && i > 0) {
                    cout << "+";
                }
                cout << k << "*sin(";
                cout << a << "*x";
                if (b >= 0) {
                    cout << "+";
                }
                cout << b << ")";
            }
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
            using namespace std;
            if (this->isFitnessCached) {
                return this->fitnessCached;
            }
            unsigned long unitNumber = this->lengthOfData / 3;
            unsigned long offsetBase = 0;
            long double k, a, b;
            long double result;
            long double datasetInput, datasetOutput, datasetSize;
            long double errorSum = 0.0;
            auto dataset = Dataset::getInstance();
            datasetSize = dataset->getTotal();
            for (unsigned long j = 0; j < datasetSize; j++) {
                datasetInput = dataset->getInput(j);
                datasetOutput = dataset->getOutput(j);
                result = 0;
                for (unsigned long i = 0; i < unitNumber; i++) {
                    offsetBase = 3 * i;
                    k = this->dataArray[offsetBase];
                    a = this->dataArray[offsetBase + 1];
                    b = this->dataArray[offsetBase + 2];
                    result += (k * sin(a * datasetInput + b));
                }
                errorSum += abs(result - datasetOutput);
            }
            this->fitnessCached = 1.0L / (errorSum / (long double)datasetSize + 1.0L);
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
            if (another->getLength() != this->lengthOfData) {
                throw "Length not equals!";
            }
            auto newChromosome = new Chromosome(this->lengthOfData);
            for (unsigned long i = 0; i < this->lengthOfData; i++) {
                newChromosome->setGene(i, (this->dataArray[i] + another->getGene(i)) / 2.0);
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
            if (r <= 0.0) {
                return;
            }
            using GeneticAlgorithm::Utils::GlobalCppRandomEngine;
            std::normal_distribution<long double> distribution(0, r);
            for (unsigned long i = 0; i < this->lengthOfData; i++) {
                this->dataArray[i] += distribution(GlobalCppRandomEngine::engine);
            }
        }

    };

}

#endif
