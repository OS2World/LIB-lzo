
#define LZO_NEED_DICT_H
#include "config1b.h"


#if !defined(COMPRESS_ID)
#define COMPRESS_ID		_LZO_ECONCAT2(DD_BITS,CLEVEL)
#endif


#include "lzo1b_c.ch"


/***********************************************************************
//
************************************************************************/

#define LZO_COMPRESS \
	_LZO_ECONCAT3(lzo1b_,COMPRESS_ID,_compress)

#define LZO_COMPRESS_FUNC \
	_LZO_ECONCAT3(_lzo1b_,COMPRESS_ID,_compress_func)



/***********************************************************************
//
************************************************************************/

const lzo_compress_t LZO_COMPRESS_FUNC = do_compress;

LZO_PUBLIC(int)
LZO_COMPRESS ( const lzo_byte *src, lzo_uint  src_len,
                     lzo_byte *dst, lzo_uint *dst_len,
                     lzo_voidp wrkmem )
{
	return _lzo1b_do_compress(src,src_len,dst,dst_len,wrkmem,do_compress);
}

/*
vi:ts=4
*/
