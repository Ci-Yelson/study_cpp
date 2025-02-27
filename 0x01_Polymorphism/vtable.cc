#include <iostream>

class SimpleClass {
    public:
        // 普通成员函数的地址在编译时就已确定，不会存储在对象的内存布局中，也不会创建虚表指针.
        void simplefunc() { // will not create a vtable entry
            std::cout << "SimpleClass::simplefunc()" << std::endl;
        }
};

class Base {};

class Base01 {
    public:
        virtual void func01() {
            std::cout << "Base01::func01()" << std::endl;
        }
        virtual void cool01() {
            std::cout << "Base01::cool01()" << std::endl;
        }
};

class Base02 {
    public:
        virtual void func02() {
            std::cout << "Base02::func02()" << std::endl;
        }
};

class Derived01 : public Base01 {
};

class Derived02 : public Base01 {
public:
    virtual void func01() {
        std::cout << "Derived02::func01()" << std::endl;
    }
    virtual void derivedfunc02() {
        std::cout << "Derived02::derivedfunc02()" << std::endl;
    }
private:
    void derivedfunc01_private() {
        std::cout << "Derived02::derivedfunc01_private()" << std::endl;
    }
};

class Derived03 : public Base01, public Base02 {
};

int main() {
    // 64-bit system
    std::cout << "sizeof(Base) = " << sizeof(Base) << std::endl;            // 1 - Empty class
    std::cout << "sizeof(Base01) = " << sizeof(Base01) << std::endl;        // 8 - 1 virtual pointer
    std::cout << "sizeof(Base02) = " << sizeof(Base02) << std::endl;        // 8 - 1 virtual pointer
    std::cout << "sizeof(Derived01) = " << sizeof(Derived01) << std::endl;  // 8 - 1 virtual pointer
    std::cout << "sizeof(Derived02) = " << sizeof(Derived02) << std::endl;  // 8 - 1 virtual pointers
    std::cout << "sizeof(Derived03) = " << sizeof(Derived03) << std::endl;  // 16 - 2 virtual pointers

    Base01 b01;
    // (gdb) p b01 查看对象的内存布局
    // $1 = {_vptr.Base01 = 0x555555557cd8 <vtable for Base01+16>}
    // 0x555555557cd8 <vtable for Base01+16> 表示虚函数表的地址
    // (gdb) info vtbl b01 查看虚函数表
    // vtable for 'Base01':
    // [0]: 0x55555555549a <Base01::func01()>
    // [1]: 0x5555555554d8 <Base01::cool01()>

    Base01 &rb01 = b01;
    // Base pointer to Derived object
    Base01 *pb01 = new Derived01;
    // (gdb) p *pb01 查看对象的内存布局
    // $2 = {_vptr.Base01 = 0x555555557cb8 <vtable for Derived01+16>} 
    // Note here 0x555555557cb8 is different from 0x555555557cd8, means the vtable is different for different object
    // (gdb) info vtbl *pb01 查看虚函数表
    // vtable for 'Base01':
    // [0]: 0x55555555549a <Base01::func01()>
    // [1]: 0x5555555554d8 <Base01::cool01()>
    
    Derived02 d02;
    Base01 &rd02 = d02;
    // (gdb) p d02 查看对象的内存布局
    // $3 = {<Base01> = {_vptr.Base01 = 0x555555557c90 <vtable for Derived02+16>}, <No data fields>} 
    // (gdb) i vtbl d02 查看虚函数表
    // vtable for 'Derived02':
    // [0]: 0x555555555554 <Derived02::func01()>        // override from Base01
    // [1]: 0x5555555554d8 <Base01::cool01()>           // inherited from Base01
    // [2]: 0x555555555592 <Derived02::derivedfunc02()> // private function
    // (gdb) p rd02
    // $4 = (Base01 &) @0x7fffffffda38: {_vptr.Base01 = 0x555555557c90 <vtable for Derived02+16>}
    // (gdb) i vtbl rd02
    // vtable for 'Base01' @ 0x555555557c90 (subobject @ 0x7fffffffda38):
    // [0]: 0x55555555558c <Derived02::func01()>        // Note here base class pointer is pointing to the Derived02 object's override function 【动态多态】
    // [1]: 0x555555555510 <Base01::cool01()>

    Derived03 d03;
    Base02 &rd03 = d03;
    // (gdb) p d03 查看对象的内存布局
    // $5 = {<Base01> = {_vptr.Base01 = 0x555555557c58 <vtable for Derived03+16>, <Base02> = {_vptr.Base02 = 0x555555557c78 <vtable for Derived03+48>}, <No data fields>}
    // Two virtual pointers are created, one for Base01 and one for Base02
    // (gdb) i vtbl d03 查看虚函数表
    // vtable for 'Derived03':
    // [0]: 0x55555555549a <Base01::func01()>           // inherited from Base01
    // [1]: 0x5555555554d8 <Base01::cool01()>           // inherited from Base01
    // vtable for 'Base02':
    // [0]: 0x555555555516 <Base02::func02()>           // inherited from Base02

    SimpleClass sc;
    std::cout << "sizeof(SimpleClass) = " << sizeof(SimpleClass) << std::endl; // 1 - Empty class, function does not contribute to size
    // (gdb) p sc 查看对象的内存布局
    // $6 = {<No data fields>}
    
    delete pb01;
}