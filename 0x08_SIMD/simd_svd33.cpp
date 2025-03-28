#include "oneapi/tbb/partitioner.h"
#include <Eigen/Dense>
#include <benchmark/benchmark.h>
#include <iostream>
#include <spdlog/fmt/fmt.h>
#include <fstream>
#include <chrono>

#include <tbb/tbb.h>
#include <igl/svd3x3.h>
#include <igl/svd3x3_sse.h>
#include <igl/svd3x3_avx.h>

using mat33 = Eigen::Matrix<float, 3, 3>;
using vec3 = Eigen::Matrix<float, 3, 1>;

std::vector<mat33> U_res, V_res;
std::vector<vec3> S_res;

std::vector<Eigen::Matrix<float, 3*4, 3>> A4_vec;
std::vector<Eigen::Matrix<float, 3*4, 3>> U4_vec, V4_vec;
std::vector<Eigen::Matrix<float, 3*4, 1>> S4_vec;
std::vector<Eigen::Matrix<float, 3*8, 3>> A8_vec;
std::vector<Eigen::Matrix<float, 3*8, 3>> U8_vec, V8_vec;
std::vector<Eigen::Matrix<float, 3*8, 1>> S8_vec;

std::vector<mat33>& LoadData() {
    static std::vector<mat33> A_vec;
    std::ifstream file("Dataset_1M.txt");
    int n;
    file >> n;
    A_vec.resize(n);
    U_res.resize(n);
    V_res.resize(n);
    S_res.resize(n);
    for (int i = 0; i < n; i++) 
        for (int j = 0; j < 9; j++) 
            file >> A_vec[i](j / 3, j % 3);
    
    A4_vec.resize(n/4);
    U4_vec.resize(n/4);
    V4_vec.resize(n/4);
    S4_vec.resize(n/4);
    for (int i = 0; i < n/4; i++) 
        for (int j = 0; j < 4; j++)
            A4_vec[i].block<3, 3>(j * 3, 0) = A_vec[i*4+j]; 

    A8_vec.resize(n/8);
    U8_vec.resize(n/8);
    V8_vec.resize(n/8);
    S8_vec.resize(n/8);
    for (int i = 0; i < n/8; i++) 
        for (int j = 0; j < 8; j++)
            A8_vec[i].block<3, 3>(j * 3, 0) = A_vec[i*8+j];
    
    return A_vec;
}

std::vector<mat33>& A_vec = LoadData();


struct ErrorResult {
    double sum_err = 0;
    double max_err = 0;
};
ErrorResult EvalError() {
    // double sum_err = 0, max_err = 0;
    // for (int i = 0; i < A_vec.size(); i++) {
    //     Eigen::JacobiSVD<mat33> svd(A_vec[i], Eigen::ComputeFullU | Eigen::ComputeFullV);
    //     const mat33& U = svd.matrixU();
    //     const mat33& V = svd.matrixV();
    //     const vec3& S = svd.singularValues();
    //     double curr_err = (U.cwiseAbs() - U_res[i].cwiseAbs()).norm() + (V.cwiseAbs() - V_res[i].cwiseAbs()).norm() + (S.cwiseAbs() - S_res[i].cwiseAbs()).norm();
    //     // if (curr_err > 1e-1) {
    //     //     std::cout << "Error: " << curr_err << std::endl;
    //     //     std::cout << "U: \n" << U << std::endl;
    //     //     std::cout << "U_res: \n" << U_res[i] << std::endl;
    //     //     std::cout << "V: \n" << V << std::endl;
    //     //     std::cout << "V_res: \n" << V_res[i] << std::endl;
    //     //     std::cout << "S: \n" << S << std::endl;
    //     //     std::cout << "S_res: \n" << S_res[i] << std::endl;
    //     //     exit(0);
    //     // }
    //     sum_err += curr_err;
    //     max_err = std::max(max_err, curr_err);
    // }
    ErrorResult ret = tbb::parallel_reduce(tbb::blocked_range<size_t>(0, A_vec.size()), ErrorResult{0, 0},
        [&](const tbb::blocked_range<size_t>& range, ErrorResult res) {
            for (size_t i = range.begin(); i < range.end(); i++) {
                Eigen::JacobiSVD<mat33> svd(A_vec[i], Eigen::ComputeFullU | Eigen::ComputeFullV);
                const mat33& U = svd.matrixU();
                const mat33& V = svd.matrixV();
                const vec3& S = svd.singularValues();
                double curr_err = (U.cwiseAbs() - U_res[i].cwiseAbs()).norm() + (V.cwiseAbs() - V_res[i].cwiseAbs()).norm() + (S.cwiseAbs() - S_res[i].cwiseAbs()).norm();
                res.sum_err += curr_err;
                res.max_err = std::max(res.max_err, curr_err);
            }
            return res;
        },
        [](ErrorResult a, ErrorResult b) {
            a.sum_err += b.sum_err;
            a.max_err = std::max(a.max_err, b.max_err);
            return a;
        }
    );
    return ret;
}

