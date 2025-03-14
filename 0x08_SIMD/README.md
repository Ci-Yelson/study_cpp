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

### WSL - 13th Gen Intel(R) Core(TM) i5-13500 - 20 CPU(s) - [14.32 ns -> 4.09 ns] (with TBB)
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
ArBr_seq                0.051 ms        0.051 ms        12726 avg_ns_per_multi: 12.80 ns, Error:  0.00
ArBr_eigen              0.016 ms        0.016 ms        41209 avg_ns_per_multi:  4.07 ns, Error:  0.00
ArBr_simd01             0.097 ms        0.097 ms         7109 avg_ns_per_multi: 24.34 ns, Error:  0.00
ArBr_simd_reg           0.032 ms        0.032 ms        22261 avg_ns_per_multi:  8.02 ns, Error:  0.00
ArBr_simd_reg_fmad      0.010 ms        0.010 ms        59721 avg_ns_per_multi:  2.41 ns, Error:  0.00

ArBc_seq                0.051 ms        0.051 ms        13150 avg_ns_per_multi: 12.64 ns, Error:  0.00
ArBc_eigen              0.037 ms        0.037 ms        19192 avg_ns_per_multi:  9.19 ns, Error:  0.00
ArBc_simd01             0.058 ms        0.058 ms        11969 avg_ns_per_multi: 14.38 ns, Error:  0.00
ArBc_simd_reg           0.021 ms        0.021 ms        33453 avg_ns_per_multi:  5.29 ns, Error:  0.00
ArBc_simd_reg_fmad      0.015 ms        0.015 ms        40675 avg_ns_per_multi:  3.86 ns, Error:  0.00

AcBr_seq                0.056 ms        0.056 ms        12623 avg_ns_per_multi: 14.10 ns, Error:  0.00
AcBr_eigen              0.016 ms        0.016 ms        44717 avg_ns_per_multi:  4.06 ns, Error:  0.00
AcBr_simd01             0.140 ms        0.140 ms         4972 avg_ns_per_multi: 34.94 ns, Error:  0.00
AcBr_simd_reg           0.043 ms        0.043 ms        16381 avg_ns_per_multi: 10.85 ns, Error:  0.00
AcBr_simd_reg_fmad      0.012 ms        0.012 ms        62435 avg_ns_per_multi:  3.00 ns, Error:  0.00

AcBc_seq                0.057 ms        0.057 ms        12745 avg_ns_per_multi: 14.32 ns, Error:  0.00
AcBc_eigen              0.016 ms        0.016 ms        43821 avg_ns_per_multi:  3.98 ns, Error:  0.00
AcBc_simd01             0.103 ms        0.103 ms         6787 avg_ns_per_multi: 25.68 ns, Error:  0.00
AcBc_simd_reg           0.033 ms        0.033 ms        20857 avg_ns_per_multi:  8.14 ns, Error:  0.00
AcBc_simd_reg_fmad      0.016 ms        0.016 ms        42109 avg_ns_per_multi:  4.09 ns, Error:  0.00
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

### WSL - 13th Gen Intel(R) Core(TM) i5-13500 - 20 CPU(s) - [29.61 ns -> 16.20 ns] (with TBB)
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
eigen                      419 ms          419 ms            2 avg_ns_per_svd: 399.91 ns, sum_error:   0.00, max_error:  0.00
igl_scalar                 395 ms          395 ms            2 avg_ns_per_svd: 376.29 ns, sum_error: 123.25, max_error:  0.39
igl_sse                   74.7 ms         74.7 ms            9 avg_ns_per_svd:  71.23 ns, sum_error: 123.25, max_error:  0.39
igl_avx                   46.3 ms         46.3 ms           15 avg_ns_per_svd:  44.13 ns, sum_error: 123.25, max_error:  0.39

eigen_parallel            31.0 ms         31.0 ms           24 avg_ns_per_svd:  29.61 ns, sum_error:   0.00, max_error:  0.00
igl_scalar_parallel       25.7 ms         25.7 ms           26 avg_ns_per_svd:  24.49 ns, sum_error: 123.25, max_error:  0.39
igl_sse_parallel          16.6 ms         16.3 ms           46 avg_ns_per_svd:  15.82 ns, sum_error: 123.25, max_error:  0.39
igl_avx_parallel          17.0 ms         15.5 ms           43 avg_ns_per_svd:  16.20 ns, sum_error: 123.25, max_error:  0.39
```