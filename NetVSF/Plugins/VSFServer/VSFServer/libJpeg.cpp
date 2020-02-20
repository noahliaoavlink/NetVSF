#include "stdafx.h"
#include "libJpeg.h"

#include "ImageFile.h"
#include "Splitter.h"

#ifdef _WIN64
#pragma comment(lib, "..\\..\\..\\lib\\jpegsr9c\\jpeg-9c\\x64\\Release\\jpeg.lib")
#else
#pragma comment(lib, "..\\..\\..\\lib\\jpegsr9c\\jpeg-9c\\Release\\jpeg.lib")
#endif

struct my_error_mgr {
	struct jpeg_error_mgr pub;	/* "public" fields */

	jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

/*
 * Here's the routine that will replace the standard error_exit method:
 */

METHODDEF(void)
my_error_exit(j_common_ptr cinfo)
{
	/* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
	my_error_ptr myerr = (my_error_ptr)cinfo->err;

	/* Always display the message. */
	/* We could postpone this until after returning, if we chose. */
	(*cinfo->err->output_message) (cinfo);

	/* Return control to the setjmp point */
	longjmp(myerr->setjmp_buffer, 1);
}

libJpeg::libJpeg()
{
}

libJpeg::~libJpeg()
{
}

int libJpeg::Encode(unsigned char* pInBuf, unsigned char** pOutBuf, int iWidth, int iHeight, int iQuality)
{
	struct jpeg_compress_struct cinfo;

	//unsigned char *mem = NULL;
	unsigned long mem_size = 0;
	struct jpeg_error_mgr jerr;
	/* More stuff */
	FILE * outfile;		/* target file */
	int row_stride;		/* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);
	jpeg_mem_dest(&cinfo, pOutBuf, &mem_size);

	cinfo.image_width = iWidth; 	/* image width and height, in pixels */
	cinfo.image_height = iHeight;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, iQuality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = iWidth * 3;	/* JSAMPLEs per row in image_buffer */

	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	while (cinfo.next_scanline < cinfo.image_height)
	{
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could pass
			* more than one scanline at a time if that's more convenient.
			*/
		row_pointer[0] = &pInBuf[cinfo.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
//	fclose(outfile);
	jpeg_destroy_compress(&cinfo);

	return mem_size;
}

int libJpeg::Decode(unsigned char* pInBuf, unsigned char* pOutBuf,int iInLen, int* w, int* h, int* iOutLen)
{
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	unsigned long mem_size = 0;
	struct my_error_mgr jerr;
	/* More stuff */
//	FILE * infile;		/* source file */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	/* In this example we want to open the input file before doing anything else,
	 * so that the setjmp() error recovery below can assume the file is open.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to read binary files.
	 */
/*
	if ((infile = fopen(szInFileName, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", szInFileName);
		return 0;
	}
	*/

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
//		fclose(infile);
		return 0;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	//jpeg_stdio_src(&cinfo, infile);
	jpeg_mem_src(&cinfo, pInBuf, iInLen);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void)jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.txt for more info.
	 */

	 /* Step 4: set parameters for decompression */

	 /* In this example, we don't need to change any of the defaults set by
	  * jpeg_read_header(), so we do nothing here.
	  */

	  /* Step 5: Start decompressor */

	(void)jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	 /* We may need to do some setup of our own at this point before reading
	  * the data.  After jpeg_start_decompress() we have the correct scaled
	  * output image dimensions available, as well as the output colormap
	  * if we asked for color quantization.
	  * In this example, we need to make an output work buffer of the right size.
	  */
	  /* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	int iCount = 0;
	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */
	//    put_scanline_someplace(buffer[0], row_stride);
		memcpy(pOutBuf + iCount, buffer[0], row_stride);
		iCount += row_stride;
	}

	*w = cinfo.output_width;
	*h = cinfo.output_height;
	*iOutLen = cinfo.output_components;

	/* Step 7: Finish decompression */

	(void)jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	 /* Step 8: Release JPEG decompression object */

	 /* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit, but why assume anything...)
	 */
//	fclose(infile);

	return 1;
}

int libJpeg::WriteJPEGFile(char* szOutFileName, int iQuality, unsigned char* pInputBuf, int iWidth, int iHeight)
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;
	/* More stuff */
	FILE * outfile;		/* target file */
	int row_stride;		/* physical row width in image buffer */

	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

