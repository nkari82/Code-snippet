#pragma once


namespace GApi
{
	namespace Media
	{
		/*
		AVCProfileIndication
		66	Baseline	������, ���ػ�, ���� �����̸� ����. CAVLC�� ���, B��ó ������� ����
		66	Constrained Baseline	Baseline�� Main���������� �������� �Ǵ� ����� ���				openh264
		77	Main	�����ȣȭ �� B��ó ������� ȿ���� ����
		88	Extended	��Ʈ������ ���� ��� �� ���� ������� ���
		FRExT	100	High	8x8 DCT ���,8x8 ȭ�鳻 ���� ���
		110	High 10	High �������� + 10bit/pixel�� ���� ����
		110	High 10 Intra	High10 �������Ͽ��� ȭ�� �� ���� ����� ���
		122	High 4:2:2	High10 �������� + 4:2:2 �÷� ���� ����
		122	High 4:2:2 Intra	High 4:2:2���� ȭ�� �� ���� ����� ���
		244	High 4:4:4 Predictive	�ִ� 14bit/pixel ����, 4:4:4 �÷� ���� ����
		244	High 4:4:4 Intra	High 4:4:4 Predictive���� ȭ�� �� ���� ����� ����
		44	CAVLC 4:4:4 Intra	High 4:4:4 Intra���� CAVLC�� ���(CABAC������� ����)
		*/

		class H264Parser
		{
		public:

			// AVCProfileIndication
			// 66	Baseline	������, ���ػ�, ���� �����̸� ����.CAVLC�� ���, B��ó ������� ����
			// 66	Constrained Baseline	Baseline�� Main���������� �������� �Ǵ� ����� ���
			// 77	Main	�����ȣȭ �� B��ó ������� ȿ���� ����
			// 88	Extended	��Ʈ������ ���� ��� �� ���� ������� ���

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
