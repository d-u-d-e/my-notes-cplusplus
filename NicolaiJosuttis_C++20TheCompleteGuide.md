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

## Chapter 2: Placeholder Types for Function Parameters

1) Since C++20, you can use placeholders such as `auto` for all functions (including member functions and operators), not just lambdas (since C++14). The only difference wrt templates is that by using `auto` you no longer have a name for the template parameter `T`. For this reason, this feature is also called the abbreviated function template syntax.

2) Because functions with `auto` are function templates, all rules of using function templates apply. That especially has the consequence that you cannot implement a function with auto parameters in one translation unit (CPP file) while calling it in a different translation unit.  Function templates are always `inline`. Note that templates may not be declared inside functions. Therefore, with that feature you can no longer define the class or data structure locally inside a function:

    ```c++
    void foo()
    {
        struct Data 
        {
            void mem(auto); // ERROR can’t declare 
            // templates insides functions
        };
    }
    ```
3) `void foo(auto... args);` is equivalent to 
    ```c++ 
    template<typename... Types>
    void foo(Types... args);
    ```

## Chapter 3: Concepts and Requirements

1) Overload resolution considers templates with constraints as more specialized than templates without constraints. By using concepts, we can even prefer some constraints over others. However, this requires the use of concepts that **subsume** other concepts.

2) You can use the concept as a type constraint when declaring parameters with `auto`.
    ```c++
    auto maxValue(IsPointer auto a, IsPointer auto b)
    {
        return maxValue(*a, *b);
    }
    ```

3) A **trailing requires clause** can be specified after the parameter list:
    ```c++
    auto maxValue(IsPointer auto a, IsPointer auto b)
    requires IsComparableWith<decltype(*a), decltype(*b)>
    {
        return maxValue(*a, *b);
    }
    ```
    It has the benefit that it can use the name of a parameter or combine even multiple parameter names to formulate constraints.

4) Note that you cannot constrain concepts:
    ```c++
    template<std::ranges::sized_range T> concept IsIntegralValType = std::integral<std::ranges::range_value_t<T>>; //ERROR
    ```
    But you must do:
    ```c++
    template<typename T> concept IsIntegralValType = std::ranges::sized_range<T> && std::integral<std::ranges::range_value_t<T>>;
    ```

5) Here is an example of constraining *alias templates* (generic using declarations):
    ```c++
    template<std::ranges::range T> using ValueType = std::ranges::range_value_t<T>;
    ```
6) Here is an example of constraining *variable templates*:
    ```c++
    template<std::ranges::range T> constexpr bool IsIntegralValType = std::integral<std::ranges::range_value_t<T>>;
    ```
7) It is not only types that you can constrain. You can also constrain values that are template parameters (non-type template parameters (NTTP)).
    ```c++
    template<int Val>
    concept LessThan10 = Val < 10;
    ```
8) Constraints help us to understand the restrictions on templates and to get more understandable error messages when requirements are broken. Constraints can be used to disable generic code for cases where the code does not make sense:  for some types, generic code might compile but not do the right thing. Constraints can be used to overload or specialize generic code so that different code is compiled for different types.

9) If two overloads or specializations have constraints, it is important that overload resolution can decide which one is better. To support this, the signatures should not differ more than necessary.

    ```c++
    template<typename Coll, typename T>
    void add(Coll& coll, const T& val) // note: pass by const reference
    {
        coll.push_back(val);
    }
    template<typename Coll, std::floating_point T>
    void add(Coll& coll, T val) // note: pass by value
    {
        ... // special code for floating-point values
        coll.push_back(val);
    }

    std::vector<double> dVec;
    add(dVec, 0.7); // ERROR: both templates match and no preference
    ```

10) Concepts can **subsume** other concepts, which means that they count as more specialized for overload resolution. In fact, the C++ standard concepts build a pretty complex subsumption graph.

11) We can use concepts inside `if constexpr`:
    ```c++
    template<typename Coll, typename T>
    void add(Coll& coll, const T& val)
    {
        if constexpr (SupportsPushBack<decltype(coll)>) {
            coll.push_back(val);
        }
        else {
            coll.insert(val);
        }
    }
    ```

    In fact, there's no need to introduce the concept, you can do:

    ```c++
    if constexpr (requires { coll.push_back(val); }) {
        coll.push_back(val);
    }
    else {
        coll.insert(val);
    }
    ```

    This is a nice way to switch between two different function calls in generic code. It is especially recommended when introducing a concept is not worthwhile.

