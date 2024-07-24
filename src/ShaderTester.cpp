#include "ShaderTester.h"
#include "Shader.h"
#include <numeric>
static float CPUSum_X(float* inputDATA, uint len){
    double sumArray = 0.0f;//float would get the incorrect result when len is greater than like 10k
    for (uint i = 0; i < len; i++)
    {
        sumArray = sumArray + inputDATA[i];
    }
    
    return sumArray;
}

static float CPUSum_X_range(float* inputDATA, uint begin, uint end) {
    double sumArray = 0.0f;
    for (uint i = begin; i < end; i++)
    {
        sumArray = sumArray + inputDATA[i];
    }

    return sumArray;
}

static void generateInputData(float* inputDATA, uint len){
    for (uint i = 0; i < len; i++)
    {
        inputDATA[i] = i*1.0f;
    }
}
void ShaderTester::init() {
    /*create shaders
    */
    string shaderFile = resourceFolder + std::string("shaders/calYSumArrayRGBATexture.comp");
    sumOfArrayShader = std::make_shared<Shader>();
    sumOfArrayShader->setComputeShader(shaderFile.c_str());
    if (!sumOfArrayShader->isValid()) {
        printf("failed to create shader: %s\n", shaderFile.c_str());
    }
    else {
        printf("succeeded to create shader: %s  programid = %d\n", shaderFile.c_str(), sumOfArrayShader->program);
    }



    /*Init data that one of the shaders needs.  The form of data could be a texture,
    a ssbo buffer, etc. Usually we can classify the data into two types. One is the input, 
    the other is the output.
    */
    Size_X = 256;
    Size_Y = 1;
    Size_Z = 1;
    inputDataSize = 1004*10;
    outputDataSize = 1;
    work_groups_x =  singleGroupOutputDataSize = (inputDataSize + Size_X - 1) / Size_X;
    work_groups_y = 1;
    work_groups_z = 1;
    alignedDataSize = work_groups_x*Size_X;
    inputData = new float[alignedDataSize];
    //memset(inputData, 0, sizeof(float)*alignedDataSize);
    for (size_t i = 0; i < alignedDataSize; i++)
    {
        inputData[i] = 0.0f;
    }
    generateInputData(inputData, inputDataSize);
    
    float initSum = 0;
    //expectedSum[0] = std::accumulate(inputData, inputData + inputDataSize, initSum);
    expectedSum[0] = CPUSum_X_range(inputData, 0, inputDataSize);
    expectedSum[1] = CPUSum_X(inputData, alignedDataSize);
    printf("The actual inputDataSize = %d\n", inputDataSize);
    printf("The actual alignedDataSize = %d\n", alignedDataSize);
    printf("The expected dataSum with actual size = %.3f\n", expectedSum[0]);
    printf("The expected dataSum with alignedDataSize = %.3f\n", expectedSum[1]);
    uint index = 0;
    float totalSum = 0;
    while (index < alignedDataSize)
    {
        float curRangeSum = CPUSum_X_range(inputData, index, index + Size_X);
        printf("range at index %d-%d, sum = %.3f \n", index, index + Size_X, curRangeSum);
        index += Size_X;
        totalSum += curRangeSum;
    }
    printf("total Sum of the sums of all ranges is %.3f\n\n", totalSum);
    glGenBuffers(1, &inputDataBufferToSum);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, inputDataBufferToSum);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * alignedDataSize, inputData, GL_DYNAMIC_COPY);
    glGenBuffers(1, &outputDataBufferSummed);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputDataBufferSummed);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * outputDataSize, nullptr, GL_DYNAMIC_COPY);
    glGenBuffers(1, &singleGroupOutputBuffer);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, singleGroupOutputBuffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * singleGroupOutputDataSize, nullptr, GL_DYNAMIC_COPY);



    sumOfArrayShader->Use();
    //sumOfArrayShader->setUniform1ui("inputDataSize", inputDataSize); CHECK_GL;
    sumOfArrayShader->setUniform1ui("numGroupX", work_groups_x); CHECK_GL;
}


void ShaderTester::run(float w, float h) {
    float sumArray = 0;
    for (uint i = 0; i < inputDataSize; i++)
    {
        sumArray = sumArray + inputData[i];
    }
    expectedSum[0] = sumArray; 
    printf("The actual inputDataSize = %d\n", inputDataSize);
    printf("The expected dataSum with actual size = %.3f\n", sumArray);

    sumOfArrayShader->Use();
    //sumOfArrayShader->
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, inputDataBufferToSum);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, outputDataBufferSummed);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, singleGroupOutputBuffer);
    glDispatchCompute(work_groups_x, work_groups_y, work_groups_z); CHECK_GL;
    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT | GL_SHADER_STORAGE_BARRIER_BIT);
    //glFinish();


    glBindBuffer(GL_SHADER_STORAGE_BUFFER, outputDataBufferSummed);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, outputDataBufferSummed);
    float* bufSum = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * outputDataSize, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT); CHECK_GL;
    if(bufSum){
        outputData[0] = bufSum[0];
        printf("the out sum = %.3f\n", outputData[0]);
        bufSum[0] = 0;
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, singleGroupOutputBuffer);
    //glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, singleGroupOutputBuffer);
    float* sumsOfSingleGroup = (float*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float) * work_groups_x, GL_MAP_READ_BIT ); CHECK_GL;
    if (sumsOfSingleGroup) {
        float sumOfAll = 0;
        for (int i = 0; i < work_groups_x; i++)
        {
            printf("the out sumsOfSingleGroup at %d,  = %.3f\n", i, sumsOfSingleGroup[i]);
            //sumsOfSingleGroup[i] = 0;
            sumOfAll += sumsOfSingleGroup[i];
            sumsOfSingleGroup[i] = 0;
        }
        printf("The sum of all the sumsOfSingleGroup is %.3f\n", sumOfAll);
    }
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}
