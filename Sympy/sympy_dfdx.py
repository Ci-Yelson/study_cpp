import sympy
from sympy import *

# \partial vec(F) / \partial vec(x)
dFdx = sympy.zeros(9,12)
# B = Dm^{-1}
B00, B01, B02 = Symbol('B00'), Symbol('B01'), Symbol('B02')
B10, B11, B12 = Symbol('B10'), Symbol('B11'), Symbol('B12')
B20, B21, B22 = Symbol('B20'), Symbol('B21'), Symbol('B22')
s0 = B00 + B10 + B20
s1 = B01 + B11 + B21
s2 = B02 + B12 + B22

# \partial vec(F) / \partial x_0
dFdx[0, 0] = -s0
dFdx[3, 0] = -s1
dFdx[6, 0] = -s2
# \partial vec(F) / \partial x_1
dFdx[1, 1] = -s0
dFdx[4, 1] = -s1
dFdx[7, 1] = -s2
# \partial vec(F) / \partial x_2
dFdx[2, 2] = -s0
dFdx[5, 2] = -s1
dFdx[8, 2] = -s2
# \partial vec(F) / \partial x_3
dFdx[0, 3] = B00
dFdx[3, 3] = B01
dFdx[6, 3] = B02
# \partial vec(F) / \partial x_4
dFdx[1, 4] = B00
dFdx[4, 4] = B01
dFdx[7, 4] = B02
# \partial vec(F) / \partial x_5
dFdx[2, 5] = B00
dFdx[5, 5] = B01
dFdx[8, 5] = B02
# \partial vec(F) / \partial x_6
dFdx[0, 6] = B10
dFdx[3, 6] = B11
dFdx[6, 6] = B12
# \partial vec(F) / \partial x_7
dFdx[1, 7] = B10
dFdx[4, 7] = B11
dFdx[7, 7] = B12
# \partial vec(F) / \partial x_8
dFdx[2, 8] = B10
dFdx[5, 8] = B11
dFdx[8, 8] = B12
# \partial vec(F) / \partial x_9
dFdx[0, 9] = B20
dFdx[3, 9] = B21
dFdx[6, 9] = B22
# \partial vec(F) / \partial x_10
dFdx[1, 10] = B20
dFdx[4, 10] = B21
dFdx[7, 10] = B22
# \partial vec(F) / \partial x_11
dFdx[2, 11] = B20
dFdx[5, 11] = B21
dFdx[8, 11] = B22


def test01():
    pprint(dFdx)

if __name__ == "__main__":
    test01()