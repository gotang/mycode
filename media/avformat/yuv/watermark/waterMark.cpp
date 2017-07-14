#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define LOG_NDEBUG 0
#define LOG_TAG "waterMark"
#include "Log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#include <waterMark.h>

//only the fg_width is 8 pixels align and the fg_height is 2 pixels align
//#define	USE_NEON

// bg_width			background width
// bg_height        background height

// left             foreground position of left
// top              foreground position of top
// fg_width         foreground width
// fg_height        foreground height

// bg_y           	point to the background YUV420sp format y component data
// bg_c           	point to the background YUV420sp format c component data
// fg_y           	point to the foreground YUV420sp format y component data
// fg_c           	point to the foreground YUV420sp format c component data
// alph             point to foreground alph value

void yuv420sp_blending(int bg_width, int bg_height,
		int left, uint32_t top, int fg_width, int fg_height,
		uint8_t *bg_y, uint8_t *bg_c, uint8_t *fg_y,
		uint8_t *fg_c, uint8_t *alph) {
	uint8_t *bg_y_p = NULL;
	uint8_t *bg_c_p = NULL;
	int i = 0;
	int j = 0;

	uint8_t *bg_y_p_line2 = NULL;
	uint8_t *fg_y_line2 = NULL;
	uint8_t *alph_line2 = NULL;

	bg_y_p = bg_y + top * bg_width + left;
	bg_c_p = bg_c + (top >> 1) * bg_width + left;

	bg_y_p_line2 = bg_y_p + bg_width;
	fg_y_line2 = fg_y + fg_width;
	alph_line2 = alph + fg_width;
	for (i = 0; i < (int) fg_height; i++) {
		if ((i & 1) == 0) {
			for (j = 0; j < (int) fg_width; j++) {
				*bg_y_p = ((256 - *alph) * (*bg_y_p) + (*fg_y++) * (*alph)) >> 8;
				*bg_c_p = ((256 - *alph) * (*bg_c_p) + (*fg_c++) * (*alph)) >> 8;

				alph++;
				bg_y_p++;
				bg_c_p++;
			}

			bg_c_p = bg_c_p + bg_width - fg_width;
		} else {
			for (j = 0; j < (int) fg_width; j++) {
				*bg_y_p = ((256 - *alph) * (*bg_y_p) + (*fg_y++) * (*alph)) >> 8;
				alph++;
				bg_y_p++;
			}
		}
		bg_y_p = bg_y_p + bg_width - fg_width;
	}
}

// bg_width			background width
// bg_height        background height

// left             foreground position of left
// top              foreground position of top
// fg_width         foreground width
// fg_height        foreground height
// bg_y           	point to the background YUV420sp format y component data

// return value : 0 dark, 1 bright

int region_bright_or_dark(int bg_width, int bg_height,
		int left, int top, int fg_width,
		int fg_height, uint8_t *bg_y) {
	uint8_t *bg_y_p = NULL;

	int i = 0;
	int j = 0;
	int bright_line_number = 0;
	int value = 0;

	bg_y_p = bg_y + top * bg_width + left;

	for (i = 0; i < (int) fg_height; i++) {
		value = 0;
		for (j = 0; j < (int) fg_width; j++) {
			value += *bg_y_p++;
		}
		value = value / fg_width;

		if (value > 128) {
			bright_line_number++;
		}

		bg_y_p = bg_y_p + bg_width - fg_width;
	}

	if (bright_line_number > fg_height / 2) {
		return 1;
	}

	return 0;
}

// bg_width			background width
// bg_height        background height

// left             foreground position of left
// top              foreground position of top
// fg_width         foreground width
// fg_height        foreground height

// bg_y           	point to the background YUV420sp format y component data
// bg_c           	point to the background YUV420sp format c component data
// fg_y           	point to the foreground YUV420sp format y component data
// fg_c           	point to the foreground YUV420sp format c component data
// alph             point to foreground alph value

