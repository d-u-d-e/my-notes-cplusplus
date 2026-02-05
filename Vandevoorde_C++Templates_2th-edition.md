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

1) It’s not always easy to find out and formulate the right expression to SFINAE out function templates for certain conditions. Suppose, for example, that we want to ensure that the function template `len()` is ignored for arguments of a type that has a `size_type` member but not a `size()` member function. This results in an error because there is no requirement on size:

    ```c++
    template<typename T>
    typename T::size_type len (T const& t)
    {
        return t.size();
    }

    std::allocator<int> x;
    std::cout << len(x) << ’\n’;
    // ERROR: len() selected, but x has no size()
    ```
    There is a common pattern or idiom to deal with such a situation:
    - Specify the return type with the trailing return type syntax
    - Define the return type using `decltype` and the comma operator
    -  Formulate all expressions that must be valid at the beginning of the comma operator (converted to void in case the comma operator is overloaded)
    - Define an object of the real return type at the end of the comma operator

    ```c++
    template<typename T>
    auto len (T const& t) -> decltype((void)(t.size()), T::size_type())
    {
        return t.size();
    }
    ```

    This is of course superseded by concepts in C++20.

2) With constexpr if, outside a template, a discarded statement is fully checked. In a template, it is not instantiated, which means that only the first translation phase (the definition time) is performed, which checks for correct syntax and names that don’t depend on template parameters.

## Chapter 9: Using Templates in Practice

1) The precompiled header scheme relies on the fact that code can be organized in such a manner that many files start with the same lines of code. Let’s assume for the sake of argument that every file to be compiled starts with the same N lines of code. We could compile these N lines and save the complete state of the compiler at that point in a precompiled header. Then, for every file in our program, we could reload the saved state and start compilation at line N+1. The key to making effective use of precompiled headers is to ensure that - as much as possible - files start with a maximum number of common lines of code. In practice this means the files must start with the same #include directives, which (as mentioned earlier) consume a substantial portion of our build time.

## Chapter 10: Basic Template Terminology

1) For a variable, initialization or the absence of an `extern` specifier causes a declaration to become a definition.

2) The declaration of a class template or function template is called a definition if it has a body.

3) *Incomplete types* are one of the following:
    - A class type that has been declared but not yet defined
    - An array type with an unspecified bound
    - An array type with an incomplete element type
    - `void`
    - An enumeration type as long as the underlying type or the enumeration values are not defined
    - Any type above to which const and/or volatile are applied

    All other types are *complete*.

4) The C++ language definition places some constraints on the redeclaration of various entities. The totality of these constraints is known as the **one-definition rule** or ODR. For now, it suffices to remember the following ODR basics:
    - Ordinary (i.e., not templates) noninline functions and member functions, as well as (noninline) global variables and static data members should be defined only once across the whole program.
    - Class types (including structs and unions), templates (including partial specializations but not full specializations), and inline functions and variables should be defined at most once per translation unit, and all these definitions should be identical.

5) The combination of the template name, followed by the arguments in angle brackets, is called a *template-id*.

6) Because template parameters are compile-time entities, they can also be used to create valid template arguments. Here is an example:
    ```c++
    template<typename T>
    class Dozen {
    public:
        ArrayInClass<T, 12> contents;
    };
    ```
    
    Note how in this example the name `T` is both a template parameter and a template argument.

## Chapter 11: Generic Libraries

1) The C++ standard library introduces the slightly broader notion of a callable type, which is either a function object type or a pointer to member. Generic code often benefits from being able to accept any kind of callable, and templates make it possible to do so.

2) When we pass the name of a function as a function argument, we don’t really pass the function itself but a pointer or reference to it. As with arrays, function arguments decay to a pointer when passed by value. Just like arrays, functions can be passed by reference without decay, but references to functions are rarely used in mainstream C++ code.

3) It is also possible for a class type object to be implicitly convertible to a pointer or reference to a surrogate call function: this is relatively unusual. Interestingly, lambdas that start with [] (no captures) produce a conversion operator to a function pointer. However, that is never selected as a surrogate call function because it is always a worse match than the normal `operator()` of the closure. A function pointer cannot carry state, so there is no way to represent a lambda that captures something as a function pointer without extra machinery. This was a design choice that improves usability and expressiveness.

4) Member functions can be used as callables. Since C++17, the C++ standard library provides a utility `std::invoke()` that conveniently unifies this case with the ordinary function-call syntax cases, thereby enabling calls to any callable object with a single form:

    ```c++
    template<typename Iter, typename Callable, typename... Args>
    void foreach (Iter current, Iter end, Callable op, Args const&... args)
    {
        while (current != end) {
            std::invoke(op, args..., *current);
            ++current;
        }
    }
    ```
    If the callable is a pointer to member, it uses the first additional argument as the `this` object. All remaining additional parameters are just passed as arguments to the callable. Otherwise, all additional parameters are just passed as arguments to the callable. Note that we can’t use perfect forwarding here for the callable or additional parameters: The first call might “steal” their values, leading to unexpected behavior calling op in subsequent iterations.

    `std::invoke()` also allows a pointer to data member as a callback type. Instead of calling a function, it returns the value of the corresponding data member in the object referred to by the additional argument.

5) A common application of `std::invoke()` is to wrap single function calls (to prepare some context for example):
    ```c++
    template<typename Callable, typename... Args>
    decltype(auto) call(Callable&& op, Args&&... args)
    {
        return std::invoke(std::forward<Callable>(op), 
        std::forward<Args>(args)...);
    }
    ```

    The other interesting aspect is how to deal with the return value of a called function to “perfectly forward” it back to the caller. `decltype(auto)` (available since C++14) is a placeholder type that determines the type of variable, return type, or template argument from the type of the associated expression.

    If you want to temporarily store the value returned by `std::invoke()` in a variable to return it after doing something else (e.g., to deal with the return value or log the end of the call), you also have to declare the temporary variable with `decltype(auto)`:

    ```c++
    decltype(auto) ret{std::invoke(std::forward<Callable>(op),
    std::forward<Args>(args)...)};
    ...
    return ret;
    ```
    Note that declaring `ret` with `auto&&` is not correct. As a reference, `auto&&` extends the lifetime of the returned value until the end of its scope (see Section 11.3 on page 167) but not beyond the return statement to the caller of the function. **Plain `auto` never deduces to a reference, and `auto&&` always deduces to a reference**. 
    However, there is also a problem with using `decltype(auto)`: If the callable has return type void, the initialization of `ret` as `decltype(auto)` is not allowed, because void is an incomplete type. You can make use of `if constexpr` to deal with void and non void cases.

6) Type traits must be used with particular care: They might behave differently than the (naive) programmer might expect. For example:
    ```c++
    std::remove_const_t<int const&> // yields int const&
    ```
    Here, because a reference is not const (although you can’t modify it), the call has no effect and
    yields the passed type. As a consequence, the order of removing references and const matters. Also there are cases where type traits have requirements. Not satisfying those requirements results in undefined behavior.

7) Don’t forget to use the `std::decay<>` type trait to ensure the default return type can’t be a reference, because `std::declval()` itself yields rvalue references. `std::declval` can be used only in unevaluated contexts, like inside `decltype`or inside concepts. 

8) Sometimes we have to perfectly forward data in generic code that does not come through a parameter. In that case, we can use `auto&&` to create a variable that can be forwarded:

    ```c++
    template<typename T>
    void foo(T x)
    {
        auto&& val = get(x);
        ...
        // perfectly forward the return value of get() to set():
        set(std::forward<decltype(val)>(val));
    }
    ```

9) When implementing templates, sometimes the question comes up whether the code can deal with incomplete types. It's possible to defer instantiation of code members by adding additional template members if they require complete types.

## Appendix A: The One-Definition Rule

1) Entities declared in unnamed namespaces are considered distinct if they appear in distinct translation units; in C++11 and later, such entities also have internal linkage by default.

2) Defining or calling a function with an argument or return type of type X, where X has not been defined is an error. Just declaring such a function doesn’t need the type to be defined. 

3) Inline functions must be defined in every translation unit in which they are used (in which they are called or their address is taken). However, unlike class types, their definition can follow the point of use.

4) The C++ standard doesn’t mandate that differences in multiple definitions be detected or diagnosed across different translation units. However, the C++ standard qualifies this as leading to undefined behavior.

5) The cross-translation unit constraints specify that when an entity is defined in two different places, the two places must consist of exactly the same sequence of tokens (the keywords, operators, identifiers, and so forth, remaining after preprocessing). Furthermore, these tokens must mean the same thing in their respective context (e.g., the identifiers may need to refer to the same variable). There is an exception for constants, provided the address is not taken.

## Appendix B: Value categories

1) For any expression `x`, `decltype((x))` (note the double parentheses) yields:
    - `type` if `x` is a prvalue
    - `type&` if `x` is an lvalue
    - `type&&` if `x` is an xvalue

2) A call to a function whose return type is an lvalue reference yields an lvalue.

3) A call to a function whose return type is an rvalue reference to an object type yields an xvalue.

4) A call to a function that returns a nonreference type yields a prvalue.

## Appendix C: Overload Resolution

1) Calls through function pointers and calls through pointers to member functions are not subject to overload resolution because the function to call is entirely determined (at run time) by the pointers.

2) Overload resolution ranks the viable candidate functions by comparing how each argument of the call matches the corresponding parameter of the candidates. For one candidate to be considered better than another, the better candidate cannot have any of its parameters be a worse match than the corresponding parameter in the other candidate:

    ```c++
    void combine(int, double);
    void combine(long, int);
    
    int main()
    {
        combine(1,2); // ambiguous!
    }
    ```
3) Given 2, we are left with specifying how well a given argument matches the
corresponding parameter of a viable candidate. As a first approximation, we can rank the possible matches as follows (from best to worst):
    - Perfect match. The parameter has the type of the expression, or it has a type that is a reference to the type of the expression (possibly with added `const` and/or `volatile` qualifiers).
    - Match with minor adjustments. This includes, for example, the decay of an array variable to a pointer to its first element or the addition of `const` to match an argument of type `int**` to a parameter of type `int const* const*`.
    - Match with promotion. Promotion is a kind of implicit conversion that includes the conversion of small integral types (such as bool, char, short, and sometimes enumerations) to int, unsigned int, long, or unsigned long, and the conversion of float to double.
    - Match with standard conversions only. This includes any sort of standard conversion (such as int to float) or conversion from a derived class to one of its public, unambiguous base classes but excludes the implicit call to a conversion operator or a converting constructor.
    - Match with user-defined conversions. This allows any kind of implicit conversion.
    - Match with ellipsis (...). An ellipsis parameter can match almost any type. However, there is one exception: Class types with a nontrivial copy constructor may or may not be valid.

4) When all other aspects of overload resolution are equal, a nontemplate function is preferred over an instance of a template (it doesn’t matter whether that instance is generated from the generic template definition or whether it is provided as an explicit specialization):

    ```c++
    template<typename T> int f(T); // #1
    void f(int); // #2
    int main()
    {
        return f(7); // ERROR: selects #2 , which doesn’t return a value
    }
    ```

5) When two templates only differ in that one adds a trailing parameter packs: The template without the pack is considered more specialized and is therefore preferred if it matches the call.

6) An implicit conversion can, in general, be a sequence of elementary conversions. There can be at most one user-defined conversion in a conversion sequence. An important principle of overload resolution is that a conversion sequence that is a subsequence of another conversion sequence is preferable over the latter sequence. 