12) You might wonder why using concepts is better than using a variable template of type bool (like type traits do) such as the following:
    ```c++
    template<typename T>
    constexpr bool SupportsPushBack = requires(T coll) {
        coll.push_back(std::declval<typename T::value_type>());
    };
    ```
    Concepts have the following benefits: they subsume, they can be used as type constraints, they can be used with a compile-time if.

13) Concepts can be used to document semantic constraint that cannot be checked at compile time:
    ```c++
    template<std::weakly_incrementable T>
    void algo1(T beg, T end); // single-pass algorithm
    
    template<std::incrementable T>
    void algo2(T beg, T end); // multi-pass algorithm
    ```

    With `weakly_incrementable` you can iterate over a range only once.

14) Introducing a concept for each attribute or functionality of types is certainly too fine-grained. Therefore, concepts should provide common and typical aspects that separate different categories of requirements or types.

15) Concepts subsume, which means that a concept can be a subset of another concept, so that in overload resolution the more constrained concept is preferred. For a compiler, it might not be easy to find out whether a set of requirements is a subset of another set of requirements. For example, when a concept for two template parameters is commutative (so that the order of two parameters should not matter), a concept needs careful design (see `std::same_as`).

16) The main benefit of concepts is that they subsume. Type traits do not subsume.

## Chapter 4: Concepts, Requirements, and Constraints in Detail

1) The operators `&&` and `||` are the only operators you can use to combine multiple constraints without having to use parentheses.

2) A simple requirement such as `*p > val || p == nullptr;` does not require that either the left or the right sub-expression is possible.  It formulates the requirement that we can combine the results of both sub-expressions with the operator `||`. To require either one of the two sub-expressions, you have to use:
    ```c++
    template<typename T1, typename T2>
    requires(T1 val, T2 p) {
    *p > val;
    // support the comparison of the result of operator* with T1
    }
    || requires(T2 p) { // OR
    p == nullptr;
    // support the comparison of a T2 with nullptr
    }
    ```

3) Type requirements are expressions that have to be well-formed when using a name of a type. This means that the specified name has to be defined as a valid type.
    ```c++
    template<typename T1, typename T2>
    requires {
    typename T1::value_type; // type member value_type required for T1
    typename std::ranges::iterator_t<T1>; // iterator type required for T1
    typename std::common_type_t<T1, T2>; // T1 and T2 have to have a common type
    }
    ```

4) Note that simple requirements check only whether a requirement is valid, not whether it is fulfilled.

5)  The benefit of nested requirements is that we can ensure that a compile-time expression (that uses parameters or sub-expressions of the requires expression) yields a certain result instead of ensuring only that the expression is valid.
    ```c++
    template<typename T>
    concept DerefAndIndexMatch = requires (T p) {
        requires std::same_as<decltype(*p), decltype(p[0])>;
    };
    ```
    or
    ```c++
    template<typename T>
    concept DerefAndIndexMatch = std::same_as<decltype(*std::declval<T>()), decltype(std::declval<T>()[0])>;
    ```

## Chapter 5: Standard Concepts in Detail

