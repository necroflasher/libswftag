/*----------------------------------------------------------Data Structures----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/

// If you're scared about the choice of unsigned long long as UI32_TYPE, that's because as far as I understand the standard, long long is the only int type guaranteed to be atleast 4 bytes(32 bits for minimum value of CHAR_BIT being 8)
// And unsigned to ensure uniformity in case it's bigger. The arithmetic ops in swfmath.h will treat it as signed because that's what the swf format does, virtually anywhere

#ifndef DATA_STRUCTS
	#define DATA_STRUCTS

	// Linked lists everywhere. That's what you pay for the modularity this library affords you
	struct doubly_linked_list_node
	{
		struct doubly_linked_list_node *next;
		struct doubly_linked_list_node *prev;

		struct doubly_linked_list_node *to_free;	// A list of elements to free when the node is freed. Yes this is pretty ugly.

		void *data;
	};
	typedef struct doubly_linked_list_node dnode;

	struct fixed16_16
	{
		ui16 hi;
		ui16 lo;
	};
	typedef struct fixed16_16 uf16_16;

	struct fixed8_8
	{
		ui8 hi;
		ui8 lo;
	};
	typedef struct fixed8_8 uf8_8;

	struct fixed32_32
	{
		ui32 hi;
		ui32 lo;
	};
	typedef struct fixed32_32 uf32_32;

	struct fixedvar
	{
		// Specified the width of the fixed point number in hi.lo format
		uf8_8 width;
		ui32 hi;
		ui32 lo;
	};
	typedef struct fixedvar fvar;


	// Compound return types
	struct return_pointer_with_error
	{
		void *pointer;
		err ret;
	};
	typedef struct return_pointer_with_error err_ptr;

	struct return_integer_with_error
	{
		ui32 integer;
		err ret;
	};
	typedef struct return_integer_with_error err_int;

	struct swf_tag
	{
		int tag;
		ui32 size;
		uchar *tag_data;
		ui16 tag_and_size;

		ui16 tag_id;

		void *tag_struct;
		dnode *parent_node;
	};
	typedef struct swf_tag swf_tag;

#endif

/*----------------------------------------------------------Library Structs----------------------------------------------------------*/
/*-----------------------------------------------------------------------------------------------------------------------------------*/
/*-----------------------------------------------------------------|-----------------------------------------------------------------*/


#ifndef LIB_STRUCTS
	#define LIB_STRUCTS

/*------------------------ TAG SUBSTRUCTURES ------------------------*/
/*---------------------------------|---------------------------------*/

// Disclaimer: This is the library's representation of the structures for convinience to be clear. This is not a accurate view into how the swfs are actually stored
// TODO : add defs for the bitfields somewhere

	struct swf_rect
	{
		ui32 fields[4];
		ui8 field_size;
	};
	typedef struct swf_rect RECT;

	struct swf_matrix
	{
		// has_scale : 0x1
		// has_rotate : 0x10
		ui8 bitfields;

		ui8 scale_bits;
		uf16_16 scale_x;
		uf16_16 scale_y;

		ui8 rotate_bits;
		uf16_16 rotate_skew0;
		uf16_16 rotate_skew1;

		ui8 translate_bits;
		uf16_16 translate_x;
		uf16_16 translate_y;
	};
	typedef struct swf_matrix MATRIX;

	struct swf_color_transform
	{
		// has_add  : 0x1
		// has_mult  : 0x10
		ui8 bitfields;
		ui8 color_bits;

		uf16_16 red_mult;
		uf16_16 green_mult;
		uf16_16 blue_mult;

		uf16_16 alpha_mult;

		uf16_16 red_add;
		uf16_16 green_add;
		uf16_16 blue_add;

		uf16_16 alpha_add;
	};
	typedef struct swf_color_transform COLOR_TRANSFORM;

	struct swf_action
	{
		ui8 id;
		ui16 action_length;

		uchar *action_data;
	};
	typedef struct swf_action ACTION;

	struct swf_rgb
	{
		ui8 red;
		ui8 green;
		ui8 blue;
	};
	typedef struct swf_rgb RGB;

	struct swf_rgba
	{
		ui8 red;
		ui8 green;
		ui8 blue;
		ui8 alpha;
	};
	typedef struct swf_rgba RGBA;

	struct swf_glyphentry
	{
		ui32 glyph_index;	// The spec is unclear whether or not glyph_bits can be >32, or anything really. But since the swf movie length (including signature) is capped at 4GiB, the glyph_index cannot possible exceed 32 bit width, and if it does, that's an error.
		ui32 glyph_advance;	// A similar logic cannot be applied here, but since nowhere else are twips stored in a field of width >32, this is valid I think. Again, exceeding this, would be an error.
	};
	typedef struct swf_glyphentry GLYPHENTRY;

	struct swf_text_record
	{
		// has_font : 0x8
		// has_color : 0x4
		// has_move_x : 0x2
		// has_move_y : 0x1
		ui8 bitfields;

		ui16 define_font_id;
		swf_tag *font_tag;

		// If family_version == 1, then Alpha = 255.
		RGBA color;

		ui16 move_x;
		ui16 move_y;
		ui16 font_height;

		// 7 bit value for ver < 7
		// 8 bit Otherwise (??)
		ui8 glyph_count;
		GLYPHENTRY *entries;
	};
	typedef struct swf_text_record TEXT_RECORD;

	struct swf_filter_blur
	{
		uf16_16 horizontal_blur;
		uf16_16 vertical_blur;
		ui8 passes;
	};
	typedef struct swf_filter_blur BLUR_FILTER;

	struct swf_filter_colormatrix
	{
		float matrix[20];
	};
	typedef struct swf_filter_colormatrix COLORMATRIX_FILTER;

	struct swf_filter_convolution
	{
		ui8 columns;
		ui8 rows;
		float divisor;
		float bias;
		float *weights;
		RGBA default_color;
		// Clamp : 0x1
		// Preserve Alpha: 0x10
		ui8 bitfields;
	};
	typedef struct swf_filter_convolution CONVOLUTION_FILTER;

	struct swf_filter_glow
	{
		ui8 count;
		RGBA *rgbas;
		ui8 *position;
		uf16_16 horizontal_blur;
		uf16_16 vertical_blur;
		uf16_16 radian_angle;
		uf16_16 distance;

		uf8_8 glow_strength;

		// inner_shadow: 0x1
		// knock_out: 0x4
		// composite_source: 0x10
		// on_top: 0x40
		ui8 bitfields;

		ui8 passes;
	};
	typedef struct swf_filter_glow GLOW_FILTER;

	union swf_filter_any
	{
		BLUR_FILTER blur;
		COLORMATRIX_FILTER colormatrix;
		CONVOLUTION_FILTER convolution;
		GLOW_FILTER glow;
	};
	typedef union swf_filter_any FILTER_ANY;

	struct swf_filter
	{
		ui8 filter_type;
		FILTER_ANY filter_data;
	};
	typedef struct swf_filter FILTER;