void yuv420sp_blending_adjust_brightness(int bg_width, int bg_height,
		int left, int top, int fg_width, int fg_height, uint8_t *bg_y, uint8_t *bg_c,
		uint8_t *fg_y, uint8_t *fg_c, uint8_t *alph, int is_brightness) {
	uint8_t *bg_y_p = NULL;
	uint8_t *bg_c_p = NULL;

	int i = 0;
	int j = 0;

	if (bg_y == NULL || bg_c == NULL || fg_y == NULL || fg_c == NULL || alph == NULL) {
		ALOGE("<yuv420sp_blending_adjust_brightness>NULL pointer error!");
		return;
	}

	uint8_t *bg_y_p_line2 = NULL;
	uint8_t *fg_y_line2 = NULL;
	uint8_t *alph_line2 = NULL;

	bg_y_p = bg_y + top * bg_width + left;
	bg_c_p = bg_c + (top >> 1) * bg_width + left;

	bg_y_p_line2 = bg_y_p + bg_width;
	fg_y_line2 = fg_y + fg_width;
	alph_line2 = alph + fg_width;

	if (is_brightness) {
		for (i = 0; i < (int) fg_height; i++) {
			if ((i & 1) == 0) {
				for (j = 0; j < (int) fg_width; j++) {
					*bg_y_p = ((256 - *alph) * (*bg_y_p) + (256 - (*fg_y++)) * (*alph)) >> 8;
					*bg_c_p = ((256 - *alph) * (*bg_c_p) + (*fg_c++) * (*alph)) >> 8;

					alph++;
					bg_y_p++;
					bg_c_p++;
				}
				bg_c_p = bg_c_p + bg_width - fg_width;
			} else {
				for (j = 0; j < (int) fg_width; j++) {
					*bg_y_p = ((256 - *alph) * (*bg_y_p) + (256 - (*fg_y++)) * (*alph)) >> 8;
					alph++;
					bg_y_p++;
				}
			}

			bg_y_p = bg_y_p + bg_width - fg_width;
		}
	} else {
		for (i = 0; i < (int) fg_height; i++) {
			if ((i & 1) == 0) {
				for (j = 0; j < (int) fg_width; j++) {
					*bg_y_p = ((256 - *alph) * (*bg_y_p) + (*fg_y++) * (*alph)) >> 8;
					*bg_c_p = ((256 - *alph) * (*bg_c_p) + (*fg_c++) * (*alph)) >> 8;

					alph++;
					bg_y_p++;
					bg_c_p++;
				}
				bg_c_p = bg_c_p + bg_width - fg_width;
			} else {
				for (j = 0; j < (int) fg_width; j++) {
					*bg_y_p = ((256 - *alph) * (*bg_y_p) + (*fg_y++) * (*alph)) >> 8;
					alph++;
					bg_y_p++;
				}
			}

			bg_y_p = bg_y_p + bg_width - fg_width;
		}
	}
}

