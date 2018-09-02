#ifndef SRT_H
#define SRT_H

typedef struct {
	int start_time;
	int end_time;
	char *lines;
	int n_lines;
} sub_t;

typedef struct {
	sub_t *sub;
	int n_subs;
	int enc;
} srt_t;

char *subzero_time_string(int stamp);
int subzero_time_int(char *stamp);

int subzero_parse_srt(srt_t *srt, char *buf);
char *subzero_write_srt(srt_t *srt);

int subzero_set_encoding(srt_t *srt, int enc);

int subzero_get_subtitle(srt_t *srt, sub_t *sub, int idx);
int subzero_insert_subtitle(srt_t *srt, sub_t *sub, int idx);
int subzero_delete_subtitle(srt_t *srt, int idx);

int subzero_get_line(sub_t *sub, int idx);
int subzero_get_text(sub_t *sub);
int subzero_insert_lines(sub_t *sub, int idx, char *text);
int subzero_delete_line(sub_t *sub, int idx);

int subzero_search_by_text(srt_t *srt, char *text, int *res);
int subzero_search_by_time(srt_t *srt, int start, int end, int *res);

int subzero_destroy_sub(sub_t *sub);
int subzero_destroy_srt(srt_t *srt);

#endif
