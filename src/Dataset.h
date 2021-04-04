#ifndef DATASET_H
#define DATASET_H

#include "GeneticAlgorithm/Utils/GlobalCppRandomEngine.h"
#include <cmath>

/**
 * 数据集
 */
class Dataset {

private:

    /** @var Dataset* 存储 Dataset 对象的指针 */
    static Dataset* datasetInstance;

    /** @var long double* 存储目标函数输入 */
    long double* inputs = nullptr;

    /** @var long double* 存储目标函数输出 */
    long double* outputs = nullptr;

    /** @var long double 采样开始点，包含 */
    long double sampleStart;

    /** @var long double 采样结束点，包含 */
    long double sampleEnd;

    /** @var unsigned long 采样点总数 */
    unsigned long sampleTotal;

public:

    /**
     * 构建数据集
     *
     * @param long double sampleStart 采样开始的位置，含。默认值为 0
     * @param long double sampleEnd 采样结束的位置，含。默认为 1
     * @param unsigned long sampleTotal 采样点总数（不含采样开始和结束点）。默认为 0
     */
    Dataset(long double sampleStart = 0.0, long double sampleEnd = 1.0, unsigned long sampleTotal = 0) {
        this->sampleStart = sampleStart;
        this->sampleEnd = sampleEnd;
        this->sampleTotal = sampleTotal;
        unsigned long size = sampleTotal + 2;
        long double interval = (sampleEnd - sampleStart) / (long double)(sampleTotal + 1);
        inputs = new long double[size];
        outputs = new long double[size];
        inputs[0] = sampleStart;
        outputs[0] = sample(sampleStart);
        for (unsigned long i = 0; i < sampleTotal; i++) {
            inputs[i + 1] = interval * (i + 1);
            outputs[i + 1] = sample(inputs[i + 1]);
        }
        inputs[size - 1] = sampleEnd;
        outputs[size - 1] = sample(sampleEnd);
    }

    /**
     * 清除数据集
     */
    ~Dataset() {
        delete[] inputs;
        delete[] outputs;
        inputs = nullptr;
        outputs = nullptr;
        datasetInstance = nullptr;
    }

    /**
     * 对目标函数进行采样计算
     *
     * @param long double x 输入
     * @return long double 输出
     */
    long double sample(long double x) {
        using namespace std;
        return 0.4 * (sin(10.0 * x) + sin(20.0 * x) + sin(30.0 * x));
    }

    /**
     * 获取 Dataset 类的唯一实例
     *
     * @return Dataset*
     */
    static Dataset* getInstance() {
        if (nullptr != datasetInstance) {
            return datasetInstance;
        }
        datasetInstance = new Dataset(0.0L, 1.0L, 100);
        return datasetInstance;
    }

    /**
     * 返回数据集大小
     *
     * @return unsigned long
     */
    unsigned long getTotal() {
        return sampleTotal + 2;
    }

    /**
     * 返回数据集中的第 i + 1 个输入数据
     *
     * @param unsigned long i
     * @return long double
     */
    long double getInput(unsigned long i) {
        return i < (sampleTotal + 2) ? inputs[i] : 0;
    }

    /**
     * 返回数据集中的第 i + 1 个输出数据
     *
     * @param unsigned long i
     * @return long double
     */
    long double getOutput(unsigned long i) {
        return i < (sampleTotal + 2) ? outputs[i] : 0;
    }

};

Dataset* Dataset::datasetInstance = nullptr;

#endif
