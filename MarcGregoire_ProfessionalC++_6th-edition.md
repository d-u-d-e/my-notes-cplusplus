# Professional C++ - Marc Gregoire - 6th edition
## Chapter 2: Working with strings and string views

1) Use `std::string_view` instead of `const string&` or `const char*` whenever a function requires a read-only string as one of its parameters.
2) Functions returning a `string` should return a `const string&` or a `string`, but never a `string_view`, in order to not introduce the risk of invalidating the returned `string_view`.
3) Since c++23 `print`, `println`, `format` require the pattern to be a compile time constant. If you need to implement translations based on a runtime format there is the family of functions `std::vprint_xxx`.
4) An interesting trick to print a character a specific number of times uses the format specifier in the following way: if you want to print '=' 16 times use: `println("|{:=>16}|", "");`
5) Starting from c++23, ranges can be formatted using a format string!
For example:
    ```c++
    vector chars { 'W', 'o', 'r', 'l', 'd', '\t', '!' };
    println("{}", chars);       // ['W', 'o', 'r', 'l', 'd', '\t', '!']
    println("{::#x}", chars);   // [0x57, 0x6f, 0x72, 0x6c, 0x64, 0x9, 0x21]
    println("{:s}", chars);     // World    !   
    println("{:?s}", chars);    // "World\t!"  
    ```
## Chapter 3: Coding with style

1) Use comments to explain usage: function's preconditions, postconditions and exceptions that could be thrown.
2) Do not provide implementation details in comments targeted to users of the code.
3) Use comments to explain complicated code (algorithmically complex or too esoteric).
4) Do not convey meta-information (such as author) because this is the job of modern version control systems.
5) Use comments to write a copyright notice.
6) Known bugs should be handled by your bug and feature tracking system.

## Chapter 4: Designing Professional C++ Programs

1) Before you use a library, make sure that you understand the process for submitting bugs and that 
you realize how long it will take for bugs to get fixed. If possible, determine how long the library will continue to be supported so that you can plan accordingly.

## Chapter 5: Designing with classes

1) Relationship has-a is preferred over relationship is-a.

## Chapter 6: Designing for reuse

1) You should design code that both you and other programmers can reuse.
2) When designing your interface, do not expose implementation details to your clients.
3) Templates are ideal for generic programming. They are type safe and result in highly optimized code for each instantiation of the template.
4) When you want to provide different behaviors for related types, use inheritance.
Use templates when you want to provide identical functionality for different types.
5) There are two opposite styles for designing safe code.
The first is called design-by-contract, which means that the 
documentation for a function or a class represents a contract with a detailed description of what the 
responsibility of the client code is and what the responsibility of your function or class is.
Preconditions, postconditions, and invariants are important aspects of design-by-contract.
The second style is that you design your functions and classes to be as safe as possible.
This is called defensive programming.
6) You should strive to design your classes in such a way that they can be extended by deriving another 
class from them, but they should be closed for modification; this is called the open/closed principle.
7) When you have a choice between following standard interface models and 
branching out in a new direction, it’s usually better to stick to the interface to which people are 
accustomed.
8) Don’t provide unnecessary functionality in your interfaces; keep them clean and simple.
A fat interface can be avoided by breaking up the interface into several smaller ones. Alternatively, the façade design pattern can be used to provide an easier interface or interfaces on top of a fat interface.
9) A good abstraction means that the exported interface has only public member functions that are stable and will not change. A specific technique to accomplish this is called the private implementation idiom, or *pimpl idiom*.

## Chapter 7: Memory management

1) CTAD is deliberately disabled for `std::unique_ptr` to avoid the issues with `std::unique_ptr(new T[N])`, which looks just like `std::unique_ptr(new T)` but needs `delete[]` instead of `delete`. This is true also for `std::shared_ptr`.
2) The only safe way to have multiple `shared_ptr` instances point to the same memory is to simply copy such shared_ptrs.

## Chapter 8: Gaining proficiency with classes and objects

1) Starting with C++23, instead of relying on the compiler to provide an implicit *this* parameter, you can use an explicit object parameter, usually of a reference type.
2) The term default constructor does not refer only to the constructor that is automatically 
generated if you fail to declare any constructors yourself. It also refers to the constructor that is defaulted to if no arguments are required.
3) If a class has data members that have a deleted default constructor, then the default constructor for the class is automatically deleted as well.
4) Ctor-initializers allow initialization of data members at the time of their creation.
5) Ctor-initializers initialize data members in their declared order in the class definition, not their order in the ctor-initializer list.
6) When a class has both an initializer-list constructor and another single-argument constructor, then you should be careful to call the correct one.
7) It is possible to pass a boolean argument to `explicit` to turn it into a conditional explicit.
If there is a real use case for implicit conversions, you can mark the constructor with `explicit(false)`.
8) You could actually declare the assignment operator to return whatever type you wanted, including `void`. However, you should always return a reference to the object on which it is called because that’s what clients expect.
9) Your assignment operator needs to take the possibility of self-assignment into account. 
10) If your class requires special handling for copy operations, always implement both the copy constructor and the copy assignment operator.

## Chapter 9: Mastering classes and objects

1) The `friend` declaration in the class serves as the function’s prototype. There’s no need to write the prototype elsewhere.
2) If a noexcept function does throw an exception, the program is terminated.
3) A requirement for implementing the exception-safe copy-and-swap idiom is that `swap()` never throws 
any exceptions, so it is marked as `noexcept`.
4) If a temporary is assigned to an rvalue reference, the lifetime of the temporary is extended for as long as the rvalue reference is in scope.
5) C++23 introduces the `auto(x)` or `auto{x}` syntax to create a copy of an object `x` as an rvalue.
6) Move constructors and move assignment operators should be marked with the noexcept specifier to tell the compiler that they don’t throw any exceptions. This is particularly important for compatibility with the Standard Library.
7) Depending on your class and depending on how you are moving one instance of your class to another 
instance, this self-assignment test might not always be necessary. However, you should always include it, just as the C++ Core Guidelines recommend.
8) When you declare one or more of the special member functions (destructor, copy constructor, move constructor, copy assignment operator, and move assignment operator), then it’s recommended to declare all of them. This is called *the rule of five*.
9) The `std::exchange()` function is useful in implementing move assignment operators.
10) You can write the move constructor and the move assignment operator in terms of the `swap()` function. For example:
    ```c++
    Spreadsheet::Spreadsheet(Spreadsheet&& src) noexcept
    {
        swap(src);
    }

    Spreadsheet& Spreadsheet::operator=(Spreadsheet&& rhs) noexcept
    {
        auto moved { std::move(rhs) };
        swap(moved);
        return *this;
    }
    ```
    The Spreadsheet move assignment operator could also be implemented as follows:
    ```c++
    Spreadsheet& Spreadsheet::operator=(Spreadsheet&& rhs) noexcept
    {
        swap(rhs);
        return *this;
    }
    ```
    However, doing so does not guarantee that the contents of `this` is immediately cleaned up. Instead, 
    the contents of `this` escapes through `rhs` to the caller of the move assignment operator and thus 
    might stay alive longer than expected.

11) Note that for NRVO, even though the copy/move constructors won’t 
be called, they still need to be accessible; otherwise, the program is ill-formed according to the standard.
12) *Prefer pass-by-value* for parameters that a function inherently would copy, but only if the parameter is of a type that supports move semantics and only if you don’t need polymorphic behavior on the parameter. Otherwise, use reference-to const parameters. Passing polymorphic types by value can result in slicing.
13) In modern C++, adopt the *rule of zero* for non polymorphing types.
14) C++23 introduces the concept of explicit object parameters. This allows you to rewrite ref-­qualified member functions using a slightly different syntax:
    ```c++
    class TextHolder
    {
    public:
        const string& getText(this const TextHolder& self)
        { 
            return self.m_text; 
        }
        string&& getText(this TextHolder&& self) 
        { 
            return move(self.m_text); 
        }
        TextHolder& operator=(this TextHolder& self, const string& rhs)
        {
            self.m_text = rhs;
            return self;
        }
    };
    ```