7) Within the category of regular pointer conversions, conversions to type bool (both from a regular pointer and from a pointer to a member) are considered worse than any other kind of standard conversion. A conversion to type `void*` is considered worse than a conversion from a derived class pointer to a base class pointer. Furthermore, if conversions to different classes related by inheritance exist, a conversion to the most derived class is preferred.

8) Initializer list arguments (initializers passed with in curly braces) can be converted to several different kinds of parameters: `initializer_list`s, class types with an `initializer_list` constructor, class types for which the initializer list elements can be treated as (separate) parameters to a constructor, or aggregate class types whose members can be initialized by the elements of the initializer list.

## Chapter 12: Fundamentals in Depth

1) Note that a constructor template (a special kind of member function template) disables the implicit declaration of the default constructor. This can be generated by defaulting it.

2) Union templates are possible (and they are considered a kind of class template):
    ```c++
    template<typename T>
    union AllocChunk {
        T object;
        unsigned char bytes[sizeof(T)];
    };
    ```

3) Member function templates cannot be declared `virtual`. This constraint is imposed because the usual implementation of the virtual function call mechanism uses a fixed-size table with one entry per virtual function. However, the number of instantiations of a member function template is not fixed until the entire program has been translated.

4) Currently templates cannot be declared in function scope or local class scope, but generic lambdas which have associated closure types that contain member function templates, can appear in local scopes, which effectively implies a kind of local member function template.

5) Since C++11, any kind of template parameter can be turned into a template parameter pack by introducing an ellipsis (...) prior to the template parameter name.  While a normal template parameter matches exactly one template argument, a template parameter pack can match any number of template arguments.

6) A template parameter for a class template, variable template, or alias template can have a default template argument only if default arguments were also supplied for the subsequent parameters. Default template arguments for template parameters of function templates do not require subsequent template parameters to have a default template argument. A number of contexts do not permit default template arguments: Partial specializations, Parameter packs, The out-of-class definition of a member of a class template.

7) A template template argument must generally be a class template or alias template with parameters that exactly match the parameters of the template template parameter it substitutes. Prior to C++17, default template arguments of a template template argument were ignored:

    ```c++
    #include <list>
    // declares in namespace std:
    // template<typename T, typename Allocator = allocator<T>>
    // class list;
    template<typename T1, typename T2, template<typename> class Cont>
    class Rel {
        ...
    };


    Rel<int, double, std::list> rel; // ERROR before C++17: 
    // std::list has more than one template parameter
    ```
    C++17 relaxed the matching rule to just require that the template template parameter be at least as specialized as the corresponding template template argument. Variadic template template parameters are an exception to the pre-C++17 “exact match” rule described above and offer a solution to this limitation. But **template parameter packs can only match template arguments of the same kind**, so this is invalid:

    ```c++
    template<template<typename... > class TT>
    class AlmostAnyTmpl {
    };

    AlmostAnyTmpl<std::array> withArray;
    // ERROR: a template type parameter pack
    // doesn’t match a nontype template parameter
    ```

8) The `sizeof...` expression is an example of a pack expansion. A **pack expansion** is a construct that expands an argument pack into separate arguments. While `sizeof...` performs this expansion just to count the number of separate arguments, other forms of parameter packs—those that occur where C++ expects a list—can expand to multiple elements within that list. Such pack expansions are identified by an ellipsis (`...`) to the right of an element in the list.

9) Each pack expansion has a **pattern**, which is the type or expression that will be repeated for each argument in the argument pack and typically comes before the ellipsis that denotes the pack expansion. The name of the parameter pack is a trivial pattern, but patterns can be arbitrarily complex:

    ```c++
    template<typename... Types>
    class PtrTuple : public Tuple<Types*...> {
    // extra operations provided only for PtrTuple
    };
    ```

    The pattern for the pack expansion `Types*...` in the example above is `Types*`.

10) Pack expansions can be used essentially anywhere in the language where the grammar provides a comma-separated list:
    ```c++
    template<typename... Mixins>
    class Point : public Mixins... { // base class pack expansion
        double x, y, z;
    public:
        Point() : Mixins()... { } // base class initializer pack expansion
        template<typename Visitor>
        void visitMixins(Visitor visitor) 
        {
            // call argument pack expansion
            visitor(static_cast<Mixins&>(*this)...); 
        }
    };
    ```

    By casting `*this` to each of the mixin types, the pack expansion produces call arguments that refer to each of the base classes corresponding to the mixins.

11) A **function parameter pack** is a function parameter that matches zero or more function call arguments. Like a template parameter pack, a function parameter pack is introduced using an ellipsis (...) prior to (or in the place of) the function parameter name. Unlike template parameter packs, function parameter packs are always pack expansions:
    ```c++
    template<typename... Types>
    void print(Types... values);
    ```
12) Pack expansions are actually semantic constructs, and the substitution of an argument pack of any size does not affect how the pack expansion (or its enclosing variadic template) is parsed. Rather, when a pack expansion expands to an empty list, the program behaves (semantically) as if the list were not present:

    ```c++
    template<typename T, typename... Types>
    void g(Types... values) {
        T v(values...);
    }
    ```
    The variadic function template `g()` creates a value `v` that is direct-initialized from the sequence of values it is given. If that sequence of values is empty, the declaration of `v` looks, syntactically, like a function declaration `T v()`. However, since substitution into a pack expansion is semantic and cannot affect the kind of entity produced by parsing, `v` is initialized with zero arguments, that is, value-initialization.

13) An instance of a function template can be made a friend by making sure the name of the friend function is followed by angle brackets. The angle brackets can contain the template arguments, but if the arguments can be deduced, the angle brackets can be left empty. Note that we cannot define a template instance (at most, we can define a specialization), and hence a friend declaration that names an instance cannot be a definition.

## Chapter 13: Names in Templates

1) A name is a **qualified name** if the scope to which it belongs is explicitly denoted using a scope resolution operator (`::`) or a member access operator (`.` or `->`).

2) A name is a **dependent name** if it depends in some way on a template parameter.

3) Qualified names are looked up in the scope implied by the qualifying construct. If that scope is a class, then base classes may also be searched. However, enclosing scopes are not considered when looking up qualified names. In contrast, unqualified names are typically looked up in successively more enclosing scopes (although in member function definitions, the scope of the class and its base classes is searched before any other enclosing scopes). This is called **ordinary lookup**. A more recent twist to the lookup of unqualified names is that—in addition to ordinary lookup—they may sometimes undergo **argument-dependent lookup (ADL)** (this was also called Koenig lookup). 

4) ADL does not happen if ordinary lookup finds the name of a member function, the name of a variable, the name of a type, or the name of a block-scope function decleration. Otherwise, if the name is followed by a list of argument expressions enclosed in parentheses, ADL proceeds by looking up the name in namespaces and classes “associated with” the types of the call arguments. ADL is preferred over ordinary lookup in case both apply. For the exact meaning of associated types and namespaces see page 219.

5) A C++ parser will look up names appearing before a < and treat the < as an angle bracket only if the name is known to be that of a template; otherwise, the < is treated as an ordinary less-than operator. This form of context sensitivity is an unfortunate consequence of having chosen angle brackets to delimit template argument lists. The tokenizer isn’t spared problems with the angle-bracket notation either: in `List<List<int>> a;` the two > characters combine into a right-shift token >> and hence are never treated as two separate tokens by the tokenizer. This is a consequence of the **maximum munch** tokenization principle. Since C++11, the C++ standard specifically calls out this case—where a nested template-id is closed by a right-shift token >>—and, within the parser, treats the right shift as being equivalent to two separate right angle brackets > and > to close two template-ids at once. Some programs changed meaning since C++11, consider this contrived example:
    ```c++
    template<int I> struct X {
        static int const c = 2;
    };

    template<> struct X<0> {
        typedef int c;
    };

    template<typename T> struct Y {
        static int const c = 3;
    };

    static int const c = 4;

    int main()
    {
        std::cout << (Y<X<1> >::c >::c>::c) << ' ';
        std::cout << (Y<X< 1>>::c >::c>::c) << '\n';
    }
    ```

    This prints `0 3` before C++11, and `0 0` since C++11.

6) In general a dependent qualified name does not denote a type unless that name is prefixed with the keyword `typename`. The following example shows why names in templates cannot always be sufficiently classified:

    ```c++
    template<typename T>
    class Trap {
    public:
        enum { x };
        // #1 x is not a type here
    };

    template<typename T>
    class Victim {
    public:
        int y;
        void poof() 
        {
            // #2 declaration or multiplication?
            Trap<T>::x * y;
        }
    };

    // evil specialization!
    template<>
    class Trap<void> {
    public:
        // #3 x is a type here
        using x = int;
    };


    void boom(Victim<void>& bomb)
    {   
        bomb.poof();
    }
    ```

    See page 229 for a discussion of when to use `typename`.

7) As is the case with type names, a compiler has to assume that a dependent name does not refer to a template unless the programmer provides extra information using the keyword `template`:

    ```c++
    template<typename T>
    class Shell {
    public:
        template<int N>
        class In {
        public:
            template<int M>
            class Deep {
            public:
                virtual void f();
            };
        };
    };

    template<typename T, int N>
    class Weird {
    public:
        void case1 (
            typename Shell<T>::template In<N>::template Deep<N>* p) 
        {
            p->template Deep<N>::f(); // inhibit virtual call
        }

        void case2 (
            typename Shell<T>::template In<N>::template Deep<N>& p) 
        {
            p.template Deep<N>::f(); // inhibit virtual call
        }
    }
    ```

    This somewhat intricate example shows how all the operators that can qualify a name (::, ->,and .) may need to be followed by the keyword `template`.

8) Consider:
    ```c++
    namespace N {
        class X {
            ...
        };
    }

    template<int I> void select(X*);

    void g (N::X* xp)
    {
        select<3>(xp); // ERROR: no ADL!
    }
    ```
    In this example, we may expect that the template `select()` is found through ADL in the call `select<3>(xp)`. However, this is not the case because a compiler cannot decide that `xp` is a function call argument until it has decided that `<3>` is a template argument list. Furthermore, a compiler cannot decide that `<3>` is a template argument list until it has found `select()` to be a template. Because this chicken-and-egg problem cannot be resolved, the expression is parsed as `(select<3)>(xp)`, which makes no sense.

    This example may give the impression that ADL is disabled for template-ids, but it is not. The code can be fixed by introducing a function template named select that is visible at the call: `template<typename T> void select();`.

9) In a class template, a nondependent base class is one with a complete type that can be determined without knowing the template arguments. In other words, the name of this base is denoted using a nondependent name:
    ```c++
    template<typename X>
    class Base {
    public:
        int basefield;
        using T = int;
    };

    // not a template case really
    class D1: public Base<Base<void>> {
    public:
        void f() { basefield = 3; }
    };

    template<typename T>
    class D2 : public Base<double> {
    // nondependent base
    public:
        void f() { 
            // usual access to inherited member
            basefield = 7; 
        }
        T strange; // T is Base<double>::T, not the template parameter!
    };
    ```
    Nondependent bases in templates behave very much like bases in ordinary nontemplate classes, but there is a slightly unfortunate surprise: when an unqualified name is looked up in the templated derivation, the nondependent bases are considered before the list of template parameters; for example, this is not valid C++:

    ```c++
    void g (D2<int*>& d2, int* p)
    {
        d2.strange = p;
        // ERROR: type mismatch!
    }
    ```

