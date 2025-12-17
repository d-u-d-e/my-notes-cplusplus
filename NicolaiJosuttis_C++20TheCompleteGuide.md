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

## Chapter 8: View Types in Detail

1) The class template `std::ranges::subrange<>` defines a view to the elements of a range that is usually passed as a pair of begin iterator and sentinel (end iterator).  Internally, the view itself represents the elements by storing begin (iterator) and end (sentinel). The major use case of the subrange view is to convert a pair of begin iterator and sentinel (end iterator) into one object. For example:
    ```c++
    std::vector<int> coll{0, 8, 15, 47, 11, -1, 13};
    std::ranges::subrange s1{std::ranges::find(coll, 15),
    std::ranges::find(coll, -1)};
    print(coll); // 15 47 11
    ```

2) The type of subranges depends only on the type of the iterators (and whether or not size() is provided). This can be used to harmonize the type of raw arrays:
    ```c++
    int a1[5] = { ... };
    int a2[10] = { ... };
    std::same_as<decltype(a1), decltype(a2)> // false
    std::same_as<decltype(std::ranges::subrange{a1}), decltype(std::ranges::subrange{a2})> // true
    ```

3) The class template `std::ranges::ref_view<>` defines a view that simply refers to a range. That way, passing the view by value has an effect like passing the range by reference. The effect is similar to type `std::reference_wrapper<>`. Passing a container to a coroutine, which usually has to take parameters by value, may be one application of this technique. Note that you can only create a ref view to an lvalue (a range that has a name). For an rvalue, you have to use an owning view. Ref views can also (and usually should) be created with a range factory: `std::views::all(rg)`. Note that all other views that take a range indirectly call all() for that passed range (by using `std::views::all_t<>`). For that reason, a ref view is almost always created automatically if you pass an lvalue that is not a view to one of the views.

4) The class template `std::ranges::owning_view<>` defines a view that takes ownership of the elements of another range. This is the only view (so far) that might own multiple elements. However, construction is still cheap, because an initial range has to be an rvalue. Owning views can also (and usually should) be created with a range factory: `std::views::all(rg)`. In addition, almost all other range adaptors also create an owning view if an rvalue is passed that is not a view yet.

5) The class template `std::ranges::common_view<>` is a view that harmonizes the begin and end iterator types of a range to be able to pass them to code where the same iterator type is required (such as to constructors of containers or traditional algorithms). It is better to use the range adaptor `std::views::common()` to create a common view. Note that by using the adaptor, it is not a compile-time error to create a common view from a range that is already common. This is the key reason to prefer the adaptor over directly initializing the view.

6) The class template `std::ranges::iota_view<>` is a view that generates a sequence of values. Iota views can also (and usually should) be created with a range factory: `std::views::iota(val)` or `std::views::iota(val, endVal)`. 

7) The class template `std::ranges::single_view<>` is a view that owns one element. Unless the value type is const, you can even modify the value. Single views can also (and usually should) be created with a range factory: `std::views::single(val)`

8) The class template `std::ranges::empty_view<>` is a view with no elements. However, you have to specify the element type. Empty views can also (and usually should) be created with a range factory, which is a variable template: `std::views::empty<type>`.

9) The class template `std::ranges::take_view<>` defines a view that refers to the first num elements of a passed range. If the passed range does not have enough elements, the view refers to all elements. Take views can also (and usually should) be created with a range adaptor: `std::views::take(rg, n)`. The class template `std::ranges::take_while_view<>` defines a view that refers to all leading elements of a passed range that match a certain predicate. Take-while views can be created with the adaptor `std::views::take_while(rg, pred)`. The passed predicate must be a callable that satisfies the concept `std::predicate`. This implies the concept `std::regular_invocable`, which means that the predicate should never modify the passed value of the underlying range. The predicate should at least be declared to take the argument by value or by const reference.

10) The class template `std::ranges::drop_view<>` defines a view that refers to all but the first numments of a passed range. It yields the opposite elements to the take view. Drop views can also (and usually should) be created with a range adaptor: `std::views::drop(rg, n)`. Note that the adaptor might not always yield a `drop_view`. The begin iterator is initialized and cached with the first call of begin(). On a range without random access this takes linear time. Therefore, it is better to reuse a drop view than to create it again from scratch. Note that for ranges that have random access (e.g., arrays, vectors, and deques), the cached offset for the beginning is copied with the view. As a rule of thumb, do not use a drop view after the underlying range has been modified. Note that you cannot always iterate over a const drop view. In fact, the referenced range has to be a random-access range and a sized range. There is also `std::ranges::drop_while_view<>`.

