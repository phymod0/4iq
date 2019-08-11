#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LINESZ 65536


struct tracker {
	FILE *fd;
	char *text;
};


static void tracker_init(struct tracker *tracker, FILE *fd)
{
	char *text;

	if (!(text = malloc(MAX_LINESZ))) {
		printf("%s: Allocation failure...\n",
		       __func__);
		return;
	}
	*tracker = (struct tracker){
		.fd = fd,
		.text = text,
	};
	return;
}


static void tracker_deinit(struct tracker *tracker)
{
	fclose(tracker->fd);
	free(tracker->text);
	return;
}


static void remove_newline(char *text)
{
	int len;
	len = strlen(text);
	if (text[len-1] == '\n')
		text[len-1] = '\0';
	return;
}


static struct tracker* tracker_min(struct tracker *a,
			    struct tracker *b)
{
	if (!a)
		return b;
	if (!b)
		return a;
	if (!a->text)
		return b;
	if (!b->text)
		return a;
	return strcmp(a->text, b->text) < 0 ? a : b;
}


/** Track the next line in the file tracked by tracker */
static void iterate_tracker(struct tracker *tracker)
{
	if (fgets(tracker->text, MAX_LINESZ, tracker->fd))
		remove_newline(tracker->text);
	else
		tracker->text = NULL;
	return;
}


static void write_merged(FILE **fd, int n_files, FILE *out)
{
	int i;
	struct tracker *trackers;

	if (!(trackers = malloc(n_files * sizeof *trackers))) {
		printf("%s: Allocation failure...\n",
		       __func__);
		return;
	}
	for (i=0; i<n_files; i++) {
		tracker_init(&trackers[i], fd[i]);
		iterate_tracker(&trackers[i]);
	}
	while (1) {
		struct tracker *min = NULL;
		for (i=0; i<n_files; i++)
			min = tracker_min(min, &trackers[i]);
		if (!min || !min->text)
			/* All files read... */
			break;
		fprintf(out, "%s\n", min->text);
		iterate_tracker(min);
	}
	for (i=0; i<n_files; i++)
		tracker_deinit(&trackers[i]);
	free(trackers);

	return;
}


static int begin_merging(char **file_list, int n_files)
{
	int i;
	FILE **fd = NULL;

	if (!(fd = malloc(n_files * sizeof *fd)))
		goto err;
	for (i=0; i<n_files; i++)
		if (!(fd[i] = fopen(file_list[i], "r"))) {
			printf("Failed to open %s...\n",
			       file_list[i]);
			goto err;
		}
	write_merged(fd, n_files, stdout);

out:
	free(fd);
	return 0;
err:
	fprintf(stderr, "%s: Encountered error...\n", __func__);
	free(fd);
	return -1;
}


int main(int argc, char **argv)
{
	int n_files;
	char **file_list;

	n_files = argc - 1;
	file_list = argv + 1;

	return begin_merging(file_list, n_files);
}