class SVD33_eigen {
public:
    void operator()() {
        for (int i = 0; i < A_vec.size(); i++) {
            Eigen::JacobiSVD<mat33> svd(A_vec[i], Eigen::ComputeFullU | Eigen::ComputeFullV);
            const mat33& U = svd.matrixU();
            const mat33& V = svd.matrixV();
            const vec3& S = svd.singularValues();
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        }
    }
};
class SVD33_eigen_parallel {
public:
    void operator()() {
        tbb::parallel_for(0, int(A_vec.size()), [&](int i) {
            Eigen::JacobiSVD<mat33> svd(A_vec[i], Eigen::ComputeFullU | Eigen::ComputeFullV);
            const mat33& U = svd.matrixU();
            const mat33& V = svd.matrixV();
            const vec3& S = svd.singularValues();
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        });
    }
};

class SVD33_igl_scalar {
public:
    void operator()() {
        for (int i = 0; i < A_vec.size(); i++) {
            mat33 U, V;
            vec3 S;
            igl::svd3x3(A_vec[i], U, S, V);
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        }
    }
};
class SVD33_igl_scalar_parallel {
public:
    void operator()() {
        tbb::parallel_for(0, int(A_vec.size()), [&](int i) {
            mat33 U, V;
            vec3 S;
            igl::svd3x3(A_vec[i], U, S, V);
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        });
    }
};

class SVD33_igl_sse {
public:
    void operator()() {
        int i = 0;
        // Eigen::Matrix<float, 3*4, 3> A;
        Eigen::Matrix<float, 3*4, 3> U;
        Eigen::Matrix<float, 3*4, 3> V;
        Eigen::Matrix<float, 3*4, 1> S;
        for (i = 0; i < A_vec.size(); i+=4) {
            // A.block<3, 3>(0, 0) = A_vec[i];
            // A.block<3, 3>(3, 0) = A_vec[i+1];
            // A.block<3, 3>(6, 0) = A_vec[i+2];
            // A.block<3, 3>(9, 0) = A_vec[i+3];
            igl::svd3x3_sse(A4_vec[i/4], U, S, V);
            for (int k = 0; k < 4; k++) {
                U_res[i+k] = U.block<3, 3>(k * 3, 0);
                V_res[i+k] = V.block<3, 3>(k * 3, 0);
                S_res[i+k] = S.block<3, 1>(k * 3, 0);
            }
        }
        for (i = i-4+1; i < A_vec.size(); i++) {
            mat33 U, V;
            vec3 S;
            igl::svd3x3(A_vec[i], U, S, V);
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        }
    }
};
class SVD33_igl_sse_parallel {
public:
    void operator()() {
        tbb::parallel_for(0, int(A4_vec.size()), [&](int i) {
            // igl::svd3x3_sse(A4_vec[i], U4_vec[i], S4_vec[i], V4_vec[i]);
            thread_local Eigen::Matrix<float, 3*4, 3> U;
            thread_local Eigen::Matrix<float, 3*4, 3> V;
            thread_local Eigen::Matrix<float, 3*4, 1> S;
            igl::svd3x3_sse(A4_vec[i], U, S, V);
            for (int k = 0; k < 4; k++) {
                U_res[i*4+k] = U.block<3, 3>(k * 3, 0);
                V_res[i*4+k] = V.block<3, 3>(k * 3, 0);
                S_res[i*4+k] = S.block<3, 1>(k * 3, 0);
            }
        });
        for (int i = int(A_vec.size())/4*4; i < A_vec.size(); i++) {
            mat33 U, V;
            vec3 S;
            igl::svd3x3(A_vec[i], U, S, V);
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        }
    }
};

