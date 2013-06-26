#pragma once

#define DEFAULT_WIDTH_LABEL 100
#define DEFAULT_WIDTH_COMMENT 200
#define WIDTH_LABEL TEXT("width_label")
#define WIDTH_COMMENT TEXT("width_comment")

int get_width_label();
int get_width_comment();
void set_width_label(int);
void set_width_comment(int);

void hook_label_functions();