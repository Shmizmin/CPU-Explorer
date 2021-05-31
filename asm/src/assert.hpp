#ifndef asm_assert_hpp
#define asm_assert_hpp

//function that performs a debug assertion and exits the program
void _assert(char const* message, const char* function, char const* filename, unsigned line);

//define the functional macros for dynamic_assert()
#ifdef NDEBUG
#define dynamic_assert(expression) ((void)0)
#else
#define dynamic_assert(expression) (void)((!!(expression)) || (::_assert(#expression, __FUNCTION__, __FILE__, (unsigned)(__LINE__)), 0))
#endif

#endif