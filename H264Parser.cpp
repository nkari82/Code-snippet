#include "Common.h"


namespace GApi
{
	namespace Media
	{
		void H264Parser::Parse(BitStream& stream)
		{
			uint8_t configurationVersion = stream.ReadBit(8);
			uint8_t AVCProfileIndication = stream.ReadBit(8);
			uint8_t profile_compatibility = stream.ReadBit(8);
			uint8_t AVCLevelIndication = stream.ReadBit(8);			// 31 => 3.1

			stream.ReadBit(6);	// skip
			uint8_t lengthSizeMinusOne = stream.ReadBit(2);

			stream.ReadBit(3);	// skip
			uint8_t numOfSequenceParameterSets = stream.ReadBit(5);

			// parse sps
			for (int32_t i = 0; i < numOfSequenceParameterSets; ++i)
			{
				uint16_t sampleSize = stream.ReadBit<uint16_t>(16);
				stream.AlignBits();

				stream.ReadBit();
				uint8_t nal_ref_idc = stream.ReadBit(2);
				uint8_t nal_unit_type = stream.ReadBit(5);

				DecodeSPS(stream);

				size_t skipBits = (sampleSize * 8) - stream.GetReadBits();
				stream.ReadBit<uint64_t>(skipBits);
			}

			numOfSequenceParameterSets = stream.ReadBit(8);
			for (int32_t i = 0; i < numOfSequenceParameterSets; ++i)
			{
				uint16_t sampleSize = stream.ReadBit<uint16_t>(16);
				stream.AlignBits();

				stream.ReadBit();
				uint8_t nal_ref_idc = stream.ReadBit(2);
				uint8_t nal_unit_type = stream.ReadBit(5);

				DecodePPS(stream);

				size_t skipBits = (sampleSize * 8) - stream.GetReadBits();
				stream.ReadBit<uint64_t>(skipBits);
			}
		}

		void H264Parser::DecodeSPS(BitStream& stream)
		{
			uint8_t profile_idc = stream.ReadBit(8);

			bool constraint_setX_flag[6];
			constraint_setX_flag[0] = stream.ReadBit();
			constraint_setX_flag[1] = stream.ReadBit();
			constraint_setX_flag[2] = stream.ReadBit();
			constraint_setX_flag[3] = stream.ReadBit();
			constraint_setX_flag[4] = stream.ReadBit();
			constraint_setX_flag[5] = stream.ReadBit();
			if (stream.ReadBit(2) != 0)
			{
				assert(false);			// must be zero
			}


			uint8_t level_idc = stream.ReadBit(8);
			int32_t seq_parameter_set_id = stream.ReadBit<uint32_t, true>();

			uint32_t chroma_format_idc = 0;
			bool separate_colour_plane_flag = false;
			uint32_t bit_depth_luma_minus8 = 0;
			uint32_t bit_depth_chroma_minus8 = 0;

			bool qpprime_y_zero_transform_bypass_flag = false;
			bool seq_scaling_matrix_present_flag = false;

			if (profile_idc == FREXT_HiP || profile_idc == FREXT_Hi10P ||
				profile_idc == FREXT_Hi422 || profile_idc == FREXT_Hi444 || profile_idc == FREXT_CAVLC444 ||
				profile_idc == 83 || profile_idc == 86 || profile_idc == MVC_HIGH ||
				profile_idc == STEREO_HIGH || profile_idc == 138 || profile_idc == 139 ||
				profile_idc == 134 || profile_idc == 135)
			{
				chroma_format_idc = stream.ReadBit<uint32_t, true>();

				if (chroma_format_idc == 3)
				{
					separate_colour_plane_flag = stream.ReadBit();
				}

				bit_depth_luma_minus8 = stream.ReadBit<uint32_t, true>();
				bit_depth_chroma_minus8 = stream.ReadBit<uint32_t, true>();

				qpprime_y_zero_transform_bypass_flag = stream.ReadBit();
				seq_scaling_matrix_present_flag = stream.ReadBit();

				if (seq_scaling_matrix_present_flag)
				{
					int i = 0;
					for (i = 0; i < ((chroma_format_idc != 3) ? 8 : 12); i++)
					{
						stream.ReadBit();
					}
				}
			}
			else
			{
			}


			uint8_t log2_max_frame_num_minus4 = stream.ReadBit<uint32_t, true>();				// OK
			uint32_t maxFrameNum = (uint32_t)pow(2, log2_max_frame_num_minus4 + 4);
			uint32_t log2_max_pic_order_cnt_lsb_minus4 = 0;
			uint32_t num_ref_frames_in_pic_order_cnt_cycle = 0;

			uint32_t pic_order_cnt_type = stream.ReadBit<uint32_t, true>();			// 0ÀÌ ³ª¿Í¾ßµÊ.
			if (pic_order_cnt_type == 0)
			{
				log2_max_pic_order_cnt_lsb_minus4 = stream.ReadBit<uint32_t, true>();
			}
			else if (pic_order_cnt_type == 1)
			{
				uint8_t delta_pic_order_always_zero_flag = stream.ReadBit();
				int32_t offset_for_non_ref_pic = stream.ReadBit<int32_t, true>();
				int32_t offset_for_top_to_bottom_field = stream.ReadBit<int32_t, true>();
				num_ref_frames_in_pic_order_cnt_cycle = stream.ReadBit<uint32_t, true>();

				for (unsigned i = 0; i < num_ref_frames_in_pic_order_cnt_cycle; i++)
				{
					int32 offset_for_ref_frame = stream.ReadBit<int32_t, true>();

					int temp = 0;
				}
			}

			uint32_t max_num_ref_frames = stream.ReadBit<uint32_t, true>();			// 2
			bool gaps_in_frame_num_value_allowed_flag = stream.ReadBit();

			uint32_t pic_width_in_mbs_minus1 = stream.ReadBit<uint32_t, true>();
			uint32_t pic_height_in_map_units_minus1 = stream.ReadBit<uint32_t, true>();

			bool frame_mbs_only_flag = stream.ReadBit();
			bool mb_adaptive_frame_field_flag = false;
			if (!frame_mbs_only_flag)
			{
				mb_adaptive_frame_field_flag = stream.ReadBit();
			}

			bool direct_8x8_inference_flag = stream.ReadBit();
			bool frame_cropping_flag = stream.ReadBit();

			if (frame_cropping_flag)
			{
				uint32_t frame_crop_left_offset = stream.ReadBit<uint32_t, true>();
				uint32_t frame_crop_right_offset = stream.ReadBit<uint32_t, true>();
				uint32_t frame_crop_top_offset = stream.ReadBit<uint32_t, true>();
				uint32_t frame_crop_bottom_offset = stream.ReadBit<uint32_t, true>();
			}

			// decodeVUI
			bool vui_parameters_present_flag = stream.ReadBit();
			if (vui_parameters_present_flag)
			{
				DecodeVUI(stream);
			}
		}

