#ifndef LLUVIA_CORE_IMPL_ENUM_UTILS_H_
#define LLUVIA_CORE_IMPL_ENUM_UTILS_H_

#include <algorithm>
#include <array>
#include <cstdint>
#include <string>
#include <tuple>
#include <typeinfo>
#include <vector>


namespace ll {
namespace impl {


template<typename T, std::size_t N, const std::array<const char*, N>& stringValues>
inline std::string enumToString(const T value) {
    return stringValues.at(static_cast<uint32_t>(value));
}


template<typename E, typename T, std::size_t N, const std::array<const char*, N>& stringValues>
inline E stringToEnum(T&& name) {

    static_assert(std::is_convertible<T, std::string>(), "T must be a string-like type");

    auto it = std::find(stringValues.cbegin(), stringValues.cend(), name);

    if (it == stringValues.cend()) {
        throw std::out_of_range("invalid string value [" + name + "] for enum type " + typeid(E).name());
    }

    return static_cast<E>(std::distance(stringValues.cbegin(), it));
}


template<typename E, typename Ebits, std::size_t N, const std::array<std::tuple<const char*, Ebits>, N>& values>
inline E vectorStringToFlags(const std::vector<std::string>& flagsVector) noexcept {

	auto flags = E {};

	for (const auto& strFlag : flagsVector) {

		for (const auto& flagPair : values) {

			if (std::get<0>(flagPair) == strFlag) {
				flags |= std::get<1>(flagPair);
				break;
			}
		}
	}

	return flags;
}


template<typename E, typename Ebits, std::size_t N, const std::array<std::tuple<const char*, Ebits>, N>& values>
inline std::vector<std::string> flagsToVectorString(E flags) noexcept {

    auto flagsVector = std::vector<std::string> {};

    for (const auto& flagPair : values) {

        const auto& vFlags = std::get<1>(flagPair);

        if ((flags & vFlags) == vFlags) {
            flagsVector.push_back(std::get<0>(flagPair));
        }
    }

    return flagsVector;
}


} // namespace impl
} // namespace ll

#endif // LLUVIA_CORE_IMPL_ENUM_UTILS_H_