11) The class template `std::ranges::filter_view<>` defines a view that iterates only over those elements of the underlying range for which a certain predicate matches. Filter views can also (and usually should) be created with a range adaptor. The adaptor simply passes its parameters to the `std::ranges::filter_view` constructor. The passed predicate must be a callable that satisfies the concept `std::predicate`. This implies the concept `std::regular_invocable`, which means that the predicate should never modify the passed value of the underlying range. However, not modifying the value is a semantic constraint and this cannot always be checked at compile time. As a consequence, the predicate should at least be declared to take the argument by value or by const reference. It has a significant impact on the performance of pipelines. In a pipeline, you should have it as early as possible. The begin iterator is initialized and usually cached with the first call of `begin()`. The view caches the position of the first element that matches the predicate so that `begin()` does not have to recalculate it. **When using filter views, there is an important additional restriction on write access: you have to ensure that the modified value still fulfills the predicate passed to the filter.** You cannot iterate over a const filter view. Be careful especially with expensive transformations ahead of filters: the reason for this is that views and adaptors in front of the filter view might have to evaluate elements multiple times, once to decide whether they pass the filter and once to finally use their value.

12) The class template `std::ranges::transform_view<>` defines a view that yields all elements of an underlying range after applying a passed transformation. The transform view yields values that have the return type of the transformation. 

13) The class template `std::ranges::elements_view<>` defines a view that selects the idx-th member/attribute/element of all elements of a passed range.  The view calls `get<idx>(elem)` for each element.

14) The class template `std::ranges::keys_view<>` defines a view that selects the first member/attribute/element from the elements of a passed range. It is nothing but a shortcut for using the elements view with the index 0. That is, it calls `get<0>(elem)` for each element. The class template `std::ranges::values_view<>` defines a view that selects the second member/attribute/element from the elements of a passed range. It is nothing but a shortcut for using the elements view with the index 1. That is, it calls `get<1>(elem)` for each element.

15) The class template `std::ranges::reverse_view<>` defines a view that iterates over the elements of the underlying range in the opposite order. A reversed reversed range yields the original range. For better performance, reverse views cache the result of `begin()` in the view (unless the range is only an input range). Note that you cannot always iterate over a const reverse view.

16) Both class templates `std::ranges::split_view<>` and `std::ranges::lazy_split_view<>` define a view that refers to multiple sub-views of a range separated by a passed separator.
    ```c++
    std::vector<int> rg{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (const auto& sub : rg | std::views::split(5)) {
        for (const auto& elem : sub) {
            std::cout << elem << ' ';
        }
        std::cout << '\n';
    }
    ```
    This prints 
    ```
    1 2 3 4
    6 7 8 9 10 11 12 13
    ```
    Note that you cannot iterate over a const split view. To support generic code, you have to use universal/forwarding references. Alternatively, you can use a `lazy_split_view`. Then, however, the sub-view elements can only be used as forward ranges.

17) The class template `std::ranges::join_view<>` defines a view that iterates over all elements of a view of multiple ranges:

    ```c++
    std::vector<int> rg1{1, 2, 3, 4};
    std::vector<int> rg2{0, 8, 15};
    std::vector<int> rg3{5, 4, 3, 2, 1, 0};
    std::array coll{rg1, rg2, rg3};
    for (const auto& elem : std::views::join(coll)) {
        std::cout << elem << ' ';
    }
    ```

    This prints
    ```
    1 2 3 4 0 8 15 5 4 3 2 1 0
    ```

    You can use a join view to join elements of multiple arrays:

    ```c++
    int arr1[]{1, 2, 3, 4, 5};
    int arr2[] = {0, 8, 15};
    int arr3[10]{1, 2, 3, 4, 5};
    std::array<std::ranges::subrange<int*>, 3> coll{arr1, arr2, arr3};
    for (const auto& elem : std::ranges::join_view{coll}) {
        std::cout << elem << ' ';
    }
    ```

    The join view is the only view in C++20 that deals with ranges of ranges.

## Chapter 9: Spans