15) Advanced C++ compilers do not require you to put definitions of inline
member functions in the same file as the class definition. For example, Microsoft Visual C++ supports *Link-­Time Code Generation* (LTCG).
16) The `constexpr` keyword specifies that a function could be executed at compile time, but it does not guarantee compile-­time execution. If you really want the guarantee that a function is always evaluated at compile time, you need to use the `consteval` keyword to turn a function into an immediate function. 
17) `if consteval` statement can be used to check if constant evaluation is happening inside a `constexpr` function.
18) You should use `static constexpr` (or `constexpr static`) data members in place of global constants when the constants apply only to the class, also called class constants.
19) Since C++20 you don't need to write all comparision operators.
If a defaulted `operator<=>` works for your class, then a single line of code to explicitly default `operator<=>` as a member function is all you need to do. In certain cases, you might need to explicitly default `operator==` as well. Otherwise, just overload and implement `operator==` and `<=>` as member functions.
A compiler will not rewrite == or != comparisons in terms of <=>. This is done to avoid performance issues, as an explicit implementation of `operator==` is typically more efficient than using <=>.
20)  If you pass a data member as an argument to the parent constructor, it will be uninitialized.
21) Calls to virtual member functions from within a constructor or destructor are resolved statically at compile time.
22) When upcasting, use a pointer or reference to the base class to avoid *slicing*.
23) In multiple inheritance, the solution to the ambiguity is either to explicitly upcast the object using a `dynamic_cast()`, essentially hiding the undesired version of the member function from the compiler, or to use a *disambiguation syntax*:

    ```c++
    class Dog
    {
    public:
        virtual void bark() { println("Woof!"); }
        virtual void eat() { println("The dog ate."); }
    };
    class Bird
    {
    public:
        virtual void chirp() { println("Chirp!"); }
        virtual void eat() { println("The bird ate."); }
    };
    class DogBird : public Dog, public Bird
    {
    };

    int main()
    {
        DogBird myConfusedAnimal;
        dynamic_cast<Dog&>(myConfusedAnimal).eat(); // Calls Dog::eat()
        myConfusedAnimal.Dog::eat(); // Calls Dog::eat()
    }

24) One of the most compelling and simple uses of multiple inheritance is for the implementation of *mixin* classes.
25) In C++, an overriding member function can change the return type as long as the return type of the member function in the base class is a pointer or reference to a class, and the return type in the derived class is a pointer or reference to a descendant.
Such types are called *covariant return types*.
A good way to figure out whether you can change the return type of an overridden member function is to consider whether existing code would still work; this is called the *Liskov substitution principle* (LSP).
This does not apply if you return smart pointers (reason is they are templates).
26) In C++, you cannot override a static member function. A member function cannot be both `static` and `virtual`.
27) To avoid obscure bugs when overriding a member function from a base class, override all overloads of that member function as well. A `using` declaration can be used to bring all overloads from the base class to the derived class when you only need to override one.
28) There’s absolutely nothing wrong with overriding a private member function. In fact, *the template member function pattern* is a common pattern in C++ that is implemented by overriding private member functions.
29) When overriding a member function that has a default argument, you should provide a default argument as well, and it should probably be the same value. Default arguments are not “inherited” in C++.
30) If your derived class does not specify its own copy constructor or `operator=`, the base class functionality continues to work. However, if the derived class does provide its own copy constructor or `operator=`, it needs to explicitly call the base class versions.
31) *Virtual base classes* are a great way to avoid ambiguity in class hierarchies. With virtual classes, the most derived class itself is responsible for calling a constructor of the shared base class.
32) You can’t `static_cast()` pointers of one type to pointers of another unrelated type. You can’t directly `static_cast()` objects of one type to objects of another type if there is no converting constructor available. Basically, you can only do things that make sense according to the type rules of C++.
33) `std::bit_cast()` is defined in `<bit>`. It’s the only cast that’s part of the Standard Library. `bit_cast()` resembles `reinterpret_cast()`, but it creates a new object of a given target type and copies the bits from a source object to this new object. It effectively interprets the bits of the source object as if they are the bits of the target object. `bit_cast()` requires that the size of the source and target objects are the same and that both are trivially copyable.

## Chapter 11: Modules, Header Files, and Miscellaneous Topics

1) Module implementation files cannot export anything; only module interface files can.
2) Changing a function or
member function implementation that is inside a module interface file does not require a recompilation of users of that module.  Two exceptions are functions marked with the inline keyword, and template definitions. For both of these, the compiler needs to know their complete implementations at the time client code using them is compiled. Hence, any change to inline functions or template definitions can trigger recompilation of client code.
3) The C++ standard does not speak about submodules as such; however, it is allowed to use dots in a module’s name, and that makes it possible to structure your modules in any hierarchy you want.
4) For each import declaration naming a header file, the compiler creates a module with an exported interface similar to what the header file defines, i.e., it implicitly exports everything from the header file. This is called a *header unit*. As such, it can be used as a standardized way to support precompiled header files.
5) If you can import a header file it’s recommended to do so. Only `#include` a header file if it’s not importable, for instance, because the content of the header file is dependent on some preprocessor `#defines`. Place all `#include` directives in a module interface or module implementation file in the *global module fragment*.
6) If a translation unit needs a helper entity that is only required within that translation unit, wrap it in an anonymous namespace to give it internal linkage. Using the static keyword for this is discouraged.
7) Include guards and #pragma once directives prevent only *one definition rule* (ODR) violations when a header file is included multiple times within a single translation unit, not across multiple translation units.
8) Initialization order of nonlocal variables in different source files is undefined, which means that the order of destruction is also undefined.

## Chapter 12:  Writing Generic Code with Templates

1) Member function definitions of class templates need to be visible to any client code using the class template. This places some restrictions on where such member function definitions can be placed. Usually, they are simply put in the same file as the class template definition itself.
2) For non-virtual member functions, the compiler generates code only for those non-virtual member functions that are actually called. The danger exists that there are compilation errors in some class template member functions that go unnoticed with implicit instantiations. Unused member functions of class templates can contain syntax errors, as these will not be compiled. This makes it hard to test all code for syntax errors. You can force the compiler to generate code for all member functions, virtual and non-virtual, by using *explicit template instantiations*, like:
`template class Grid<string>;`
3) You can place the class template member function definitions in a separate module interface partition file. You then also need to put the class template definition in its own module interface partition. See page 463.
4) Virtual member functions and destructors cannot be member function templates.
5) You can also write your own *user-defined deduction guides* to help the compiler. These allow you to write rules for how the template type parameters have to be deduced. The following is an example demonstrating their use:
    ```c++
    template <typename T>
    class SpreadsheetCell
    {
    public:
        explicit SpreadsheetCell(T t) : m_content { move(t) } { }
        const T& getContent() const { return m_content; }
    private:
        T m_content;
    };

    SpreadsheetCell(const char*) - > SpreadsheetCell<std::string>;
    ```
    The general syntax is as follows. The explicit keyword is optional and behaves the same as 
    explicit for constructors. Such deduction guides are, more often than not, templates as well:
    ```c++
    template <...>
    explicit TemplateName(Parameters) - > DeducedTemplate<...>;
    ```
6) When writing a function that should work with different data types and for which the body of the function is the same for all data types, provide a function template. If the body of the function is different for every data type, provide function overloads.
7) When the overload resolution process of the compiler results in two possible candidates, one being a function template, the other being a non-template function, then the compiler always prefers to use the non-template function.
8) The compiler deduces the type of function template parameters based on the arguments passed to the function template. Template parameters that  cannot be deduced have to be specified explicitly.
9) Now that C++ supports `auto` return type deduction and `decltype(auto)`, it is recommended to use one of these mechanisms, instead of the alternative function syntax or `declval<>()`.
10) Every function parameter that is specified as `auto` becomes a template type parameter. Each parameter specified as `auto` becomes a *different* template type parameter. This is called the *abbreviated function template syntax*.
11) C++ supports variable templates. Example:
    ```c++
    template <typename T>
    constexpr T pi { T { 3.141592653589793238462643383279502884 } };
    float piFloat { pi<float> };
    ```
12) *Concepts* are named requirements used to constrain template arguments of class and function templates. These are written as predicates and evaluated at compile time to verify template arguments passed to a template. With concepts, the compiler can verify that provided template arguments satisfy certain constraints before it even starts instantiating a template.
13) Avoid concepts that validate only for syntactic aspects without any semantic meaning, such as a concept that just checks if a type supports `operator+`.
14) The syntax of a *concept definition* is as follows:
    ```c++
    template <parameter-list>
    concept concept-name = constraints-expression;
    ```
    The `constraints-expression` can be any constant 
    expression, that is, any expression that can be evaluated at compile time. The constraints expression must result in a Boolean value.

     A *concept expression* has the following syntax:
     ```c++
     concept-name<argument-list>
     ```
    Concept expressions evaluate to either true or false. If it evaluates to true, then it is said that the given template arguments *model the concept*.
15) Constant expressions that evaluate to a Boolean can directly be used as constraints for a concept definition:
    ```c++
    template <typename T>
    concept Big = sizeof(T) > 4;

    static_assert(Big<long double>);
    ```
16) A *requires expression* is a constant expression and has the following syntax:
    ```c++
    requires (parameter-list) { requirements; }
    ```
    where `(parameter-list)` is optional and is syntactically similar to the parameter list of functions except that default argument values are not allowed. The parameter list of a requires expression is used to introduce named variables that are local to the body of the requires expression. The body of a requires expression cannot have regular variable declarations. The `requirements` is a sequence of requirements. Each requirement must end with a semicolon. There are four types of requirements: simple, type, compound, and nested requirements.

    A **simple requirement** is an arbitrary expression statement, not starting with requires. Variable declarations, loops, conditional statements, and so on are not allowed. This expression statement is never evaluated; the compiler simply validates that it compiles. 
    
    A **type requirement** verifies that a certain type is valid. It starts with the keyword `typename` followed by the type to check. For example:
    ```c++
    template <typename T>
    concept C = requires { typename T::value_type; };
    ```
    verifies that `T` has a `value_type` member. A type requirement can also be used to verify that a certain template can be instantiated with a given type. Here’s an example:
    ```c++
    template <typename T>
    concept C = requires { typename SomeTemplate<T>; };
    ```

    A **compound requirement** can be used to verify that something does not throw any exceptions and/or to verify that a certain function returns a certain type. The syntax is as follows:
    ```c++
    { expression } noexcept -> type-constraint;
    ```
    Both the `noexcept` and `->type-constraint` are optional.
    There is no semicolon after expression inside the curly brackets, but there is a semicolon at the end of the compound requirement.
    The `type-constraint` can be any type constraint (NOT a type). A *type constraint* is simply the name of a concept with zero or more template type arguments. The type of the expression on the left of the arrow is automatically passed as the first template type argument to the type constraint.
    ```c++
    template <typename T>
    concept C = requires (const T x) {
        { x.size() } - > convertible_to<size_t>;
    };
    ```
    `std::convertible_to<From, To>` is a concept predefined by the Standard Library in `<concepts>` and has two template type parameters. The type of the expression on the left of the arrow is automatically passed as the first template type argument to the `convertible_to` type constraint. As such, you only need to specify the `To` template type argument, `size_t` in this case.

    A requires expression can have **nested requirements**. E.g.:
    ```c++
    template <typename T>
    concept C = requires (T t) {
        ++t; --t; t++; t--;
        requires sizeof(t) == 4;
    };
    ```
17) Existing concept expressions can be combined using conjunctions (`&&`) and disjunctions (`||`).
    ```c++
    template <typename T>
    concept IncrementableAndDecrementable = Incrementable<T> && Decrementable<T>;
    ```
18) Using type constraints with auto type deduction makes the code more self-documenting. It also results in better error messages if the constraints get violated at some point. E.g.: the following causes a compilation error stating that the constraints are not satisfied. The deduced type is `std::string`, due to the use of the standard literal s, and string does not model `Incrementable`:
`Incrementable auto value { "abc"s };`
19) There are several syntactically different ways to use type constraints with function templates. A first syntax is to use a requires clause as follows:
    ```c++
    template <typename T> requires constraints-expression
    void process(const T& t);
    ```
    The `constraints-expression` can be any constant expression, or a conjunction and disjunction of constant expressions, resulting in a Boolean type. For example, it can be a concept expression, a requires expression (yes there are two `requires`), any costant expression resulting in bool like ` (sizeof(T) == 4)`, a combination of conjunctions and disjunctions, a type trait. The requires clause can also be specified after the function header, called a *trailing requires clause*:
    ```c++
    template <typename T>
    void process(const T& t) requires Incrementable<T>;
    ```
    Another syntax is to use the familiar template<> syntax, but instead of using typename (or class), you use a type constraint. Here are two examples:
    ```c++
    template <convertible_to<bool> T>
    void process(const T& t);

    template <Incrementable T>
    void process(const T& t);
    ```
    The former is equivalent to:
    ```c++
    template <typename T> requires convertible_to<T, bool>
    void process(const T& t);
    ```
    Another more elegant syntax uses the abbreviated template syntax so it becomes:
    ```c++
    void process(const Incrementable auto& t);
    ```
    Mind that this is still a template, although no `template` keyword appears.