/*--------------------------- LIB STRUCTS ---------------------------*/
/*---------------------------------|---------------------------------*/

	struct parse_peculiarity
	{
		ui16 pattern;
		size_t offset;
	};
	typedef struct parse_peculiarity peculiar;

	// FileHeader pseudo-tag and DefineSprite are the only tags for which a new swf_scope opens. END tags close the scope

	struct parse_data
	{
		ui8 compression;
		ui8 version;
		ui32 movie_size;
		uchar signature[8];
		uchar *u_movie;	// Uncompressed movie data

		ui32 reported_movie_size;

		ui8 avm1;
		ui8 avm2;

		RECT movie_rect;
		// 8.8 fixed point
		uf8_8 movie_fr;
		ui16 movie_frame_count;

		dnode *pec_list;	// List of parsing peculiarities that are not necessarily errors
		dnode *pec_list_end;

		uchar *tag_buffer;
		dnode *tag_stream;
		dnode *tag_stream_end;

		dnode *scope_stack;

		ui32 n_tags;	// Number of tags

		swf_tag **id_list[256]; // 2D array of 1<<16 swf_tag pointers, Each 256 pointer subarray is dynamically allocated.
	};
	typedef struct parse_data pdata;

/*----------------------- MAIN TAG STRUCTURES -----------------------*/
/*---------------------------------|---------------------------------*/

	// defineshape will go here

	struct swf_tag_placeobject
	{
		ui16 obj_depth;

		MATRIX matrix;
		ui8 has_color_transform;
		COLOR_TRANSFORM color_transform;
	};

	struct swf_tag_removeobject
	{
		ui16 obj_depth;
	};

	struct swf_tag_definebitsx
	{
		ui8 family_version;
		ui32 alpha_offset;
		ui16 deblocking_filter_parameter;

		uchar *encoding_tables;
		uchar *image_data;
		uchar *alpha;
	};

	struct swf_tag_definebutton
	{
		// Only for version 8+
		// blend_mode : 0x1
		// filter_list : 0x10
		ui8 bitfields;

		// square_test : 0x8
		// down : 0x4
		// hover : 0x2
		// up : 0x1
		ui8 button_state;
		ui16 layer;
		MATRIX matrix;

		COLOR_TRANSFORM color_transform;

		ui8 filter_count;
		FILTER filter;

		ui8 blend_mode;
	};

	struct swf_tag_setbackgroundcolor
	{
		RGB color;
	};

	struct swf_tag_jpegtables
	{
		uchar *data;
		swf_tag bind_tag;
	};

/*
	struct swf_tag_definefont
	{
		ui16 glyphs_count;

		ui16 glyph_offsets*

		////////SHAPE glyph_shapes*;
	};
*/

	struct swf_tag_definetextx
	{
		ui8 family_version;
		ui16 id;

		RECT rect;
		MATRIX mat;
		ui8 glyph_bits;
		ui8 advance_bits;
		TEXT_RECORD record;
	};

	struct swf_tag_avm1action
	{
		ui8 initaction; // Boolean
		ui16 action_sprite;
		ACTION *actions;
	};

	struct swf_tag_fontinfox
	{
		ui8 family_version;

		ui16 define_font_id;	// Refers to the define font id. Map the define font tag with the matching id before the font info tag in the current stream to. If there are more than one tags with the same id, that's an error.
		ui8 name_length;
		uchar *name;

		// For font info 2
		// V7+ small_text : 0x20
		// For font info 1
		// is_unicode : 0x20
		// is_shiftjis : 0x10
		// is_ansi (latin) : 0x08
		// Always
		// italic : 0x4
		// bold : 0x2
		// wide: 0x1	; Must be 1 for font info 2 and font info 1 v6+
		ui8 bitfields;
		ui8 lang;

		swf_tag *font_tag;

		void *font_info_map;
	};

	struct swf_tag_definesound
	{
		ui8 format;
		ui8 rate_power;	// Determines the exponent of 2 to multiply with 5512.5Hz

		// 16bit/compressed audio : 0x2
		// stereo : 0x1
		ui8 bitfields;

		ui32 samples_count;
		uchar *sound_data;
	};

#endif