10) The C++ standard specifies that a nondependent name appearing in a template is looked up as soon as it is encountered. But  nondependent names are not looked up in dependent base classes. So something like this is an error:

    ```c++
    template<typename T>
    class DD : public Base<T> {
    // dependent base
    public:
        void f() { basefield = 0; } // problem...
    };
    ```
    To correct the code, it suffices to make the name `basefield` dependent because dependent names can be looked up only at the time of instantiation, and at that time the concrete base instance that must be explored will be known. To delay the lookup use `this->basefield`. An alternative consists in introducing a dependency using a qualified name: ` Base<T>::basefield`. The latter alternative can inhibit the virtual call mechanism so care must be taken. If you find that the repeated qualifications are cluttering up your code, you can bring a name from a dependent base class in the derived class once and for all with: `using Base<T>::basefield;`.

11) The lookup rule that causes a name in nondependent bases to hide an identically named template parameter is an oversight, but suggestions to change the rule have not garnered support from the C++ standardization committee. It is best to avoid code with template parameter names that are also used in nondependent base classes.

## Chapter 14: Instantiation

1) **Two-phase lookup**: the first phase is the parsing of a template, and the second phase is
its instantiation.

2) During the first phase, while parsing a template, nondependent names are looked up using both the ordinary lookup rules and, if applicable, the rules for argument-dependent lookup (ADL). During the second phase, while instantiating a template at a point called the point of instantiation (POI), dependent qualified names are looked up (with the template parameters replaced with the template arguments for that specific instantiation), and an additional ADL is performed for the unqualified dependent names that were looked up using ordinary lookup in the first phase. For unqualified dependent names, the initial ordinary lookup—while not complete—is used to decide whether the name is a template. So this does not compile

    ```c++
    template <typename T>
    void f1(T x)
    {
        g1(x);
    }

    void g1(int)
    {

    }

    int main()
    {
        f1(7);
    }
    // POI for f1<int>(int)
    ```

    This is because `f1(7)` creates a POI for `f1<int>(int)`, where lookup of `g1` fails. When `f1` is first parsed, `g1` is unqualified and dependant, so ordinary lookup is performed. No `g1` is visible at that point. At the POI, ADL is performed on `g1` but no `g1` is found in associated namespaces and classes.

    **A POI is a point in the source where the substituted template could be inserted.**

3) The **POI for a reference to a generated class instance** is defined to be
the point immediately *before* the nearest namespace scope declaration or definition that contains the reference to that instance. **The POI for a reference to a function template specialization** is immediately after the nearest namespace scope declaration or definition that contains that reference. **The POI for variable templates** is handled similarly to that of function templates.

4) It is possible to create explicitly a point of instantiation for a template specialization. The construct that achieves this is called an **explicit instantiation directive**:
    ```c++
    template<typename T>
    void f(T)
    {}

    // four valid explicit instantiations:
    template void f<int>(int);
    template void f<>(float);
    template void f(long);
    template void f(char);
    ```

    It consists of the keyword `template` followed by a declaration of the specialization to be instantiated. Members of class templates can also be explicitly instantiated in this way; furthermore, all the members of a class template specialization can be explicitly instantiated by explicitly instantiating the class template specialization. A template specialization that is explicitly instantiated should not be explicitly specialized, and vice versa, because that would imply that the two definitions could be different (thus violating the ODR).

5) Many C++ programmers have observed that automatic template instantiation has a nontrivial negative impact on build times, because the same template specializations may be instantiated and optimized in many different translation units. A technique to improve build times consists in manually instantiating those template specializations that the program requires in a single location and inhibiting the instantiation in all other translation units. One portable way to ensure this inhibition is to not provide the template definition except
in the translation unit where it is explicitly instantiated:

    ```c++
    //translation unit 1:
    template<typename T> void f(); // no definition: prevents instantiation
    // in this translation unit

    void g()
    {
        f<int>();
    }

    //translation unit 2:
    template<typename T> void f()
    {
    // implementation
    }

    template void f<int>();
    // manual instantiation

    void g();
    int main()
    {
        g();
    }
    ```
    In the first translation unit, the compiler cannot see the definition of the function template `f`, so it will not (cannot) produce an instantiation of `f<int>`. The second translation unit provides the definition of `f<int>` via an explicit instantiation definition; without it, the program would fail to link.

    This is called **manual instantiation** and has its drawbacks.

6) An **explicit instantiation declaration** is an explicit instantiation directive prefixed by the keyword `extern`. Explicit instantiation declarations can be used to improve compile or link times when certain specializations are used in many different translation units. It suppresses automatic instantiation of the named template specialization, because it declares that the named template specialization will be defined somewhere in the program.

7) With constexpr if, during the instantiation of templates (including generic lambdas), the
discarded branch is not instantiated. So this compiles:
    ```c++
    template<typename T> bool f(T p) {
        if constexpr (sizeof(T) <= sizeof(long long)) {
            return p>0;
        } else {
            return p.compare(0) > 0;
        }
    }

    bool g(int n) {
        return f(n); // OK
    }
    ```

    If it weren’t discarded, it would be instantiated and we’d run into an error for the expression `p.compare(0)` (not valid for integer `p`). Prior to C++17 and its `constexpr if` statements, avoiding such errors required explicit template specialization or overloading:
    ```c++
    template<bool b> struct Dispatch {
        template<typename T>
        static bool f(T p) {
            return p.compare(0) > 0;
        }
    };

    template<> struct Dispatch<true> {
        template<typename T>
        static bool f(T p) {
            return p > 0;
        }
    };

    template<typename T> bool f(T p) {
        return Dispatch<sizeof(T) <= sizeof(long long)>::f(p);
    }
    
    bool g(int n) {
        return f(n); // OK
    }
    ```

## Chapter 15: Template Argument Deduction

1) Some constructs are **non deduced contexts**: qualified type names (i.e. everything to the left of `::`), non type expressions that are not just a nontype parameter. For example `template <int N> int f(int x = N * N)`, or `int(&)[sizeof(S<T>)]`: `N` and `T` are not deduced.

2) When the argument of a function call is an initializer list, that argument doesn’t have a specific type, so in general no deduction will be performed. However, if the parameter type P, after removing references and top-level const and volatile qualifiers, is equivalent to `std::initializer_list<P'>` for some type `P'` that has a deducible pattern, deduction proceeds by comparing `P'` to the type of each element in the initializer list.

3) When performing template argument deduction for variadic templates, however, the 1:1 relationship between parameters and arguments no longer holds, because a parameter pack can match multiple arguments:

    ```c++
    template<typename T, typename U> class pair { };

    template<typename T, typename... Rest>
    void h1(pair<T, Rest> const&...);

    template<typename... Ts, typename... Rest>
    void h2(pair<Ts, Rest> const&...);

    void foo(pair<int, float> pif, pair<int, double> pid,
    pair<double, double> pdd)
    {
        h1(pif, pid); // OK: deduces T to int, Rest to {float, double}
        h2(pif, pid); // OK: deduces Ts to {int, int}, Rest to {float, double}
        h1(pif, pdd); // ERROR: T deduced to int from the 1st arg, but to double from the 2nd
        h2(pif, pdd); // OK: deduces Ts to {int, double}, Rest to {float, double}
    }
    ```

4) **Reference collapsing rules**: any `const` or `volatile` qualifiers applied on top of the inner reference are simply discarded (i.e. only qualifiers under the inner reference are retained). Then the two references are reduced to a single reference according to the rule: if either reference is an lvalue reference, so is the resulting type; otherwise, it is an rvalue reference. Template argument deduction behaves in a special way when a function parameter is a forwarding reference: in this case, template argument deduction considers not just the type of the function call argument but also whether that argument is an lvalue or an rvalue. **In the cases where the argument is an lvalue, the type determined by template argument deduction is an lvalue reference to the argument type, and the reference collapsing rules (see above) ensure that the substituted parameter will be an lvalue reference. Otherwise, the type deduced for the template parameter is simply the argument type (not a reference type), and the substituted parameter is an rvalue reference to that type.**

5) Treating a parameter of rvalue reference type as an lvalue is intended as a **safety feature**, because anything with a name (like a parameter) can easily be referenced multiple times in a function. If each of those references could be implicitly treated as an rvalue, its value could be destroyed unbeknownst to the programmer.

6) Despite its name, perfect forwarding is not, in fact, “perfect” in the sense that it does not capture all interesting properties of an expression:
    ```c++
    void g(int*);
    void g(...);

    template<typename T> void forwardToG(T&& x)
    {
        g(std::forward<T>(x)); // forward x to g()
    }

    void foo()
    {
        g(0); // calls g(int*)
        forwardToG(0); // eventually calls g(...)
    }
    ```

    Using `nullptr` instead of `0` works.

7) Perfect forwarding for return types is done using either the auto trailing syntax with the `decltype` of the return expression, or using (since C++14) `decltype(auto)` as the return type. The latter indicates that the compiler should deduce the return type from the definition of the function.

8) If you want a template to support only rvalues, you can use a combination of SFINAE and traits like:
    ```c++
    void int_rvalues(int&&); // accepts rvalues of type int
    template<typename T> void anything(T&&); // SURPRISE: accepts lvalues and 
    // rvalues of any type

    // this is the right way to generalize:
    template<typename T>
    typename std::enable_if<!std::is_lvalue_reference<T>::value>::type
    rvalues(T&&); // accepts rvalues of any type
    ```

9) SFINAE protects against attempts to form invalid types or expressions, including errors due to ambiguities or access control violations, that occur within the *immediate context* of the function template substitution. Specifically, during function template substitution for the purpose of deduction, anything that happens during the instantiation of:
    - the definition of a class template
    - the definition of a function template
    - the initializer of a variable template
    - a default argument
    - a default member initializer
    - exception specifications (such as `noexcept(condition)`)

    is not part of the immediate context. So if substituting the template parameters of a function template declaration requires the instantiation of the body of a class template because a member of that class is being referred to, an error during that instantiation is not in the immediate context of the function template substitution and is therefore a real error. For example:

    ```c++
    template<typename T>
    class Array {
    public:
        using iterator = T*;
    };

    template<typename T>
    void f(Array<T>::iterator first, Array<T>::iterator last);

    template<typename T>
    void f(T*, T*);

    int main()
    {
        f<int&>(0, 0); // ERROR: substituting int& for T in the first function template 
        // instantiates Array<int&>, which then fails
    }
    ```

10) There are some cases where deduction succeeds even if the parametrized type P is not identical to the argument type A, for example when the substituted P type is a base
class type of the A type or a pointer to a base class type of the class type for which A is a pointer type:
    ```c++
    template<typename T>
    class B {
    };

    template<typename T>
    class D : public B<T> {
    };

    template<typename T> void f(B<T>*);

    void g(D<long> dl)
    {
        f(&dl); // deduction succeeds with T substituted with long
    }
    ```

11) If P does not contain a template parameter in a deduced context, then all implicit conversion are permissible. For example:
    ```c++
    template<typename T> int f(T, typename T::X);
    struct V {
        V();
        struct X {
            X(double);
        };
    } v;

    int r = f(v, 7.0);
    // OK: T is deduced to int through the first parameter,
    // which causes the second parameter to have type V::X
    // which can be constructed from a double value
    ```