20) It’s possible to put additional constraints on specific member functions of a class template. For example, the `move()` member function of the `GameBoard` class template could be further constrained to require that type `T` is moveable. Such a requires clause also needs to be repeated on the member function definition.
21) When writing new class or function templates, try to put proper type constraints on all template type parameters. Unconstrained template type parameters should be a thing of the past.


## Chapter 13: Demystifying C++ I/O

1) A link can be established between any input and output streams to give them flush-on-access behavior. In other words, when data is requested from an input stream, its linked output stream is automatically flushed. This behavior is available to all streams but is particularly useful for file streams that may be dependent upon each other.
    ```c++
    ifstream inFile { "input.txt" }; // Note: input.txt must exist.
    ofstream outFile { "output.txt" };
    // Set up a link between inFile and outFile.
    inFile.tie(&outFile);
    // Output some text to outFile. Normally, this would
    // not flush because std::endl is not send.
    outFile << "Hello there!";
    // outFile has NOT been flushed.
    // Read some text from inFile. This will trigger flush() on outFile.
    string nextToken;
    inFile >> nextToken;
    // outFile HAS been flushed.
    ```
    You can also link an output stream to another output stream. Such a relationship means that every time you write to one file, the buffered data that has been sent to the other file is flushed. You can use this mechanism to keep two related files synchronized. One example of this stream linking is the link between `cout` and `cin`. There is also a link between `cerr` and `cout`, meaning that any output to `cerr` causes `cout` to flush.
2) You can use `getline()` to read the entire contents of a file by specifying \0 as the delimiter. This works only as long as the file doesn’t contain any \0 characters in its contents.

## Chapter 14: Handling Errors

1) Always document the possible exceptions a function can throw in its code documentation, because users of the function need to know which exceptions might get thrown so they can properly handle them.
2) Always throw objects as exceptions, and always catch exception objects as reference-to-const! This avoids object *slicing*.
3) C++17 has removed support for exception specifications, apart from `noexcept` and `throw()`. The latter was equivalent to `noexcept`. Since C++20, support for `throw()` has been removed as well.
4) If you are using custom exceptions, embed a stack trace in them for easier debugging of errors (easy since C++23).
5) Do not throw exceptions from constructors of global objects. Such exceptions cannot be caught because these objects are constructed before `main()` even starts executing.
6) If an exception leaves a constructor, the destructor for that object will never be called! C++ guarantees that it will run the destructor for all fully constructed “subobjects.” Therefore, any constructor that completes without an exception will cause the corresponding destructor to be run.
7) The following piece of pseudo-code shows the basic syntax for a *function-try-block* for a constructor:
    ```c++
    MyClass::MyClass()
    try
    : <ctor-­initializer>
    {
        /* ... constructor body ... */
    }
    catch (const exception& e)
    {
        /* ... */
    }
    ```
    The catch statements catch any exception thrown either directly or indirectly by the ctor-initializer or by the constructor body. The catch statements have to rethrow the current exception or throw a new exception. When a catch statement catches an exception in a function-try-block, all fully constructed base classes and members of the object are destroyed before execution of the catch statement starts. Inside catch statements you should not access data members that are objects because these are destroyed prior to executing the catch statements. The catch statements in a function-try-block for a constructor cannot use the `return` keyword.  function-try-blocks for constructors are useful only in a limited number of situations: to convert an exception thrown by the ctor-initializer to another exception, to log a message to a log file, to free raw resources that have been allocated in the ctor-initializer prior to the exception being thrown.
8) Function-try-blocks are not limited to constructors. They can be used with ordinary functions as well. However, for normal functions, there is no useful reason to use function-try-blocks because they can just as easily be converted to a simple try/catch block inside the function body. For normal functions, it's not required to rethrow the exception. It's also allowed to use the `return` keyword. In general, *avoid* using function-try-blocks!
9) You should not let any exceptions be thrown from destructors. Destructors are implicitly marked as `noexcept`.
10) **Strong exception guarantee**: If an exception is thrown, all involved objects are rolled back to the state they were in before the function was called. **Basic exception guarantee**: if an exception is thrown, all involved objects remain in a valid state, and no resources are leaked. However, the objects could be in another state than they were before the function was called. **Nothrow (or nofail) exception guarantee**: the function never throws any exceptions.

## Chapter 15: Overloading C++ Operators

1) You cannot add new operator symbols. You can only redefine the meanings of operators already in the language. There are a few operators that you cannot overload, such as `.`, `::` and others.
2) You can change the arity only for the function call, new, and delete operators, and, since C++23, also for the subscripting operator (array index), []. For all other operators, you cannot change the arity.
3) You cannot change the precedence nor the associativity of an operator. 
4) You cannot redefine operators for built-­in types.
5) The C++ language requires some operators to be member functions of a class because they don’t make sense outside of a class. For example, `operator=` is tied so closely to the class that it can’t exist anywhere else
6) When you write an overloaded operator as a global function, put it in the same namespace that contains the class for which the operator is written.
7) Starting with C++23, a function call operator can be marked as `static` if its implementation does not require access to `this`, or, in other words, does not need access to non-­static data members and non-­static member functions. This is similar to how subscripting operators, discussed earlier in this chapter, can be marked as `static`, and doing so allows the compiler to better optimize the code.
8) You can overload `operator new` and `operator delete` to control memory allocation and deallocation, but you cannot overload the new-­expression or the delete-­expression. Thus, you can customize the actual memory allocation and deallocation, but not the calls to the constructor and destructor.
9) You can replace the global operator new and operator delete routines if you want. These functions are called for every new-expression and delete-expression in the program, unless there are more specific routines in individual classes. A more useful technique is to overload operator new and operator delete for specific classes. These overloaded operators will be called only when you allocate and deallocate objects of that particular class. 
10) Overload all forms of operator new and operator delete, or explicitly delete overloads that you don’t want to get used, to prevent inconsistencies in the memory allocations.

## Chapter 16: Overview of the C++ Standard Library

1) The following comparison functions are available: `std::cmp_equal()`, `cmp_not_equal()`, `cmp_less()`, `cmp_less_equal()`, `cmp_greater()`, and `cmp_greater_equal()`, all defined in `<utility>`. These perform comparisons of two integers and are safe to use on mixed signed and unsigned comparisons.
2) `std::source_location`, defined in `<source_location>`, can be used to query information about your source code, such as filename, function name, line number, and column number, and replaces the old C-­style macros `__FILE__` and `__LINE__`. Example use cases are to provide source code information when logging messages or when throwing exceptions.
3) `forward_list`, defined in `<forward_list>`, is a singly linked list. It supports forward iteration only and requires a bit less memory than a `list`.
4) The elements of a `deque` are not stored contiguously in memory, and thus a deque might be slower than a `vector`.
5) A span, defined in `<span>`, represents a view on a contiguous sequence of data. It can be either a read-­only view or a view with read/write access to the underlying elements. A span allows you to write a single function that can work with data coming from, for example, vectors, arrays, C-­style arrays, and so on. C
6) The Standard Library provides three nonassociative container adapters: `queue`, `priority_queue`, and `stack`. These are higher level containers than for example `vector` or `deque`. You can model a queue or a stack using a deque as the underlying low level container.
7) C++23 introduces four flat associative container adapters: `flat_map`, `flat_set`, `flat_multimap`, `flat_multiset`. These are adapters on top of sequential containers and provide an associative container interface. The interface provided by these adapters is almost identical to their ordered counterpart associative container. The only difference is that the flat adapters don’t provide any of the node-­related member functions, as the flat adapters are not node-­based data structures.
8) You should use the container-­specific member function form of an algorithm, if provided, because it is generally more efficient or appropriate for the container at hand.
9) The Standard Library does not guarantee any thread safety for accessing containers simultaneously from multiple threads.
10) The Standard Library does not provide any generic tree or graph structures.

## Chapter 17: Understanding iterators and the Ranges Library

1) The standard defines six categories of iterators: input, output, forward, bidirectional, random access, contiguous. Every contiguous iterator is also random access, every random-­access iterator is also bidirectional, every bidirectional iterator is also forward, and every forward iterator is also input. Forward iterators are just input iterators which can be default constructed. Other types are almost self-explaining, see page 654-655.
2) Iterators that additionally satisfy the requirements for output iterators are called *mutable iterators*; otherwise, they are called *constant iterators*.
3) Every data structure of the Standard Library that supports iterators provides public type aliases for its iterator types, called `iterator` and `const_iterator`. Containers that allow you to iterate over their elements in reverse order also provide public type aliases called `reverse_iterator` and `const_reverse_iterator`.
4) A sequence specified by two iterators is called a **common range** to disambiguate it from ranges defined by the ranges library.
5) Some algorithm implementations need additional information about their iterators. For example, they might need to know the type of the elements referred to by the iterator to store temporary values, or perhaps they want to know whether the iterator is bidirectional or random access. C++ provides a class template called `iterator_traits`, defined in `<iterator>`, that allows you to retrieve this information. You instantiate the `iterator_traits` class template with the iterator type of interest.
    ```c++
    template <typename IteratorType>
    void iteratorTraitsTest(IteratorType it)
    {
        typename iterator_traits<IteratorType>::value_type temp;
        temp = *it;
        println("{}", temp);
    }

    vector v { 5 };
    iteratorTraitsTest(cbegin(v));
    ```