		void H264Parser::DecodeVUI(BitStream& stream)
		{
			bool aspect_ratio_info_present_flag = stream.ReadBit();

			if (aspect_ratio_info_present_flag)
			{
				uint8_t aspect_ratio_idc = stream.ReadBit(8);

				if (aspect_ratio_idc == 255)
				{
					uint16_t sar_width = stream.ReadBit<uint16_t>(16);
					uint16_t sar_height = stream.ReadBit<uint16_t>(16);
				}
			}

			bool overscan_info_present_flag = stream.ReadBit();
			if (overscan_info_present_flag)
			{
				bool overscan_appropriate_flag = stream.ReadBit();
			}

			bool video_signal_type_present_flag = stream.ReadBit();
			if (video_signal_type_present_flag)
			{
				uint8_t video_format = stream.ReadBit(3);
				bool video_full_range_flag = stream.ReadBit();
				bool colour_description_present_flag = stream.ReadBit();
				if (colour_description_present_flag)
				{
					uint8_t colour_primaries = stream.ReadBit(8);
					uint8_t transfer_characteristics = stream.ReadBit(8);
					uint8_t matrix_coefficients = stream.ReadBit(8);
				}
			}

			bool chroma_loc_info_present_flag = stream.ReadBit();
			if (chroma_loc_info_present_flag)
			{
				uint32_t chroma_sample_loc_type_top_field = stream.ReadBit<uint32_t, true>();
				uint32_t chroma_sample_loc_type_bottom_field = stream.ReadBit<uint32_t, true>();
			}

			bool timing_info_present_flag = stream.ReadBit();
			if (timing_info_present_flag)
			{
				uint32_t num_units_in_tick = stream.ReadBit<uint32_t>(32);
				uint32_t time_scale = stream.ReadBit<uint32_t>(32);
				bool fixed_frame_rate_flag = stream.ReadBit();
			}

			bool nal_hrd_parameters_present_flag = stream.ReadBit();
			if (nal_hrd_parameters_present_flag)
			{
				// Hypothetical Reference Decoder
				DecodeHRD(stream);
			}

			bool vcl_hrd_parameters_present_flag = stream.ReadBit();
			if (vcl_hrd_parameters_present_flag)
			{
				// Hypothetical Reference Decoder
				DecodeHRD(stream);
			}

			if (nal_hrd_parameters_present_flag == true || vcl_hrd_parameters_present_flag == true)
			{
				bool low_delay_hrd_flag = stream.ReadBit();
			}

			bool pic_struct_present_flag = stream.ReadBit();
			bool bitstream_restriction_flag = stream.ReadBit();

			if (bitstream_restriction_flag)
			{
				bool motion_vectors_over_pic_boundaries_flag = stream.ReadBit();
				uint32_t max_bytes_per_pic_denom = stream.ReadBit<uint32_t, true>();
				uint32_t max_bits_per_mb_denom = stream.ReadBit<uint32_t, true>();
				uint32_t log2_max_mv_length_horizontal = stream.ReadBit<uint32_t, true>();
				uint32_t log2_max_mv_length_vertical = stream.ReadBit<uint32_t, true>();
				uint32_t num_reorder_frames = stream.ReadBit<uint32_t, true>();
				uint32_t max_dec_frame_buffering = stream.ReadBit<uint32_t, true>();
			}
		}

