#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../srt.h"

int main(int argc, char **argv) {
	if (argc != 3) {
		printf("Invalid arguments\n"
			"Usage: %s <.srt file> <time interval in secs>\n"
			"* Note - the time interval can be a fraction, as well as negative\n", argv[0]);
		return 0;
	}

	FILE *f = fopen(argv[1], "rb");
	if (!f) {
		printf("Could not find \"%s\"\n", argv[1]);
		return 1;
	}

	fseek(f, 0, SEEK_END);
	int sz = ftell(f);
	rewind(f);

	if (sz < 1) {
		printf("Could not read \"%s\"\n", argv[1]);
		return 2;
	}

	char *buf = malloc(sz);
	fread(buf, 1, sz, f);
	fclose(f);

	subzero_srt srt = {0};
	int r = subzero_parse_srt(&srt, buf, sz);
	free(buf);
	if (r < 0) {
		printf("Error: \"%s\" is not an srt file (%d)\n", argv[1], r);
		free(buf);
		return 3;
	}

	int shift = atof(argv[2]) * 1000.0f;
	int i;
	for (i = 0; i < srt.n_subs; i++) {
		srt.sub[i].start_time += shift;
		srt.sub[i].end_time += shift;
	}

	int out_sz;
	char *out = subzero_write_srt(&srt, &out_sz);
	subzero_destroy_srt(&srt);

	f = fopen(argv[1], "wb");
	fwrite(out, 1, out_sz, f);
	fclose(f);
	free(out);

	return 0;
}