1) A span with a specified fix number of elements is called a span with fixed extent. It is declared by specifying the element type and the size as template parameters or by initializing it either with an array (raw array or `std::array<>`) or with an iterator and a size. For such a span, the member function size() always yields the size specified as part of the type. A span where the number of elements is not stable over its lifetime is called a span with dynamic extent. The number of elements depends on the sequence of elements the span refers to and might change due to the assignment of a new underlying range.

2) Note that declaring the span itself as `const` does not provide read-only access to the referenced elements (as usual for views, const is not propagated).

3) The fact that spans perform element access with raw pointers to the memory means that a span type erases the information of where the elements are stored. A span to the elements of a vector has the same type as a span to the elements of an array (provided they have the same extent). However, note that class template argument deduction for spans deduces a fixed extent from arrays and a dynamic extent from vectors.

4) One difference with subranges is that spans do not require iterator support for the type they refer to. You can pass any type that provides a `data()` member for access to a sequence of elements.

5) Spans are views that have reference semantics. In that sense, they behave like pointers: if a span is const, it does not automatically mean that the elements the span refers to are const.

6) `std::ranges::cbegin` is broken for spans (and for views) until C++23 since it returns an iterator to the first element of the const-qualified argument. Since C++23 it returns a **constant** iterator to the first element of the argument. However `std::cbegin()` is still broken as of C++23.

7) One property of spans is that they are borrowed ranges, meaning that the lifetime of iterators does not depend on the lifetime of the span. For that reason, we can use a temporary span as a range in algorithms that yield iterators to it.
    ```c++
    std::vector<int> coll{25, 42, 2, 0, 122, 5, 7};
    auto pos1 = std::ranges::find(std::span{coll.data(), 3}, 42); // OK
    std::cout << *pos1 << '\n';
    ```

## Chapter 10: Formatted Output

1) The implementation of `std::format()` has a pretty good performance when compared with other ways of formatting. However, memory has to be allocated for the resulting string. To save time, you can use `std::format_to_n()`, which writes to a preallocated array of characters. You have to specify both the buffer to write to and its size.

2) The functions `std::format()`, `std::format_to()`,
and `std::format_to_n()` require that the format string is a compile-time value. You have to pass a string literal or a constexpr string. Of course, applications sometimes have to compute formatting details at runtime. In that case, you have to use `std::vformat()` or `std::vformat_to()` and pass all arguments with `std::make_format_args()`.

3) C++20 specifies a standard formatting of chrono types, in addition to arithmetic types, strings, and raw pointers.

4) If a format failure is detected at runtime, an exception of type `std::format_error` is thrown.

5) A formatter is a specialization of the class template `std::formatter<>` for your type `T`. Inside the formatter, two member functions have to be defined:
    - `parse()` to implement how to parse the format string specifiers for your type
    - `format()` to perform the actual formatting for an object/value of your type

    The parse function has the following signature: `constexpr auto parse(std::format_parse_context& ctx)`. `ctx.begin()` points to the first character of the format specifier for the value to be parsed or the `}` if there is no specifier.

    The format function has the following signature:
    `auto format(const T& value, std::format_context& ctx) const`. A `std::format_context` provides the API to write the resulting characters of the formatting. Inside parse you can throw a `std::format_error`. The compiler catches it at compile time to validate the specifier.

    You don't have to reinvent the wheel for specifiers. You can create a local formatter and delegate to it, like `std::formatter<int>`, or you can just inherit from it, so the parse function is automatically available.

## Chapter 12: std::jthread and Stop Tokens

1) C++20 introduces a new type for representing threads: `std::jthread`. `std::thread`has a severe design flaw: it is not an RAII type. `std::thread` requires that at the end of its lifetime if representing a running thread, either `join()` (to wait for the end of the thread) or `detach()` (to let the thread run in the background) is called. If neither has been called, the destructor immediately causes an abnormal program termination. Before calling `join()`, you should make sure that the thread you wait for will cancel its execution. However, with `std::thread`, there is no mechanism for that. You have to implement the request for cancellation and the reaction to it yourself.

2) The class `std::jthread` provides a mechanism for signaling cancellation using stop tokens. To react to the stop request, the callable can add a new optional first parameter of type `std::stop_token` and check from time to time whether a stop was requested:
    ```c++
    void task (std::stop_token st,
    std::string s, double value)
    {
        while (!st.stop_requested()) { // stop requested (e.g., by the destructor)?
        ... // ensure we check from time to time
        }
    }
    ```

    You can also manually request a stop for a jthread that has started by calling `request_stop()` on the thread object.

