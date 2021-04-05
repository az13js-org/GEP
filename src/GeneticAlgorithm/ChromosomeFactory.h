#ifndef GENETICALGORITHM_CHROMOSOMEFACTORY_H
#define GENETICALGORITHM_CHROMOSOMEFACTORY_H

#include "Chromosome.h"
#include "Utils/GlobalCppRandomEngine.h"
#include <random>

namespace GeneticAlgorithm {

    /* 工厂类，用于创建Chromosome的实例
     */
    class ChromosomeFactory {

    public:

        /**
         * 从一个数组中创建染色体
         *         *
         * @param long double data[] 数组
         * @param unsigned long lengthOfData 数组的长度
         * @return Chromosome*
         */
        Chromosome* buildFromArray(long double data[], unsigned long lengthOfData) {
            auto buildChromosome = this->buildEmpty(lengthOfData);
            for (unsigned long i = 0; i < lengthOfData; i++) {
                buildChromosome->setGene(i, data[i]);
            }
            return buildChromosome;
        }

        /**
         * 随机地创建染色体
         *
         * @param unsigned long lengthOfData 染色体长度
         * @param long double numberOpMin 最小值
         * @param long double numberOpMax 最大值
         * @return Chromosome*
         */
        Chromosome* buildRandomChromosome(unsigned long lengthOfData, long double numberOpMin, long double numberOpMax) {
            using Utils::GlobalCppRandomEngine;
            std::uniform_real_distribution<long double> formateRandomNumberRange(numberOpMin, numberOpMax);
            auto buildChromosome = this->buildEmpty(lengthOfData);
            for (unsigned long i = 0; i < lengthOfData; i++) {
                buildChromosome->setGene(i, formateRandomNumberRange(GlobalCppRandomEngine::engine));
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
            auto result = this->buildEmpty(existsChromosome->getLength());
            for (unsigned long i = 0; i < result->getLength(); i++) {
                result->setGene(i, existsChromosome->getGene(i));
            }
            return result;
        }

    };

}

#endif
