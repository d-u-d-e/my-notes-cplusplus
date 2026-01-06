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

2) For class templates with base classes that depend on template parameters, using a name `x` by itself is not always equivalent to `this->x`, even though a member `x` is inherited. For the moment, as a rule of thumb, we recommend that you always qualify any symbol that is declared in a base that is somehow dependent on a template parameter with `this->` or `Base<T>::`.

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

1) Don’t assume that `T&&` for a template parameter T behaves as `X&&` for a specific type `X`.

2) `X&&` for a specific type `X` declares a parameter to be an rvalue reference. It can only be bound to a movable object (a prvalue, such as a temporary object, and an xvalue, such as an object passed with `std::move()`). It is always mutable and you can always “steal” its value (A type like `X const&&` is valid but provides no common semantics in practice because “stealing” the internal representation of a movable object requires modifying that object. It might be used, though, to force passing only temporaries or objects marked with `std::move()` without being able to modify them.)

3) `T&&` for a template parameter `T` declares a **forwarding reference** (also called universal reference). It can be bound to a mutable, immutable (i.e., const), or movable object. Note that `T` must really be the name of a template parameter. Depending on a template parameter is not sufficient. For a template parameter `T`, a declaration such as typename `T::iterator&&` is just an rvalue reference, not a forwarding reference.

4) Member function templates can also be used as special member functions, including as a constructor, which, however, might lead to surprising behavior: for example, for a nonconstant lvalue `Person p`:
    ```c++
    class Person
    {
    private:
        std::string name;
    public:
        template<typename STR>
        Person(STR&& n)
    ...
    }
    ```
    the templated constructor is a better match than the usal copy constructor:
    ```c++
    Person (Person const& p)
    ```
    because the latter involves a const conversion. You want to disable the member template for the case that the passed argument is a `Person` or an expression that can be converted to a `Person`.

5) The common way to use `std::enable_if<>` is to use an additional function template argument with a default value:
    ```c++
    template<typename T,
    typename = std::enable_if_t<(sizeof(T) > 4)>>
    void foo() {}
    ```

6) We can solve the problem in 4 by writing:
    ```c++
    template<typename STR, typename = std::enable_if_t<
    std::is_convertible_v<STR, std::string>>>
    Person(STR&& n);
    ```

    With concepts, from C++20, we simply have to write the following:

    ```c++
    template<typename STR>
    requires std::is_convertible_v<STR, std::string>
    Person(STR&& n) : name(std::forward<STR>(n)) {
    ...
    }
    ```

7)  You can templify (and apply `enable_if<>`) to special member functions by deleting the predefined special member functions for `const volatile` (eg copy constructor).

## Chapter 7: By Value or by Reference?

1) Since C++17 passing by value is usually expensive if we pass an lvalue (copy elision).

2) When passing arguments to a parameter by value, the type decays. This means that raw arrays get converted to pointers and that qualifiers such as `const` and `volatile` are removed.

3) Under the hood, passing an argument by reference is implemented by passing the address of the argument. Addresses are encoded compactly, and therefore transferring an address from the caller to the callee is efficient in itself. However, passing an address can create uncertainties for the compiler when it compiles the caller’s code: What is the callee doing with that address? In theory, the callee can change all the values that are “reachable” through that address. That means, that the compiler has to assume that all the values it may have cached (usually, in machine registers) are invalid after the call. Reloading all those values can be quite expensive. You may be thinking that we are passing by constant reference: Cannot the compiler deduce from that that no change can happen? Unfortunately, that is not the case because the caller may modify the referenced object through its own, non-const reference. This bad news is moderated by inlining. If the compiler can expand the call inline, it can reason about the caller and the callee together and in many cases “see” that the address is not used for anything but passing the underlying value.

4) When passing arguments to parameters by reference, they do not decay.

