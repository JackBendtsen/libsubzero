#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../srt.h"

char *subzero_time_string(int time) {
	if (time < 0)
		time = -time;

	int ms = time % 1000;
	time /= 1000;

	int secs = time % 60;
	time /= 60;

	int mins = time % 60;
	time /= 60;

	int hrs = time;

	char *str = calloc(13, 1);
	sprintf(str, "%02d:%02d:%02d,%03d", hrs, mins, secs, ms);

	return str;
}

int subzero_time_num(char *time) {
	if (!time || strlen(time) < 12)
		return -1;

	if (time[2] != ':' || time[5] != ':' || time[8] != ',')
		return -2;

	int npos[] = {0, 1, 3, 4, 6, 7, 9, 10, 11};
	int i;
	for (i = 0; i < 9; i++) {
		if (time[npos[i]] < '0' || time[npos[i]] > '9')
			return -3;
	}

	int hrs = (time[0] - '0') * 10 + (time[1] - '0');
	int mins = (time[3] - '0') * 10 + (time[4] - '0');
	int secs = (time[6] - '0') * 10 + (time[7] - '0');
	int ms = (time[9] - '0') * 100 + (time[10] - '0') * 10 + (time[11] - '0');

	int num = hrs * 60 * 60 * 1000;
	num += mins * 60 * 1000;
	num += secs * 1000;
	num += ms;

	return num;
}

char *dos2unix(char *in, int in_sz, int *out_sz) {
	if (!in || in_sz < 1)
		return NULL;

	char *out = calloc(in_sz, 1);
	int i, sz = 0;
	for (i = 0; i < in_sz; i++) {
		if (in[i] != 0xd)
			out[sz++] = in[i];
	}

	if (out_sz)
		*out_sz = sz;

	return out;
}

int subzero_parse_srt(subzero_srt *srt, char *buf, int size) {
	if (!srt || !buf || size < 1)
		return -1;

	memset(srt, 0, sizeof(subzero_srt));

	char *in = dos2unix(buf, size, &size);
	if (!in)
		return -2;

	char *p = in;
	if (!memcmp(in, "\xef\xbb\xbf", 3)) {
		srt->enc = 1;
		p += 3;
	}

	int cond = 0;
	while (p-in < size) {
		if (atoi(p) != srt->n_subs + 1) {
			cond = 1;
			break;
		}

		while (p-in < size && *p >= '0' && *p <= '9')
			p++; // find the end of the line
		while (p-in < size && (*p < '0' || *p > '9'))
			p++; // find the start of the next line

		// 28 being the minimum length of the timestamp line
		if (p-in >= size - 28) {
			cond = 2;
			break;
		}

		int start = subzero_time_num(p);
		if (start < 0) {
			printf("p: %.12s, start: %d\n", p, start);
			cond = 3;
			break;
		}

		p += 12; // sizeof timestamp string
		while (p-in < size && (*p < '0' || *p > '9'))
			p++; // find next timestamp

		int end;
		if (p-in >= size-12 || ((end = subzero_time_num(p)) < 0)) {
			cond = 4;
			break;
		}

		p += 12;
		while (p-in < size && *p == '\n')
			p++; // find next line

		char *text = p;
		while (p-in < size-1 && !(p[0] == '\n' && p[1] == '\n'))
			p++; // find the end of the subtitle
		p--;

		srt->n_subs++;
		srt->sub = realloc(srt->sub, srt->n_subs * sizeof(subzero_sub));

		subzero_sub *s = &srt->sub[srt->n_subs-1];
		s->start_time = start;
		s->end_time = end;
		s->text = calloc((p-text) + 2, 1);
		memcpy(s->text, text, (p-text) + 1);

		while (p-in < size && (*p < '0' || *p > '9'))
			p++; // find next subtitle (starts with a number)
	}

	//printf("cond: %d\n", cond);

	free(in);
	return srt->sub ? 0 : -2;
}

char *subzero_write_srt(subzero_srt *srt, int *out_sz) {
	if (!srt || !srt->sub || srt->n_subs < 1)
		return NULL;

	if (out_sz)
		*out_sz = 0;

	char *out = NULL;
	int sz = 0;

	if (srt->enc == 1) {
		out = malloc(3);
		memcpy(out, "\xef\xbb\xbf", 3);
		sz += 3;
	}

	char head[40];
	int i;
	for (i = 0; i < srt->n_subs; i++) {
		char *start_str = subzero_time_string(srt->sub[i].start_time);
		char *end_str = subzero_time_string(srt->sub[i].end_time);

		int len = sprintf(head, "%d\n%s --> %s\n", i+1, start_str, end_str);
		out = realloc(out, sz + len + strlen(srt->sub[i].text) + 2 + 1);

		strcpy(out + sz, head); sz += len;
		strcpy(out + sz, srt->sub[i].text); sz += strlen(srt->sub[i].text);

		if (i == srt->n_subs - 1)
			out[sz++] = '\n';
		else {
			strcpy(out + sz, "\n\n");
			sz += 2;
		}

		free(start_str);
		free(end_str);
	}

	if (out_sz)
		*out_sz = sz;

	return out;
}

void subzero_destroy_srt(subzero_srt *srt) {
	if (!srt || !srt->sub)
		return;

	int i;
	for (i = 0; i < srt->n_subs; i++)
		free(srt->sub[i].text);

	free(srt->sub);
	memset(srt, 0, sizeof(subzero_srt));
}