6) The Standard Library provides three insert iterator adapters that really insert elements into a container: `insert_iterator`, `back_insert_iterator`, and `front_insert_iterator`. They are all parametrized on a container type and take an actual container reference in their constructor. Suppose you define:
    ```c++
        template <input_iterator InputIter,
        output_iterator<iter_reference_t<InputIter>> OutputIter>
        void myCopy(InputIter begin, InputIter end, OutputIter target)
        {
            for (auto iter { begin }; iter != end; ++iter, ++target) { 
                *target = *iter; 
            }
        }
    ```
    Then you can do:
    ```c++
    vector vectorOne { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
    vector<int> vectorTwo;
    back_insert_iterator<vector<int>> inserter { vectorTwo };
    myCopy(cbegin(vectorOne), cend(vectorOne), inserter);
    println("{:n}", vectorTwo);
    ```
    You can also use the `std::back_inserter()` utility function to create a `back_insert_iterator`.
7) Another type of iterator adapter is the `std::reverse_iterator` class template that iterates through a bidirectional or random-­access iterator in a reverse direction.
8) Another type of iterator adapter is the `std::move_iterator`. The dereferencing operator of a `move_iterator` automatically converts the value to an rvalue reference, which means that the value can be moved to a new destination without the overhead of copying.
9) Ranges provided by the ranges library are an abstraction layer on top of iterators, eliminating mismatching iterator errors, and adding extra functionality such as allowing range adapters to lazily filter and transform underlying sequences of elements.
10) A range is a concept (see Chapter 12) defining the requirements for a type that allows iteration over its elements. Any data structure that supports `begin()` and `end()` is a valid range. For example, `std::array`, `vector`, `string_view`, `span`, fixed-­size C-­style arrays, and so on, are all valid ranges.
11) The ranges library, defined in `<ranges>`, consists of the following major components: **ranges**, **constrained algorithms**, **projection**, **views**, **factories**.
12) With constrained algortihms, the following become possibile:
    ```c++
    ranges::sort(data);
    ```
    instead of:
    ```c++
    vector data { 33, 11, 22 };
    sort(begin(data), end(data));
    ```
13) Most Standard Library algorithms, introduced in Chapter 16 and discussed in detail in Chapter 20, have constrained equivalents in the `std::ranges` namespace. It’s recommended to always use these constrained algorithms, if possible, as the compiler can provide better error messages when such algorithms are used with the wrong types, thanks to their type constraints.
14) A lot of the constrained algorithms have a projection parameter, a callback used to transform each element before it is handed over to the algorithm.
15) A *view* allows you to perform operations on an underlying range’s elements, such as filtering and transforming. Views can be chained/composed together to form a pipeline performing multiple operations on the elements of a range. Composing views is easy, you just combine different operations using the bitwise OR operator, `operator|`. For example, you can easily filter the elements of a range first and then transform the remaining elements. In contrast, if you want to do something similar, filtering followed by transforming, using the unconstrained algorithms, your code will be much less readable and possibly less performant, as you’ll have to create temporary containers to store intermediate results.
16) A view has the following important properties: **Lazily evaluated**: the operations of a view are applied only at the moment you iterate over the elements of the view and dereference such an iterator. **Nonowning**: a view doesn’t own any elements (with C++23 a view can own its elements, but only if it can guarantee that it's either non-copyable or copyable in constant time). The container is the owner of the data. **Nonmutating**: a view never modifies the data in the underlying range. 

    A view itself is also a range, but not every range is a view. A container is a range but not a view, as it owns its elements.
    Views can be created using range adapters. A range adapter accepts an underlying sequence of elements, and optionally some arguments, and creates a new view.
17) If a range is not read-­only, then you can modify the elements of that range through a view.
18) The ranges library provides the following range factories to construct views that produce elements lazily on demand:
    ```c++
    auto result { views::iota(10)
    | views::filter([](const auto& value) { return value % 2 == 0; })
    | views::transform([](const auto& value) { return value * 2.0; })
    | views::take(10) };
    printRange("Result: ", result);
    ```
    The output is `Result: 20, 24, 28, 32, 36, 40, 44, 48, 52, 56`.

    Another example is:
    ```c++
    printRange("Repeating view: ", views::repeat(42, 5));
    ```
    with output: `Repeating view: 42, 42, 42, 42, 42`.
19) Before C++23, it was not easy to convert a range into a container. C++23 introduces `std::ranges::to()` to make such conversions straightforward. This can also be used to convert the elements of a view into a container, as a view is a range. Even more, since a container is a range as well, you can use `ranges::to()` to convert one container into a different container, even with different element types.

    ```c++
    // Lazily generate the integers from 10 to 14, divide these by 2,
    // and store the result in a vector of doubles.
    auto vec2 { views::iota(10, 15)
    | views::transform([](const auto& v) { return v / 2.0; })
    | ranges::to<vector<double>>() };
    println("{:n}", vec2);
    ```

## Chapter 18: Standard Library containers

1) The difference in performance between `emplace_back()` and `push_back()` using move semantics depends on how your specific compiler implements these operations.
2) If you don’t control the reallocations explicitly, you should assume that all insertions cause a reallocation and thus invalidate all iterators.
3) Removing elements from a vector decreases the size of the vector, but never its capacity. How then can you reclaim its memory?
    ```c++
    vector<int> values;
    // Populate values ...
    vector<int>().swap(values);
    ```
4) Standard Library containers will only use move member functions if they guarantee not to throw any exceptions. Thus the move constructor and move assignment operator of the type stored in the container must be marked as `noexcept`!
5) You must specify `typename` explicitly whenever you access a type based on one or more template parameters.
6) A deque never moves its elements to a bigger array (as vector does) when inserting elements at the front or at the back. This also means that a deque does not invalidate any iterators in such cases.
7) *Splicing* is destructive to the list passed as an argument, i.e., it removes the spliced elements from one list to insert them into the other.
8) An array can also not be moved in constant time, while a vector can.
9) `std::to_array()`, defined in `<array>`, converts a given C-style array to an `std::array`, using copy-initialization of the elements.
10) A `span` provides a one-dimensional, non-owning view over a contiguous sequence of data. An `mdspan` generalizes this concept and allows the creation of multidimensional, non-owning views over a contiguous sequence of data. Unlike `string_view` that provides a read-only view of a string, a span can provide read/write access to the underlying elements. Remember that a span just contains a pointer to the first element in a sequence and the number of elements; that is, a span never copies data! As such, modifying an element in a span actually modifies the element in the underlying sequence. If this is not desired, a span of const elements can be created.
11)  When writing a function accepting a `const vector<T>&`, consider accepting a `span<const T>` instead, so the function can work with views and subviews of data sequences coming from vectors, arrays, C-style arrays and more. If the function accepts a `vector<T>&`, consider accepting a `span<T>`, unless the function needs to add or remove elements from the vector.
12) This `mdspan` definition specifies the extent of all dimensions as compile-time constants, i.e., static extents. It is also possible to combine static and dynamic extents.
13) The adapters don’t provide the capability to erase multiple elements simultaneously, nor do they provide iterators. The latter means you cannot use them with range-based for loops or with any of the standard iterator-based algorithms.
14) As discussed earlier with multimaps, looking up elements in unordered_multimaps cannot be done using `operator[]` because it is not provided. You can use `find()`, but it returns an iterator referring to any one of the elements with a given key (not necessarily the first element with that key). Instead, it’s best to use the `equal_range()` member function, which returns a pair of iterators: one referring to the first element matching a given key, and one referring to one-past-the-last element matching that key. 
15) A `flat_map` and a `flat_multimap` require two underlying containers, one to store the keys and another one to store the values. The underlying container must support random-access iterators, such as vector and deque. By default, vector is used.
16) Because the flat associative container adapters store their data sorted in sequential containers, they all have linear time complexity for adding and removing elements, which can potentially be slower than adding and removing elements from ordered and unordered containers. Lookups have logarithmic complexity, just as the ordered associative containers. However, for the flat variants, lookups and especially iteration over the elements are more efficient than for the ordered ones because the former store their data in sequential containers and thus have a much more efficient and cache-friendly memory layout. 

## Chapter 19: Function Pointers, Function Objects, and Lambda Expressions

1) Starting with C++23, an overloaded `operator()` can be marked as `static` if it doesn’t require access to any non-static data members and member functions of the functor. Doing so allows the compiler to better optimize the code.
2) The arithmetic function objects are just wrappers around the arithmetic operators. To use them on objects of a certain type, you have to make sure that those types implement the appropriate operations, such as `operator*` or `operator+`.
3) C++ supports transparent operator functors, which allow you to omit the template type argument. An important feature of these transparent operators is that they are heterogeneous. That is, they are not only more concise than the non-transparent functors, but they also have real functional advantages. See page 770.
4) Use he `auto` keyword as the return type of `bind()`, since it is unspecified by the C++ standard and thus is implementation specific.
5) If you want to use `bind()` with overloaded functions, you need to explicitly specify which of the overloads you want to bind.
    ```c++
    void overloaded(int num) {}
    void overloaded(float f) {}
    auto f4 { bind((void(*)(float))overloaded, placeholders::_1) }; // OK
    ```
6) A lambda closure has an overloaded function call operator that is marked as `const` by default. That means that even if you capture a non-const variable by value in a lambda expression, the lambda expression is not able to modify this copy. You can mark the function call operator as non-const by specifying the lambda expression as `mutable`.
7) When using a capture default, only those variables that are really used in the body of the lambda expression are captured, either by value (=) or by reference (&). Unused variables are not captured.  By using a = capture default, you might accidentally cause an expensive copy. By using an & capture default, you might accidentally modify a variable in the enclosing scope. I recommend you explicitly specify which variables you want to capture and how.
8) **Global variables are always captured by reference**, even if asked to capture by value!
9) For lambda expressions that do not capture anything, the compiler automatically provides a conversion operator that converts the lambda expression to a function pointer. Such lambda expressions can then, for example, be used to pass to functions that accept a function pointer as one of their arguments.
10) Lambda capture expressions allow you to initialize capture variables with any expression. It can be used to introduce variables in the lambda expression that are not captured from the enclosing scope.
    ```c++
    double pi { 3.1415 };
    auto myLambda { [myCapture = "Pi: ", pi]{ println("{}{}", myCapture, pi); } };
    ```
11) *Templated lambda expressions* allow you to get easier access to type information of parameters of
generic lambda expressions. For example, suppose you have a lambda expression that requires a
vector to be passed as an argument.
You can write:
    ```c++
    auto lambda { [] <typename T> (const vector<T> & values) {
    T someValue { };
    } };
    ```
    instead of:
    ```c++
    auto lambda { [](const auto& values) {
    using V = decay_t<decltype(values)>; // The real type of the vector.
    using T = typename V::value_type; // The type of the elements of the vector.
    T someValue { };
    } };
    ```
