#pragma once
namespace fl::xorstr {
	template<typename T, size_t N>
	class XorStr
	{
	private:
		volatile T _data[N + 1];
		volatile T _keys[N + 1];

		constexpr __forceinline T xor(const T c, const T k) {
			return c ^ k;
		}

		template<typename T, SIZE_T I, INT32 A, INT32 C, INT32 M, INT32 First>
		constexpr T randomKey() {
			if constexpr (I == 0) {
				return static_cast<T>(First);
			}
			else {
				return (randomKey<T, I - 1, A, C, M, First>() * A + C) % M;
			}
		}

		template<class T, INT32 I>
		constexpr T createKey() {
			return randomKey<T, I, __TIME__[3] * 32, __TIME__[4] * 64, __TIME__[6] * 128, __TIME__[7] * 256>();
		}

	public:
		constexpr __forceinline const T* get()
		{
			for (size_t i = 0; i < N; ++i) {
				_data[i] = xor(_data[i], _keys[i]);
			}

			_data[N] = 0;
			return const_cast<const T*>(_data);
		}

		~XorStr() {
			for (size_t i = 0; i < N; ++i) {
				_data[i] = 0;
			}
		}

		template<SIZE_T ...I>
		constexpr __forceinline XorStr(const T* str, std::index_sequence<I...>) : _data{ xor(str[I],createKey<T,I>()) ... }, _keys{ createKey<T,I>() ... }{ }
	};

	template<class T> using Decay = typename std::remove_cv<typename std::remove_reference<T>::type>::type;
};

#define XORSTR(buffer) (fl::xorstr::XORSTR<fl::xorstr::Decay<decltype(*buffer)>, sizeof(buffer) - 1>(buffer, std::make_index_sequence<sizeof(buffer) - 1>())).get()