3) You can call `wait()` for a condition variable with a passed stop token so that the wait is suspended when a stop is requested. For technical reasons, you have to use the type `std::condition_variable_any` for the condition variable.

4) C++20 provides stop tokens not only for threads. It is a general-purpose mechanism to asynchronously request to stop with various ways to react to this request. The first step is simply to create the `stop_source` object, which provides the API to request a stop. The constructor also creates the associated shared stop state in the heap. Then, you can ask the stop source for the `stop_token` object, which provides the API to react to a stop request. You can then pass the token (and/or the source) to locations/threads to establish the asynchronous communication between the places that might request a stop and those that might react to a stop. A stop callback is an object of the RAII type `std::stop_callback`. The constructor registers a callable (function, function object, or lambda) to be called when a stop is requested for a specified stop token. The callable is called in the thread that requested the stop if the callback has already been initialized without the destructor being called. If the callback has not been initialized by the time the stop request is made, it is called in the thread where it is initialized. If it has been destroyed, it won't be called. The destructor of a callback waits for its callable to finish if it is just called by another thread. A callback should not throw.

## Chapter 13: Concurrency Features

1) A **latch** is a new synchronization mechanism for concurrent execution that supports a single-use asynchronous countdown. Starting with an initial integral value, various threads can atomically count this value down to zero. The moment the counter reaches zero, all threads waiting for this countdown continue. One application of this is to ensure (as well as possible) that multiple threads start their actual work together even when starting and initializing the threads might take some time.

2) A **barrier** is a new synchronization mechanism for concurrent execution that allows you to synchronize multiple asynchronous tasks multiple times. After setting an initial count, multiple threads can count it down and wait until the counter reaches zero. However, in contrast to latches, when zero is reached, an (optional) callback is called and the counter reinitializes to the initial count again. A barrier is useful when multiple threads repeatedly compute/perform something together. Whenever all threads have done their task, the optional callback can process the result or new state and after that, the asynchronous computation/processing can continue with the next round. Note that the callback is called by the thread that finally decremented the counter to zero. Note that the C++ standard requires that callbacks for barriers guarantee not to throw.

3) C++20 introduced new types for dealing with semaphores: `std::counting_semaphore<>` to limit the use of multiple resources up to a maximum value, and `std::binary_semaphore<>` to limit the use of a single resource.

4) C++20 now introduces the class template `std::atomic_ref<>` to provide an atomic API for trivially copyable reference types.

5) All atomic types (`std::atomic<>`, `std::atomic_ref<>`, and `std::atomic_flag`) now provide a simple
API to let threads block and wait for changes of their values caused by other threads:
    ```c++
    int value = 100;
    std::atomic_ref<int> aVal{value};
    int lastValue = aVal.load();
    aVal.wait(lastValue); // block unless/until value changed (and notified)
    ```

    However, as for condition variables, `wait()` might end due to a spurious wake-up.

    ```c++
    while ((int val = aVal.load()) != expectedVal) {
        aVal.wait(val);
        // here, aVal may or may not have changed
    }
    ```

    **Note that there is no guarantee that you will get all updates.**

6) C++20 provides a new mechanism for synchronizing concurrent output to streams. We only have to use a `std::osyncstream` initialized with the corresponding output stream. There is also a manipulator for writing the output before the destructor is called: `std::flush_emit`:
    ```c++
    std::osyncstream coutSync{std::cout};
    for (int i = 0; i < num ; ++i) {
        coutSync << "squareroot of " << i << " is " << std::sqrt(i) << '\n' << std::flush_emit;
    }
    ```
    If we had left out the flush emit, the entire output would have been printed all at once when the sync stream’s destructor was reached. For output streams that are not synchronized, it has no effect.

7) With this definition:
    ```c++
    inline auto syncOut(std::ostream& strm = std::cout) {
        return std::osyncstream{strm};
    }
    ```
    you can simply use `syncOut()` instead of `std::cout` to ensure that concurrent output is written line by line. For example:
    ```c++
    void foo(std::string name) {
    syncOut() << "calling foo(" << name << ") in thread " << std::this_thread::get:id() << '\n';
    }
    ```

## Chapter 17: Lambda Extensions

