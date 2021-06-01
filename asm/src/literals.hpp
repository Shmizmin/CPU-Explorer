#ifndef cpu_literals_hpp
#define cpu_literals_hpp

//necessary standard library includes
import std.core;

//create an operator literal of a specified type and signedness
#define OPERATOR_V(n, s, x)                                                   \
inline constexpr std::s##nt##n##_t operator"" _##x##n(std::size_t i) noexcept \
{                                                                             \
	return static_cast<std::s##nt##n##_t>(i);                                 \
}                                                                             \

//for each operator literal, create a signed and unsigned version
#define OPERATOR_C(n)        \
		OPERATOR_V(n, ui, u) \
		OPERATOR_V(n, i, i)  \

//create operator literals for the common bit quantities
OPERATOR_C(8)
OPERATOR_C(16)
OPERATOR_C(32)
OPERATOR_C(64)

//verify macro leakage safety
//UPDATE: can safely remove, using header units now
#undef OPERATOR_V
#undef OPERATOR_C

//specialized size_t integer literal
inline constexpr std::size_t operator"" _uz(std::size_t n) noexcept
{
	return n;
}

//specialized std::byte integer literal
inline constexpr std::byte operator"" _byte(std::size_t n) noexcept
{
	return std::byte{ static_cast<std::underlying_type_t<std::byte>>(n) };
}

#endif