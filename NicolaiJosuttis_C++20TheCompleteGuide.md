# C++20 The Complete Guide - Nicolai M. Josuttis
 
## Chapter 1: Comparisons and Operator <=>

1) C++20 simplifies the definition of comparisons for user-defined types and introduces better ways of dealing with them. The new operator `<=>` (also called the spaceship operator) was introduced for this purpose. Before C++20, you had to define **six** operators for a type to provide full support for all possible comparisons of its objects.

2) Operator `==` Implies Operator `!=`. The compiler performs rewriting, for example with `bool operator==(const TypeA&, const TypeB&);` or `TypeA::operator==(const TypeB&)` the compiler will also be able to compile `b == a;`, `a != b`, `b != a` if `a` is of type `TypeA` and `b` of type `TypeB`.

3) In general, `operator==` handles the equality of objects by defining `==` and `!=`, while `operator<=>` handles the order of objects by defining the relational operators. By defaulting  `operator<=>` you get a default `operator==` as well.

4) The return type of `operator<=>` is not an integral value. The return type is a type that signals the comparison category, which could be **strong ordering**, **weak ordering**, or **partial ordering**. These types support the comparison with `0` to deal with the result. With strong ordering (also called total ordering), any value of a given type is less than or equal to or greater than any other value of this type (including itself). With weak ordering, any value of a given type is less than or equivalent to or greater than any other value of this type (including itself). However, equivalent values do not have to be equal (have the same value). With partial ordering, any value of a given type *could* be less than or equivalent to or greater than any other value of this type (including itself). However, in addition, it may not be possible to specify a specific order between two values at all. A typical example of this category are floating-point types, because they might have the special value NaN. Thus `std::partial_ordering` has the value `unordered`, representing neither less nor equal nor greater.

5) You should only call `operator<=>` directly when implementing `operator<=>`.

6) To compile `x <= y`, the compiler might try all of the following:
    - `x.operator<=(y)`
    - `operator<=(x, y)`
    - `x.operator<=>(y) <= 0`
    - `operator<=>(x, y) <= 0`
    - `0 <= y.operator<=>(x)`
    
    The last form is tried to support an implicit type conversion for the first operand, for which it has to become a parameter.

7) After introducing the new rules for comparisons, it turned out that C++20 introduces some issues that may be a problem when switching from older C++ versions:
    ```c++
    class MyType {
    private:
        int value;
    public:
        MyType(int i)
        : value{i} {
        }

        // implicit constructor from int:
        bool operator==(const MyType& rhs) const {
            return value == rhs.value;
        }
    };

    bool operator==(int i, const MyType& t) {
        return t == i;
    // OK with C++17
    }

    int main()
    {
        MyType x = 42;
        if (0 == x) {
            std::cout << "'0 == MyType{42}' works\n";
        }
    }
    ```

    This code no longer works in C++20. It results in an endless recursion. This is because inside the global function, the expression `t == i` can also call the global `operator==()` itself, because the compiler also tries to rewrite the call as `i == t`. Unfortunately, the rewritten statement is a better match because it does not need the implicit type conversion. We have no solution yet to support backward compatibility here; however, compilers are already starting to warn about code like this.