12) Default arguments to template functions cannot be used to deduce template arguments.

13) It is possible to explicitly specify some template arguments while having others be deduced. However, the explicitly specified ones are always matched left-to-right with the template parameters. Therefore, parameters that cannot be deduced (or that are likely to be specified explicitly) should be specified first. **A pack can be partially explicitly specified and partially deduced**.

14) C++11 includes the ability to declare a variable whose type is deduced from its initializer. This is called **Deduction from Initializers and Expressions**.

15) The `auto` type specifier can be used in a number of places (primarily, namespace scopes and local scopes) to deduce the type of a variable from its initializer. In such cases, `auto` is called a placeholder type. Deduction for `auto` uses the same mechanism as template argument deduction. **One of the immediate consequences of this is that a variable of type `auto` will never be a reference type.** Deduction for `auto&` and `auto&&` follows the same mechanism as well, respectively for `T&` and `T&&`, where the latter is a forwarding reference:

    ```c++
    int x;
    auto&& rr = 42;
    // OK: rvalue reference binds to an rvalue (auto = int)
    auto&& lr = x;
    // Also OK: auto = int& and reference collapsing makes
    // lr an lvalue reference
    ```

    `std::forward<T>()` can be invoked as usual on the variable declared with `auto&&` as usual, if perfect forwarding of the bound value is desired.

16) If we want to force a homogeneous pack of nontype template parameters, that is possible too:
    ```c++
    template<auto V1, decltype(V1)... VRest> struct HomogeneousValues {};
    ```
    However, the template argument list cannot be empty in that particular case.

17) If e is the name of an entity (such as a variable, function, enumerator, or data member) or a class member access, `decltype(e)` yields the declared type of that entity or the denoted class member. Thus, decltype can be used to inspect the type of a variable. Otherwise, if e is any other expression, `decltype(e)` produces a type that reflects the type and value category of that expression:
    -  If `e` is an lvalue of type `T`, `decltype(e)` produces `T&`
    -  If `e` is an xvalue of type `T`, `decltype(e)` produces `T&&`
    - If `e` is a prvalue of type `T`, `decltype(e)` produces `T`

18) `decltype(e)` preserves enough information about an expression to make it possible to describe the return type of a function that returns the expression e itself “perfectly”:
    ```c++
    decltype(f()) g()
    {
        return f();
    }
    ```

    We don't need to write `f` twice, since C++14 added `decltype(auto)`.

19) Parentheses in the initializer of a `decltype(auto)` may be significant since they are significant for the `decltype` construct:
    ```c++
    int x;
    decltype(auto) z = x; // object of type int
    decltype(auto) r = (x); // reference of type int&

    decltype(auto) f() {
        int r = g();
        return (r);
        // run-time ERROR: returns reference to temporary
    }
    ```

20) There are a few special situations for the otherwise simple deduction rules of `auto`. The first is when the initializer for a variable is an initializer list. The corresponding deduction for a function call would fail, because we cannot deduce a template type parameter from an initializer list argument:

    ```c++
    template<typename T>
    void deduceT(T);
    ...
    deduceT({ 2, 3, 4}); // ERROR
    deduceT({ 1 }); // ERROR
    ```

    However, if our function has a more specific parameter as follows:
    ```c++
    template<typename T>
    void deduceInitList(std::initializer_list<T>);
    ...
    deduceInitList({ 2, 3, 5, 7 }); // OK: T deduced as int
    ```
    then deduction succeeds. 
    
21) Copy-initializing (i.e., initialization with the = token) an `auto` variable
with an initializer list is therefore defined in terms of that more specific parameter:
    ```c++
    auto primes = { 2, 3, 5, 7 }; // primes is std::initializer_list<int>
    deduceT(primes); // T deduced as std::initializer_list<int>
    ```

    Before C++17, the corresponding direct-initialization of `auto` variables (i.e., without the = token) was also handled that way, but this was changed in C++17. Thus prior to C++17 `auto val { 2 }` makes `val` an initializer list. Now it's simply an `int` with value `2`.

22) Returning a braced initializer list for a function with a deducible placeholder type is invalid. That is because an initializer list in function scope is an object that points into an underlying array object (with the element values specified in the list) that expires when the function returns. Allowing the construct would thus encourage what is in effect a dangling reference.

23) Any use of a function template with a deduced return type requires the immediate instantiation of that template to determine the return type with certainty. That, however, has a surprising consequence when it comes to SFINAE:

    ```c++
    template<typename T, typename U>
    auto addA(T t, U u) -> decltype(t+u)
    {
        return t + u;
    }
    void addA(...);

    template<typename T, typename U>
    auto addB(T t, U u) -> decltype(auto)
    {
        return t + u;
    }
    void addB(...);

    struct X {};
    using AddResultA = decltype(addA(X(), X())); // OK: AddResultA is void
    using AddResultB = decltype(addB(X(), X())); // ERROR: instantiation of addB<X>
    //is ill-formed
    ```

    The function body of the addB() template must be fully instantiated to determine its return type. That instantiation isn’t in the immediate context of the call to `addB()` and therefore doesn’t fall under the SFINAE filter but results in an outright error.

    **It is therefore important to remember that deduced return types are not merely a shorthand for a complex explicit return type and they should be used with care (i.e., with the understanding that they shouldn’t be called in the signatures of other function templates that would count on SFINAE properties).**

24) Three different kinds of entities can initialize a *structured binding*:
    -  The first case is the simple *class* type, where all the nonstatic data members are public. In that case, the number of bracketed identifiers must equal the number of members.
    - The second case corresponds to arrays. With `auto []` the array is copied from the initializer, element by element. `auto& []` does not involve copying, but the identifiers become aliases for the array elements.
    - Finally, a third option allows `std::tuple`-like classes to have their elements decomposed through a template-based protocol using `get<>()`.

25) An `auto` in a parameter of a lambda is handled similarly to an `auto` in the type of a variable with an initializer: It is replaced by an invented template type parameter `T`. The member function template is instantiated when the closure is invoked, which is usually not at the point where the lambda expression appears.

26) For the purposes of template argument deduction, template aliases are transparent: they can be used to clarify and simplify code but have no effect on how deduction operates.

27) C++17 introduces **CTAD: class template argument deduction**. All parameters must be determined by the deduction process or from default arguments. Deduction guides are needed, except for cases where they are implicit, like constructors. 

28) CTAD introduced  subtleties like:
    ```c++
    template<typename T> struct X {
        template<typename Iter> X(Iter b, Iter e);
        template<typename Iter> auto f(Iter b, Iter e) {
            return X(b, e); // What is this?
        }
    };
    ```
    This code is valid C++14: The `X` in `X(b, e)` is the injected class name and is equivalent to `X<T>` in this context. The rules for class template argument deduction, however, would naturally make that `X` equivalent to `X<Iter>`. In order to maintain backward compatibility, however, class template argument deduction is disabled if the name of the template is an injected class name.

    Consider:

    ```c++
    template<typename T> struct Y {
        Y(T const&);
        Y(T&&);
    };

    void g(std::string s) {
        Y y = s;
    }
    ```

    Clearly, the intent here is that we deduce `T` to be `std::string` through the implicit deduction guide associated with the copy constructor. Writing the implicit deduction guides as explicitly declared guides reveals a surprise, however:

    ```c++
    template<typename T> Y(T const&) -> Y<T>; // #1
    template<typename T> Y(T&&) -> Y<T>; // #2
    ```

    Implicit guide #1 deduces `T` to be `std::string` but requires the argument
    to be adjusted from `std::string` to `std::string const`. Guide #2 , however, deduces `T` to be a reference type `std::string&`, which is a better match because no `const` must be added for type adjustment purposes. This outcome would be rather surprising and likely would result in instantiation errors when the class template parameter is used in contexts that do not permit reference types or even worse dangling references. The C++ standardization committee therefore decided to disable the special deduction rule for `T&&` when performing deduction for implicit deduction guides if the `T` was originally a class template parameter (as opposed to a constructor template parameter; for those, the special deduction rule remains).

29) A deduction guide can be declared with the keyword `explicit`. It is then considered only for direct-initialization cases, not for copy-initialization cases.

30) Deduction guides are not function templates: they are only used to deduce template parameters and are not “called.” That means that the difference between passing arguments by reference or by value is not important for guiding declarations:

    ```c++
    template<typename T> struct X {
    ...
    };

    template<typename T> struct Y {
        Y(X<T> const&);
        Y(X<T>&&);
    };

    template<typename T> Y(X<T>) -> Y<T>;
    ```

    Note how the deduction guide does not quite correspond to the two constructors of `Y`. However, that does not matter, because the guide is only used for deduction. Given a value `xtt` of type `X<TT>` lvalue or rvalue, it will select the deduced type `Y<TT>`. Then, initialization will perform overload resolution on the constructors of `Y<TT>` to decide which one to call (which will depend on whether `xtt` is an lvalue or an rvalue).

## Chapter 16: Specialization and Overloading

1) Two functions can coexist in a program if they have distinct signatures. We define the **signature** of a function as the following information:
    - The unqualified name of the function (or the name of the function template from which it was generated)
    - The class or namespace scope of that name and, if the name has internal linkage, the translation unit in which the name is declared
    -  The `const`, `volatile`, or `const volatile` qualification of the function (if it is a member func- tion with such a qualifier)
    -  The `&` or `&&` qualification of the function (if it is a member function with such a qualifier)
    - The types of the function parameters (before template parameters are substituted if the function is generated from a function template)
    - Its return type, if the function is generated from a function template
    - The template parameters and the template arguments, if the function is generated from a function template

    This means that the following templates and their instantiations could, in principle, coexist in the same program:

    ```c++
    template<typename T1, typename T2>
    void f1(T1, T2);
    template<typename T1, typename T2>
    void f1(T2, T1);
    template<typename T>
    long f2(T);
    template<typename T>
    char f2(T);
    ```

    However, they cannot always be used when they’re declared in the same scope because instantiating both creates an overload ambiguity. For example, calling `f2(42)` when both the templates above are declared will clearly create an ambiguity.

    Similarly:

    ```c++

    template<typename T1, typename T2>
    void f1(T1, T2)
    {
        std::cout << "f1(T1, T2)\n";
    }

    template<typename T1, typename T2>
    void f1(T2, T1)
    {
        std::cout << "f1(T2, T1)\n";
    }

    // fine so far
    int main()
    {
        f1<char, char>(’a’, ’b’); // ERROR: ambiguous
    }
    ```

