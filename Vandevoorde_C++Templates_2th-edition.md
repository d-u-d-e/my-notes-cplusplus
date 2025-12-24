# C++ Templates The Complete Guide - D. Vandevoorde, N. Josuttis, D. Gregor (covers C++17)

## Chapter 1: Function Templates

1) Templates are “compiled” in two phases: 1) without instantiation at **definition time**, the template code itself is checked for correctness ignoring the template parameters: for example static assertions that don't depend on template parameters are checked. 2)  At **instantiation time**, the template code is checked (again) to ensure that all code is valid. That is, now especially, all parts that depend on template parameters are double-checked.

2) When declaring call parameters by reference, even trivial conversions do not apply to type deduction. When declaring call parameters by value, only trivial conversions that decay are supported: Qualifications with `const` or `volatile` are ignored, references convert to the referenced type, and raw arrays or functions convert to the corresponding pointer type. For two arguments declared with the same template parameter `T` the decayed types must match. Type deduction does not work for default call arguments: you must specify a default template type as well.

3) Template argument deduction does not take return types into account. The use of `auto` for the return type without a corresponding trailing return type (which would be introduced with a `->` at the end) indicates that the actual return type must be deduced from the return statements in the function body. Note that an initialization of type `auto` always decays. This also applies to return values when the return type is just `auto` (i.e. there is no trailing return type). A way to write the max template using the trailing return type is:
    ```c++
    template<typename T1, typename T2>
    auto max (T1 a, T2 b) -> typename std::decay<decltype(true?a:b)>::type
    {
        return b < a ? a : b;
    }
    ```
    Note the use of `decay`, because the result of `decltype` could be a reference. Only since C++14 it is possible to let the compiler determine the return type without making the implementation (as above) part of its declaration.

4) Since C++11, the C++ standard library provides a means to specify choosing “the more general type.”`std::common_type<>::type` yields the “common type” of two (or more) different types passed as template arguments.  It decays. `std::common_type_t<T1, T2>` is available from C++14. The above `max` can be implemented by returing the common type of `T1` and `T2`.

5) Like ordinary functions, function templates can be overloaded. A nontemplate function can coexist with a function template that has the same name and can be instantiated with the same type. All other factors being equal, the overload resolution process prefers the nontemplate over one generated from the template. If the template can generate a function with a better match, however, then the template is selected. Recall that with template argument deduction, conversions are not possibile, so the compiler would choose a non template function if available.

## Chapter 2: Class Templates

1) Inside a class template using the class name not followed by template arguments represents the class with its template parameters as its arguments.

2) Unlike nontemplate classes, you can’t declare or define class templates inside functions or block scope. In general, templates can only be defined in global/namespace scope or inside class declarations.

3) Until C++17 you must always specify the template arguments
explicitly. C++17 introduced CTAD, which allows skipping template arguments if they can be derived from the constructor.

4) When passing arguments of a template type T by reference,
the parameter doesn’t decay. So a C string would not decay to a pointer, but remain an array of fixed length.

5) You can define specific deduction guides to provide additional or fix existing class template argument deductions.

## Chapter 3: Nontype Template Parameters

1) With C++17 NTTP can be only constant integral values (including enumerations), pointers to objects/functions/members, lvalue references to objects or functions, or `std::nullptr_t`. This has been relaxed with C++20. Since C++17, you can define a nontype template parameter to generically accept any type that is allowed for a nontype parameter, by using `auto`.

2) Since C++17 even static locally declared string literals can be passed as non type template args. Note that you cannot pass string literals.

## Chapter 4: Variadic Templates

1) `sizeof...` expands to the number of elements a parameter pack contains.

2) Since C++17, there is a feature to compute the result of using a binary operator over all the arguments of a parameter pack (with an optional initial value). If the parameter pack is empty, the expression is usually ill-formed (with the exception that for operator `&&` the value is `true`, for operator `||` the value is `false`, and for the comma operator the value for an empty parameter pack is `void()`). For example, you can use a fold expression to traverse a path in a binary tree using operator `->*`:
    ```c++
    struct Node {
        int value;
        Node* left;
        Node* right;
        Node(int i = 0) : value(i), left(nullptr), right(nullptr) {}
    };

    auto left = &Node::left;
    auto right = &Node::right;

    // traverse tree, using fold expression:
    template<typename T, typename... TP>
    Node* traverse (T np, TP... paths) {
        return (np ->* ... ->* paths);
    }

    int main()
    {
        // init binary tree structure:
        Node* root = new Node{0};
        root->left = new Node{1};
        root->left->right = new Node{2};
        // traverse binary tree:
        Node* node = traverse(root, left, right);
    }
    ```

