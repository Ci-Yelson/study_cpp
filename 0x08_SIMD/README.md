## SIMD Mat 4x4 Multiplication 

### WSL - AMD Ryzen 7 3700X 8-Core Processor - 16 CPU(s) - [19.25 ns -> 5.79 ns]
```
Run on (16 X 3593.3 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 16384 KiB (x1)
Load Average: 0.57, 0.27, 0.10
-------------------------------------------------------------
Benchmark                   Time             CPU   Iterations
-------------------------------------------------------------
ArBr_seq                0.076 ms        0.077 ms         9098 avg_ns_per_multi: 18.99 ns, Error:  0.00
ArBr_eigen              0.021 ms        0.021 ms        32887 avg_ns_per_multi:  5.33 ns, Error:  0.00
ArBr_simd01             0.074 ms        0.074 ms         7905 avg_ns_per_multi: 18.54 ns, Error:  0.00
ArBr_simd_reg           0.029 ms        0.028 ms        24342 avg_ns_per_multi:  7.34 ns, Error:  0.00
ArBr_simd_reg_fmad      0.016 ms        0.016 ms        44232 avg_ns_per_multi:  4.09 ns, Error:  0.00

ArBc_seq                0.079 ms        0.077 ms         9013 avg_ns_per_multi: 19.75 ns, Error:  0.00
ArBc_eigen              0.035 ms        0.034 ms        20413 avg_ns_per_multi:  8.68 ns, Error:  0.00
ArBc_simd01             0.065 ms        0.064 ms        11025 avg_ns_per_multi: 16.31 ns, Error:  0.00
ArBc_simd_reg           0.024 ms        0.024 ms        28884 avg_ns_per_multi:  6.12 ns, Error:  0.00
ArBc_simd_reg_fmad      0.024 ms        0.023 ms        30039 avg_ns_per_multi:  5.89 ns, Error:  0.00

AcBr_seq                0.078 ms        0.078 ms         9056 avg_ns_per_multi: 19.52 ns, Error:  0.00
AcBr_eigen              0.023 ms        0.022 ms        31366 avg_ns_per_multi:  5.68 ns, Error:  0.00
AcBr_simd01             0.096 ms        0.095 ms         7406 avg_ns_per_multi: 24.07 ns, Error:  0.00
AcBr_simd_reg           0.035 ms        0.035 ms        19900 avg_ns_per_multi:  8.85 ns, Error:  0.00
AcBr_simd_reg_fmad      0.017 ms        0.017 ms        42239 avg_ns_per_multi:  4.13 ns, Error:  0.00

AcBc_seq                0.077 ms        0.078 ms         8711 avg_ns_per_multi: 19.25 ns, Error:  0.00
AcBc_eigen              0.022 ms        0.022 ms        31097 avg_ns_per_multi:  5.54 ns, Error:  0.00
AcBc_simd01             0.073 ms        0.074 ms         8936 avg_ns_per_multi: 18.30 ns, Error:  0.00
AcBc_simd_reg           0.028 ms        0.029 ms        23890 avg_ns_per_multi:  7.05 ns, Error:  0.00
AcBc_simd_reg_fmad      0.023 ms        0.023 ms        29637 avg_ns_per_multi:  5.79 ns, Error:  0.00
```

## SIMD Mat 3x3 SVD with TBB

### WSL - AMD Ryzen 7 3700X 8-Core Processor - 16 CPU(s) - [39.49 ns -> 9.87 ns]
```
Run on (16 X 3593.3 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x8)
  L1 Instruction 32 KiB (x8)
  L2 Unified 512 KiB (x8)
  L3 Unified 16384 KiB (x1)
Load Average: 0.41, 0.26, 0.15
-----------------------------------------------------
Benchmark           Time             CPU   Iterations
-----------------------------------------------------
eigen                      597 ms          597 ms            1 avg_ns_per_svd: 569.47 ns, sum_error:   0.00, max_error:  0.00
igl_scalar                 450 ms          450 ms            2 avg_ns_per_svd: 429.23 ns, sum_error: 123.00, max_error:  0.39
igl_sse                   77.5 ms         77.5 ms            9 avg_ns_per_svd:  73.93 ns, sum_error: 123.00, max_error:  0.39
igl_avx                   44.3 ms         44.3 ms           16 avg_ns_per_svd:  42.27 ns, sum_error: 123.00, max_error:  0.39

eigen_parallel            41.4 ms         41.4 ms           17 avg_ns_per_svd:  39.49 ns, sum_error:   0.00, max_error:  0.00
igl_scalar_parallel       38.5 ms         37.3 ms           21 avg_ns_per_svd:  36.70 ns, sum_error: 123.00, max_error:  0.39
igl_sse_parallel          10.1 ms        10.00 ms           74 avg_ns_per_svd:   9.59 ns, sum_error: 123.00, max_error:  0.39
igl_avx_parallel          10.3 ms         10.2 ms           73 avg_ns_per_svd:   9.87 ns, sum_error: 123.00, max_error:  0.39
```

### WSL - 13th Gen Intel(R) Core(TM) i5-13500 - 20 CPU(s) - [27.37 ns -> 15.94 ns] (with TBB)
```
Run on (20 X 2496 MHz CPU s)
CPU Caches:
  L1 Data 48 KiB (x10)
  L1 Instruction 32 KiB (x10)
  L2 Unified 1280 KiB (x10)
  L3 Unified 24576 KiB (x1)
Load Average: 1.15, 1.61, 1.39
--------------------------------------------------------------
Benchmark                    Time             CPU   Iterations
--------------------------------------------------------------
eigen_parallel            28.7 ms         28.7 ms           26 avg_ns_per_svd:  27.37 ns, sum_error:   0.00, max_error:  0.00
igl_scalar_parallel       24.5 ms         24.3 ms           28 avg_ns_per_svd:  23.33 ns, sum_error: 123.25, max_error:  0.39
igl_sse_parallel          16.0 ms         15.0 ms           53 avg_ns_per_svd:  15.29 ns, sum_error: 123.25, max_error:  0.39
igl_avx_parallel          16.7 ms         15.3 ms           61 avg_ns_per_svd:  15.94 ns, sum_error: 123.25, max_error:  0.39
```