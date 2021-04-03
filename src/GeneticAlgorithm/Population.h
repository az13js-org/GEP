#ifndef GENETICALGORITHM_POPULATION_H
#define GENETICALGORITHM_POPULATION_H

#include "Chromosome.h"
#include <iostream>
#include <algorithm>

namespace GeneticAlgorithm {

    /* 种群
     */
    class Population {

    public:
        // 创建种群，空的种群
        Population(unsigned long numberOfChromosome) {
            this->chromosomeArray = new Chromosome*[numberOfChromosome];
            for (unsigned long i = 0; i < numberOfChromosome; i++) { // 新建立的数组，任意的 this->chromosomeArray[i] 未必是 0x00
                this->chromosomeArray[i] = nullptr;
            }
            this->numberOfChromosome = numberOfChromosome;
        }

        // 删除，释放内存
        ~Population() {
            for (unsigned long i = 0; i < this->numberOfChromosome; i++) {
                if (nullptr != this->chromosomeArray[i]) {
                    delete this->chromosomeArray[i];
                }
            }
            delete[] this->chromosomeArray;
        }

        // 设置给定位置的个体
        bool setChromosome(unsigned long offset, Chromosome *chromosome) {
            if (offset >= this->numberOfChromosome) {
                return false;
            }
            if (nullptr == this->chromosomeArray[offset]) {
                this->chromosomeArray[offset] = chromosome;
                this->isMaxFitnessChromosomeCache = false;
                return true;
            }
            Chromosome* origin = this->chromosomeArray[offset];
            if ((void*)origin == (void*)chromosome) {
                return true;
            }
            if (this->isMaxFitnessChromosomeCache) {
                if ((void*)(this->maxFitnessChromosomeCache) == (void*)chromosome) {
                    return false;
                }
                if (this->maxFitnessChromosomeCache->getFitness() > chromosome->getFitness()) {
                    this->isMaxFitnessChromosomeCache = false;
                } else {
                    this->maxFitnessChromosomeCache = chromosome;
                }
            }
            this->chromosomeArray[offset] = chromosome;
            delete origin;
            return true;
        }

        // 替换给定位置的个体
        bool replaceChromosome(unsigned long offset, Chromosome *chromosome) {
            return this->setChromosome(offset, chromosome);
        }

        // 获取给定位置的个体
        Chromosome* getChromosome(unsigned long offset) {
            if (offset >= this->numberOfChromosome) {
                throw "Error, offset out of range, in \"Population::getChromosome\".";
            }
            if (!this->chromosomeArray[offset]) {
                throw "Null pointer exception.";
            }
            return this->chromosomeArray[offset];
        }

        // 获取种群的个体数量
        unsigned long getSize() {
            return this->numberOfChromosome;
        }

        // 获取Fitness最大值的Chromosome
        Chromosome* getMaxFitnessChromosome() {
            if (this->isMaxFitnessChromosomeCache) {
                return this->maxFitnessChromosomeCache;
            }
            long double maxFitness = std::numeric_limits<long double>::min();
            unsigned long offset = 0;
            this->maxFitnessChromosomeCache = this->chromosomeArray[offset];
            for (unsigned long i = 0; i < this->numberOfChromosome; i++) {
                if (this->chromosomeArray[i]->getFitness() > maxFitness) {
                    offset = i;
                    maxFitness = this->chromosomeArray[i]->getFitness();
                }
            }
            this->isMaxFitnessChromosomeCache = true;
            this->maxFitnessChromosomeCache = this->chromosomeArray[offset];
            this->maxFitnessChromosomeOffset = offset;
            return this->chromosomeArray[offset];
        }

        // 对种群中个体按照从大到小的顺序排序
        void sort() {
            std::sort(
                &(this->chromosomeArray[0]),
                &(this->chromosomeArray[this->numberOfChromosome]),
                [](Chromosome* a, Chromosome* b) -> bool {
                    return a->getFitness() > b->getFitness();
                }
            );
            this->isMaxFitnessChromosomeCache = true;
            this->maxFitnessChromosomeCache = this->chromosomeArray[0];
            this->maxFitnessChromosomeOffset = 0;
        }

    private:
        // 染色体数量
        unsigned long numberOfChromosome;
        // 存储所有染色体
        Chromosome** chromosomeArray;
        // 是否有效缓存了当前种群中最大的适应度个体的适应度
        bool isMaxFitnessChromosomeCache = false;
        // 缓存的当前种群最大适应度个体的适应度
        Chromosome* maxFitnessChromosomeCache;
        // 最大适应度个体的偏移位置
        unsigned long maxFitnessChromosomeOffset;

    };

}

#endif