1) Some concepts:
    - `std::movable<T>`: Guarantees that type T is movable and swappable. That is, you can move construct, move assign, and swap with another object of the type.
    - `std::copyable<T>`: Guarantees that type T is copyable (which implies movable and swappable).
    - `std::semiregular<T>`:  Guarantees that type T is semiregular (can default initialize, copy, move, and swap).
    - `std::regular<T>`:  Guarantees that type T is regular (can default initialize, copy, move, swap, and check for equality).
    - `std::same_as<T1, T2>`: Guarantees that the types T1 and T2 are the same. The concept calls the `std::is_same_v` type trait twice to ensure that the order of the parameters does not matter.
    - `std::convertible_to<From, To>`: Guarantees that objects of type From are both implicitly and explicitly convertible to objects of type To.
    - `std::derived_from<D, B>`: Guarantees that type D is publicly derived from type B (or D and B are the same) so that any pointer of type D can be converted to a pointer of type B.
    - `std::common_with<T1, T2>`: Guarantees that types T1 and T2 share a common type to which they can be explicitly converted.
    - `std::common_reference_with<T1, T2>`: Guarantees that types T1 and T2 share a common_reference type to which they can be explicitly and implicitly converted.
    - `std::equality_comparable<T>`: Guarantees that objects of type T are comparable with the operators == and !=. The order should not matter.
    - `std::totally_ordered<T>`: Guarantees that objects of type T are comparable with the operators ==, !=, <, <=, >, and >= so that two values are always either equal to, or less than, or greater than each other. The concept **does not** claim that type T has a total order for all values.  In fact, the expression `std::totally_ordered<double>` yields true.
    - `std::ranges::range<Rg>`: Guarantees that Rg is a valid range. This is the case if the range is either an array or provides `begin()` and `end()` members or can be used with free-standing `begin()` and `end()` functions.
    - `std::ranges::output_range<Rg, T>`: Guarantees that Rg is a range that provides at least output iterators (iterators that you can use to write) that accept values of type T.
    - `std::ranges::input_range<Rg>`, `std::ranges::forward_range<Rg>`, `std::ranges::bidirectional_range<Rg>`, `std::ranges::random_access_range<Rg>`, `std::ranges::contiguous_range<Rg>` are similar.
    - `std::ranges::sized_range<Rg>`: Guarantees that Rg is a range where the number of elements can be computed in constant time. Note that the performance aspect of this concept is a semantic constraint, which cannot be checked at compile time. To signal that a type does not satisfy this concept even though it provides `size()`, you can define that `std::disable_sized_range<Rg>` yields `true`.
    - `std::ranges::common_range<Rg>`: Guarantees that Rg is a range where the begin iterator and the sentinel (end iterator) have the same type. The guarantee is always given by all standard containers, but not by all views (for instance iota views with no end value).
    - `std::ranges::view<Rg>`:  Guarantees that Rg is a view (a range that is cheap to copy or move, assign, and destroy).
    - `std::invocable<Op, ArgTypes...>`: Guarantees that Op can be called for the arguments of types `ArgTypes...`. To document that neither the operation nor the passed arguments are modified, you can use `std::regular_invocable` instead. However, note that there is only a semantic difference between these two concepts, which cannot be checked at compile time. Op can be a function, function object, lambda, or pointer-to-member (note that it's okay to invoke a pointer to a data member, and the result is a reference to the data member).
    - `std::predicate<Op, ArgTypes...>`: Requires `std::regular_invocable<Op>` and all calls of Op with `ArgTypes...` yield a value that can be used as a as Boolean value.
    - `std::relation<Pred, T1, T2>`: Guarantees that any two objects of types T1 and T2 have a binary relationship in that they can be passedas arguments to the binary predicate Pred.

## Chapter 6: Ranges and Views

1) Most standard algorithms support passing ranges as one argument since C++20. However, there is no support for parallel algorithms and numeric algorithms (yet). Besides taking only one argument for a range as a whole, the new algorithms may have a couple of minor differences:
    - They use concepts for iterators and ranges to ensure that you pass valid iterators and ranges.
    - They might have different return types.
    - They might return borrowed iterators, signaling that there is no valid iterator because a temporary range (rvalue) was passed.

2) **Views** are lightweight ranges that are cheap to create and
copy/move. Views can refer to ranges and subranges, own temporary ranges,  filter out elements, yield transformed values of the elements, generate a sequence of values themselves.

3) Most views on lvalues (ranges that have a name) have reference semantics. Therefore, you have to ensure that the referred ranges and iterators of views still exist and are valid. Besides having reference semantics, views use lazy evaluation. This means that views do their processing for the next element when an iterator to the view calls `begin()` or `++` or the value of the element is requested. In addition, some views use caching.

4) Inserting or removing elements in the underlying range may have significant impact on the functionality of a view. After such modification, a view might behave differently or even no longer be valid (due to optimizations for example). Therefore, it is strongly recommended to use views right before you need them. Create views to use them ad hoc. If modifications happen between initializing a view and using it, care has to be taken.

5) In the ranges library, **sentinels** define the end of a range. In the traditional approach of the STL, sentinels would be the end iterators, which usually have the same type as the iterators that iterate over a collection. However, with C++20 ranges, having the same type is no longer required.  Creating an end iterator may be expensive or might not even be possible.
Note that before C++20, we could also have these kinds of sentinels but they were required to have the same type as the iterators. One example was input stream iterators: a default constructed iterator of type `std::istream_iterator<>` was used to create an end-of-stream iterator so that you could process input from a stream with an algorithm until end-of-file or an error occurs:

    ```c++
    std::for_each(
        std::istream_iterator<int>{std::cin},
        std::istream_iterator<int>{},
        [] (int val) {
            std::cout << val << '\n';
        }
    );
    ```
    By relaxing the requirement that sentinels (end iterators) now have to have the same type as iterating iterators, we gain a couple of benefits: We can skip the need to find the end before we start to process, for the end iterator, we can use types that disable operations that cause undefined behavior (eg operator *), defining an end iterator becomes easier:

    ```c++
    struct NullTerm {
        bool operator== (auto pos) const {
            return *pos == '\0';
        }
    };

    const char* rawString = "hello world";
    std::ranges::for_each(
        rawString, 
        NullTerm{}, 
        [] (char c) {std::cout << ' ' << c;}
    );
    ```

    The algorithms in the namespace `std` still require that the begin iterator and the end iterator have the same type.