1) C++20 introduces an extension to enable the use of template parameters for generic lambdas:
    ```c++
    auto foo = []<typename T>(const T& param) { // OK since C++20
        T tmp{}; // declare object with type of the template parameter
        ...
    };
    ```
    If you are wondering why you would not use a function template in these cases, remember that lambdas provide some benefits that functions cannot provide: they can be defined inside functions, they can capture runtime values, you can pass them as arguments without the need to specify the parameter types.

    Explicit template parameters help to avoid the need for `decltype`:

    ```c++
    []<typename... Types>(Types&&... args) {
        foo(std::forward<Types>(args)...);
    };
    ```
    rather than
    ```c++
    [] (auto&&... args) {
        foo(std::forward<decltype(args)>(args)...);
    };
    ```
2) Since C++20, lambdas with no captures have a default constructor and an assignment operator. Some STL containers accept lambdas as arguments, like:
    ```c++
    auto lessName = [] (const Customer& c1, const Customer& c2) {
        return c1.getName() < c2.getName(); 
    };
    std::set<Customer, decltype(lessName)> coll1{lessName};
    ```
    Before C++20 you had to specify the type and pass the lambda to the constructor of the container. Now since lambdas with no captures have a default constructor, it's enough to pass the type:

    ```c++
    std::set<Customer, decltype(lessName)> coll1;
    ```
    You can even define the lambda inside the container type, passing it to `decltype`.

3) Since C++20, lambdas can be used as non-type template parameters (NTTPs):
    ```c++
    template<std::invocable auto GetVat>
    int addTax(int value)
    {
        return static_cast<int>(std::round(value * (1 + GetVat())));
    }
    auto defaultTax = [] {
        return 0.19;
    };

    std::cout << addTax<defaultTax>(100) << '\n';
    ```

4) By using the new `consteval` keyword with lambdas, you can now require that lambdas become immediate functions so that “function calls” of them have to be evaluated at compile time:
    ```c++
    auto hashed = [] (const char* str) consteval {
        ...
    };
    auto hashWine = hashed("wine"); // hash() called at compile time
    ```

5) The implicit capture of `*this` is deprecated. `[=, this]` is now allowed as a lambda capture. Since C++20, the capture of structured bindings (introduced with C++17) is allowed. C++20 introduces a syntax to use init-captures with parameter packs (previously using `[&]` you would return a lambda that refers to a destroyed parameter pack, while  using `[args...]` or `[=]` you would copy the passed parameter pack):
    ```c++
    template<typename... Args>
    void foo(Args... args)
    {
        auto l4 = [...args = std::move(args)] { // OK since C++20
            bar(args...);
        };
    }
    ```

    You can also capture all by reference:

    ```c++
    template<typename... Args>
    void foo(Args... args)
    {
        auto l4 = [&...fooArgs = args] {  // OK since C++20
            bar(fooArgs...);
        };
    }
    ```

6) Here's an example with the abbreviated syntax:
    ```c++
    auto createToCall(auto op, auto... args)
    {
        return [op, ...args = std::move(args)] () -> decltype(auto) {
            return op(args...);
        };
    }

    void printWithGAndNoG(std::string_view s)
    {
        std::cout << s << "g " << s << '\n';
    }

    int main()
    {
        auto printHero = createToCall(printWithGAndNoG, "Zhan");
        printHero();
    }
    ```

## Chapter 18: Compile-Time Computing

1) One new keyword C++20 introduces is `constinit`. It can be used to force and ensure that a mutable static or global variable is initialized at compile time. Roughly speaking, the effect is described as: `constinit = constexpr - const`.

2) You cannot initialize a constinit value with another constinit value. The reason is that the initial value must be a constant value known at compile time,  but constinit values are not constant. When initializing objects, a compile-time constructor is required. However, a compile-time destructor is not required. For this reason, you can use constinit for smart pointers.

3) `constinit` does not imply `inline` (this is different from `constexpr`).

4) In C++, there is a problem called *static initialization order fiasco*, which constinit can solve. The problem is that the order of static and global initializations in different translation units is not defined. constinit ensures that an object is always initialized before it is used because the initialization happens at compile time.