2) In what follows, we describe the exact procedure to determine whether one function template participating in an overload set is more specialized than the other. Note that these are partial ordering rules: It is possible that given two templates, neither can be considered more specialized than the other. If overload resolution must select between two such templates, no decision can be made, and the program contains an ambiguity error. 
    -  Function call parameters that are covered by a default argument and ellipsis parameters that are not used are ignored in what follows.
    - We synthesize two artificial lists of argument types by substituting every template parameter as follows: 1. Replace each template type parameter with a unique invented type. 2. Replace each template template parameter with a unique invented class template. 3. Replace each nontype template parameter with a unique invented value of the appropriate type.
    - If template argument deduction of the second template against the first synthesized list of argument types succeeds with an exact match, but not vice versa, then the first template is more specialized than the second. Conversely, if template argument deduction of the first template against the second synthesized list of argument types succeeds with an exact match, but not vice versa, then the second template is more specialized than the first. Otherwise (either no deduction succeeds or both succeed), there is no ordering between the two templates.

    Consider:

    ```c++
    template<typename T>
    void t(T*, T const* = nullptr, ...);

    template<typename T>
    void t(T const*, T*, T* = nullptr);

    void example(int* p)
    {
        t(p, p);
    }
    ```

    First, because the actual call does not use the ellipsis parameter for the first template and the last parameter of the second template is covered by its default argument, these parameters are ignored in the partial ordering.

    The synthesized lists of argument types are `(A1*, A1 const*)` and `(A2 const*, A2*)`.  Template argument deduction of `(A1*, A1 const*)` versus the second template actually succeeds with the substitution of `T` with `A1 const`, but the resulting match is not exact because a qualification adjustment is needed to call `t<A1 const>(A1 const*, A1 const*, A1 const* = 0)` with arguments of types `(A1*, A1 const*)`.  Similarly for the first template from the argument type list `(A2 const*, A2*)`. Therefore, there is no ordering relationship between the two templates, and the call is ambiguous.

3) Function templates can be overloaded with nontemplate functions. All else being equal, the non template function is preferred in selecting the actual function being called. But take care when `const` qualifier appears:

    ```c++
    class C {
    public:
        C() = default;
        C (C const&) {
            std::cout << "copy constructor\n";
        }
        C (C&&) {
            std::cout << "move constructor\n";
        }
        template<typename T>
        C (T&&) {
            std::cout << "template constructor\n";
        }
    };

    int main()
    {
        C x;
        C x2{x}; // prints: template constructor
        C x3{std::move(x)}; // prints: move constructor
        C const c;
        C x4{c}; // prints: copy constructor
        C x5{std::move(c)}; // prints: template constructor
    }
    ```

    **For this reason, usually you have to partially disable such member function templates when they might hide copy or move constructors.**

4) There's a special rule that considers nonvariadic templates (those with a fixed number of parameters) to be more specialized than variadic templates (with a variable number of parameters). Point 2) is extended so that any argument derived from a variadic parameter cannot match a parameter that is not a parameter pack.

5) The ability to overload function templates, combined with the partial ordering rules to select the “best” matching function template, allows us to add more specialized templates to a generic implementation to tune code transparently for greater efficiency. However, class templates and variable templates cannot be overloaded. Instead, another mechanism was chosen to enable transparent customization of class templates: **explicit specialization**. The standard term explicit specialization refers to a language feature that we call **full specialization** instead. **Partial specialization** is similar to full specialization, but instead of fully substituting the template parameters, some parameterization is left in the alternative implementation of a template.

6) A full class specialization is introduced with a sequence of three tokens: `template`, `<`, and `>`.

7) Because it is not a template declaration, the members of a full class template specialization can be defined using the ordinary out-of-class member definition syntax (in other words, the `template<>` prefix cannot be specified).

8) A full specialization is a replacement for the instantiation of a certain generic template, and it is not valid to have both the explicit and the generated versions of a template present in the same program:

    ```c++
    template<typename T>
    class Invalid {
    };

    Invalid<double> x1;
    // causes the instantiation of Invalid<double>
    template<>
    class Invalid<double>; // ERROR: Invalid<double> already instantiated
    ```

9) Care must be taken to ensure
that the declaration of the specialization is visible to all the users of the generic template. In practical terms, this means that a declaration of the specialization should normally follow the declaration of the template in its header file.

10) A full specialization is in many ways similar to a normal declaration (or rather, a normal redeclaration). In particular, it does not declare a template, and therefore only one definition of a noninline full function template specialization should appear in a program.

11) Variable templates can also be fully specialized. By now, the syntax should be intuitive:
    ```c++
    template<typename T> constexpr std::size_t SZ = sizeof(T);
    template<> constexpr std::size_t SZ<void> = 0;
    ```
    Interestingly, a variable template specialization is not required to have a type matching that of the template being specialized.

12) If, while attempting
to match a partial specialization an invalid construct is formed, that specialization is silently abandoned and another candidate is examined if one is available. If no matching specializations is found, the primary template is selected. If multiple matching specializations are found, the most specialized one (in the sense defined for overloaded function templates) is selected; if none can be called most specialized, the program contains an ambiguity error.

13) Class templates can be partially specialized, whereas function templates are simply overloaded.

14) Template argument deduction for pack expansions only works when the pack expansion occurs at the end of the parameter or argument list.

## Chapter 17: Future Directions

## Chapter 18: The Polymorphic Power of Templates

1) Templates can also be used to implement polymorphism. However, they don’t rely on the factoring of common behavior in base classes.

2) Polymorphism implemented via inheritance is bounded and dynamic:
    -  **Bounded** means that the interfaces of the types participating in the polymorphic behavior are predetermined by the design of the common base class (other terms for this concept are invasive and intrusive).
    -  **Dynamic** means that the binding of the interfaces is done at run time (dynamically).

    Polymorphism implemented via templates is unbounded and static:
    - **Unbounded** means that the interfaces of the types participating in the polymorphic behavior are not predetermined (other terms for this concept are noninvasive and nonintrusive).
    - **Static** means that the binding of the interfaces is done at compile time (statically).

3) Dynamic polymorphism in C++ exhibits the following strengths:
    - Heterogeneous collections are handled elegantly.
    - The executable code size is potentially smaller (because only one polymorphic function is needed, whereas distinct template instances must be generated to handle different types).
    - Code can be entirely compiled; hence no implementation source must be published (distributing template libraries usually requires distribution of the source code of the template implementations).

4) In contrast, the following can be said about static polymorphism in C++:
    - Collections of built-in types are easily implemented. More generally, the interface commonality need not be expressed through a common base class.
    - Generated code is potentially faster (because no indirection through pointers is needed a priori and nonvirtual functions can be inlined much more often).
    - Concrete types that provide only partial interfaces can still be used if only that part ends up being exercised by the application.

## Chapter 19: Implementing Traits

1) Consider:

    ```c++
    template<typename T>
    T accum (T const* beg, T const* end)
    {
        T total{}; // assume this actually creates a zero value
        while (beg != end) {
            total += *beg;
            ++beg;
        }
        return total;
    }
    ```

    The only slightly subtle decision here is how to create a zero value of the correct type to start our summation. Instantiating the template for the type `char`, turns out to be too small a range for the accumulation of even relatively small values. We could resolve this by introducing an additional template parameter `AccT` that describes the type used for the variable `total` (and hence the return type). However, this would put an extra burden on all users of our template. An alternative approach to the extra parameter is to create an association between each type `T` for which `accum()` is called and the corresponding type that should be used to hold the accumulated value. **This association could be considered characteristic of the type `T`, and therefore the type in which the sum is computed is sometimes called a trait of `T`**.

    ```c++
    template<typename T>
    struct AccumulationTraits;

    template<>
    struct AccumulationTraits<char> {
        using AccT = int;
    };

    template<typename T>
    auto accum (T const* beg, T const* end)
    {
        // return type is traits of the element type
        using AccT = typename AccumulationTraits<T>::AccT;
        
        AccT total{}; // assume this actually creates a zero value
        while (beg != end) {
            total += *beg;
            ++beg;
        }
        return total;
    }
    ```
2) Traits are not limited to a main type as above, but can be associated to values as well. These are called *value traits*:
    ```c++
    template<typename T>
    struct AccumulationTraits;

    template<>
    struct AccumulationTraits<char> {
        using AccT = int;
        static AccT const zero = 0;
    };
    ```

    This way we can write `AccT total = AccumulationTraits<T>::zero;` to have the guarantee that `total` is initialized to zero for that specific type.

3) The use of traits in `accum()` in the previous sections is called **fixed**, because once the decoupled trait is defined, it cannot be replaced in the algorithm. There may be cases when such overriding is desirable. We can address this problem by adding a template parameter `AT` for the trait itself having a default value determined by our traits template: `template<typename T, typename AT = AccumulationTraits<T>>`.

4) So far we have equated accumulation with summation. However, we can imagine other kinds of accumulations. For example, we could multiply the sequence of given values. In all the alternatives, the only `accum() ` operation that needs to change is `total += *beg`. This operation can be called a **policy** of our accumulation process. Here is an example of how we could introduce such a policy in our `accum()` function template:
    ```c++
    template<typename T,
    typename Policy = SumPolicy,
    typename Traits = AccumulationTraits<T>>
    auto accum (T const* beg, T const* end)
    {
        using AccT = typename Traits::AccT;
        AccT total = Traits::zero();
        while (beg != end) {
            Policy::accumulate(total, *beg);
            ++beg;
        }
        return total;
    }
    ```

    In this case, we may recognize that the initialization of an accumulation loop is a part of the accumulation policy. This policy may or may not make use of the trait `zero()`. Other alternatives are not to be forgotten: not everything must be solved with traits and policies. For example, the `std::accumulate()` function of the C++ standard library takes the initial value as a third (function call) argument.

5) 
    - **Traits represent natural additional properties of a template parameter**
    - **Policies represent configurable behavior for generic functions and types (often with some commonly used defaults)**
    - **Traits can be useful as fixed traits (i.e., without being passed through template parameters)**
    - **Traits mostly combine types and constants rather than member functions**
    - **Policy classes don’t contribute much if they aren’t passed as template parameters**
    - **Policy parameters need not have default values and are often specified explicitly**
    - **Policy classes mostly combine member functions**

6) To implement an accumulation policy, we can choose to express `SumPolicy` and `MultPolicy` as ordinary classes with a member template. An alternative consists of designing the policy class interface using class templates, which are then used as template template arguments:
    ```c++
    template<typename T1, typename T2>
    class SumPolicy {
    public:
        static void accumulate (T1& total, T2 const& value) {
            total += value;
        }
    };

    template<typename T,
    template<typename, typename> class Policy = SumPolicy, typename Traits = AccumulationTraits<T>>
    auto accum (T const* beg, T const* end)
    {
        using AccT = typename Traits::AccT;
        AccT total = Traits::zero();
        while (beg != end) {
            Policy<AccT,T>::accumulate(total, *beg);
            ++beg;
        }
        return total;
    }

    ```

7)  With templates, we can additionally define **type functions**: functions that takes some type as arguments and produce a type or a constant as a result. A very useful built-in type function is `sizeof`. How is a type function useful? For example, it allows us to parameterize a template in terms of a container type without also requiring parameters for the element type and other characteristics:
    ```c++
    // instead of:
    template<typename T, typename C>
    T sumOfElements (C const& c);

    // we write:
    template<typename C>
    struct ElementT {
        using Type = typename C::value_type;
    };
    template<typename C>
    typename ElementT<C>::Type sumOfElements (C const& c);
    ```

8) In addition to providing access to particular aspects of a main parameter type, traits can also perform transformations on types, such as adding or removing references or `const` and `volatile` qualifiers. For example:
    ```c++
    template<typename T>
    struct RemoveReferenceT {
        using Type = T;
    };

    template<typename T>
    struct RemoveReferenceT<T&> {
        using Type = T;
    };

    template<typename T>
    struct RemoveReferenceT<T&&> {
        using Type = T;
    };
    ```
    Again, a convenience alias template makes the usage simpler:
    ```c++
    template<typename T>
        using RemoveReference = typename RemoveReference<T>::Type;
    ```