6) A **subrange** is the generic type that can be used to convert a range defined by an iterator and a sentinel into a single object that represents this range. In fact, the range is even a view, which internally, just stores the iterator and the sentinel. This means that subranges have reference semantics and are cheap to copy. You create it through `std::ranges::subrange` by passing the iterator and the sentinel.

7) As another example of sentinels, look at `std::unreachable_sentinel`. This is a value that C++20 defines to represent the “end” of an endless range. It can help you to optimize code so that it never compares against the end (because that comparison is useless if it always yields false).

8) The most convenient way to create a range with a begin iterator and a count is to use the range adaptor `std::views::counted()`. It creates a cheap view to the first n elements of a begin iterator/pointer.

9) `sort()` and many other algorithms for ranges usually have an additional optional template parameter, a projection: the optional additional parameter allows you to specify a transformation (projection) for each element before the algorithm processes it further.

    ```c++
        std::ranges::sort(coll,
            std::ranges::less{},
            // still compare with <
            [] (auto val) {
                // but use the absolute value
                return std::abs(val);
            }
        );
    ```

10) In C++20, ranges also have some major limitations and drawbacks that should be mentioned in a general introduction to them: 
    - There is no ranges support for numeric algorithms yet
    - There is no support for ranges for parallel algorithms yet. For some views, concurrent iterations cause undefined behavior. Only dothis after declaring the view to be const. **For some views, you cannot iterate over elements when the views are const**. Generic code might therefore have to use universal/forwarding references.
    - When views refer to containers, their propagation of constness may be broken.

11) Many algorithms return iterators to the ranges they operate on. However, when passing ranges as a single argument, we can get a new problem that was not possible when a range required two arguments (the begin iterator and the end iterator): if you pass a temporary range (such as a range returned by a function) and return an iterator to it, the returned iterator might become invalid at the end of the statement when the range is destroyed. Using the returned iterator (or a copy of it) would result in undefined behavior. To deal with this problem, the ranges library has introduced the concept of **borrowed iterators**. By using type `std::ranges::borrowed_iterator_t<>`, algorithms can declare the returned iterator as borrowed. This means that the algorithm always returns an iterator that is safe to use after the statement. The consequence of this feature is that you cannot pass a temporary object to an algorithm even if the resulting code would be valid:
    ```c++
    process(std::ranges::find(getData(), 42));
    ```
    Although the iterator would be valid during the function call (the temporary vector would be destroyed after the call), `find()` returns a `std::ranges::dangling` object.

12) Range types can claim that they are **borrowed ranges**. This means that their iterators can still be used when the range itself no longer exists. All lvalues (objects with names) are borrowed ranges, which means that the returned iterator cannot be dangling as long as the iterator exists in the same scope or a sub-scope of the range. For temporary views it depends. View that hold copies are declared as borrowed ranges (i.e. iota). If you implement a container or a view, you can signal that it is a borrowed range by specializing the variable template `std::ranges::enable_borrowed_range<>`.
    ```c++
    auto pos3 = std::ranges::find(std::views::iota(8), 8);
    // borrowed range
    std::cout << *pos3; // OK
    ```
    The iota view iterators hold copies of the element they refer to, which means that iota views are declared as borrowed ranges.

13) `std::views::all` is used as a factory function to generate a view from a range. There are other factories such as `iota`, `single`, `empty`, ecc. You can explicitly convert elements of a container to a view by passing a begin iterator and an end (sentinel) or a size to a `std::ranges::subrange` or `std::views::counted()`. Or you can create a view directly by usually passing the range to the constructor: `std::ranges::take_view first4{coll, 4};`.

14) The following: 
    ```c++
    for (const auto& elem : getColl() | std::views::take(5)) {
        std::cout << "- " << elem << '\n';
    }
    ```
    is valid, although in general, **it is a fatal runtime error to use a reference to a temporary as a collection a range-based for loop iterates over** (the lifetime of any temporary within range_expression is not extended, but it's okay to just have a temporary). Because passing a temporary range object (rvalue) moves the range into an `owning_view`, the view does not refer to an external container and therefore there is no runtime error.

15) Modifying leading elements of ranges (changing their value or inserting/deleting elements) may invalidate views if and only if `begin()` has already been called before the modification. Therefore, iterating over the elements of a view might invalidate a later use if its referred range has been modified in the meantime.