int watermark_blending(BackGroudLayerInfo *bg_info, WaterMarkInfo *wm_info,
		ShowWaterMarkParam *wm_Param) {
	int i;
	int id;
	int total_width = 0;
	int total_height = 0;
	int pos_x;
	int pos_y;

	for (i = 0; i < wm_Param->number; ++i) {
		id = wm_Param->id_list[i];
		total_width += wm_info->single_pic[id].width;
		total_height += wm_info->single_pic[id].height;
	}

	bool invalid = false;
	ALOGV("coordinate (%d, %d)", wm_Param->pos.x, wm_Param->pos.y);
	if(wm_info->rotate == 90) {
		pos_y = wm_Param->pos.x;
		pos_x = bg_info->width - wm_Param->pos.y - wm_info->single_pic[id].width;
		invalid = (pos_y + total_height) > bg_info->height;
	} else if(wm_info->rotate == 180) {
		pos_x = bg_info->width - wm_Param->pos.x - wm_info->single_pic[id].width;
		pos_y = bg_info->height - wm_Param->pos.y - wm_info->single_pic[id].height;
		invalid = total_width > pos_x;
	} else if(wm_info->rotate == 270) {
		pos_x = wm_Param->pos.x;
		pos_y = bg_info->height - wm_Param->pos.y - wm_info->single_pic[id].height;
		invalid = total_height > pos_y;
	} else {
		pos_x = wm_Param->pos.x;
		pos_y = wm_Param->pos.y;
		invalid = (pos_x + total_width) > bg_info->width;
	}

	if(invalid) {
		ALOGE("invalid region total (%d, %d), background (%d, %d)",
				total_width, total_height, bg_info->width, bg_info->height);
		return -1;
	}

	ALOGV("region total (%d, %d), background (%d, %d)",
			total_width, total_height, bg_info->width, bg_info->height);
	ALOGV("coordinate (%d, %d)", pos_x, pos_y);
	for (i = 0; i < wm_Param->number; i++) {
		id = wm_Param->id_list[i];
		yuv420sp_blending(bg_info->width, bg_info->height, pos_x, pos_y,
				wm_info->single_pic[id].width, wm_info->single_pic[id].height,
				bg_info->y, bg_info->c, wm_info->single_pic[id].y,
				wm_info->single_pic[id].c, wm_info->single_pic[id].alph);
		if(wm_info->rotate == 90) {
			pos_y += wm_info->single_pic[id].height;
		} else if(wm_info->rotate == 180) {
			pos_x -= wm_info->single_pic[id].width;
		} else if(wm_info->rotate == 270) {
			pos_y -= wm_info->single_pic[id].height;
		} else {
			pos_x += wm_info->single_pic[id].width;
		}
	}
	return 0;
}

int watermark_blending_ajust_brightness(BackGroudLayerInfo *bg_info, WaterMarkInfo *wm_info,
		ShowWaterMarkParam *wm_Param, AjustBrightnessParam *ajust_Param) {
	int i;
	int id;
	int is_brightness;
	int total_width = 0;
	int pos_x;

	for (i = 0; i < wm_Param->number; ++i) {
		id = wm_Param->id_list[i];
		total_width += wm_info->single_pic[id].width;
	}
	if (total_width > bg_info->width) {
		ALOGE("error region(total_width=%d, bg_width=%d)", total_width, bg_info->width);
		return -1;
	}

	if (ajust_Param->cur_frame > ajust_Param->recycle_frame) {
		ajust_Param->cur_frame = 0;
	} else {
		ajust_Param->cur_frame++;
	}

	pos_x = wm_Param->pos.x;
	if (ajust_Param->cur_frame == 0) {
		for (i = 0; i < wm_Param->number; i++) {
			id = wm_Param->id_list[i];
			is_brightness = region_bright_or_dark(bg_info->width, bg_info->height, pos_x, wm_Param->pos.y, wm_info->single_pic[id].width, wm_info->single_pic[id].height, bg_info->y);
			ajust_Param->Brightness[i] = is_brightness;

			yuv420sp_blending_adjust_brightness(bg_info->width, bg_info->height, pos_x, wm_Param->pos.y, wm_info->single_pic[id].width, wm_info->single_pic[id].height, bg_info->y, bg_info->c, wm_info->single_pic[id].y, wm_info->single_pic[id].c, wm_info->single_pic[id].alph, is_brightness);
			pos_x += wm_info->single_pic[id].width;
		}
	} else {
		for (i = 0; i < wm_Param->number; i++) {
			id = wm_Param->id_list[i];

			is_brightness = ajust_Param->Brightness[i];

			yuv420sp_blending_adjust_brightness(bg_info->width, bg_info->height, pos_x, wm_Param->pos.y, wm_info->single_pic[id].width, wm_info->single_pic[id].height, bg_info->y, bg_info->c, wm_info->single_pic[id].y, wm_info->single_pic[id].c, wm_info->single_pic[id].alph, is_brightness);
			pos_x += wm_info->single_pic[id].width;
		}
	}

	return 0;
}

#ifdef __cplusplus
}
#endif /* __cplusplus */