9) Transformation traits can be composed, such as creating a `RemoveCVT` trait that removes both `const` and `volatile`:
    ```c++
    template<typename T>
        struct RemoveCVT : RemoveConstT<typename RemoveVolatileT<T>::Type> {
    };
    
    template<typename T>
        using RemoveCV = typename RemoveCVT<T>::Type;
    ```

10) A decay type trait can be implemented as follows:
    ```c++
    //  First, we define the nonarray, nonfunction case, which
    //  simply deletes any const and volatile qualifiers
    template<typename T>
    struct DecayT : RemoveCVT<T> {};

    // Next we handle arrays with known and unknown bounds
    template<typename T>
    struct DecayT<T[]> {
        using Type = T*;
    };

    template<typename T, std::size_t N>
    struct DecayT<T[N]> {
        using Type = T*;
    };

    // Next we handle the function-to-pointer decay
    template<typename R, typename... Args>
    struct DecayT<R(Args...)> {
        using Type = R (*)(Args...);
    };

    template<typename R, typename... Args>
    struct DecayT<R(Args..., ...)> {
        using Type = R (*)(Args..., ...);
    };
    ```
    Note that the second partial specialization matches any function type that uses C-style varargs.

11) In general, we can develop type functions that depend on multiple arguments:
    ```c++
    template<typename T1, typename T2>
    struct IsSameT {
        static constexpr bool value = false;
    };

    template<typename T>
    struct IsSameT<T, T> {
        static constexpr bool value = true;
    };
    ```

    For traits that produce a constant value, we cannot provide an alias template, but we can provide a `constexpr` variable template that fulfills the same role:
    ```c++
    template<typename T1, typename T2>
    constexpr bool isSame = IsSameT<T1, T2>::value;
    ```

12) Another example of type functions that deal with multiple types are **result type traits**. To motivate the idea, let’s write a function template that allows us to add two Array containers:
    ```c++
    template<typename T>
    Array<T> operator+ (Array<T> const&, Array<T> const&);
    ```

    This would be nice, but because the language allows us to add a `char` value to an `int` value, we really would prefer to allow such mixed-type operations with arrays too. We are then faced with determining what the return type of the resulting template should be:
    ```c++
    template<typename T1, typename T2>
    Array<???> operator+ (Array<T1> const&, Array<T2> const&);
    ```
    A result type template allows us to fill in the question marks in the previous declaration as follows:
    ```c++
    template<typename T1, typename T2>
    Array<typename PlusResultT<T1, T2>::Type>
    operator+ (Array<T1> const&, Array<T2> const&);
    ```

    This can be done as follows:
    ```c++
    template<typename T1, typename T2>
    struct PlusResultT {
        using Type = decltype(T1() + T2());
    };

    template<typename T1, typename T2>
    using PlusResult = typename PlusResultT<T1, T2>::Type;
    ```

    This trait template uses `decltype` to compute the type of the expression `T1() + T2()`, leaving the hard work of determining the result type (including handling promotion rules and overloaded operators) to the compiler. However, for the purpose of our motivating example, `decltype` actually *preserves too much in- formation*: our formulation of `PlusResultT` may produce a reference type. In fact, what we want is to transform the result type by removing references and qualifiers, as discussed in the previous section:

    ```c++
    template<typename T1, typename T2>
    Array<RemoveCV<RemoveReference<PlusResult<T1, T2>>>>
    operator+ (Array<T1> const&, Array<T2> const&);
    ```

    Because the expression `T1() + T2()` attempts to value-initialize values of types T1 and T2, both of these types must have an accessible, nondeleted, default constructor (or be nonclass types). Fortunately, it is fairly easy to produce values for the + expression without requiring a constructor, by using a function that produces values of a given type T. For this, the C++ standard provides `std::declval<>`.  It is defined in `<utility>` simply as follows:
    ```c++
    namespace std {
    template<typename T>
        add_rvalue_reference_t<T> declval() noexcept;
    }
    ```

    This function template is intentionally left undefined because it’s only meant to be used within `decltype`, `sizeof`, or some other context where no definition is ever needed. So we can write:
    ```c++
    template<typename T1, typename T2>
    struct PlusResultT {
        using Type = decltype(std::declval<T1>() + std::declval<T2>());
    };
    ```
13) While originally intended to avoid spurious failures with function template overloading, SFINAE also enables remarkable compile-time techniques that can determine if a particular type or expression is valid. Our first foray into SFINAE-based traits illustrates the basic technology using SFINAE with function overloading to find out whether a type is default constructible, so that you can create objects without any value for initialization:
    ```c++
    template<typename T>
    struct IsDefaultConstructibleT {
    private:
        // test() trying substitute call of a default constructor for T passed as U:
        template<typename U, typename = decltype(U())>
        static char test(void*);
        // test() fallback:
        template<typename>
        static long test(...);
    public:
        static constexpr bool value
        = IsSameT<decltype(test<T>(nullptr)), char>::value;
    };
    ```

     The second overload is the fallback: It always matches the call, but because it matches “with ellipsis” (i.e., a vararg parameter), any other match would be preferred. Note that we can’t use the template parameter `T` in the first `test()` directly. Another implementation is as follows:
     ```c++
    template<typename T>
    struct IsDefaultConstructibleHelper {
    private:
        // test() trying substitute call of a default constructor for T passed as U:
        template<typename U, typename = decltype(U())>
        static std::true_type test(void*);
        // test() fallback:
        template<typename>
        static std::false_type test(...);
    public:
        using Type = decltype(test<T>(nullptr));
    };
    
    template<typename T>
    struct IsDefaultConstructibleT : IsDefaultConstructibleHelper<T>::Type {
    };
     ```
    This is the preferred way since a predicate trait, which returns a Boolean value, should return a value derived from `std::true_type` or `std::false_type`.

14) The second approach to implement SFINAE-based traits uses partial specialization. Again, we can use the example to find out whether a type `T` is default constructible:
    ```c++
    // helper to ignore any number of template parameters:
    template<typename...> using VoidT = void;

    // primary template:
    template<typename, typename = VoidT<>>
    struct IsDefaultConstructibleT : std::false_type
    {};

    // partial specialization (may be SFINAE’d away):
    template<typename T>
    struct IsDefaultConstructibleT<T, VoidT<decltype(T())>> : std::true_type
    {};
    ```

    In C++17, the C++ standard library introduced a type trait `std::void_t<>` that corresponds to the type `VoidT` introduced here.

15) In general, a type trait should be able to answer a particular query without causing the program to become ill-formed. Recall the definition of `PlusResultT`:
    ```c++
    template<typename T1, typename T2>
    struct PlusResultT {
    using Type = decltype(std::declval<T1>() + std::declval<T2>());
    };

    template<typename T1, typename T2>
    using PlusResult = typename PlusResultT<T1, T2>::Type;
    ```
    In this definition, the + is used in a context that is not protected by SFINAE. 
    Therefore, if a program attempts to evaluate `PlusResultT` for types that do not have a suitable + operator, the evaluation of `PlusResultT` itself will cause the program to become ill-formed, as in the following attempt to declare the return type of adding arrays of unrelated types A and B:

    ```c++
    template<typename T>
    class Array {
    ...
    };

    // declare + for arrays of different element types:
    template<typename T1, typename T2>
    Array<typename PlusResultT<T1, T2>::Type>
    operator+ (Array<T1> const&, Array<T2> const&);
    ```

    The program may fail to compile even if we add a specific overload to adding A and B arrays, because C++ does not specify whether the types in a
    function template are actually instantiated if another overload would be better:

    ```c++
    // declare generic + for arrays of different element types:
    template<typename T1, typename T2>
    Array<typename PlusResultT<T1, T2>::Type>
    operator+ (Array<T1> const&, Array<T2> const&);

    // overload + for concrete types:
    Array<A> operator+(Array<A> const& arrayA, Array<B> const& arrayB);

    void addAB(Array<A> const& arrayA, Array<B> const& arrayB) {
        auto sum = arrayA + arrayB;  // ERROR?: depends on whether the compiler
        // instantiates PlusResultT<A,B>
        ...
    }
    ```

    If the compiler can determine that the second declaration of operator+ is a better match without performing deduction and substitution on the first (template) declaration of operator+, it will accept this code. However, while deducing and substituting a function template candidate, anything that happens during the instantiation of the definition of a class template is not part of the immediate context of that function template substitution, and SFINAE does not protect us from attempts to form invalid types or expressions there.

    To solve this problem, we have to make the `PlusResultT` **SFINAE-friendly**:

    ```c++
    template<typename, typename, typename = std::void_t<>>
    struct HasPlusT : std::false_type{};

    // partial specialization (may be SFINAE’d away):
    template<typename T1, typename T2>
    struct HasPlusT<T1, T2, std::void_t<decltype(std::declval<T1>() + std::declval<T2>())>>
    : std::true_type {};

    template<typename T1, typename T2, bool = HasPlusT<T1, T2>::value>
    struct PlusResultT {
        // primary template, used when HasPlusT yields true
        using Type = decltype(std::declval<T1>() + std::declval<T2>());
    };
    
    template<typename T1, typename T2>
    struct PlusResultT<T1, T2, false> {};
    ```

16) Consider the following trait:
    ```c++
    #include <type_traits>

    // helper to ignore any number of template parameters:
    template<typename...> using VoidT = void;
    // primary template:
    template<typename, typename = VoidT<>>
    struct HasSizeTypeT : std::false_type
    {};

    // partial specialization (may be SFINAE’d away):
    template<typename T>
    struct HasSizeTypeT<T, VoidT<typename T::size_type>> : std::true_type
    {};
    ```

    This trait checks for a member type `size_type`. (It’s also worth noting that this trait technique will also produce a true value for injected class names). Defining a trait such as `HasSizeTypeT` raises the question of how to parameterize the trait to be able to check for any member type name. Unfortunately, this can currently be achieved only via macros, because there is no language mechanism to describe a “potential” name (until C++26 reflection):

    ```c++
    #include <type_traits>
    #define DEFINE_HAS_TYPE(MemType) \
    template<typename, typename = std::void_t<>> \
    struct HasTypeT_##MemType \
    : std::false_type { }; \
    template<typename T> \
    struct HasTypeT_##MemType<T, std::void_t<typename T::MemType>> \
    : std::true_type { }
    ```

17) `decltype(call-expression)` does not require a nonreference, non-void return type to be complete, unlike call expressions in other contexts. Using `decltype(std::declval<T>().begin(), 0)` instead does add the requirement that the return type of the call is complete, because the returned value is no longer the result of the `decltype` operand.

18) Some boilerplate code is always needed to define traits: overloading and calling two `test()` member functions or implementing multiple partial specializations. Next, we will show how in C++17, we can minimize this boilerplate by specifying the condition to be checked in a generic lambda:
    ```c++
    // helper: checking validity of f(args...) for F f and Args... args:
    template<typename F, typename... Args, typename = decltype(std::declval<F>()(std::declval<Args&&>()...))>
    std::true_type isValidImpl(void*);

    // fallback if helper SFINAE’d out:
    template<typename F, typename... Args>
    std::false_type isValidImpl(...);

    // define a lambda that takes a lambda f and returns whether calling f with args is valid
    inline constexpr auto isValid = [](auto f) {
        return [](auto&&... args) {
            return decltype(isValidImpl<decltype(f), decltype(args)&&...>(nullptr)){};
        };
    };

    // helper template to represent a type as a value
    template<typename T>
    struct TypeT {
        using Type = T;
    };

    // helper to wrap a type as a value
    template<typename T>
    constexpr auto type = TypeT<T>{};

    // helper to unwrap a wrapped type in unevaluated contexts
    template<typename T>
    T valueT(TypeT<T>); // no definition needed
    ```

    Then we can do:

    ```c++
    // define to check for data member first:
    constexpr auto hasFirst = isValid([](auto x) -> decltype((void)valueT(x).first) {});
    cout << "hasFirst: " << hasFirst(type<pair<int,int>>) << '\n'; // true

    // define to check for member type size_type:
    constexpr auto hasSizeType = isValid([](auto x) -> typename decltype(valueT(x))::size_type {});
    struct CX {
        using size_type = std::size_t;
    };
    cout << "hasSizeType: " << hasSizeType(type<CX>) << '\n'; // true
    ```

