#include <stdio.h>
#include <assert.h>

char buf[10000];

void SaveVideo(int n)
{
	char filename[32];
	sprintf(filename, ",video-%05d", n);
	FILE* f = fopen(filename, "w");
	int cc = fread(buf, 1, 6144, stdin);
	assert(cc == 6144);
	fwrite(buf, 1, 6144, f);
	fclose(f);
}
void SaveAudio(int n)
{
	char filename[32];
	sprintf(filename, ",audio-%05d", n);
	FILE* f = fopen(filename, "w");
	int cc = fread(buf, 1, 1600, stdin);
	assert(cc == 1600);
	fwrite(buf, 1, 1600, f);
	fclose(f);
}
void SaveCmd()
{
	FILE* f = fopen(",tv-cmd", "w");
	fread(buf, 1, 985, stdin);
	fwrite(buf, 1, 985, f);
	fclose(f);
}
int main() {
	SaveCmd();
	SaveAudio(0);
	for (int i = 1; i <= 900; i++) {
		SaveAudio(i);
		SaveVideo(i);
	}
}