12) Stateless lambda expressions can be default constructed, copied, and assigned to.
13) With C++23’s explicit object parameters feature it's possible to write recursive lambdas as follows:
    ```c++
    auto fibonacci = [](this auto& self, int n) {
    if (n < 2) { return n; }
    return self(n -­1) + self(n -­2);
    };

    println("First 20 Fibonacci numbers:");
    for (int i { 0 }; i < 20; ++i) { print("{} ", fibonacci(i)); }
    ```
14) By themselves, `invoke()` and `invoke_r()` are not that useful because you might as well just call the function or the lambda expression directly. However, they can be very useful when writing generic templated code where you need to invoke some arbitrary callable.

## Chapter 20: Mastering Standard Library Algorithms

1) The Standard Library has a large collection of constrained algorithms, sometimes called range-­based algorithms. These are able to work with iterators and ranges and are properly constrained, so the compiler can produce more readable error messages when an algorithm is used wrongly.
2) To prevent callbacks from getting copied by algorithms, you can use the `std::ref()` helper function to pass a callback reference to the algorithm instead.
3) `std::copy` and `std::move` do not create or extend containers; they merely replaces existing elements, so the destination should be big enough to hold all elements to be copied or moved.
4) The `remove` algorithms have access only to the iterator abstraction, not to the container. Thus, they cannot really remove elements from the underlying container, contrary to `erase`. Instead, the algorithms work by replacing the elements that match a given value or predicate with the next element that does not match the given value or predicate. It does so using move assignments .So you first use the `remove()` or `remove_if()` algorithm, and then you must call `erase()` on the container to erase all the elements from the returned iterator up to the end of the range. This process is called the *remove-­erase-­idiom*. This is used before C++20, since the `erase()` and `erase_if()` algorithms have now been added.
5) C++ supports executing more than 60 Standard Library iterator-­based algorithms in parallel to improve their performance. Examples include `std::for_each()`, `all_of()`, `copy()`, `count_if()`, `find()`, `replace()`, `search()`, `sort()`, `transform()`, and many more. Algorithms that support parallel execution have an optional execution policy as their first parameter. The execution policy allows you to specify whether an algorithm is allowed to be vectorized and/or executed in parallel. When a compiler vectorizes code, it replaces several CPU instructions with a single vector CPU instruction. Callbacks passed to parallel algorithms are not allowed to throw any uncaught exceptions. Doing so will trigger a call to `std::terminate()` which terminates the application.
6) `std::accumulate()` is one of the few algorithms that does not support parallel execution. Instead, you need to use `std::reduce()` to calculate a generalized sum with the option to execute it in parallel.

## Chapter 21: String Localization and Regular Expressions

1) The C++ standard does not define the size for `wchar_t`.
2) Multibyte strings are strings with characters composed of one or more bytes using a locale-dependent encoding. A multibyte string could use Unicode encoding, or any other kind of encoding such as Shift-JIS.
3) Unfortunately, the support for `char8_t`, `char16_t`, and `char32_t` doesn’t go much further. There are some conversion classes available (see later in this chapter), but, for example, there is nothing like a version of `cout`, `cin`, `println()`, `format()`, and so on, that supports these character types; this makes it difficult to print such strings to a console or to read them from user input. If you want to do more with such strings, you need to resort to third-party libraries.
4) C++23 improves things slightly. It allows a u8 UTF-8 string literal to initialize an array of type `const char` or `const unsigned char`, and functions like `std::format()` and `print()` do support `const char[]`. For example, the following initializes a `const char[]` array with a UTF-8 string literal and then prints it using `println()`. If your environment is set up to handle Japanese characters, then the output is “Hello world” in Japanese.
    ```c++
    const char hello[] { u8"こんにちは世界" };
    println("{}", hello);
    ```
    But `println()` doesn’t understand the `char8_t` type.
5) The standard C++ mechanism that groups specific data about a particular set of cultural parameters is called a *locale*. An individual component of a locale, such as date format, time format, number format, and so on, is called a *facet*. An example of a locale is US English. An example of a facet is the format used to display a date.
6) There are third-party libraries available that make it easier to work with locales. One example is `boost.locale`.
7) The default locale is the classic/neutral locale, and not the user’s locale. The classic locale uses ANSI C conventions and has the name *C*. By default, `std::print()` and `println()` use the C locale.
8) `std::format()` also supports locales by using the L format specifier and optionally accepts a locale as first argument:
    ```c++
    cout << format(locale { "en-US" }, "format() with en-­US locale: {:L}", 32767);
    ```
9) `<locale>` also defines two character conversion functions: `std::toupper()` and `tolower()`. They accept two parameters: the character to convert and the locale to use for the conversion.

## Chapter 22: Date and Time Utilities

1) The Ratio library allows you to exactly represent any finite rational number that you can use at compile time. Because of the compile-time nature of these rational numbers, using them might look a bit different than what you are used to. You cannot define a ratio object the same way as you define normal objects, and you cannot call member functions on it. Instead, ratio is a class template, and a specific instantiation of the ratio class template represents one specific rational number. To name such specific instantiations, you can use type aliases.
    ```c++
    // Define a compile-time rational number.
    using r1 = ratio<1, 60>;
    // Get numerator and denominator.
    intmax_t num { r1::num };
    intmax_t den { r1::den };
    println("1) r1 = {}/{}", num, den);
    // Add two rational numbers.
    using r2 = ratio<1, 30>;
    println("2) r2 = {}/{}", r2::num, r2::den);
    using result = ratio_add<r1, r2>::type;
    println("3) sum = {}/{}", result::num, result::den);
    // Compare two rational numbers.
    using res = ratio_less<r2, r1>;
    println("4) r2 < r1: {}", res::value);
    ```
2) A *duration* is an interval between two points in time. It is represented by the duration class template, which stores a number of ticks and a tick period. The tick period is the time in seconds between two ticks and is represented as a compile-time ratio constant, which means it could be a fraction of a second.
    ```c++
    template <class Rep, class Period = ratio<1>> class duration {...}
    ```
    Durations support arithmetic operations such as +, -, *, /, %, ++, --, +=, -=, *=, /=, and %=, and they support the comparison operators == and <=>.
    Here is a duration where each tick period is a sixtieth of a second:
    ```c++
    duration<double, ratio<1, 60>> d3;
    ```
3) Because the standard requires that the predefined durations use integer types, there can be compilation errors if a conversion could end up with a non-integral value. While integer division normally truncates, in the case of durations, which are implemented with ratio types, the compiler declares any computation that could result in a non-zero remainder as a compile-time error. For example, the following code does not compile because converting 90 seconds to minutes results in 1.5 minutes:
    ```c++
    seconds s { 90 };
    minutes m { s };
    ```
    Converting in the other direction works perfectly fine because the minutes duration uses an integral type, and converting it to seconds always results in an integral value.

4) The chrono library provides the `hh_mm_ss` class template, which accepts a `Duration` and splits the given duration into hours, minutes, seconds, and subseconds.

5) A `clock` is a class consisting of a `time_point` and a `duration`. The *epoch* of a clock
is the time at which the clock starts counting. For example, `steady_clock` guarantees its time point never decreases. The `utc_clock` is the only clock that tracks leap seconds. It’s recommended to use `system_clock` to work with wall clock time and `steady_clock` to measure durations. Every clock has a static `now()` member function to get the current time as a `time_point`.

6) To time how long it takes for a piece of code to execute, you want to use a clock that is guaranteed not to go backwards. Hence, you should use a `steady_clock`.

7) A point in time is represented by the `time_point` class and stored as a duration relative to an epoch, representing the beginning of time. A `time_point` is always associated with a certain `clock`, and the epoch is the origin of this associated clock.

    ```c++
    // Create a time_point representing the epoch of the associated steady clock.
    time_point<steady_clock> tp1;
    // Add 10 minutes to the time_point.
    tp1 += minutes { 10 };
    // Store the duration between epoch and time_point.
    auto d1 { tp1.time_since_epoch() };
    // Convert the duration to seconds and output to the console.
    duration<double> d2 { d1 };
    println("{}", d2);
    ```
8) The Standard Library supports working with calendar dates. At this moment, only the Gregorian calendar is supported.

## Chapter 23: Random Number Facilities

1) The quality of a random number generator is measured by its entropy. The `entropy()` member function of the `random_device` engine returns 0.0 if it is using a software-based pseudorandom number generator and returns a non-zero value if it is using a hardware device. The non-zero value is an estimate of the entropy of the hardware device.
    ```c++
    random_device rnd;
    println("Entropy: {}", rnd.entropy());
    println("Min value: {}, Max value: {}", rnd.min(), rnd.max());
    println("Random number: {}", rnd());
    ```
    A possible output of this program could be as follows:
    ```
    Entropy: 32
    Min value: 0, Max value: 4294967295
    Random number: 3590924439
    ```
2) A `random_device` is much slower than a pseudorandom number engine. Therefore, if you need to generate a lot of random numbers, use a pseudorandom number engine and generate a seed for it with a `random_device`.

3) Next to the `random_device` engine, there are three pseudorandom number engines:
    - Linear congruential engine
    - Mersenne twister
    - Subtract with carry engine

    However, creating an instance of one of the three pseudorandom number generators requires you to specify a number of mathematical parameters, which can be daunting.
    For this reason, the standard defines a couple of predefined engines. One example is the mt19937 Mersenne twister.

4) A random number engine *adapter* modifies the result of a random number engine you associate it with, which is called the base engine. This is an example of the adapter pattern.
5) Once the engine and distribution are defined, random numbers can be generated by calling the function call operator of the distribution and passing the engine as an argument:
    ```c++
    auto generator { bind(distribution, engine) };
    vector<int> values(10);
    ranges::generate(values,println("{:n}", values);
    ```
6) The random number generators are not thread safe.

## Chapter 24: Additional Vocabulary Types