19) Another example of a trait:
    ```c++
    // primary template: yield the second argument by default and rely on 
    // a partial specialization to yield the third argument
    // if COND is false
    template<bool COND, typename TrueType, typename FalseType>
    struct IfThenElseT {
        using Type = TrueType;
    };

    // partial specialization: false yields third argument
    template<typename TrueType, typename FalseType>
    struct IfThenElseT<false, TrueType, FalseType> {
        using Type = FalseType;
    };

    template<bool COND, typename TrueType, typename FalseType>
    using IfThenElse = typename IfThenElseT<COND, TrueType, FalseType>::Type;
    ```

    Note that, unlike a normal C++ if-then-else statement, the template arguments for both the “then” and “else” branches are evaluated before the selection is made, so neither branch may contain ill-formed code, or the program is likely to be ill-formed. Suppose we want to make a trait that makes a type unsigned if it makes sense to do so, otherwise just return the passed type. This naive solution is invalid:

    ```c++
    template<typename T>
    struct UnsignedT {
    using Type = IfThenElse<std::is_integral<T>::value && !std::is_same<T, bool>::value,
        typename std::make_unsigned<T>::type, T>;
    };
    ```
    The instantiation of `UnsignedT<bool>` is still undefined behavior, because the compiler will still attempt to form the type from `typename std::make_unsigned<T>::type`. We can solve this using indirection like:

    ```c++
    // yield T when using member Type:
    template<typename T>
    struct IdentityT {
        using Type = T;
    };

    // to make unsigned after IfThenElse was evaluated:
    template<typename T>
    struct MakeUnsignedT {
        using Type = typename std::make_unsigned<T>::type;
    };

    template<typename T>
    struct UnsignedT {
        using Type = typename IfThenElse<std::is_integral<T>::value && !std::is_same<T,bool>::value,
                MakeUnsignedT<T>,
                IdentityT<T>
            >::Type;
    };
    ```
    The `IfThenElseT` template is available in the C++ standard library as `std::conditional<>`.

20) This is another trait that checks whether a move contructor throws:
    ```c++
    template<typename T>
    struct IsNothrowMoveConstructibleT
    : std::bool_constant<noexcept(T(std::declval<T>()))>
    {};
    ```
    However, this is not SFINAE-friendly, since `T(std::declval<T>())` is invalid if the trait is instantiated with a type that does not have a usable move or copy constructor:

    ```c++
    // primary template:
    template<typename T, typename = std::void_t<>>
    struct IsNothrowMoveConstructibleT : std::false_type
    {};

    // partial specialization (may be SFINAE’d away):
    template<typename T>
    struct IsNothrowMoveConstructibleT
    <T, std::void_t<decltype(T(std::declval<T>()))>>
    : std::bool_constant<noexcept(T(std::declval<T>()))>
    {};
    ```

    Note that there is no way to check whether a move constructor throws without being able to call it directly. That is, it is not enough that the move constructor is public and not deleted, it also requires that the corresponding type is no abstract class (references or pointers to abstract classes work fine). For this reason, the type trait is named `IsNothrowMoveConstructible` instead of `HasNothrowMoveConstructor`.

21) Alias templates can be useful to reduce verbosity of traits; for example given:
    ```c++
    template<typename T1, typename T2>
    Array<typename RemoveCVT<
            typename RemoveReferenceT<typename PlusResultT<T1, T2>::Type>::Type
        >::Type
    >
    operator+ (Array<T1> const&, Array<T2> const&);
    ```
    We can do:

    ```c++
    template<typename T>
        using RemoveCV = typename RemoveCVT<T>::Type;

    template<typename T>
        using RemoveReference = typename RemoveReferenceT<T>::Type;

    template<typename T1, typename T2>
        using PlusResult = typename PlusResultT<T1, T2>::Type;

    template<typename T1, typename T2>
    Array<RemoveCV<RemoveReference<PlusResultT<T1, T2>>>>
    operator+ (Array<T1> const&, Array<T2> const&);
    ```

    Note that alias templates cannot be specialized. In the Standard Library the type traits class templates yield a type in `type` and have no specific suffix (many were introduced in C++11). Corresponding alias templates (that produce the type directly) started being introduced in C++14, and were given a `_t` suffix.

    Similarly `constexpr` variable templates offer a way to reduce verbosity for traits returning a `::value`.

22) In C and C++, function call arguments are passed by value by default. With templates, of course, things get a little more delicate: we don’t know a priori how large the type substituted for the template parameter will be. Furthermore, the decision doesn’t depend just on size: a small structure may come with an expensive copy constructor that would still justify passing read-only parameters by reference-to-const. This problem is conveniently handled using a policy traits template that is a type function: the function maps an intended argument type `T` onto the optimal parameter type `T` or `T const&`. As a first approximation, the primary template can use by-value passing for types no larger than two pointers and by reference-to-const for everything else:
    ```c++
    template<typename T>
    struct RParam {
        using Type = typename IfThenElseT<sizeof(T) <= 2*sizeof(void*),
        T,
        T const&>::Type;
    };
    ```
    On the other hand, container types for which sizeof returns a small value may involve expensive copy constructors, so we may need many specializations and partial specializations, such as the following:

    ```c++
    template<typename T>
    struct RParam<Array<T>> {
        using Type = Array<T> const&;
    };
    ```

    Or:

    ```c++
    template<typename T>
    struct RParam {
        using Type = IfThenElse<(sizeof(T) <= 2*sizeof(void*)
        && std::is_trivially_copy_constructible<T>::value
        && std::is_trivially_move_constructible<T>::value),
        T,
        T const&>;
    };
    ```

    Then client code can do:

    ```c++
    template<typename T1, typename T2>
    void foo (typename RParam<T1>::Type p1, typename RParam<T2>::Type p2)
    {
        ...
    }
    ```

    Unfortunately, there are some significant downsides to using `RParam`. First, the function declaration is significantly messier. Second, and perhaps more objectionable, is the fact that a function like `foo()` cannot be called with argument deduction because the template parameter appears only in the qualifiers of the function parameters. Call sites must therefore specify explicit template arguments. An unwieldy workaround for this option is the use of an inline wrapper function template that provides perfect forwarding, but it assumes the inline function will be elided by the compiler:

    ```c++
    // function that allows parameter passing by value or by reference
    template<typename T1, typename T2>
    void foo_core (typename RParam<T1>::Type p1, typename RParam<T2>::Type p2)
    {
        ...
    }

    // wrapper to avoid explicit template parameter passing
    template<typename T1, typename T2>
    void foo (T1 && p1, T2 && p2)
    {
        foo_core<T1,T2>(std::forward<T1>(p1),std::forward<T2>(p2));
    }
    ```

## Chapter 20: Overloading on Type Properties

1) Consider the `advanceIter()` function which moves an iterator `x` forward by `n` steps. This general algorithm can operate on any input iterator:
    ```c++
    template<typename InputIterator, typename Distance>
    void advanceIter(InputIterator& x, Distance n)
    {
        while (n > 0) { // linear time
            ++x;
            --n;
        }
    }
    ```

    For a certain class of iterators—those that provide random access operations—we can provide a more efficient implementation:

    ```c++
    template<typename RandomAccessIterator, typename Distance>
    void advanceIter(RandomAccessIterator& x, Distance n) {
        x += n;
        // constant time
    }
    ```

    **Unfortunately, defining both of these function templates will result in a compiler error, because function templates that differ only based on their template parameter names are not overloadable.**

2) One approach to algorithm specialization is to “tag” different implementation variants of an algorithm with a unique type that identifies that variant. This is called **tag dispatching**:

    ```c++
        template<typename Iterator, typename Distance>
        void advanceIterImpl(Iterator& x, Distance n, std::random_access_iterator_tag) {
            x += n;
            // constant time
        }

        template<typename Iterator, typename Distance>
        void advanceIter(Iterator& x, Distance n)
        {
            advanceIterImpl(x, n, typename std::iterator_traits<Iterator>::iterator_category());
        }
    ```

    Tag dispatching works well when there is a natural hierarchical structure to the properties used by the algorithm and an existing set of traits that provide those tag values.

3) Another technique is based on the `EnableIf` trait that enable or disable the function:

    ```c++
    template<typename Iterator>
    constexpr bool IsRandomAccessIterator =
    IsConvertible<
        typename std::iterator_traits<Iterator>::iterator_category,
        std::random_access_iterator_tag>;
    
    template<typename Iterator, typename Distance>
    EnableIf<IsRandomAccessIterator<Iterator>>
    advanceIter(Iterator& x, Distance n) {
        x += n; // constant time
    }
    ```
    The `EnableIf` specialization here is used to enable this variant of `advanceIter()` only when the
    iterator is in fact a random access iterator.

    ```c++
    template<bool, typename T = void>
        struct EnableIfT {
    };
    
    template<typename T>
    struct EnableIfT<true, T> {
        using Type = T;
    };
    
    template<bool Cond, typename T = void>
    using EnableIf = typename EnableIfT<Cond, T>::Type;
    ```

    When the condition is `false`, `EnableIf` does not produce a valid type, because the primary class template for `EnableIfT` has no member named `Type`. Normally, this would be an error, but in a SFINAE context—such as the return type of a function template—it has the effect of causing template argument deduction to fail, removing the function template from consideration.
    **We now have established how to explicitly “activate” the more specialized template for the types to which is applies. However, that is not sufficient: we also have to “de-activate” the less specialized template, because a compiler has no way to “order” the two and will report an ambiguity error if both versions apply**. We just use the same `EnableIf` pattern on the less specialized template, except that we negate the condition expression:

    ```c++
    template<typename Iterator, typename Distance>
    EnableIf<!IsRandomAccessIterator<Iterator>>
    advanceIter(Iterator& x, Distance n)
    {
        while (n > 0) { // linear time
            ++x;
            --n;
        }
    }
    ```

4) The previous pattern generalizes to cases where more than two alternative implementations are needed: we equip each alternative with `EnableIf` constructs whose conditions are mutually exclusive for a specific set of concrete template arguments. Generally speaking, tag dispatching supports simple dispatching based on hierarchical tags, while `EnableIf` supports more advanced dispatching based on arbitrary sets of properties determined by type traits.

