//#define LOG_NDEBUG 0
#define LOG_TAG "RotateDemo"
#include "Log.h"

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <arm_neon.h>
#include <RotateDemo.h>
namespace android {
void rotate(uint8_t* dst, const uint8_t* src, size_t width, size_t height, int degree)
{
	size_t i,j;
	if(degree == 90) {
	    for (i = 0; i < height; ++i) {
	        for (j = 0; j < width; ++j) {
	            dst[j * height + height - 1 - i] = src[i * width + j];
	        }
	    }
	} else if(degree == 180) {
	    for (i = 0; i < height; ++i) {
	        for (j = 0; j < width; ++j) {
	            dst[(height - 1 - i) * width + width - 1 - j] = src[i * width + j];
	        }
	    }
	} else if(degree == 270) {
		for (i = 0; i < height; ++i) {
			for (j = 0; j < width; ++j) {
				dst[(width - 1 - j) * height + i] = src[i * width + j];
			}
		}
	}
}

//#define CLOCKWISE
void rotateFast(uint8_t* dst, const uint8_t* src, size_t width, size_t height, int degree)
{
	size_t i,j;

	if((width & 0x7) || (height & 0x7))
	{
		if(degree == 90) {
		    for (i = 0; i < height; ++i) {
	        	for (j = 0; j < width; ++j) {
	            	dst[j * height + height - 1 - i] = src[i * width + j];
	        	}
	    	}
		} else if(degree == 180) {
		    for (i = 0; i < height; ++i) {
	        	for (j = 0; j < width; ++j) {
	            	dst[(height - 1 - i) * width + width - 1 - j] = src[i * width + j];
	        	}
	    	}
		} else if(degree == 270) {
			for (i = 0; i < height; ++i) {
				for (j = 0; j < width; ++j) {
					dst[(width - 1 - j) * height + i] = src[i * width + j];
				}
			}
		}
	}
	else
    {
    	size_t heightMul8 = height*8;
		if(degree == 90) {
			const uint8_t *pSrc;
			uint8_t *pDst0, *pDst1, *pDst2, *pDst3, *pDst4, *pDst5, *pDst6, *pDst7;
		    for (i = 0; i < height; i+=8) {
		    	pSrc = src + i * width;
	 			pDst0 = dst + height - 8 - i;
	 			pDst1 = pDst0 + height;
	 			pDst2 = pDst1 + height;
	 			pDst3 = pDst2 + height;
	 			pDst4 = pDst3 + height;
	 			pDst5 = pDst4 + height;
	 			pDst6 = pDst5 + height;
	 			pDst7 = pDst6 + height;
	        	for (j = 0; j < width; j+=8) {
	        		uint8x8x2_t mat1, mat2, mat3, mat4;
	        		uint8x8x2_t temp1, temp2, temp3, temp4;
	        		uint16x4x2_t temp5, temp6, temp7, temp8;
	        		uint16x4x2_t temp9, temp10, temp11, temp12;
	        		uint32x2x2_t temp13, temp14, temp15, temp16;
	        		uint32x2x2_t temp17, temp18, temp19, temp20;

	        		mat1.val[0] = vld1_u8(pSrc);
		            mat1.val[1] = vld1_u8(pSrc+width);
		            mat2.val[0] = vld1_u8(pSrc+2*width);
		            mat2.val[1] = vld1_u8(pSrc+3*width);
		            mat3.val[0] = vld1_u8(pSrc+4*width);
		            mat3.val[1] = vld1_u8(pSrc+5*width);
		            mat4.val[0] = vld1_u8(pSrc+6*width);
		            mat4.val[1] = vld1_u8(pSrc+7*width);

		            pSrc += 8;
#ifdef CLOCKWISE
					temp1 = vtrn_u8(mat1.val[0],mat1.val[1]);
		            temp2 = vtrn_u8(mat2.val[0],mat2.val[1]);
		            temp3 = vtrn_u8(mat3.val[0],mat3.val[1]);
		            temp4 = vtrn_u8(mat4.val[0],mat4.val[1]);

		            temp5.val[0] = vreinterpret_u16_u8(temp1.val[0]);
		            temp5.val[1] = vreinterpret_u16_u8(temp1.val[1]);
		            temp6.val[0] = vreinterpret_u16_u8(temp2.val[0]);
		            temp6.val[1] = vreinterpret_u16_u8(temp2.val[1]);
		            temp7.val[0] = vreinterpret_u16_u8(temp3.val[0]);
		            temp7.val[1] = vreinterpret_u16_u8(temp3.val[1]);
		            temp8.val[0] = vreinterpret_u16_u8(temp4.val[0]);
		            temp8.val[1] = vreinterpret_u16_u8(temp4.val[1]);

		            temp9 = vtrn_u16(temp5.val[0],temp6.val[0]);
		            temp10 = vtrn_u16(temp5.val[1],temp6.val[1]);
		            temp11 = vtrn_u16(temp7.val[0],temp8.val[0]);
		            temp12 = vtrn_u16(temp7.val[1],temp8.val[1]);

		            temp13.val[0] = vreinterpret_u32_u16(temp9.val[0]);
		            temp13.val[1] = vreinterpret_u32_u16(temp9.val[1]);
		            temp14.val[0] = vreinterpret_u32_u16(temp10.val[0]);
		            temp14.val[1] = vreinterpret_u32_u16(temp10.val[1]);
		            temp15.val[0] = vreinterpret_u32_u16(temp11.val[0]);
		            temp15.val[1] = vreinterpret_u32_u16(temp11.val[1]);
		            temp16.val[0] = vreinterpret_u32_u16(temp12.val[0]);
		            temp16.val[1] = vreinterpret_u32_u16(temp12.val[1]);

		            temp17 = vtrn_u32(temp13.val[0],temp15.val[0]);
		            temp18 = vtrn_u32(temp13.val[1],temp15.val[1]);
		            temp19 = vtrn_u32(temp14.val[0],temp16.val[0]);
		            temp20 = vtrn_u32(temp14.val[1],temp16.val[1]);
#else
					temp1 = vtrn_u8(mat1.val[1],mat1.val[0]);
		            temp2 = vtrn_u8(mat2.val[1],mat2.val[0]);
		            temp3 = vtrn_u8(mat3.val[1],mat3.val[0]);
		            temp4 = vtrn_u8(mat4.val[1],mat4.val[0]);

		            temp5.val[0] = vreinterpret_u16_u8(temp1.val[0]);
		            temp5.val[1] = vreinterpret_u16_u8(temp1.val[1]);
		            temp6.val[0] = vreinterpret_u16_u8(temp2.val[0]);
		            temp6.val[1] = vreinterpret_u16_u8(temp2.val[1]);
		            temp7.val[0] = vreinterpret_u16_u8(temp3.val[0]);
		            temp7.val[1] = vreinterpret_u16_u8(temp3.val[1]);
		            temp8.val[0] = vreinterpret_u16_u8(temp4.val[0]);
		            temp8.val[1] = vreinterpret_u16_u8(temp4.val[1]);

		            temp9 = vtrn_u16(temp6.val[0],temp5.val[0]);
		            temp10 = vtrn_u16(temp6.val[1],temp5.val[1]);
		            temp11 = vtrn_u16(temp8.val[0],temp7.val[0]);
		            temp12 = vtrn_u16(temp8.val[1],temp7.val[1]);

		            temp13.val[0] = vreinterpret_u32_u16(temp9.val[0]);
		            temp13.val[1] = vreinterpret_u32_u16(temp9.val[1]);
		            temp14.val[0] = vreinterpret_u32_u16(temp10.val[0]);
		            temp14.val[1] = vreinterpret_u32_u16(temp10.val[1]);
		            temp15.val[0] = vreinterpret_u32_u16(temp11.val[0]);
		            temp15.val[1] = vreinterpret_u32_u16(temp11.val[1]);
		            temp16.val[0] = vreinterpret_u32_u16(temp12.val[0]);
		            temp16.val[1] = vreinterpret_u32_u16(temp12.val[1]);

		            temp17 = vtrn_u32(temp15.val[0],temp13.val[0]);
		            temp18 = vtrn_u32(temp15.val[1],temp13.val[1]);
		            temp19 = vtrn_u32(temp16.val[0],temp14.val[0]);
		            temp20 = vtrn_u32(temp16.val[1],temp14.val[1]);
#endif

		            temp1.val[0] = vreinterpret_u8_u32(temp17.val[0]);
		            temp1.val[1] = vreinterpret_u8_u32(temp19.val[0]);
		            temp2.val[0] = vreinterpret_u8_u32(temp18.val[0]);
		            temp2.val[1] = vreinterpret_u8_u32(temp20.val[0]);
		            temp3.val[0] = vreinterpret_u8_u32(temp17.val[1]);
		            temp3.val[1] = vreinterpret_u8_u32(temp19.val[1]);
		            temp4.val[0] = vreinterpret_u8_u32(temp18.val[1]);
		            temp4.val[1] = vreinterpret_u8_u32(temp20.val[1]);

		            vst1_u8(pDst0, temp1.val[0]);
		            vst1_u8(pDst1, temp1.val[1]);
		            vst1_u8(pDst2, temp2.val[0]);
		            vst1_u8(pDst3, temp2.val[1]);
		            vst1_u8(pDst4, temp3.val[0]);
		            vst1_u8(pDst5, temp3.val[1]);
		            vst1_u8(pDst6, temp4.val[0]);
		            vst1_u8(pDst7, temp4.val[1]);

	        		pDst0 += heightMul8;
	        		pDst1 += heightMul8;
	        		pDst2 += heightMul8;
	        		pDst3 += heightMul8;
	        		pDst4 += heightMul8;
	        		pDst5 += heightMul8;
	        		pDst6 += heightMul8;
	        		pDst7 += heightMul8;
	        	}
	    	}
		} else if(degree == 180) {
			const uint8_t *pSrc = src;
			uint8_t *pDst = dst + height * width - 8;
	    	for (i = 0; i < height; ++i) {
	        	for (j = 0; j < width; j+=8) {
	        		uint8x8_t temp;
	        		temp = vld1_u8(pSrc);
	        		temp = vrev64_u8(temp);
	        		vst1_u8(pDst, temp);
	        		pSrc += 8;
	        		pDst -= 8;
	        	}
	    	}
		} else if(degree == 270) {
			const uint8_t *pSrc;
			uint8_t *pDst0, *pDst1, *pDst2, *pDst3, *pDst4, *pDst5, *pDst6, *pDst7;
			for (i = 0; i < height; i+=8) {
				pSrc = src + i * width;
	 			pDst0 = dst + (width - 1) * height + i;
	 			pDst1 = pDst0 - height;
	 			pDst2 = pDst1 - height;
	 			pDst3 = pDst2 - height;
	 			pDst4 = pDst3 - height;
	 			pDst5 = pDst4 - height;
	 			pDst6 = pDst5 - height;
	 			pDst7 = pDst6 - height;
				for (j = 0; j < width; j+=8) {
					uint8x8x2_t mat1, mat2, mat3, mat4;
	        		uint8x8x2_t temp1, temp2, temp3, temp4;
	        		uint16x4x2_t temp5, temp6, temp7, temp8;
	        		uint16x4x2_t temp9, temp10, temp11, temp12;
	        		uint32x2x2_t temp13, temp14, temp15, temp16;
	        		uint32x2x2_t temp17, temp18, temp19, temp20;

	        		mat1.val[0] = vld1_u8(pSrc);
		            mat1.val[1] = vld1_u8(pSrc+width);
		            mat2.val[0] = vld1_u8(pSrc+2*width);
		            mat2.val[1] = vld1_u8(pSrc+3*width);
		            mat3.val[0] = vld1_u8(pSrc+4*width);
		            mat3.val[1] = vld1_u8(pSrc+5*width);
		            mat4.val[0] = vld1_u8(pSrc+6*width);
		            mat4.val[1] = vld1_u8(pSrc+7*width);

		            pSrc += 8;
#ifdef CLOCKWISE
		            temp1 = vtrn_u8(mat1.val[1],mat1.val[0]);
		            temp2 = vtrn_u8(mat2.val[1],mat2.val[0]);
		            temp3 = vtrn_u8(mat3.val[1],mat3.val[0]);
		            temp4 = vtrn_u8(mat4.val[1],mat4.val[0]);

		            temp5.val[0] = vreinterpret_u16_u8(temp1.val[0]);
		            temp5.val[1] = vreinterpret_u16_u8(temp1.val[1]);
		            temp6.val[0] = vreinterpret_u16_u8(temp2.val[0]);
		            temp6.val[1] = vreinterpret_u16_u8(temp2.val[1]);
		            temp7.val[0] = vreinterpret_u16_u8(temp3.val[0]);
		            temp7.val[1] = vreinterpret_u16_u8(temp3.val[1]);
		            temp8.val[0] = vreinterpret_u16_u8(temp4.val[0]);
		            temp8.val[1] = vreinterpret_u16_u8(temp4.val[1]);

		            temp9 = vtrn_u16(temp6.val[0],temp5.val[0]);
		            temp10 = vtrn_u16(temp6.val[1],temp5.val[1]);
		            temp11 = vtrn_u16(temp8.val[0],temp7.val[0]);
		            temp12 = vtrn_u16(temp8.val[1],temp7.val[1]);

		            temp13.val[0] = vreinterpret_u32_u16(temp9.val[0]);
		            temp13.val[1] = vreinterpret_u32_u16(temp9.val[1]);
		            temp14.val[0] = vreinterpret_u32_u16(temp10.val[0]);
		            temp14.val[1] = vreinterpret_u32_u16(temp10.val[1]);
		            temp15.val[0] = vreinterpret_u32_u16(temp11.val[0]);
		            temp15.val[1] = vreinterpret_u32_u16(temp11.val[1]);
		            temp16.val[0] = vreinterpret_u32_u16(temp12.val[0]);
		            temp16.val[1] = vreinterpret_u32_u16(temp12.val[1]);

		            temp17 = vtrn_u32(temp15.val[0],temp13.val[0]);
		            temp18 = vtrn_u32(temp15.val[1],temp13.val[1]);
		            temp19 = vtrn_u32(temp16.val[0],temp14.val[0]);
		            temp20 = vtrn_u32(temp16.val[1],temp14.val[1]);
#else

		            temp1 = vtrn_u8(mat1.val[0],mat1.val[1]);
		            temp2 = vtrn_u8(mat2.val[0],mat2.val[1]);
		            temp3 = vtrn_u8(mat3.val[0],mat3.val[1]);
		            temp4 = vtrn_u8(mat4.val[0],mat4.val[1]);

		            temp5.val[0] = vreinterpret_u16_u8(temp1.val[0]);
		            temp5.val[1] = vreinterpret_u16_u8(temp1.val[1]);
		            temp6.val[0] = vreinterpret_u16_u8(temp2.val[0]);
		            temp6.val[1] = vreinterpret_u16_u8(temp2.val[1]);
		            temp7.val[0] = vreinterpret_u16_u8(temp3.val[0]);
		            temp7.val[1] = vreinterpret_u16_u8(temp3.val[1]);
		            temp8.val[0] = vreinterpret_u16_u8(temp4.val[0]);
		            temp8.val[1] = vreinterpret_u16_u8(temp4.val[1]);

		            temp9 = vtrn_u16(temp5.val[0],temp6.val[0]);
		            temp10 = vtrn_u16(temp5.val[1],temp6.val[1]);
		            temp11 = vtrn_u16(temp7.val[0],temp8.val[0]);
		            temp12 = vtrn_u16(temp7.val[1],temp8.val[1]);

		            temp13.val[0] = vreinterpret_u32_u16(temp9.val[0]);
		            temp13.val[1] = vreinterpret_u32_u16(temp9.val[1]);
		            temp14.val[0] = vreinterpret_u32_u16(temp10.val[0]);
		            temp14.val[1] = vreinterpret_u32_u16(temp10.val[1]);
		            temp15.val[0] = vreinterpret_u32_u16(temp11.val[0]);
		            temp15.val[1] = vreinterpret_u32_u16(temp11.val[1]);
		            temp16.val[0] = vreinterpret_u32_u16(temp12.val[0]);
		            temp16.val[1] = vreinterpret_u32_u16(temp12.val[1]);

		            temp17 = vtrn_u32(temp13.val[0],temp15.val[0]);
		            temp18 = vtrn_u32(temp13.val[1],temp15.val[1]);
		            temp19 = vtrn_u32(temp14.val[0],temp16.val[0]);
		            temp20 = vtrn_u32(temp14.val[1],temp16.val[1]);
#endif

		            temp1.val[0] = vreinterpret_u8_u32(temp17.val[0]);
		            temp1.val[1] = vreinterpret_u8_u32(temp19.val[0]);
		            temp2.val[0] = vreinterpret_u8_u32(temp18.val[0]);
		            temp2.val[1] = vreinterpret_u8_u32(temp20.val[0]);
		            temp3.val[0] = vreinterpret_u8_u32(temp17.val[1]);
		            temp3.val[1] = vreinterpret_u8_u32(temp19.val[1]);
		            temp4.val[0] = vreinterpret_u8_u32(temp18.val[1]);
		            temp4.val[1] = vreinterpret_u8_u32(temp20.val[1]);

		            vst1_u8(pDst0, temp1.val[0]);
		            vst1_u8(pDst1, temp1.val[1]);
		            vst1_u8(pDst2, temp2.val[0]);
		            vst1_u8(pDst3, temp2.val[1]);
		            vst1_u8(pDst4, temp3.val[0]);
		            vst1_u8(pDst5, temp3.val[1]);
		            vst1_u8(pDst6, temp4.val[0]);
		            vst1_u8(pDst7, temp4.val[1]);

	        		pDst0 -= heightMul8;
	        		pDst1 -= heightMul8;
	        		pDst2 -= heightMul8;
	        		pDst3 -= heightMul8;
	        		pDst4 -= heightMul8;
	        		pDst5 -= heightMul8;
	        		pDst6 -= heightMul8;
	        		pDst7 -= heightMul8;
				}
			}
		}
	}
}

void rotate_u16(uint16_t* dst, const uint16_t* src, size_t width, size_t height, int degree)
{
	//for semi-plannar, rotate UV together.
	size_t i,j;
	if(degree == 90) {
	    for (i = 0; i < height; ++i) {
	        for (j = 0; j < width; ++j) {
	            dst[j * height + height - 1 - i] = src[i * width + j];
	        }
	    }
	} else if(degree == 180) {
	    for (i = 0; i < height; ++i) {
	        for (j = 0; j < width; ++j) {
	            dst[(height - 1 - i) * width + width - 1 - j] = src[i * width + j];
	        }
	    }
	} else if(degree == 270) {
		for (i = 0; i < height; ++i) {
			for (j = 0; j < width; ++j) {
				dst[(width - 1 - j) * height + i] = src[i * width + j];
			}
		}
	}
}

void rotateFast_u16(uint16_t* dst, const uint16_t* src, size_t width, size_t height, int degree)
{
	//for semi-plannar, rotate UV together.
	size_t i,j;

	if((width & 0x3) || (height & 0x3))
	{
		if(degree == 90) {
		    for (i = 0; i < height; ++i) {
		        for (j = 0; j < width; ++j) {
		            dst[j * height + height - 1 - i] = src[i * width + j];
		        }
		    }
		} else if(degree == 180) {
		    for (i = 0; i < height; ++i) {
		        for (j = 0; j < width; ++j) {
		            dst[(height - 1 - i) * width + width - 1 - j] = src[i * width + j];
		        }
		    }
		} else if(degree == 270) {
			for (i = 0; i < height; ++i) {
				for (j = 0; j < width; ++j) {
					dst[(width - 1 - j) * height + i] = src[i * width + j];
				}
			}
		}
	}
	else
	{
		size_t heightMul4 = height*4;
		if(degree == 90) {
			const uint16_t *pSrc;
			uint16_t *pDst0, *pDst1, *pDst2, *pDst3;
		    for (i = 0; i < height; i+=4) {
		    	pSrc = src + i * width;
	 			pDst0 = dst + height - 4 - i;
	 			pDst1 = pDst0 + height;
	 			pDst2 = pDst1 + height;
	 			pDst3 = pDst2 + height;
		        for (j = 0; j < width; j+=4) {
	        		uint16x4x2_t temp1, temp2, temp3, temp4;
	        		uint32x2x2_t temp5, temp6, temp7, temp8;

	        		temp1.val[0] = vld1_u16(pSrc);
		            temp1.val[1] = vld1_u16(pSrc+width);
		            temp2.val[0] = vld1_u16(pSrc+2*width);
		            temp2.val[1] = vld1_u16(pSrc+3*width);

		            pSrc += 4;

#ifdef CLOCKWISE
					temp3 = vtrn_u16(temp1.val[0],temp1.val[1]);
		            temp4 = vtrn_u16(temp2.val[0],temp2.val[1]);

		            temp5.val[0] = vreinterpret_u32_u16(temp3.val[0]);
		            temp5.val[1] = vreinterpret_u32_u16(temp3.val[1]);
		            temp6.val[0] = vreinterpret_u32_u16(temp4.val[0]);
		            temp6.val[1] = vreinterpret_u32_u16(temp4.val[1]);

		            temp7 = vtrn_u32(temp5.val[0],temp6.val[0]);
		            temp8 = vtrn_u32(temp5.val[1],temp6.val[1]);
#else
					temp3 = vtrn_u16(temp1.val[1],temp1.val[0]);
		            temp4 = vtrn_u16(temp2.val[1],temp2.val[0]);

		            temp5.val[0] = vreinterpret_u32_u16(temp3.val[0]);
		            temp5.val[1] = vreinterpret_u32_u16(temp3.val[1]);
		            temp6.val[0] = vreinterpret_u32_u16(temp4.val[0]);
		            temp6.val[1] = vreinterpret_u32_u16(temp4.val[1]);

		            temp7 = vtrn_u32(temp6.val[0],temp5.val[0]);
		            temp8 = vtrn_u32(temp6.val[1],temp5.val[1]);
#endif

		            temp1.val[1] = vreinterpret_u16_u32(temp7.val[0]);
		            temp1.val[0] = vreinterpret_u16_u32(temp8.val[0]);
		            temp2.val[1] = vreinterpret_u16_u32(temp7.val[1]);
		            temp2.val[0] = vreinterpret_u16_u32(temp8.val[1]);

		            vst1_u16(pDst0, temp1.val[0]);
		            vst1_u16(pDst1, temp1.val[1]);
		            vst1_u16(pDst2, temp2.val[0]);
		            vst1_u16(pDst3, temp2.val[1]);

	        		pDst0 += heightMul4;
	        		pDst1 += heightMul4;
	        		pDst2 += heightMul4;
	        		pDst3 += heightMul4;
		        }
		    }
		} else if(degree == 180) {
			const uint16_t *pSrc = src;
			uint16_t *pDst = dst + height * width - 4;
	    	for (i = 0; i < height; ++i) {
	        	for (j = 0; j < width; j+=4) {
	        		uint16x4_t temp;
	        		temp = vld1_u16(pSrc);
	        		temp = vrev64_u16(temp);
	        		vst1_u16(pDst, temp);
	        		pSrc += 4;
	        		pDst -= 4;
	        	}
	    	}
		} else if(degree == 270) {
			const uint16_t *pSrc;
			uint16_t *pDst0, *pDst1, *pDst2, *pDst3;
			for (i = 0; i < height; i+=4) {
				pSrc = src + i * width;
	 			pDst0 = dst + (width - 1) * height + i;
	 			pDst1 = pDst0 - height;
	 			pDst2 = pDst1 - height;
	 			pDst3 = pDst2 - height;
				for (j = 0; j < width; j+=4) {
	 			    uint16x4x2_t temp1, temp2, temp3, temp4;
	        		uint32x2x2_t temp5, temp6, temp7, temp8;

	        		temp1.val[0] = vld1_u16(pSrc);
		            temp1.val[1] = vld1_u16(pSrc+width);
		            temp2.val[0] = vld1_u16(pSrc+2*width);
		            temp2.val[1] = vld1_u16(pSrc+3*width);

		            pSrc += 4;
#ifdef CLOCKWISE
		            temp3 = vtrn_u16(temp1.val[1],temp1.val[0]);
		            temp4 = vtrn_u16(temp2.val[1],temp2.val[0]);

		            temp5.val[0] = vreinterpret_u32_u16(temp3.val[0]);
		            temp5.val[1] = vreinterpret_u32_u16(temp3.val[1]);
		            temp6.val[0] = vreinterpret_u32_u16(temp4.val[0]);
		            temp6.val[1] = vreinterpret_u32_u16(temp4.val[1]);

		            temp7 = vtrn_u32(temp6.val[0],temp5.val[0]);
		            temp8 = vtrn_u32(temp6.val[1],temp5.val[1]);
#else
					temp3 = vtrn_u16(temp1.val[0],temp1.val[1]);
		            temp4 = vtrn_u16(temp2.val[0],temp2.val[1]);

		            temp5.val[0] = vreinterpret_u32_u16(temp3.val[0]);
		            temp5.val[1] = vreinterpret_u32_u16(temp3.val[1]);
		            temp6.val[0] = vreinterpret_u32_u16(temp4.val[0]);
		            temp6.val[1] = vreinterpret_u32_u16(temp4.val[1]);

		            temp7 = vtrn_u32(temp5.val[0],temp6.val[0]);
		            temp8 = vtrn_u32(temp5.val[1],temp6.val[1]);
#endif
		            temp1.val[0] = vreinterpret_u16_u32(temp7.val[0]);
		            temp1.val[1] = vreinterpret_u16_u32(temp8.val[0]);
		            temp2.val[0] = vreinterpret_u16_u32(temp7.val[1]);
		            temp2.val[1] = vreinterpret_u16_u32(temp8.val[1]);

		            vst1_u16(pDst0, temp1.val[0]);
		            vst1_u16(pDst1, temp1.val[1]);
		            vst1_u16(pDst2, temp2.val[0]);
		            vst1_u16(pDst3, temp2.val[1]);

	        		pDst0 -= heightMul4;
	        		pDst1 -= heightMul4;
	        		pDst2 -= heightMul4;
	        		pDst3 -= heightMul4;
				}
			}
		}
	}
}
}
