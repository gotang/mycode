#ifndef ROTATE_H_
#define ROTATE_H_
namespace android {
#ifndef _UCHAR8C_
#define _UCHAR8C_
typedef struct {
	uint8_t c0;
	uint8_t c1;
	uint8_t c2;
	uint8_t c3;
	uint8_t c4;
	uint8_t c5;
	uint8_t c6;
	uint8_t c7;
}UCHAR8C;
#endif

#ifndef _USHORT4C_
#define _USHORT4C_
typedef struct {
	uint16_t c0;
	uint16_t c1;
	uint16_t c2;
	uint16_t c3;
}USHORT4C;
#endif

void rotate(uint8_t* dst, const uint8_t* src, size_t width, size_t height, int degree);
void rotate_u16(uint16_t* dst, const uint16_t* src, size_t width, size_t height, int degree);
void rotateFast(uint8_t* dst, const uint8_t* src, size_t width, size_t height, int degree);
void rotateFast_u16(uint16_t* dst, const uint16_t* src, size_t width, size_t height, int degree);
};
#endif//ROTATE_H_