1) Starting with C++23, variant is a `constexpr` class, so it can be used at compile time.
2) Instances of any can be stored in Standard Library containers. This allows you to have heterogeneous data in a single container. The only downside is that you have to perform explicit `any_cast`s to retrieve specific values.
3) Iterating over the values of a tuple is unfortunately not straightforward. You cannot write a simple loop and call something like `get<i>(mytuple)` because the value of `i` must be known at compile time.
4) With CTAD, you do not explicitly specify the types stored in a tuple and so you cannot use & to specify references. If you want to use CTAD to generate a tuple containing a reference-to-non-const or a reference-to-const, then you need to use `ref()` or `cref()`, respectively, both defined in `<functional>`. These create instances of `reference_wrapper<T>` or `reference_wrapper<const T>`. For example, the following statements result in a tuple of type `tuple<int, double&, const double&, string&>`:
    ```c++
    double d { 3.14 };
    string str1 { "Test" };
    tuple t2 { 16, ref(d), cref(d), ref(str1) };
    ```
5) Without class template argument deduction, you can use the `std::make_tuple()` function template to create a tuple. Since it is a function template, it supports function template argument deduction and hence also allows you to create a tuple by only specifying the actual values.
6) Structured bindings, available since C++17, make it easy to decompose a tuple into separate variables. With structured bindings, you cannot ignore specific elements while decomposing a tuple.
7) If you want to decompose a tuple without structured bindings, you can use the `std::tie()` utility function, which generates a tuple of references.
    ```c++
    tuple t1 { 16, "Test"s, true };
    int i { 0 };
    string str;
    bool b { false };
    println("Before: i = {}, str = \"{}\", b = {}", i, str, b);
    tie(i, str, b) = t1;
    println("After: i = {}, str = \"{}\", b = {}", i, str, b);
    ```
    The result is:
    ```
    Before: i = 0, str = "", b = false
    After: i = 16, str = "Test", b = true
    ```
    With `tie()` you can ignore certain elements that you do not want to be decomposed. Instead of a variable name for the decomposed element, you use the special `std::ignore` value.
8) You can use `std::tuple_cat()` to concatenate two tuples into one.
9) Tuples support all comparison operators. For the comparison operators to work, the element types stored in the tuple should support them as well. This is handy if you need to compare types based on a subset of their fields:
    ```c++
    class Foo
    {
        public:
        explicit Foo(int i, string s, bool b)
        : m_int { i }, m_str { move(s) }, m_bool { b } { }
        private:
        int m_int;
        string m_str;
        bool m_bool;
    };
    ```
    If you use `auto operator<=>(const Foo& rhs) const = default;` then all the fields are compared. If you want to exclude the boolean, you use:
    ```c++ 
    auto operator<=>(const Foo& rhs) const
    {
        return tie(m_int, m_str) <=> tie(rhs.m_int, rhs.m_str);
    }
    ```

10) `std::make_from_tuple<T>()` constructs an object of a given type `T`, passing the elements of a given tuple as arguments to the constructor of `T`. This function comes in handy when writing generic code using templates and template metaprogramming.

11) `std::apply()` calls a given callable, passing the elements of a given tuple as arguments. This function is also more useful when writing generic code using templates and template metaprogramming than for everyday use.

12) C++23 adds three new member functions to `optional`, collectively called monadic operations. The following monadic operations are available:
    - `transform(F)`: Returns an optional containing the result of invoking `F` with the value of `*this` as argument if `*this` has a value; otherwise, returns an empty `optional`
    - `and_then(F)`: Returns the result (which must be an `optional`) of invoking `F` with the value of `*this` as argument if `*this` has a value; otherwise, returns an empty `optional`
    - `or_else(F)`: Returns `*this` if `*this` has a value; otherwise, returns the result (which must be an `optional`) of invoking `F`
13) Starting with C++23, `optional` is a `constexpr` class.
14) A default constructed `optional` is empty! In other words, a default constructed `expected` represents success, while a default constructed `optional` represents an error.
15) `expected` supports monadic operations: `and_then()`, `transform()`, `or_else()`, and `transform_error()`. The first three are analogous to the monadic operations supported by `optional`. `transform_error(F)` returns `*this` if `*this` has an expected value; otherwise, returns an expected containing the unexpected value transformed by invoking `F` with the unexpected value as argument.
16) The error type of expected can be any type you want. Returning multiple error types is also possible by using the `variant` vocabulary type.

## Chapter 25: Customizing and Extending the Standard Library

1) the Standard Library has the concept of polymorphic memory allocators. Basically, the problem with the allocator for a container being specified as a template type parameter is that two containers that are similar but have different allocator types are completely different types. For example, `vector<int, A1>` and `vector<int, A2>` are different and so cannot, e.g., be assigned to one another. The polymorphic memory allocators, defined in `<memory_resource>` in the `std::pmr namespace`, help to solve this problem. The class `std::pmr::polymorphic_allocator` is a proper Allocator class because it satisfies all the allocator requirements.
2) You can write your own containers that can work with the Standard Library algorithms by providing iterators that conform to the Standard Library guidelines. Keep in mind, though, that you are not allowed to put your own containers and algorithms in the `std` namespace.
3) The C++ standard requires that every Standard Library container provide the following public **type aliases**:
    - `value_type`: The element type stored in the container
    - `reference`: A reference to the element type stored in the container
    - `const_reference`: A reference-­to-­const to the element type stored in the container
    - `iterator`: The type for iterating over elements of the container
    - `const_iterator`: A version of `iterator` for iterating over `const` elements of the container
    - `size_type`: A type that can represent the number of elements in the container; this is usually just `size_t`
    - `difference_type`: A type that can represent the difference of two iterators for the container; this is usually just `ptrdiff_t`

4) In addition to the obligatory type aliases, every container must provide the following member functions: Default constructor, Copy constructor, Move constructor, Copy assignment operator, Move assignment operator, Destructor. And:
    - `iterator begin(); const_iterator begin() const;`
    - `iterator end(); const_iterator end() const;`: these refer to one-past-the-last element in the container
    - `const_iterator cbegin() const; const_iterator cend() const;`
    - `operator==`
    - `void swap(Container&) noexcept;`
    - `size_type size() const;`
    - `size_type max_size() const;`: maximum number of elements the container can hold
    - `bool empty() const;`

    It's okay to use the rule of zero.
5) Your iterator should generally provide overloaded `operator++`, `*`, `-­>`, and `==`, plus some other operations depending on its specific behavior. If you want your iterator to be usable by the generic algorithms, you must specify its traits. You need to provide aliases for `value_type`, `difference_type`, `iterator_category`, `pointer`, and `reference`.

## Chapter 26: Advanced Templates

1) The syntax rule for a template template parameter, more generically, is this:
    ```c++
    template <..., template <TemplateTypeParams> class ParameterName, ...>
    ```
    I recommend avoiding template template parameters. In fact, the Standard Library itself never uses template template parameters.
2) Up until C++20, non-­type template parameters cannot be objects, or even `double`s or `float`s. They are restricted to integral types, `enum`s, pointers, and references. Since C++20, these restrictions are relaxed a bit and it is now allowed to have non-­type template parameters of floating-­point types, and even certain class types.

3) The C++ standard does not permit partial template specialization of function templates. Instead, you can overload the function template with another function template.

4) Variadic templates can take a variable number of template parameters. For example, the following code defines a template that can accept any number of template parameters, using a parameter pack.
5) Inside the body of a function using a parameter pack, you can retrieve the number of arguments in the pack using `sizeof...(pack)`. Notice that this is not doing a pack expansion with `...`, but is using the special keyword-­like syntax `sizeof...`.
6) C++ supports fold expressions. This makes working with parameter packs in variadic templates much easier: let `Ѳ` be an operator, then we have the following type of fold expressions:
    - Unary right fold: `(pack Ѳ...)` -> `pack_{0} Ѳ (...Ѳ (pack_{n-1} Ѳ pack_{n}))`
    - Unary left fold: `(...Ѳ pack)`
    - Binary right fold: `(pack Ѳ...Ѳ Init)` -> `pack_{0} Ѳ (...Ѳ (pack_{n-1} Ѳ (pack_{n} Ѳ Init)))`
    - Binary left fold: `(Init Ѳ...Ѳ pack)`

    ```c++
    template <typename... Values>
    void printValues(const Values&... values) { (println("{}", values) , ...); }
    ```
    Here the unfolding is done with the comma operator.

7) Concepts, discussed in Chapter 12, can also be variadic.
    ```c++
    template <typename T, typename... Us>
    concept SameTypes = (std::same_as<T, Us> && ...);

    template <typename T, typename... Values>
    requires SameTypes<T, Values...>
    auto sumValues(const T& init, const Values&... values)
    { return (init + ... + values); }
    ```

8) Parameter packs with zero length are allowed for unary folds, but only in combination with the logical AND (`&&`), logical OR (`||`), and comma (`,`) operators. For an empty parameter pack, applying `&&` to it results in true, applying `||` results in false, and applying , results in `void()`, i.e., a no-­op.
    ```c++
    template <typename... Values>
    bool allEven(const Values&... values) { return ((values % 2 == 0) && ...); }
    template <typename... Values>
    bool anyEven(const Values&... values) { return ((values % 2 == 0) || ...); }

    int main()
    {
        println("{} {} {}", allEven(2,4,6), allEven(2,3), allEven());//true false true
        println("{} {} {}", anyEven(1,2,3), anyEven(1,3), anyEven());//true false false
    }
    ```
9) An example of template metaprogramming is to unroll loops at compile time instead of executing the loop at run time:
    ```c++
    template <int i>
    class Loop
    {
    public:
        template <typename FuncType>
        static void run(FuncType func) {
            Loop<i -­ 1>::run(func);
            func(i);
        }
    };

    template <>
    class Loop<0>
    {
    public:
        template <typename FuncType>
        static void run(FuncType /* func */) { }
    };
    ```

10) The `enable_if` trait is often used on the return types of your set of overloads.
    ```c++
    template <typename T1, typename T2>
    enable_if_t<is_same_v<T1, T2>, bool>
    checkType(const T1& t1, const T2& t2)
    {
        println("'{}' and '{}' are the same types.", t1, t2);
        return true;
    }
    template <typename T1, typename T2>
    enable_if_t<!is_same_v<T1, T2>, bool>
    checkType(const T1& t1, const T2& t2)
    {
        println("'{}' and '{}' are different types.", t1, t2);
        return false;
    }

    int main()
    {
        checkType(1, 32);
        checkType(1, 3.01);
        checkType(3.01, "Test"s);
    }
    ```

    If you don’t want to clutter your return types with `enable_if`, then another option is to use
    `enable_if` with extra non-­type template parameters.

    ```c++
    template <typename T1, typename T2, enable_if_t<is_same_v<T1, T2>>* = nullptr>
    bool checkType(const T1& t1, const T2& t2)
    {
        println("'{}' and '{}' are the same types.", t1, t2);
        return true;
    }
    ```