class SVD33_igl_avx {
public:
    void operator()() {
        int i = 0;
        // Eigen::Matrix<float, 3*8, 3> A;
        Eigen::Matrix<float, 3*8, 3> U;
        Eigen::Matrix<float, 3*8, 3> V;
        Eigen::Matrix<float, 3*8, 1> S;
        for (i = 0; i < A_vec.size(); i+=8) {
            // for (int k = 0; k < 8; k++) {
            //     A.block<3, 3>(k * 3, 0) = A_vec[i+k];
            // }
            igl::svd3x3_avx(A8_vec[i/8], U, S, V);
            for (int k = 0; k < 8; k++) {
                U_res[i+k] = U.block<3, 3>(k * 3, 0);
                V_res[i+k] = V.block<3, 3>(k * 3, 0);
                S_res[i+k] = S.block<3, 1>(k * 3, 0);
            }
        }
        for (i = i-8+1; i < A_vec.size(); i++) {
            mat33 U, V;
            vec3 S;
            igl::svd3x3(A_vec[i], U, S, V);
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        }
    }
};

class SVD33_igl_avx_parallel {
public:
    void operator()() {
        tbb::parallel_for(tbb::blocked_range<size_t>(0, A8_vec.size()), [&](const tbb::blocked_range<size_t>& range) {
            thread_local Eigen::Matrix<float, 3*8, 3> U;
            thread_local Eigen::Matrix<float, 3*8, 3> V;
            thread_local Eigen::Matrix<float, 3*8, 1> S;
            for (size_t i = range.begin(); i < range.end(); ++i) {
                // __builtin_prefetch(&A8_vec[i+1], 0, 3);  // 预取下一个数据块
                // igl::svd3x3_avx(A8_vec[i], U8_vec[i], S8_vec[i], V8_vec[i]);
                igl::svd3x3_avx(A8_vec[i], U, S, V);
                for (int k = 0; k < 8; k++) {
                    U_res[i*8+k] = U.block<3, 3>(k * 3, 0);
                    V_res[i*8+k] = V.block<3, 3>(k * 3, 0);
                    S_res[i*8+k] = S.block<3, 1>(k * 3, 0);
                }
            }
        }, tbb::auto_partitioner());
        for (int i = int(A8_vec.size())*8; i < A_vec.size(); i++) {
            mat33 U, V;
            vec3 S;
            igl::svd3x3(A_vec[i], U, S, V);
            U_res[i] = U, V_res[i] = V, S_res[i] = S;
        }
    }
};

static std::string CyanHead() { return "\x1b[6;30;96m"; }
static std::string CyanTail() { return "\x1b[0m"; }

template <typename Method>
void BM_Test(benchmark::State& state) {
    // 在主函数或需要限制线程的地方
    tbb::global_control global_limit(
        tbb::global_control::max_allowed_parallelism, 
        16
    );
    auto start = std::chrono::high_resolution_clock::now();
    Method method;
    for (auto _ : state) {
        method();
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    double avg_ns_per_svd = double(duration.count()) / (A_vec.size() * state.iterations());
    auto [sum_err, max_err] = EvalError();    
    std::string info = fmt::format("avg_ns_per_svd: {}{:6.2f} ns{}, sum_error: {:6.2f}, max_error: {:5.2f}", CyanHead(), avg_ns_per_svd, CyanTail(), sum_err, max_err);
    state.SetLabel(info);
}

#define REGISTER_BENCHMARK(Method, MethodName) \
    BENCHMARK_TEMPLATE(BM_Test, Method)->Name(MethodName)->Unit(benchmark::kMillisecond)

REGISTER_BENCHMARK(SVD33_eigen, "eigen");
REGISTER_BENCHMARK(SVD33_igl_scalar, "igl_scalar");
REGISTER_BENCHMARK(SVD33_igl_sse, "igl_sse");
REGISTER_BENCHMARK(SVD33_igl_avx, "igl_avx");

REGISTER_BENCHMARK(SVD33_eigen_parallel, "eigen_parallel");
REGISTER_BENCHMARK(SVD33_igl_scalar_parallel, "igl_scalar_parallel");
REGISTER_BENCHMARK(SVD33_igl_sse_parallel, "igl_sse_parallel");
REGISTER_BENCHMARK(SVD33_igl_avx_parallel, "igl_avx_parallel");

BENCHMARK_MAIN();