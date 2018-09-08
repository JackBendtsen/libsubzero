#ifndef SRT_H
#define SRT_H

typedef struct {
	int start_time;
	int end_time;
	char *text;
} subzero_sub;

typedef struct {
	subzero_sub *sub;
	int n_subs;
	int enc;
} subzero_srt;

char *subzero_time_string(int stamp);
int subzero_time_num(char *stamp);

int subzero_parse_srt(subzero_srt *srt, char *buf, int size);
char *subzero_write_srt(subzero_srt *srt, int *size);

int subzero_set_encoding(subzero_srt *srt, int enc);

int subzero_get_subtitle(subzero_srt *srt, subzero_sub *sub, int idx);
int subzero_insert_subtitle(subzero_srt *srt, subzero_sub *sub, int idx);
int subzero_delete_subtitle(subzero_srt *srt, int idx);

int subzero_get_line(subzero_sub *sub, int idx);
int subzero_get_text(subzero_sub *sub);
int subzero_insert_lines(subzero_sub *sub, int idx, char *text);
int subzero_delete_line(subzero_sub *sub, int idx);

int subzero_search_by_text(subzero_srt *srt, char *text, int *res);
int subzero_search_by_time(subzero_srt *srt, int start, int end, int *res);

int subzero_destroy_sub(subzero_sub *sub);
void subzero_destroy_srt(subzero_srt *srt);

#endif
