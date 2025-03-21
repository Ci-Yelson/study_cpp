----------------------------------------------------------------------
Benchmark                            Time             CPU   Iterations
----------------------------------------------------------------------
STL_Insert_MaxInsertTime           319 ms          319 ms            2 max_time_insert_ns: 67777171
Abseil_Insert_MaxInsertTime        136 ms          136 ms            8 max_time_insert_ns: 12469890
STL_Insert                         181 ms          181 ms            4
Abseil_Insert                     17.7 ms         17.7 ms           37
STL_Lookup                        26.8 ms         26.8 ms           28
Abseil_Lookup                     25.8 ms         25.8 ms           30
STL_Delete                        4.30 ms         4.30 ms          163
Abseil_Delete                     2.77 ms         2.77 ms          216