3) Note that the same rules apply to variadic function template parameters as for ordinary parameters. For example, if passed by value, arguments are copied and decay (e.g., arrays become pointers), while if passed by reference, parameters refer to the original parameter and don’t decay:
    ```c++
    // args are copies with decayed types:
    template<typename... Args> void foo (Args... args);
    // args are nondecayed references to passed objects:
    template<typename... Args> void bar (Args const&... args);
    ```

4) You can also declare nontype template parameters to be parameter packs. For example:
    ```c++
    template<std::size_t... Idx, typename C>
    void printIdx (C const& coll)
    {
        print(coll[Idx]...);
    }

    ...
    std::vector<std::string> coll = {"good", "times", "say", "bye"};
    printIdx<2, 0, 3>(coll);
    ```

5) Even deduction guides can be variadic. For example, the C++ standard library defines the following deduction guide for `std::array`s:
    ```c++
    namespace std {
        template<typename T, typename... U> array(T, U...)
        -> array<enable_if_t<(is_same_v<T, U> && ...), T>, (1 + sizeof...(U))>;
    }
    ```

## Chapter 5: Tricky Basics

1) `typename` has to be used whenever a name that depends on a template parameter is a type.

2) For class templates with base classes that depend on template parameters, using a name `x` by itself is not always equivalent to `this->x`, even though a member `x` is inherited. For the moment, as a rule of,thumb, we recommend that you always qualify any symbol that is declared in a base that is somehow dependent on a template parameter with `this->` or `Base<T>::`.

3) When passing raw arrays or string literals to templates, some care has to be taken. First, if the template parameters are declared as references, the arguments don’t decay. That is, a passed argument of `"hello"` has type `char const[6]`. Only when passing the argument by value, the types decay, so that string literals are converted to type `char const*`. Note that a **call parameter** declared as an array (with or without length) by language rules really has a pointer type.

4) Sometimes, it is necessary to explicitly qualify template arguments when calling a member template. In that case, you have to use the template keyword to ensure that a `<` is the beginning of the template argument list:
    ```c++
    template<unsigned long N>
    void printBitset (std::bitset<N> const& bs) {
        std::cout << bs.template to_string<char, std::char_traits<char>, std::allocator<char>>();
    }
    ```
    Without that extra use of `.template`, the compiler does not know that the less-than token (<) that follows is not really less-than but the beginning of a template argument list. Note that this is a problem only if the construct before the period depends on a template parameter. In our example, the parameter `bs` depends on the template parameter `N`. This is another good example:

    ```c++
    template< class T >
    void f( T &x ) {
        x->variable < T::constant < 3 >;
    }
    ```

    Either `T::variable` or `T::constant` must be a template. The function means different things depending which is and which isn't: either `T::constant` gets compared to `3` and the Boolean result becomes a template argument to `T::variable<>` or `T::constant<3>` gets compared to `x->variable`. If it is the first case:

    ```c++
    template< class T >
    void f( T &x ) {
        x->template variable < T::constant < 3 >;
    }
    ```

    If it is the latter:
    ```c++
    template< class T >
    void f( T &x ) {
        x->variable < T::template constant < 3 >;
    }
    ```

    It would be kind of nice if the keyword were only required in actual ambiguous situations (which are kind of rare), but it makes the parser much easier to write.

    The `.template` notation (and similar notations such as `->template` and `::template`) should be used only inside templates and only if they follow something that depends on a template parameter.

5) Since C++14, variables also can be parameterized by a specific type. Such a thing is called a **variable template**. Since C++17, the standard library uses the technique of variable templates to define shortcuts for all type traits in the standard library that yield a (Boolean) value. For example, to be able to write:

    ```c++
    std::is_const_v<T>
    ```
    
    the library defines:

    ```c++
    namespace std {
        template<typename T> constexpr bool is_const_v = is_const<T>::value;
    }
    ```

6) It can be useful to allow a template parameter itself to be a class template. This is called a **template template parameter**. Prior to C++17, default template arguments of template template arguments were not considered, so that a match couldn’t be achieved by leaving out arguments that have default values. For example you could not leave out the allocator parameter in this code:
    ```c++
    template<typename T, 
        template<typename Elem, typename Alloc = std::allocator<Elem>> class Cont = std::deque>
    class Stack {
    private:
        Cont<T> elems;
        ...
    };
    ```

    With C++17 you can avoid adding the `Alloc` template parameter since default arguments are considered (`deque` has a default argument for the allocator).

## Chapter 6: Move Semantics and `enable_if<>`