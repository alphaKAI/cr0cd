#include <stdio.h>
#include <stdlib.h>

int main(void) {
	char bits[33];
	FILE *fp = fopen("/proc/cr0cd", "r");

	if (fp == NULL) {
		fprintf(stderr, "Failed to read cr0cd\n");
		exit(EXIT_FAILURE);
	}

	fread(bits, 33, 1, fp);

	printf("CR0: %s\n", bits);

	fclose(fp);
}