## Chapter 27: Multithreaded Programming with C++

1) You can optimize your data structures by using explicit memory alignments to make sure data that is worked by multiple threads does not share any cache lines. To do this in a portable manner, a constant called `hardware_destructive_interference_size`, defined in `<new>`, can be used, which returns you the minimum recommended offset between two concurrently accessed objects to avoid cache line sharing. You can use that value in combination with the `alignas` keyword to properly align your data.
2) If a thread object that is still joinable is destroyed, the destructor calls `std::terminate()`, which abruptly terminates all threads and the application itself.
3) Thread function arguments are always copied into some internal storage for the thread. Use std::ref() or std::cref() from `<functional>` to pass them by reference.
4) Instead of using function pointers, you can also use a function object to execute in a thread. Function objects are always copied into some internal storage for the thread. If you want to execute `operator()` on a specific instance of your function object instead of on a copy, you should use one of the `std::ref()` or `std::cref()` helper functions from `<functional>` to pass your instance by reference, for example: `Counter c { 2, 12 };` `thread t2 { ref(c) };` However, since `t2` now has a reference to `c`, it will be a data race (and thus undefined behavior) to read or write the contents of `c` from the main thread before `t2`
has finished and been joined.
5) You can specify a member function of a class to be executed in a thread:
    ```c++
    class Request
    {
    public:
        explicit Request(int id) : m_id { id } { }
        void process() { println("Processing request {}", m_id); }
    private:
        int m_id { 0 };
    };
    int main()
    {
        Request req { 100 };
        thread t { &Request::process, &req };
        t.join();
    }
    ```
    With this technique, you are executing a member function on a specific object in a separate thread. If
    other threads are accessing the same object, you need to make sure this happens in a thread-­safe way
    to avoid data races

6) The C++ standard supports thread-­local storage. With a keyword called `thread_local`, you can mark any variable as thread-­local, which means that each thread will have its own unique copy of the variable, and it will last for the entire duration of the thread. `thread_local` works for globals, `static` data members of classes and `static` local variables of functions. Inside a function, and only inside a function, declaring a variable as `thread_local` implies `static`, but it’s recommended to be explicit about this.
7) The C++ standard does not include any mechanism for canceling a running thread from another thread.
8) `<thread>` also defines `std::jthread`, which is virtually identical to thread, except: 1)  It automatically joins in its destructor 2) It supports *cooperative cancellation*.
9) If you have a `jthread` instance, you can get access to its `stop_token` and `stop_source` by using the `get_stop_token()` and `get_stop_source()` member functions. The callable passed to a constructor of `jthread` can have a `stop_token` as its first parameter so it can check whether it needs to stop.
10) In most cases you are probably interested in results produced by the thread. For example, if your thread performs some mathematical calculations, you really would like to get the results out of the thread once the thread is finished. One way is to pass a pointer or reference to a result variable to the thread in which the thread stores the results. Another technique is to store the results inside class data members of a function object, which you can retrieve later once the thread has finished executing. This works only if you use `std::ref()` to pass your function object by reference to the `jthread` constructor. However, there is another easier mechanism to obtain a result from threads: *futures*.
11) If a thread throws an exception and it is not caught inside the thread, the C++ runtime calls `std::terminate()`.
12) `atomic_flag` is an atomic Boolean, always lock-­free, guaranteed by the C++ standard. It differs from `atomic<bool>` in that it does not provide an assignment operator; instead, it provides named member functions `clear()`, `test()`, and `test_and_set()`.
13) `std::atomic_ref` is basically the same as `std::atomic`, even with the same interface, but it works with references, while atomic always makes a copy of the value it is provided with. The `atomic_ref<T>` class template can be used with any trivially copyable type `T`, just as `std::atomic` can.
14) It's possible to wait efficiently on an atomic, using `wait(oldValue)`. It blocks the thread until another thread calls `notify_one()` or `notify_all()`, and the value of the atomic variable has changed, that is, is not equal to `oldValue` anymore. If the current value is already unequal to `oldValue`, then the function doesn’t block at all.
15) A spinlock can be implemented using a single atomic type: `atomic_flag`.
16) The `shared_mutex` class supports the concept of shared lock ownership, also known as readers-­writer lock.
17) `std::unique_lock`, defined in `<mutex>`, is a more sophisticated lock that allows you to defer lock acquisition until later in the execution, long after the declaration.
18) C++ has two generic lock functions that you can use to obtain locks on multiple mutex objects at once without the risk of creating deadlocks. The first function, `lock()`, locks all the given mutex objects in an unspecified order without the risk of deadlocks. `try_lock()` has a similar prototype, but it tries to obtain a lock on all the given mutex objects by calling `try_lock()` on each of them in sequence. It returns `-­1` if all calls to `try_lock()` succeed. If any `try_lock()` fails, `unlock()` is called on all locks that have already been obtained, and the return value is the zero-­based index of the mutex argument on which `try_lock()` failed. `std::scoped_lock`, defined in `<mutex>`, is similar to `lock_guard`, except that it accepts a variable number of mutexes.
19) You can use `std::call_once()` in combination with `std::once_flag` to make sure a certain function or member function is called exactly one time, no matter how many threads try to call `call_once()` with the same `once_flag`.
20) `<syncstream>` defines `std::basic_osyncstream` with predefined type aliases `osyncstream` and `wosyncstream` for `char` and `wchar_t` streams, respectively. These classes guarantee that all output done through them will appear in the final output stream the moment the synchronized stream is destroyed.
21) Function-­local static variables are called magic statics or thread-­safe statics. C++ guarantees that such local static variables are initialized in a thread-­safe fashion.
22) Avoid the double-­checked locking pattern! Instead, use other mechanisms such as simple locks, atomic variables, `call_once()` (c++11), magic statics, and so on.
23) Threads waiting on a condition variable can wake up when another thread calls `notify_one()` or `notify_all()`, or after a relative timeout, or when the system time reaches a certain time. However, they can also wake up spuriously. This means that a thread can wake up even if no other thread has called any notify member function and no timeouts have been reached yet. Thus, when a thread waits on a condition variable and wakes up, it needs to check why it woke up. One way to check for this is by using one of the versions of `wait()` accepting a predicate.
24) A latch is a single-­use thread coordination point. A number of threads block at a latch point. Once a given number of threads reach the latch point, all threads are unblocked and allowed to continue execution. A latch is implemented by `std::latch`, defined in `<latch>`. The constructor accepts the required number of threads that need to reach the latch point. An example demonstrates a use case for a latch point in which some data needs to be loaded into memory (I/O bound) that is subsequently processed in parallel in multiple threads:
    ```c++
    latch startLatch { 1 };
    vector<jthread> threads;
    for (int i { 0 }; i < 10; ++i) {
        threads.emplace_back([&startLatch] {
            // Do some initialization... (CPU bound)
            // Wait until the latch counter reaches zero.
            startLatch.wait();
            // Process data...
            });
    }
    // Load data... (I/O bound)
    // Once all data has been loaded, decrement the latch counter
    // which then reaches zero and unblocks all waiting threads.
    startLatch.count_down();
    ```
25) A *barrier* is a reusable thread coordination mechanism consisting of a sequence of phases. A number of threads block at the barrier point. Each time a given number of threads reach the barrier, a phase completion callback is executed, all blocking threads are unblocked, the thread counter is reset, and the next phase starts. A barrier is implemented by the class template `std::barrier`, defined in `<barrier>`.

26) A *future* can be used to more easily get the result out of a thread and to transport exceptions from one thread to another thread, which can then handle the exception however it wants. A *promise* is something in which a thread stores its result. A future is used to get access to the result stored in a promise. A promise/future pair is an inter-­thread one-­shot communication channel for a result.

27) The call to `get()` retrieves the result and stores it in the variable result. If calculating the result is not finished yet, the call to `get()` blocks until the value becomes available. You can call `get()` only once on a future. The behavior of calling it a second time is undefined by the standard. If you want to avoid blocking, you can first ask the future if there is a result available:
    ```c++
    if (myFuture.wait_for(0)) { // Value is available.
        T result { myFuture.get() };
    } else {
        // Value is not yet available.
        ...
    }
    ```
    C++ provides the `std::promise` class as one way to implement the concept of a promise. You can call `set_value()` on a promise to store a result, or you can call `set_exception()` on it to store an exception in the promise. You can call `set_value()` or `set_exception()` only once on a specific promise. If you call it multiple times, an `std::future_error` exception will be thrown.

28) A `std::packaged_task` makes it easier to work with promises than explicitly using `std::promise`. A `packaged_task` automatically creates a promise, and automatically stores the result of the callable ­in the promise, no matter whether that result is a value or a thrown exception.
    ```c++
    int calculateSum(int a, int b) { return a + b; }

    int main()
    {
        // Create a packaged task to run calculateSum.
        packaged_task task { calculateSum };
        // Get the future for the result of the packaged task.
        auto theFuture { task.get_future() };
        // Create a thread, move the packaged task into it, and
        // execute the packaged task with the given arguments.
        jthread theThread { move(task), 39, 3 };
        // Do some more work...

        // Get the result.
        int result { theFuture.get() };
        println("Result: {}", result);
    }
    ```

29) If you want to give the C++ runtime more control over whether or not a thread is created to calculate something, you can use `std::async()`.  It accepts a callable to be executed and returns a future that you can use to retrieve the result. There are two ways in which async() can run a callable: 1) By running it on a separate thread asynchronously, 2) By running it on the calling thread synchronously at the time you call `get()` on the returned future. You can influence the runtime’s behavior by specifying a policy argument.

30) A future returned by a call to `async()` blocks in its destructor until the result is available. (This is not true of ordinary futures; only the kind returned from `async()`.) This means that if you call `async()` without capturing the returned future, the `async()` call effectively becomes a blocking call! For example, the following line synchronously calls `calculateSum()`: `async(calculateSum, 39, 3);`. What happens with this statement is that `async()` creates and returns a future. This future is not captured, so it is a temporary. Because it is a temporary future, its destructor is called at the end of this statement, and this destructor blocks until the result is available.