	if ((outfile = fopen(szOutFileName, "wb")) == NULL)
	{
		//fprintf(stderr, "can't open %s\n", filename);
		//exit(1);
		return -1;
	}

	jpeg_stdio_dest(&cinfo, outfile);

	cinfo.image_width = iWidth; 	/* image width and height, in pixels */
	cinfo.image_height = iHeight;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);
	jpeg_set_quality(&cinfo, iQuality, TRUE /* limit to baseline-JPEG values */);
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = iWidth * 3;	/* JSAMPLEs per row in image_buffer */

	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */
	while (cinfo.next_scanline < cinfo.image_height)
	{
		/* jpeg_write_scanlines expects an array of pointers to scanlines.
			* Here the array is only one element long, but you could pass
			* more than one scanline at a time if that's more convenient.
			*/
		row_pointer[0] = &pInputBuf[cinfo.next_scanline * row_stride];
		(void)jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}

	jpeg_finish_compress(&cinfo);
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);

	return 0;
}

int libJpeg::ReadJPEGFile(char* szInFileName, unsigned char* pOutBuf, int* w, int* h, int* iBytes)
{
	/* This struct contains the JPEG decompression parameters and pointers to
   * working space (which is allocated as needed by the JPEG library).
   */
	struct jpeg_decompress_struct cinfo;
	/* We use our private extension JPEG error handler.
	 * Note that this struct must live as long as the main JPEG parameter
	 * struct, to avoid dangling-pointer problems.
	 */
	struct my_error_mgr jerr;
	/* More stuff */
	FILE * infile;		/* source file */
	JSAMPARRAY buffer;		/* Output row buffer */
	int row_stride;		/* physical row width in output buffer */

	/* In this example we want to open the input file before doing anything else,
	 * so that the setjmp() error recovery below can assume the file is open.
	 * VERY IMPORTANT: use "b" option to fopen() if you are on a machine that
	 * requires it in order to read binary files.
	 */

	if ((infile = fopen(szInFileName, "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", szInFileName);
		return 0;
	}

	/* Step 1: allocate and initialize JPEG decompression object */

	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = my_error_exit;
	/* Establish the setjmp return context for my_error_exit to use. */
	if (setjmp(jerr.setjmp_buffer)) {
		/* If we get here, the JPEG code has signaled an error.
		 * We need to clean up the JPEG object, close the input file, and return.
		 */
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return 0;
	}
	/* Now we can initialize the JPEG decompression object. */
	jpeg_create_decompress(&cinfo);

	/* Step 2: specify data source (eg, a file) */

	jpeg_stdio_src(&cinfo, infile);

	/* Step 3: read file parameters with jpeg_read_header() */

	(void)jpeg_read_header(&cinfo, TRUE);
	/* We can ignore the return value from jpeg_read_header since
	 *   (a) suspension is not possible with the stdio data source, and
	 *   (b) we passed TRUE to reject a tables-only JPEG file as an error.
	 * See libjpeg.txt for more info.
	 */

	 /* Step 4: set parameters for decompression */

	 /* In this example, we don't need to change any of the defaults set by
	  * jpeg_read_header(), so we do nothing here.
	  */

	  /* Step 5: Start decompressor */

	(void)jpeg_start_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	 /* We may need to do some setup of our own at this point before reading
	  * the data.  After jpeg_start_decompress() we have the correct scaled
	  * output image dimensions available, as well as the output colormap
	  * if we asked for color quantization.
	  * In this example, we need to make an output work buffer of the right size.
	  */
	  /* JSAMPLEs per row in output buffer */
	row_stride = cinfo.output_width * cinfo.output_components;
	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)
		((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

	int iCount = 0;
	/* Step 6: while (scan lines remain to be read) */
	/*           jpeg_read_scanlines(...); */

	/* Here we use the library's state variable cinfo.output_scanline as the
	 * loop counter, so that we don't have to keep track ourselves.
	 */
	while (cinfo.output_scanline < cinfo.output_height) {
		/* jpeg_read_scanlines expects an array of pointers to scanlines.
		 * Here the array is only one element long, but you could ask for
		 * more than one scanline at a time if that's more convenient.
		 */
		(void)jpeg_read_scanlines(&cinfo, buffer, 1);
		/* Assume put_scanline_someplace wants a pointer and sample count. */
	//    put_scanline_someplace(buffer[0], row_stride);
		memcpy(pOutBuf + iCount, buffer[0], row_stride);
		iCount += row_stride;
	}

	*w = cinfo.output_width;
	*h = cinfo.output_height;
	*iBytes = cinfo.output_components;

	/* Step 7: Finish decompression */

	(void)jpeg_finish_decompress(&cinfo);
	/* We can ignore the return value since suspension is not possible
	 * with the stdio data source.
	 */

	 /* Step 8: Release JPEG decompression object */

	 /* This is an important step since it will release a good deal of memory. */
	jpeg_destroy_decompress(&cinfo);

	/* After finish_decompress, we can close the input file.
	 * Here we postpone it until after no more JPEG errors are possible,
	 * so as to simplify the setjmp error logic above.  (Actually, I don't
	 * think that jpeg_destroy can do an error exit, but why assume anything...)
	 */
	fclose(infile);

	return 1;
}

void libJpeg::Test()
{
	ImageFile img_file;
	Splitter splitter;
	unsigned long ulCurPos = 0;

	img_file.Load("D:\\temp\\20170527061543_29.bmp");
	img_file.GetWidth();
	img_file.GetHeight();
	img_file.GetBuffer();

	char szFileName[512] = "D:\\temp\\test_libjpeg.jpg";
	int iQuality = 90;

	img_file.RGB2BGR(img_file.GetBuffer(), img_file.GetWidth(), img_file.GetHeight());

#if 0
	int iRet = WriteJPEGFile(szFileName, iQuality, img_file.GetBuffer(), img_file.GetWidth(), img_file.GetHeight());
#else
	int iOutW;
	int iOutH;
	int iOutLen;
	int iBufLen = 2992 * 2000 * 3;

	unsigned char* pEnBuf = new unsigned char[iBufLen];
	unsigned char* pEnBuf2 = new unsigned char[iBufLen];
	unsigned char* pDeBuf = new unsigned char[iBufLen];

	int iRet = Encode(img_file.GetBuffer(), &pEnBuf, img_file.GetWidth(), img_file.GetHeight(), iQuality);

	if (iRet > 0)
	{
		FILE * outfile;		/* target file */
		
		if ((outfile = fopen(szFileName, "wb")) == NULL)
		{
			//fprintf(stderr, "can't open %s\n", filename);
			//exit(1);
			return ;
		}

		memset(pEnBuf2,0, iBufLen);
		splitter.DoSplit(iRet);

		for (int i = 0;i < splitter.GetTotal();i++)
		{
			UnitPositionInfo* pCurItem = splitter.GetItemInfo(i);
			if (pCurItem)
			{
				memcpy(pEnBuf2 + ulCurPos, pEnBuf + pCurItem->ulStart, pCurItem->ulEnd - pCurItem->ulStart);

				ulCurPos += (pCurItem->ulEnd - pCurItem->ulStart);
			}
		}

		//fwrite(pEnBuf, 1,iRet, outfile);
		fwrite(pEnBuf2, 1, iRet, outfile);
		fclose(outfile);
	}

	int iDeRet = Decode(pEnBuf, pDeBuf, iBufLen, &iOutW, &iOutH, &iOutLen);
	if (iDeRet == 1)
		img_file.SaveBmpFile("D:\\temp\\test_libjpeg.bmp", pDeBuf, iOutW, iOutH);

	if(pEnBuf)
		delete pEnBuf;

	if (pEnBuf2)
		delete pEnBuf2;

	if (pDeBuf)
		delete pDeBuf;
#endif
}