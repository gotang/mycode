#ifndef _WATER_MARK_INTERFACE_H
#define _WATER_MARK_INTERFACE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct {
	uint8_t *y;
	uint8_t *c;
	int posx;
	int posy;
	int width;
	int height;
	char *display;
    int main_channel;
	float scale;
} WaterMarkInData;

int doWaterMarkMultiple(WaterMarkInData *data, void *ctrl, const char *time_str);
void *initialwaterMark(int wm_height, int rotate);
int releaseWaterMark(void *ctrl);
int waterMarkSetMultiple(void *ctrl, const char *waterMark);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _WATER_MARK_INTERFACE_H */