		void H264Parser::DecodePPS(BitStream& stream)
		{
			// PPS decoding
			////////////////////////////////////////////////////////////////////////

			uint32_t pic_parameter_set_id = stream.ReadBit<uint32_t, true>();
			uint32_t seq_parameter_set_id = stream.ReadBit<uint32_t, true>();

			bool entropy_coding_mode_flag = stream.ReadBit();
			bool bottom_field_pic_order_in_frame_present_flag = stream.ReadBit();

			uint32_t num_slice_groups_minus1 = stream.ReadBit<uint32_t, true>();
			if (num_slice_groups_minus1 > 0)
			{
				uint32_t slice_group_map_type = stream.ReadBit<uint32_t, true>();
				if (slice_group_map_type == 0)
				{
					for (uint32_t iGroup = 0; iGroup <= num_slice_groups_minus1 && iGroup < 8; iGroup++)
					{
						stream.ReadBit<uint32_t, true>();
					}
				}
				else if (slice_group_map_type == 2)
				{
					for (uint32_t iGroup = 0; iGroup <= num_slice_groups_minus1 && iGroup < 8; iGroup++)
					{
						stream.ReadBit<uint32_t, true>();
						stream.ReadBit<uint32_t, true>();
					}
				}
				else if (slice_group_map_type == 3 || slice_group_map_type == 4 || slice_group_map_type == 5)
				{
					bool slice_group_change_direction_flag = stream.ReadBit();
					uint32_t slice_group_change_rate_minus1 = stream.ReadBit<uint32_t, true>();
				}
				else if (slice_group_map_type == 6)
				{
					uint32_t pic_size_in_map_units_minus1 = stream.ReadBit<uint32_t, true>();
					for (uint32_t i = 0; i <= pic_size_in_map_units_minus1; i++)
					{
					}
				}
			}

			uint32_t num_ref_idx_l0_default_active_minus1 = stream.ReadBit<uint32_t, true>();
			uint32_t num_ref_idx_l1_default_active_minus1 = stream.ReadBit<uint32_t, true>();
			bool weighted_pred_flag = stream.ReadBit();
			uint8_t weighted_bipred_idc = stream.ReadBit(2);
			int32_t pic_init_qp_minus26 = stream.ReadBit<int32_t, true>();
			int32_t pic_init_qs_minus26 = stream.ReadBit<int32_t, true>();
			int32_t chroma_qp_index_offset = stream.ReadBit<int32_t, true>();
			bool deblocking_filter_control_present_flag = stream.ReadBit();
			bool constrained_intra_pred_flag = stream.ReadBit();
			bool redundant_pic_cnt_present_flag = stream.ReadBit();
		}

#define MAX_CPB     32
		void H264Parser::DecodeHRD(BitStream& stream)
		{
			// HRD decoding
			////////////////////////////////////////////////////////////////////////

			uint32_t cpb_cnt_minus1 = stream.ReadBit<uint32_t, true>();
			uint8_t bit_rate_scale = stream.ReadBit(4);
			uint8_t cpb_size_scale = stream.ReadBit(4);

			for (uint32_t SchedSelIdx = 0;
				SchedSelIdx <= cpb_cnt_minus1 && SchedSelIdx < MAX_CPB;
				SchedSelIdx++)
			{
				stream.ReadBit<uint32_t, true>();
				stream.ReadBit<uint32_t, true>();
				stream.ReadBit();
			}

			uint8_t initial_cpb_removal_delay_length_minus1 = stream.ReadBit(5);
			uint8_t cpb_removal_delay_length_minus1 = stream.ReadBit(5);
			uint8_t dpb_output_delay_length_minus1 = stream.ReadBit(5);
			uint8_t time_offset_length = stream.ReadBit(5);
		}
	}
}