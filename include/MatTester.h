#pragma once
#include "Common.h"
#include <iostream>
#include <vector>
#include "RenderState.h"
#include <string>
using std::string;
using std::vector;
class Shader;


class MATTester{
public:
    void lduDecomposition(vector<vector<double>>& matrix, vector<vector<double>>& L, 
    vector<vector<double>>& D, vector<vector<double>>& U);
    void luDecomposition(const std::vector<std::vector<double>>& A, 
    std::vector<std::vector<double>>& L, 
    std::vector<std::vector<double>>& U);

    void testldu();
    void testLUDecomposition();
};