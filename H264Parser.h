#pragma once


namespace GApi
{
	namespace Media
	{
		/*
		AVCProfileIndication
		66	Baseline	저전력, 저해상도, 낮은 딜레이를 가짐. CAVLC만 사용, B픽처 사용하지 않음
		66	Constrained Baseline	Baseline과 Main프로파일의 교집합이 되는 기술만 사용				openh264
		77	Main	산술부호화 및 B픽처 사용으로 효율을 높임
		88	Extended	스트리밍을 위한 기술 및 오류 내성기술 사용
		FRExT	100	High	8x8 DCT 사용,8x8 화면내 예측 사용
		110	High 10	High 프로파일 + 10bit/pixel인 영상 지원
		110	High 10 Intra	High10 프로파일에서 화면 내 예측 기술만 사용
		122	High 4:2:2	High10 프로파일 + 4:2:2 컬러 포맷 지원
		122	High 4:2:2 Intra	High 4:2:2에서 화면 내 예측 기술만 사용
		244	High 4:4:4 Predictive	최대 14bit/pixel 지원, 4:4:4 컬러 포맷 지원
		244	High 4:4:4 Intra	High 4:4:4 Predictive에서 화면 내 예측 기술만 지원
		44	CAVLC 4:4:4 Intra	High 4:4:4 Intra에서 CAVLC로 사용(CABAC사용하지 않음)
		*/

		class H264Parser
		{
		public:

			// AVCProfileIndication
			// 66	Baseline	저전력, 저해상도, 낮은 딜레이를 가짐.CAVLC만 사용, B픽처 사용하지 않음
			// 66	Constrained Baseline	Baseline과 Main프로파일의 교집합이 되는 기술만 사용
			// 77	Main	산술부호화 및 B픽처 사용으로 효율을 높임
			// 88	Extended	스트리밍을 위한 기술 및 오류 내성기술 사용

			enum H264Profiles
			{
				BASELINE = 66,   //!< YUV 4:2:0/8  "Baseline profile"
				MAINP = 77,   //!< YUV 4:2:0/8  "Main profile"
				UNKNOWN1 = 83,   //!< Unknown profile
				UNKNOWN2 = 86,   //!< Unknown profile
				EXTENDED = 88,   //!< YUV 4:2:0/8  "Extended profile"

				FREXT_CAVLC444 = 44,   //!< YUV 4:4:4/14 "CAVLC 4:4:4"
				FREXT_HiP = 100,  //!< YUV 4:2:0/8  "High"
				FREXT_Hi10P = 110,  //!< YUV 4:2:0/10 "High 10"
				FREXT_Hi422 = 122,  //!< YUV 4:2:2/10 "High 4:2:2"
				FREXT_Hi444 = 244,  //!< YUV 4:4:4/14 "High 4:4:4"

				MVC_HIGH = 118,  //!< YUV 4:2:0/8  "Multiview High"
				MVC_FIELDHIGH = 119,  //!< YUV 4:2:0/8  "Multiview Field High"
				STEREO_HIGH = 128   //!< YUV 4:2:0/8  "Stereo High"
			};

			void Parse(BitStream& stream);

		private:
			void DecodeSPS(BitStream& stream);
			void DecodeHRD(BitStream& stream);
			void DecodeVUI(BitStream& stream);
			void DecodePPS(BitStream& stream);
		};
	}
}