31) A big advantage of using futures is that they can transport exceptions between threads. Calling `get()` on a future either returns the calculated result or rethrows any exception that has been stored in the promise linked to the future. When you use `packaged_task` or `async()`, any exception thrown from the launched callable is automatically stored in the promise. If you use `std::promise` directly as your promise, you can call `set_exception()` to store an exception in it.

32) `std::future<T>` only requires `T` to be move-­constructible. When you call `get()` on a `future<T>`, the result is moved out of the future and returned to you. This means you can call `get()` only once on a `future<T>`. If you want to be able to call `get()` multiple times, even from multiple threads, then you need to use an `std::shared_future<T>`. A `shared_future` can be created by using `std::future::share()` or by passing a future to the `shared_future` constructor. A future is not copyable, so you have to move it into the `shared_future` constructor.
    ```c++
    promise<void> thread1Started, thread2Started;
    promise<int> signalPromise;

    auto signalFuture { signalPromise.get_future().share() };
    //shared_future<int> signalFuture { signalPromise.get_future() };

    auto function1 { [&thread1Started, signalFuture] {
        thread1Started.set_value();
        // Wait until parameter is set.
        int parameter { signalFuture.get() };
        // ...
    } };

    auto function2 { [&thread2Started, signalFuture] {
        thread2Started.set_value();
        // Wait until parameter is set.
        int parameter { signalFuture.get() };
        // ...
    } };

    // Run both lambda expressions asynchronously.
    // Remember to capture the future returned by async()!
    auto result1 { async(launch::async, function1) };
    auto result2 { async(launch::async, function2) };

    // Wait until both threads have started.
    thread1Started.get_future().wait();
    thread2Started.get_future().wait();

    // Both threads are now waiting for the parameter.
    // Set the parameter to wake up both of them.
    signalPromise.set_value(42);
    ```

## Chapter 28: Maximizing Software Engineering Methods

1) A classic life cycle model for software is the **waterfall model**: Planning -> Design -> Implementation -> Unit testing -> Subsystem testing -> Integration testing -> Evaluation. The main problem with the waterfall model is that, in practice, it is nearly impossible to complete one stage without at least exploring the next stage. A design cannot be set in stone without writing at least some code. Furthermore, if testing reveals bugs, you’ll have to go “back up” the waterfall and do some more coding whether your model formally accounted for that or not.

2) A number of refinements to the waterfall model have been formalized. One such refinement is called the **sashimi model**. The main advancement that the sashimi model brought was the concept of overlap between stages. The name, sashimi model, comes from a Japanese fish dish, called sashimi, in which different pieces of fish are overlapping each other.

3) With **spiral-like** models the fundamental idea is that it’s okay if something goes wrong because you’ll fix it in the next iteration.

4) The **agile model** is just a high-­level foundation; it does not specify exactly how the model should be implemented in real life. That’s where **Scrum** comes into play; it’s an agile methodology with precise descriptions of how to use it on a daily basis. Scrum is an iterative process. It is popular as a means to manage software development projects. In Scrum, each iteration is called a sprint cycle. Before a sprint is started, the team decides on the sprint goals. The team makes a commitment to deliver these goals at the end of the sprint. There are three roles in Scrum. The first role, *product owner*, is the connection to the customer and to other people. The product owner writes high-­level *user stories* based on input from the customer, gives each user story a priority, and puts the stories on the Scrum *product backlog*. Actually, everyone on the team is allowed to write high-­level user stories for the product backlog, but the product owner decides which user stories are kept and which are removed.The second role, *Scrum master*, is responsible for keeping the process running and can be part of the team but isn’t considered the leader of the team, because with Scrum the team leads itself. The Scrum master ensures that the Scrum process is followed correctly by the team, for example, by organizing the daily Scrum meetings. The Scrum master and product owner should be two different people. The third and final role in the Scrum process is the team itself. Teams develop the software and should be kept small, preferably fewer than 10 members. Before the start of each sprint cycle, there is a sprint planning meeting in which team members must decide which product features they will implement in the new sprint. This is formalized in a sprint backlog. The features are selected from the product backlog containing prioritized user stories, which are high-­level requirements of new features. User stories from the product backlog are broken down into smaller tasks with an effort estimation and are put on the sprint backlog. The sprint planning meeting is usually split into two parts: a meeting with the product owner and the team to discuss the priority of product backlog items and a meeting with only the team to complete the sprint backlog. Tasks are not assigned to people during a meeting; instead, every team member can go to the board, pick one of the To Do tasks, and move that paper to the In Progress column. When the team member is finished with that task, the paper is moved to the Done column. Instead of a physical Scrum board, you can also use software to work with a virtual Scrum board. The three columns, To Do, In Progress, and Done, are not set in stone. Your team can add any additional columns to include other steps, for instance In Review, In Testing. The Scrum process enforces a daily meeting called the *daily replanning*, *daily Scrum*, or *daily standup*. In this meeting, all team members stand together with the Scrum master. This meeting should start every day at the same time and location but should be no longer than 15 minutes. The sprint board should be visible for all team members during this meeting. In this meeting, the team looks at the current progress toward the sprint goal. Any blocking points or delays are discussed. If need be, the team can remove or add tasks to the current sprint to make sure the sprint goal can be reached at the end of the sprint. Once a sprint cycle is finished, there are two meetings: the sprint review and the sprint retrospective. During the sprint review meeting, a demo is given to demonstrate the sprint results and the current state of the software to all interested stakeholders. The sprint review meeting also includes a discussion of the results of the sprint cycle, including what tasks were completed, what tasks were not completed, and why. The sprint retrospective allows the team to think about how the last sprint cycle was executed for continuous improvement.

## Chapter 30: Becoming Adept At Testing

1) **Fuzz testing**, also known as fuzzing, involves a fuzzer that automatically generates random input data for a program or component to try to find unhandled edge cases.

2) An **integration test** covers areas where components meet. Unlike a unit test, which generally acts on the level of a single class, an integration test usually involves two or more classes. Integration tests do not always need to occur at integration points in your own code. Many times,integration tests are written to test the interaction between your code and a third-­party library. In terms of implementation, integration tests are often written by using a unit testing framework.

3) **System tests** operate at an even higher level than integration tests. These tests examine the program as a whole. System tests often make use of a virtual user that simulates a human being working with the program. Of course, the virtual user must be programmed with a script of actions to perform. Other system tests rely on scripts or a fixed set of inputs and expected outputs.

4) **Regression testing** is more of a testing concept than a specific type of test. The idea is that once a feature works, developers tend to put it aside and assume that it will continue to work. Unfortunately, new features and other code changes often conspire to break previously working functionality. Bugs and tests are closely related. **Bug fixes should be proven by writing regression tests**. A comment with a test could refer to the original bug number.

## Chapter 31: Conquering Debugging

1) A bug in a computer program is incorrect run-­time behavior. This undesirable behavior includes both **catastrophic** and **noncatastrophic** bugs. The latter are also called **logical bugs**. There are also **cosmetic** bugs, where something is visually not correct, but otherwise works correctly.

2) Make sure your programs create crash dumps, also called memory dumps or core dumps. A crash dump is a dump file that is created when your application crashes. It contains information about which threads were running at the time of the crash, a call stack of all the threads, and so on. How you create such dumps is platform dependent, so you should consult the documentation of your platform or use a third-­party library that takes care of it for you. Breakpad (github.com/google/breakpad/) is an example of such an open-­source cross-­platform library that can write and process crash dumps.

3) Make sure you set up a **symbol server** and a version control server. The symbol server is used to store debugging symbols of released binaries of your software. These symbols are used later to interpret crash dumps received from customers.

## Chapter 32: Incorporating Design Techniques and Frameworks

1) The copy-and-swap idiom is used implement a possibly throwing operation on an object with a strong exception-­safety guarantee, that is, all-­or-­nothing. You simply create a copy of the object, modify that copy (can be a complex algorithm, possibly throwing exceptions), and finally, when no exceptions have been thrown, swap the copy with the original object using a non-­throwing `swap()`. An assignment operator is an example of an operation for which you can use the copy-­and-­swap idiom.

2) Double dispatch is really a special case of multiple dispatch, in which a behavior is chosen depending on the run-­time types of two or more objects.

3) An option to implement a mixin class in C++ is to make the mixin class itself a class template accepting a template type parameter and then deriving itself from that type. Another technique to implement a mixin class in C++ is to use the **curiously recurring template pattern** (CRTP). The mixin class itself is again a class template, but this time it accepts a template type parameter representing the type of a derived class and doesn’t inherit itself from any other class.
    ```c++
    template <typename Derived>
    class SelfDrivable
    {
    public:
        void drive()
        {
            auto& self { static_cast<Derived&>(*this) };
            self.setSpeed(1.2);
        }
    };

    class Car : public SelfDrivable<Car>
    {
    public:
        void setSpeed(double speed) { 
            println("Car speed set to {}.", speed); 
        }
    };
    ```
    Thanks to the C++23 “deducing this”
    feature, the `SelfDrivable` class can be implemented more elegantly as follows using an explicit
    object parameter.

    ```c++
    class SelfDrivable
    {
    public:
        void drive(this auto& self) { self.setSpeed(1.2); }
    };

    class Car : public SelfDrivable  {};
    ```

## Chapter 33: Applying Design Patterns

1) The **strategy** design pattern is one way to support the dependency inversion principle (DIP). If a component needs a set of services, interfaces to those services are injected into the component, a mechanism called dependency injection. Strategy makes it easy to introduce different implementations which helps with mocking (stub versions) for unit testing.

2) **Factory** methods are one way to implement virtual constructors, which are member functions that create objects of different types.

3) The first step in implementing the **adapter pattern** is to define the new interface for the underlying functionality.

4) A **proxy** object serves as a stand-­in for a real object. Such objects are generally used when using the real object would be time-­consuming or impossible.

5) The **iterator pattern** provides a mechanism for separating algorithms or operations from the structure of the data on which they operate. Basically, an iterator allows algorithms to navigate a data structure without having to know the actual structure of the data.

6) The **observer pattern** is used to have observers be notified by observable objects (= subjects). Concrete observers are registered with the observable object they are interested in. When the observable object’s state changes, it notifies all registered observers of this change. One issue with the observer pattern that you need to keep in mind is the lifetime coupling between observers and subjects.

7) Avoid using the **singleton pattern** in new code because of its numerous problems. Prefer other patterns, such as the strategy design pattern.