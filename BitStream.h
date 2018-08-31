#pragma once

namespace GApi
{
	namespace Media
	{
		class BitStream
		{
		public:
			class IO
			{
			public:
				virtual bool Read(void* buffer, size_t size) = 0;
				virtual void Skip(size_t size) = 0;
			};

		private:
			IO					&io_;
			uint8_t			_bits;				// octet
			size_t				_offsetBits;
			size_t				_readBits;

		public:
			BitStream(IO& io) :
				io_(io),
				_bits(0),
				_offsetBits(0),
				_readBits(0)
			{}

			//Number of bits read
			size_t GetReadBits()
			{
				return _readBits;
			}

			bool Read(void* buffer, size_t size)
			{
				assert(_offsetBits == 0);

				return io_.Read(buffer, size);
			}

			template<class T = uint8_t>
			T ReadBit(size_t n = 1)
			{
				T value = 0;
	
				if (_offsetBits < n)
				{
					uint8_t readByte = (uint8_t)ceil((double)(n-_offsetBits) / 8.0);
					uint8_t readBits = readByte * 8;							

					T bytes = 0;
					io_.Read(&bytes, readByte);

					value = (T)(_bits << (n - _offsetBits));
					value |= (T)(bytes >> (_offsetBits+(readBits-n)));

					bytes <<= (n - _offsetBits);
					bytes &= 0xff;
					_bits = (uint8_t)(bytes >> (n - _offsetBits));

					_offsetBits = readBits - (n - _offsetBits);
				}
				else
				{
					value = (T)(_bits >> (_offsetBits - n));
					_bits <<= (8 - (_offsetBits - n));
					_bits >>= (8 - (_offsetBits - n));
					_offsetBits -= n;									
				}


				if (n > 8)
				{
					size_t count = n / 8;
					size_t remain = n % 8;
					T temp = value;
	
					for (size_t i = 0; i < count; ++i)
					{
						value = (value << 8) | (0xff & (temp >> (i * 8)));
					}

					if (remain > 0)
					{
						value |= (value << 8) | (0xff & (temp >> remain));
					}
				}

				_readBits += n;

				return value;
			}

			void SkipBit(size_t n = 1)
			{
				assert(false && "not implementation");
			}

			void AlignBits()
			{
				if (_offsetBits > 0)
				{
					uint8_t temp;
					io_.Read(&temp, 1);
				}

				_offsetBits = 0;
				_bits = 0;
				_readBits = 0;
			}

			template<typename T, bool eglomb>
			T ReadBit() { assert(0); return 0; }

			template<>
			uint32_t ReadBit<uint32_t, true>()
			{
				int32_t leadingZeroBits = -1;
				for (uint8_t b = 0; !b; leadingZeroBits++)
				{
					b = ReadBit();
				}

				uint32_t codeNum = 0;
				if (leadingZeroBits > 0)
				{
					codeNum = (uint32_t)std::pow(2, leadingZeroBits) - 1 + ReadBit<int32_t>(leadingZeroBits);
				}

				return codeNum;
			};

			template<>
			int32_t ReadBit<int32_t, true>()
			{
				uint32_t codeNum = ReadBit<uint32_t, true>();
				return (int32_t)std::pow(-1.0, codeNum + 1) * (int32_t)std::ceil(codeNum / 2.0);
			};
		};
	}
}