5) `constexpr` functions also serve as “normal” runtime functions. C++20 introduces a similar keyword `consteval`, which mandates compile-time computing. Functions marked with `consteval` cannot be called at runtime; instead, they are required to be called at compile time. You can also declare lambdas to be consteval now. This requires that the lambda is evaluated at compile time (since C++17, all lambdas are implicitly constexpr if possible). consteval functions are implicitly inline. Parameters and the return type (if it is not void) have to be literal types.  constexpr functions cannot call consteval functions for variables known at runtime.

6) Functions marked with consteval are also not permitted to call pure runtime functions (functions marked with neither constexpr nor consteval). However, this is only checked if the call is really performed. For a consteval function, it is not an error to contain statements that call runtime functions as long as they are not reached. By using this trick, you can constrain compile-time functions to certain values:
    ```c++
    void compileTimeError()
    {

    }

    consteval int nextTwoDigitValue(int val)
    {
        if (val < 0 || val >= 99) {
            compileTimeError();
        }
        return ++val;
    }

    constexpr int i1 = nextTwoDigitValue(0); // OK
    constexpr int i2 = nextTwoDigitValue(99); // ERROR

    ```

7) **The constraints for constexpr and consteval functions are now as follows**:
    -  Parameters and the return type (if there is one) must be literal types.
    -  The body may define only variables of literal types. These variables may neither be `static` nor `thread_local`.
    -  Using `goto` and labels is not allowed.
    - The functions cannot be used as coroutines
    -  Constructors and destructors may be compile-time functions only if the class has no virtual base class.

8) C++20 provides a new helper function that allows programmers to implement different code for compile-time and runtime computing: `std::is_constant_evaluated()`:

    ```c++
    constexpr int len(const char* s)
    {
        if (std::is_constant_evaluated()) {
            int idx = 0;
            while (s[idx] != '\0') {
                // compile-time friendly code
                ++idx;
            }
            return idx;
        }
        else {
            return std::strlen(s);
            // function called at runtime
        }
    }
    ```

    it makes no sense to use `std::is_constant_evaluated()` in the following situations: ss a condition in a compile-time if because that always yields true, inside a pure runtime function, because that usually yields false, inside a consteval function, because that always yields true. Therefore, using `std::is_constant_evaluated()` usually only makes sense in `constexpr` functions. It also makes no sense to use `std::is_constant_evaluated()` inside a constexpr function to call a consteval function, because calling a consteval function from a constexpr function is not allowed in general:

    ```c++
    consteval int funcConstEval(int i) {
        return i;
    }

    constexpr int foo(int i) {
        if (std::is_constant_evaluated()) {
            return funcConstEval(i); // ERROR
        }
        else {
            return funcRuntime(i);
        }
    }
    ```

    C++23 allows this with `if consteval` (followed by no codition).

9) Since C++20, compile-time functions can allocate memory **provided the memory is also released at compile time**. For this reason, you can now use strings or vectors at compile time. However, there is an important constraint: the strings or vectors created at compile time cannot be used at runtime. The reason is that memory allocated at compile time also has to be released at compile time:

    ```c++
    template<std::ranges::input_range T>
    constexpr auto modifiedAvg(const T& rg)
    {
        using elemType = std::ranges::range_value_t<T>;
        // initialize compile-time vector with passed elements:
        std::vector<elemType> v{std::ranges::begin(rg),
        std::ranges::end(rg)};
        // perform several modifications:
        v.push_back(elemType{});
        std::ranges::sort(v);
        auto newEnd = std::unique(v.begin(), v.end());
        // return average of modified vector:
        auto sum = std::accumulate(v.begin(), newEnd,
        elemType{});
        return sum / static_cast<double>(v.size());
    }

    int main()
    {
        constexpr std::array orig{0, 8, 15, 132, 4, 77};
        constexpr auto avg = modifiedAvg(orig);
        std::cout << "average: " << avg << '\n';
    }

    ```

    Due to the fact that `avg` is declared with constexpr, `modifiedAvg()` is evaluated at compile time. Note that at the end we don't return a vector, but a value computed with the help of a compile-time vector.