5) If you want to disable passing constant objects to nonconstant references, you can do the following:
    ```c++
    template<typename T>
    void foo(T& arg) {
        static_assert(!std::is_const<T>::value,
        "out parameter of foo<T>(T&) is const");
        ...
    }
    ```
    or

    ```c++
    template<typename T,
    typename = std::enable_if_t<!std::is_const<T>::value>
    void foo(T& arg) {
        ...
    }
    ```
    or, even better since C++20:

    ```c++
    template<typename T>
    requires !std::is_const_v<T>
    void foo(T& arg) {
        ...
    }
    ```

6) One reason to use call-by-reference is to be able to perfect forward a parameter. Beware that forwarding references can cause problems like:
    ```c++
    template<typename T>
    void passR(T&& arg) { // arg is a forwarding reference
        T x;
        // for passed lvalues, x is a reference, which requires an initializer
        ...
    }
    ```

7) Since C++11, you can let the caller decide, for a function template argument, whether to pass it by value or by reference. When a template is declared to take arguments by value, the caller can use `std::cref()` and `std::ref()`, declared in header file `<functional>`, to pass the argument by reference:

    ```c++
    template<typename T>
    void printT (T arg) {
        ...
    }
    
    std::string s = "hello";
    printT(std::cref(s)); // pass s “as if by reference”
    ```

    However, note that `std::cref()` does not change the handling of parameters in templates. Instead, it uses a trick: It wraps the passed argument `s` by an object that acts like a reference. In fact, it creates an object of type `std::reference_wrapper<>` referring to the original argument and passes this object by value. The wrapper more or less supports only one operation: an implicit type conversion back to the original type, yielding the original object. Note that the compiler has to know that an implicit conversion back to the original type is necessary. In fact this does not work:

    ```c++
    template<typename T>
    void printV (T arg) {
        std::cout << arg << ’\n’;
    }
    ...

    std::string s = "hello";
    printV(s); // OK
    printV(std::cref(s)); // ERROR: no operator << for reference wrapper defined
    ```

8) When decaying arrays to pointers, you lose the ability to distinguish between handling pointers to elements from handling passed arrays. On the other hand, when dealing with parameters where string literals may be passed, not decaying can become a problem, because string literals of different size have different types.

9) We should ensure that function templates return their result by value. However, as discussed in this chapter, using a template parameter `T` is no guarantee that it is not a reference, because `T` might sometimes implicitly be deduced as a reference:

    ```c++
    template<typename T>
    T retR(T&& p) // p is a forwarding reference
    {
        return T{...}; // OOPS: returns by reference when called for lvalues
    }
    ```

    Even when `T` is a template parameter deduced from a call-by-value call, it might become a reference type when explicitly specifying the template parameter to be a reference:

    ```c++
    template<typename T>
    T retV(T p) // Note: T might become a reference
    {
        return T{...}; // OOPS: returns a reference if T is a reference
    }

    int x;
    retV<int&>(x); // retV() instantiated for T as int&
    ```

    To be safe, you have two options: use the type trait `std::remove_reference<>`,  or let the compiler deduce the return type by just declaring the return type to be `auto`, because `auto` always decays.

10)  By default, declare parameters to be passed by value. This is simple and usually works even with string literals. The performance is fine for small arguments and for temporary or movable objects. The caller can sometimes use `std::ref()` and `std::cref()` when passing existing large objects (lvalues) to avoid expensive copying. If you need an out or inout parameter, which returns a new object or allows to modify an argument to/for the caller, pass the argument as a nonconstant reference (unless you prefer to pass it via a pointer). However, you might consider disabling accidentally accepting const objects. If a template is provided to forward an argument, use perfect forwarding. That is, declare parameters to be forwarding references and use `std::forward<>()` where appropriate. Consider using `std::decay<>` or `std::common_type<>` to “harmonize” the different types of string literals and raw arrays. If performance is key and it is expected that copying arguments is expensive, use constant references. This, of course, does not apply if you need a local copy anyway.

## Chapter 8: Compile-Time Programming