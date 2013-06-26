#pragma once

#define DEFAULT_WIDTH_LABEL 100
#define DEFAULT_WIDTH_COMMENT 200
#define WIDTH_LABEL TEXT("label_width")
#define WIDTH_COMMENT TEXT("comment_width")
#define LABEL_ENABLE TEXT("label_enable")

int get_width_label_now();
int get_width_comment_now();

void hook_label_functions();