10) Although you cannot return a compile-time vector so that it can be used at runtime, there is a way to return a collection of elements computed at compile time: you can return a `std::array<>`. The only problem is that you need to know the size of the array. If we do not know the resulting size of the array at compile time, we have to declare the returned array with a maximum size and return the resulting size in addition:

    ```c++
    template<std::ranges::input_range T>
    consteval auto mergeValuesSz(T rg, auto... vals)
    {
        // create compile-time vector from passed range:
        std::vector<std::ranges::range_value_t<T>> v{std::ranges::begin(rg), std::ranges::end(rg)};
        (... , v.push_back(vals));
        std::ranges::sort(v);
        // return extended collection as array and its size:
        constexpr auto maxSz = std::ranges::size(rg) + sizeof...(vals);
        std::array<std::ranges::range_value_t<T>, maxSz> arr{};
        auto res = std::ranges::unique_copy(v, arr.begin());
        return std::pair{arr, res.out - arr.begin()};
    }

    int main()
    {
        // compile-time initialization of array:
        constexpr std::array orig{0, 8, 15, 132, 4, 77, 3};
        // initialization of sorted extended array:
        auto tmp = mergeValuesSz(orig, 42, 4);
        auto merged = std::views::counted(tmp.first.begin(), tmp.second);
        // print elements:
        for(const auto& i : merged) {
            std::cout << i << ' ';
        }
    }
    ```

    A similar trick can be done to return strings computed at compile time.

## Chapter 19: Non-Type Template Parameter (NTTP) Extensions

1) Since C++20, you can use new types for non-type template parameters: Floating-point types, Structures and simple classes, Lambdas. You can use an object/value of a data structure or class as a non-type template parameter provided all members are public and the type is a literal type. As a consequence, you can now use compile-time objects of types `std::pair<>` and `std::array<>` as
template parameters.

2) The class or data structure used as  NTTP must be a structural type. This roughly means:
    - All base classes (if there are any) are inherited publicly and also structural types
    - The type is a literal type (is either an aggregate or has a constexpr constructor, no copy/move constructor, no destructor, no copy/move constructor or destructor, and where every initialization of data members is a constant expression)
    -  All non-static members are public and not mutable and use only structural types or arrays thereof

3) Note that a data structure that has a character array as a public member is a structural type. That way, we can now quite easily pass string literals as template arguments.

4) Note the following constraints for using lambdas as non-type template parameters: The lambda may not capture anything, It must be possible to use the lambda at compile time. Fortunately, since C++17, any lambda is implicitly constexpr provided it uses only features that are valid for compile-time computing.

## Chapter 20: New Type Traits

1) `std::common_reference_t<T...>` yields the common type (if there is one) of all types T... to which you can assign a value. Therefore, given types T1, T2, and T3, the trait yields the type where you can assign values of all three types.

2) `std::type_identity_t<T>` yields just type T. This type trait has a surprising number of use cases:
    - You can disable the fact that a parameter is used to deduce a template parameter:
        ```c++
        template<typename T>
        void insert(std::vector<T>& coll, const std::type_identity_t<T>& value)
        {
            coll.push_back(value);
        }

        std::vector<double> coll;
        insert(coll, 42);
        // OK: type of 42 not used to deduce type T
        ```
        If the parameter value were to be declared with just `const T&`, the compiler would raise an error because it would deduce two different types for type T (double and int).
    - You can use it as a building block to define type traits that yield types:
        ```c++
        template<typename T>
        struct remove_const : std::type_identity<T> {
        };

        template<typename T>
        struct remove_const<const T> : std::type_identity<T> {
        };
        ```

## Chapter 21: Small Improvements for the Core Language

1) C++17 introduced optional initialization for the if and switch control structures. C++20 now introduces such an optional initialization for the range-based for loop.

2) To make the use of scoped enumeration types more convenient, you can now use a `using enum` declaration.

3) Using enum declarations can also be used to delegate enumeration values to different scopes:

    ```c++
    namespace MyProject {
    class Task {
    public:
        enum class Status{open, progress, done = 9};
        Task();
    };

    // expose the values of Status to MyProject
    using enum Task::Status;
    }

    // OK: x has value MyProject::Task::open
    auto x = MyProject::open;
    ```

    note that the using enum declaration exposes only the values, not the type. To expose the type you need an ordinary type alias: `using Status = Task::Status;`.

4) Before C++20, you could only use curly braces to initialize aggregates with values. Since C++20, you can also use parentheses as outer characters for a direct initialization without =.

5) The new attributes `[[likely]]` and `[[unlikely]]` were introduced by C++20 to help the compiler when performing branch optimizations.

6) You can pass a string literal to `[[nodiscard]]` to be displayed by the compiler.

## Chapter 22: Small Improvements for Generic Programming