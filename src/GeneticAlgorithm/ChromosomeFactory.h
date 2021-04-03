#ifndef GENETICALGORITHM_CHROMOSOMEFACTORY_H
#define GENETICALGORITHM_CHROMOSOMEFACTORY_H

#include "../Op.h"
#include "Chromosome.h"
#include "Utils/GlobalCppRandomEngine.h"
#include <random>
#include <iostream>

namespace GeneticAlgorithm {

    /* 工厂类，用于创建Chromosome的实例
     */
    class ChromosomeFactory {

    public:

        /**
         * 从一个 Op对象指针的数组中创建染色体
         *
         * 会拷贝 Op 对象，而不是直接保留引用
         *
         * @param Op* data[] 数组
         * @param unsigned long lengthOfData 数组的长度
         * @return Chromosome*
         */
        Chromosome* buildFromArray(Op* data[], unsigned long lengthOfData) {
            Op* tmpOp;
            Chromosome* buildChromosome = this->buildEmpty(lengthOfData);
            for (unsigned long i = 0; i < lengthOfData; i++) {
                tmpOp = Op::createLike(data[i]);
                if (!buildChromosome->setGene(i, tmpOp)) {
                    delete tmpOp;
                    throw "Error, \"Chromosome::setGene\" return false. Exception throw in method \"ChromosomeFactory::buildFromArray\".";
                }
            }
            return buildChromosome;
        }

        /**
         * 随机地创建染色体
         *
         * @param unsigned long lengthOfData 染色体长度
         * @param long double numberOpMin 数字 Op 的最小值
         * @param long double numberOpMax 数字 Op 的最大值
         * @return Chromosome*
         */
        Chromosome* buildRandomChromosome(unsigned long lengthOfData, long double numberOpMin, long double numberOpMax) {
            // 尾部的数字 OP 数量至少等于头部的操作 OP 的数量 + 2
            if (lengthOfData < 4) {
                throw "lengthOfData must >= 4";
            }
            unsigned long beginOfTail = lengthOfData / 2 - 1;
            auto buildChromosome = this->buildEmpty(lengthOfData);
            for (unsigned long i = 0; i < beginOfTail; i++) {
                buildChromosome->setGene(i, Op::getRandomOptionOp());
            }
            for (unsigned long i = beginOfTail; i < lengthOfData; i++) {
                buildChromosome->setGene(i, Op::getRandomNumberOp(numberOpMin, numberOpMax));
            }
            return buildChromosome;
        }

        /**
         * 创建空的染色体，其中实数都初始化为 0
         *
         * @param unsigned long lengthOfData 染色体中存储随机实数的个数
         * @return Chromosome*
         */
        Chromosome* buildEmpty(unsigned long lengthOfData) {
            return new Chromosome(lengthOfData);
        }

        /**
         * 深度拷贝创建
         *
         * @param Chromosome* existsChromosome
         * @return Chromosome*
         */
        Chromosome* buildFromChromosome(Chromosome* existsChromosome) {
            Chromosome* result = this->buildEmpty(existsChromosome->getLength());
            for (unsigned long i = 0; i < result->getLength(); i++) {
                result->setGene(i, Op::createLike(existsChromosome->getGene(i)));
            }
            return result;
        }

    };

}

#endif
