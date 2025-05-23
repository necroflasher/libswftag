#include<libswftag/swftag.h>
#include<libswftag/error.h>
#include<libswftag/check_functions.h>
#include<libswftag/swfmath.h>

#define ANALYZE_DEEP 0	// Placeholder flag to set whether or not to analyze tags in depth. Anti-feature for security and thus disabled by default

// Note that currently, few of them save the data they parse somewhere. only verify it, but for the greater library
// the whole structure of the swf should be optionally accessible and editable through the tag stream, so this is something to note for the future.
// All optional ofcourse.

// Considering just making a global variable to handle temp error values on these macros but that seems contrived
#define C_RAISE_ERR(error) ER_RAISE_ERROR_ERR(handler_ret, state, error)
#define C_RAISE_ERR_PTR(pointer, error) ER_RAISE_ERROR_ERR_PTR(handler_ret, pointer, state, error)
#define C_RAISE_ERR_INT(integer, error) ER_RAISE_ERROR_ERR_INT(handler_ret, integer, state, error)

// These macros are not library features, they're here just to make implementation simpler within the check_functions
#define C_TAG_BOUNDS_EVAL(buffer, offset) if(M_BUF_BOUNDS_CHECK(buffer, offset, state))return C_RAISE_ERR(ESW_SHORTFILE);if(((uchar *)buffer + offset) > ((uchar *)(tag_data->tag_data) + tag_data->size))return C_RAISE_ERR(ESW_IMPROPER);

// Yes it's horrible, but it's less horrible than other options imho
#define C_INIT_TAG(newstruct) err_ptr tag_ret=alloc_push_freelist(state, sizeof(struct newstruct), tag_data->parent_node);if(ER_ERROR(tag_ret.ret))return tag_ret.ret;struct newstruct *tag_struct=tag_ret.pointer;tag_data->tag_struct=tag_struct

err check_invalidtag(pdata *state, swf_tag *tag_data)
{
	return push_peculiarity(state, PEC_INVAL_TAG, tag_data->tag_data - state->u_movie);
}

err check_end(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}

	if(tag_data->size)
	{
		err ret = push_peculiarity(state, PEC_TAG_EXTRA, tag_data->tag_data - state->u_movie);
		if(ER_ERROR(ret))
		{
			return ret;
		}
		C_TAG_BOUNDS_EVAL(tag_data->tag_data, tag_data->size);
	}
	return pop_scope(state);
}

err check_showframe(pdata *state, swf_tag *tag_data) //--DONE--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}

	if(tag_data->size)
	{
		err ret = push_peculiarity(state, PEC_TAG_EXTRA, tag_data->tag_data - state->u_movie);
		if(ER_ERROR(ret))
		{
			return ret;
		}
		C_TAG_BOUNDS_EVAL(tag_data->tag_data, tag_data->size);
	}
	return 0;
}

err check_defineshape(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_freecharacter(pdata *state, swf_tag *tag_data) //--DONE--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return push_peculiarity(state, PEC_MYTHICAL_TAG, tag_data->tag_data - state->u_movie);
}

err check_placeobject(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_placeobject);

	C_TAG_BOUNDS_EVAL(base, 4);

	tag_data->tag_id = geti16((uchar *)base);

	tag_struct->obj_depth = geti16((uchar *)base + 2);
	tag_struct->has_color_transform = 0;
	/*
	TODO: Checks to verify these values?
	*/
	offset += 4;
	err_int ret = swf_matrix_parse(state, &(tag_struct->matrix), base + offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;

	ret = swf_color_transform_parse(state, &(tag_struct->color_transform), base + offset, tag_data);

	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	if(ret.integer)
	{
		tag_struct->has_color_transform = 1;
	}

	offset += M_ALIGN(ret.integer, 3)>>3;
	return 0;
}

err check_removeobject(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	uchar *base = tag_data->tag_data;

	C_INIT_TAG(swf_tag_removeobject);

	C_TAG_BOUNDS_EVAL(base, 4);

	tag_data->tag_id = geti16((uchar *)base);

	tag_struct->obj_depth = geti16((uchar *)base + 2);
	/*
	TODO: Checks to verify these values?
	*/
	return 0;
}

err check_definebitsjpeg(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_definebitsx);
	tag_struct->family_version = 1;

	C_TAG_BOUNDS_EVAL(base, 2);

	tag_data->tag_id = geti16((uchar *)base);
	handler_ret = id_register(state, tag_data->tag_id, tag_data);
	if(ER_ERROR(handler_ret))
	{
		return handler_ret;
	}

	if(ANALYZE_DEEP)
	{
		/*
		TODO: The rest of the tag is the jpeg image. Analyzing that comes later
		*/
	}
	return 0;
}

err check_definebutton(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_TAG_BOUNDS_EVAL(base, 2);

	ui16 id = geti16((uchar *)base);
	offset += 2;

	// TODO: A lot, starting with defining it properly in tag_structs. The chain for it is a little big so I'm saving it for later
	return 0;
}

err check_jpegtables(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	// To make or not to make a new struct for this if all it's data is just tag_data->tag_data
	if(ANALYZE_DEEP)
	{
		/*
		TODO: Analyze the jpeg tables and bind it to the last T_DEFINEBITSJPEG tag
		*/
	}
	return 0;
}

