#ifndef cpu_literals_hpp
#define cpu_literals_hpp

//create an operator literal of a specified type and signedness
#define OPERATOR_V(n, x)                                            \
inline constexpr n operator"" _##x(unsigned long long i) noexcept \
{                                                                   \
	return static_cast<n>(i);                                       \
}                                                                   \

//for each operator literal, create a signed and unsigned version
#define OPERATOR_C(n, x)             \
		OPERATOR_V(  signed n, i##x) \
		OPERATOR_V(unsigned n, u##x) \


//create operator literals for the common bit quantities
OPERATOR_C(__int8, 8)
OPERATOR_C(__int16, 16)
OPERATOR_C(__int32, 32)
OPERATOR_C(__int64, 64)

//verify macro leakage safety
//UPDATE: can safely remove, using header units now
#undef OPERATOR_V
#undef OPERATOR_C

//specialized size_t integer literal
inline constexpr size_t operator"" _uz(size_t n) noexcept
{
	return n;
}

#endif