5) `EnableIf` is typically used in the return type of the function template. However, this approach does not work for constructor templates or conversion function templates, because neither has a specified return type. In such cases, we can instead embed the `EnableIf` in a defaulted template argument:

    ```c++
    template<typename T>
    class Container {
    public:
    // construct from an input iterator sequence:
    template<typename Iterator, typename = EnableIf<IsInputIterator<Iterator>>> Container(Iterator first, Iterator last);
    
    // convert to a container so long as the value types are convertible:
    template<typename U, typename = EnableIf<IsConvertible<T, U>>> operator Container<U>() const; };
    ```

    However, there is a problem here. If we attempt to add yet another overload (e.g., a more efficient version of the `Container` constructor for random access iterators), it will result in an error:
    
    ```c++
    template<typename Iterator, typename = EnableIf<IsInputIterator<Iterator> &&
    !IsRandomAccessIterator<Iterator>>>
    Container(Iterator first, Iterator last);
    ```

    The problem is that the two constructor templates are identical except for the default template argument, but default template arguments are not considered when determining whether two templates are equivalent. We can alleviate this problem by adding yet another defaulted template parameter, so the two constructor templates have a different number of template parameters:

    ```c++
    template<typename Iterator, typename = EnableIf<IsRandomAccessIterator<Iterator>>, typename = int>
    // extra dummy parameter to enable both constructors
    Container(Iterator first, Iterator last); // OK now
    ```

6) It’s worth noting here that C++17’s `constexpr` if feature (see Section 8.5 on page 134) avoids the need for `EnableIf` in many cases:

    ```c++
    template<typename Iterator, typename Distance>
    void advanceIter(Iterator& x, Distance n) 
    {
        if constexpr(IsRandomAccessIterator<Iterator>) 
        {
            // implementation for random access iterators:
            x += n;
            // constant time
        }
        else if constexpr(IsBidirectionalIterator<Iterator>) 
        {
            // implementation for bidirectional iterators:
            if (n > 0) {
                for ( ; n > 0; ++x, --n) {}
            }
            else {
                for ( ; n < 0; --x, ++n) {}
            }
        }
        else{
            ...
        }
    }
    ```

    **However, there are downsides. Using constexpr if in this way is only possible when the difference in the generic component can be expressed entirely within the body of the function template. We still need `EnableIf` in the following situations:**

    - Different “interfaces” are involved
    - Different class definitions are needed
    - No valid instantiation should exist for certain template argument lists

    C++20 concepts solve all of these problems as well. The `requires` clause has additional benefits over `EnableIf`. Constraint subsumption provides an ordering among templates that differ only in their requires clauses, eliminating the need for tag dispatching. Additionally, a `requires` clause can be attached to a nontemplate.

7) The way to enable/disable different implementations of class templates is to use enabled/disabled partial specializations of class templates. To use `EnableIf` with class template partial specializations, we first introduce an unnamed, defaulted template parameter like `void`. This new template parameter serves as an anchor for `EnableIf`, which now can be embedded in the template argument list of the partial specialization. Unlike with overloaded function templates, we don’t need to disable any condition on the primary template, because any partial specialization takes precedence over the primary template:

    ```c++
    template<typename Key, typename Value>
    class Dictionary<Key, Value, EnableIf<HasLess<Key>>>
    {
    private:
        map<Key, Value> data;
    public:
    value& operator[](Key const& key) {
        return data[key];
    }
    ...
    };
    ```

     However, when we add another implementation for keys with a hashing operation, we need to ensure that the conditions on the partial specializations are mutually exclusive.

8) Tag dispatching, too, can be used to select among class template partial specializations. The challenge is finding the best match for the tag:

    ```c++
    // primary template (intentionally undefined):
    template<typename Iterator,
        typename Tag = BestMatchInSet<typename std::iterator_traits<Iterator>::iterator_category,
            std::input_iterator_tag,
            std::bidirectional_iterator_tag,
            std::random_access_iterator_tag>>
    class Advance;

    // bidirectional, constant-time algorithm for random access iterators:
    template<typename Iterator>
    class Advance<Iterator, std::random_access_iterator_tag>
    {
    public:
    using DifferenceType = typename std::iterator_traits<Iterator>::difference_type;
        void operator() (Iterator& x, DifferenceType n) const
        {
            x += n;
        }
    }

    // ... other specializations
    ```

    `BestMatchInSet` is implemented by emulating overload resolution:

    ```c++
    // construct a set of match() overloads for the types in Types...:
    template<typename... Types>
    struct MatchOverloads;

    // basis case: nothing matched:
    template<>
    struct MatchOverloads<> {
        static void match(...);
    };

    // recursive case: introduce a new match() overload:
    template<typename T1, typename... Rest>
    struct MatchOverloads<T1, Rest...> : public MatchOverloads<Rest...> {
        static T1 match(T1);
        // introduce overload for T1
        using MatchOverloads<Rest...>::match; // collect overloads from bases
    };

    // find the best match for T in Types...:
    template<typename T, typename... Types>
    struct BestMatchInSetT {
        using Type = decltype(MatchOverloads<Types...>::match(declval<T>()));
    };

    template<typename T, typename... Types>
    using BestMatchInSet = typename BestMatchInSetT<T, Types...>::Type;
    ```

    In C++17, one can eliminate the recursion with pack expansions in the base class list and in a `using` declaration.

9) What if we took this notion of `EnableIf` to the extreme and encoded every operation that the template performs on its template arguments as part of the `EnableIf` condition? The instantiation of such a template could never fail, because template arguments that do not provide the required operations would cause a deduction failure (via `EnableIf`) rather than allowing the instantiation to proceed. We refer to such templates as **“instantiation-safe”** templates. For example, take:

    ```c++
    template<typename T>
    T const& min(T const& x, T const& y)
    {
        if (y < x) {
            return y;
        }
        return x;
    }
    ```
    This template requires the type `T` to have a `< operator` able to compare two `T` values (specifically, two `T const` lvalues) and then implicitly convert the result of that comparison to `bool` for use in the `if` statement.

## Chapter 21: Templates and Inheritance

1) The designers of C++ had various reasons to avoid zero-size classes. For example, an array of zero- size classes would presumably have size zero too, but then the usual properties of pointer arithmetic would no longer apply. Even though there are no zero-size types in C++, the C++ standard does specify that when an empty class is used as a base class, no space needs to be allocated for it provided that it does not cause it to be allocated to the same address as another object or subobject of the same type.

    The EBCO (**Empty Base Class Optimization**) is an important optimization for template libraries because a number of techniques rely on the introduction of base classes simply for the purpose of introducing new type aliases or providing extra functionality without adding new data.

2) In the context of templates, template parameters are often substituted with empty class types:
    ```c++
    template<typename T1, typename T2>
    class MyClass {
    private:
        T1 a;
        T2 b;
    ...
    };
    ```
    It is entirely possible that one or both template parameters are substituted by an empty class type. If this is the case, then the representation of `MyClass<T1,T2>` may be suboptimal and may waste a word of memory for every instance of a `MyClass<T1,T2>`. This can be avoided by making the template arguments base classes instead:

    ```c++
    template<typename T1, typename T2>
    class MyClass : private T1, private T2 {
    };
    ```

    However, this straightforward alternative has its own set of problems: it doesn’t work when `T1` or `T2` is substituted with a nonclass type or with a union type, it also doesn’t work when the two parameters are substituted with the same type, the class may be `final`.

    A more practical tool can be devised for the common case when a template parameter is known to be substituted by class types only and when another member of the class template is available. The main idea is to "merge" the potentially empty type parameter with the other member using the EBCO. For example, instead of writing:

    ```c++
    template<typename CustomClass>
    class Optimizable {
    private:
        CustomClass info; // might be empty
        void* storage;
    ...
    };
    ```
    A template implementer would use the following:

    ```c++
    template<typename CustomClass>
    class Optimizable {
    private:
        BaseMemberPair<CustomClass, void*> info_and_storage;
    ...
    };
    ```
    `BaseMemberPair` simply inherits from `CustomClass`.

3) Another pattern is the **Curiously Recurring Template Pattern (CRTP)**. This oddly named pattern refers to a general class of techniques that consists of passing a derived class as a template argument to one of its own base classes. In its simplest form, C++ code for such a pattern looks as follows:
    ```c++
    template<typename Derived>
    class CuriousBase {
        ...
    };

    class Curious : public CuriousBase<Curious> {
        ...
    };
    ```

    Or even:

    ```c++
    template<typename Derived>
    class CuriousBase {
        ...
    };

    template<typename T>
    class CuriousTemplate : public CuriousBase<CuriousTemplate<T>> {
        ...
    };
    ```

    By passing the derived class down to its base class via a template parameter, the base class can customize its own behavior to the derived class without requiring the use of virtual functions.

4) A simple application of CRTP consists of keeping track of how many objects of a certain class type were created.

5) CRTP can be useful for **facades**: a CRTP base class defines most or all of the public interface of a class in terms of a much smaller (but easier to implement) interface exposed by the CRTP derived class. This pattern, called the **facade pattern**, is particularly useful when defining new types that need to meet the requirements of some existing interface—numeric types, iterators, containers, and so on.

6) **Mixins** provide an alternative way to customize the behavior of a type without inheriting from it. Mixins essentially invert the normal direction of inheritance, because the new classes are "mixed in" to the inheritance hierarchy as base classes of a class template rather than being created as a new derived class. This approach allows the introduction of new data members and other operations without requiring any duplication of the interface.

    A class template that supports mixins will typically accept an arbitrary number of extra classes from which it will derive:

    ```c++

    template<typename P>
    class Polygon
    {
    private:
        std::vector<P> points;
    public:
    ... // public operations
    };

    template<typename... Mixins>
    class Point : public Mixins...
    {
    public:
        double x, y;
        Point() : Mixins()..., x(0.0), y(0.0) { }
        Point(double x, double y) : Mixins()..., x(x), y(y) { }
    };

    class Label
    {
    public:
        std::string label;
        Label() : label("") { }
    };
    using LabeledPoint = Point<Label>;

    class Color
    {
    public:
        unsigned char red = 0, green = 0, blue = 0;
    };
    using MyPoint = Point<Label, Color>;
    ```

    With this mixin-based Point, it becomes easy to introduce additional information to `Point` without changing its interface, so `Polygon` becomes easier to use and evolve. Users need only apply the implicit conversion from the Point specialization to their mixin class (`Label` or `Color`, above) to access that data or interface.

    Mixins are useful in cases where a template needs some small level of customization—such as decorating internally stored objects with user-specified data.

7) Mixins can be made more powerful by combining them with the Curiously Recurring Template Pattern (CRTP). Here, each of the mixins is actually a class template that will be provided with the type of the derived class. A CRTP-mixin version of `Point` would be written as follows:
    ```c++
    template<template<typename>... Mixins>
    class Point : public Mixins<Point>...
    {
    public:
        double x, y;
        Point() : Mixins<Point>()..., x(0.0), y(0.0) { }
        Point(double x, double y) : Mixins<Point>()..., x(x), y(y) { }
    };
    ```

    This formulation requires some more work for each class that will be mixed in, so classes such as `Label` and `Color` will need to become class templates. However, the mixed-in classes can now tailor their behavior to the specific instance of the derived class they’ve been mixed into. For example, we can mix the previously discussed `ObjectCounter` template into `Point` to count the number of points created by `Polygon` and compose that mixin with other application-specific mixins as well.

## Chapter 22: Bridging Static and Dynamic Polymorphism

## Chapter 23: Metaprogramming

1) *Value metaprogramming* is the computation of compile-time values. Prior to C++14/C++11 this was done using recursive template instantiations, but C++14 removed most of that challenge with the introduction of `constexpr` functions.