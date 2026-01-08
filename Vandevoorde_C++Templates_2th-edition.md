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