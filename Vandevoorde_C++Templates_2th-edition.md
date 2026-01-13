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