err check_setbackgroundcolor(pdata *state, swf_tag *tag_data) //--DONE--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	C_INIT_TAG(swf_tag_setbackgroundcolor);

	C_TAG_BOUNDS_EVAL(tag_data->tag_data, 3);
	tag_struct->color.red = M_SANITIZE_BYTE(tag_data->tag_data[0]);
	tag_struct->color.green = M_SANITIZE_BYTE(tag_data->tag_data[1]);
	tag_struct->color.blue = M_SANITIZE_BYTE(tag_data->tag_data[2]);
	return 0;
}

err check_definefont(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definetext(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	/*
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;

	C_INIT_TAG(swf_tag_definetextx);
	C_TAG_BOUNDS_EVAL(base, 2);
	tag_struct->family_version = 1;

	tag_struct->id = geti16((uchar *)base);
	offset += 2;
	err_int ret = swf_rect_parse(state, &(tag_struct->rect), base + offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;

	ret = swf_matrix_parse(state, &(tag_struct->mat), base + offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;

	C_TAG_BOUNDS_EVAL(base + offset, 2);

	tag_struct->glyph_bits = M_SANITIZE_BYTE(base[offset]);
	tag_struct->advance_bits = M_SANITIZE_BYTE(base[offset+1]);
	offset += 2;

	ret = swf_text_record_parse(state, &(tag_struct->record), base + offset, tag_data);
	if(ER_ERROR(ret.ret))
	{
		return ret.ret;
	}
	offset += M_ALIGN(ret.integer, 3)>>3;
	*/
	return 0;
}

err check_doaction(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	C_INIT_TAG(swf_tag_avm1action);
	tag_struct->initaction = 0;

	if(ANALYZE_DEEP)
	{
		/* TODO: Only for analyzing if all actions are valid for the given version */
	}
	return 0;
}

err check_definefontinfo(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	uchar *base = tag_data->tag_data;
	ui32 offset = 0;
	C_INIT_TAG(swf_tag_fontinfox);
	tag_struct->family_version = 1;

	C_TAG_BOUNDS_EVAL(base, 3);
	offset += 3;
	tag_struct->define_font_id = geti16((uchar *)base);

	/* Uncomment after define font is properly implemented

	err_ptr ret_tag = id_get_tag(state, tag_struct->define_font_id);
	if(ER_ERROR(ret_tag.ret))
	{
		return ret_tag.ret;
	}
	tag_struct->font_tag = (swf_tag *)ret_tag.pointer;
	if(tag_struct->font_tag->tag != T_DEFINEFONT)
	{
		C_RAISE_ERR(ESW_IMPROPER);
	}

	*/

	tag_struct->name_length = M_SANITIZE_BYTE(base[2]);

	C_TAG_BOUNDS_EVAL(base+offset, M_SANITIZE_BYTE(base[2]));
	tag_struct->name = base+offset;
	offset += M_SANITIZE_BYTE(base[2]);

	tag_struct->bitfields = base[offset];

	tag_struct->font_info_map = base + offset + 1;
	// TODO: Find and connect tag_struct->font_tag to the tag with id tag_struct->define_font_id, and do a bounds check with the "((tag_struct->bitfields & 1) + 1)" times "glyphs_count" value of that on tag_struct->font_info_map. Then check if that same size exceeds the tag_size and if so, push a peculiarity

	return 0;
}

err check_definesound(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_startsound(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebuttonsound(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_soundstreamhead(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_soundstreamblock(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebitslossless(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebitsjpeg2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_defineshape2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebuttoncxform(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_protect(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_pathsarepostscript(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_placeobject2(pdata *state, swf_tag *tag_data) //--TODO: STARTED, BUT NOT FINISHED--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	uchar *base = tag_data->tag_data;
	ui16 flags = 0;
	ui32 offset = 0;
	if(state->version >= 8)
	{
		if(M_BUF_BOUNDS_CHECK(base, 2, state))
		{
			C_RAISE_ERR(ESW_SHORTFILE);
		}
		flags = geti32((uchar *)base);
	}

	return 0;
}

err check_removeobject2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_syncframe(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_freeall(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_defineshape3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definetext2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebutton2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebitsjpeg3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebitslossless2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_defineedittext(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definevideo(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definesprite(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_namecharacter(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_productinfo(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definetextformat(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_framelabel(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_soundstreamhead2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definemorphshape(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_generateframe(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definefont2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_generatorcommand(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definecommandobject(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_characterset(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_externalfont(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_export(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_import(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_enabledebugger(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_doinitaction(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definevideostream(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_videoframe(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definefontinfo2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_debugid(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_enabledebugger2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_scriptlimits(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_settabindex(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_fileattributes(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_placeobject3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_import2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_doabcdefine(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definefontalignzones(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_csmtextsettings(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definefont3(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_symbolclass(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_metadata(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definescalinggrid(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_doabc(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_defineshape4(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definemorphshape2(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definesceneandframedata(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebinarydata(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definefontname(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definebitsjpeg4(pdata *state, swf_tag *tag_data) //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_definefont4(pdata *state, swf_tag *tag_data)  //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

err check_enabletelemetry(pdata *state, swf_tag *tag_data)  //--TODO: NOT STARTED YET--//
{
	err handler_ret;
	if(!tag_data || !state)
	{
		C_RAISE_ERR(EFN_ARGS);
	}
	return 0;
}

#undef C_INIT_TAG
#undef C_TAG_BOUNDS_EVAL

#undef C_RAISE_ERR_INT
#undef C_RAISE_ERR_PTR
#undef C_RAISE_ERR

