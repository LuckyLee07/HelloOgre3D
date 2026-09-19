#include <errno.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef HELLO_DEFAULT_SAMPLE
#define HELLO_DEFAULT_SAMPLE "Sandbox19"
#endif

int main(int argc, char** argv)
{
	(void)argc;

	char executablePath[PATH_MAX];
	uint32_t executablePathSize = sizeof(executablePath);
	if (_NSGetExecutablePath(executablePath, &executablePathSize) != 0)
	{
		fprintf(stderr, "HelloOgre3D launcher path is too long\n");
		return 1;
	}

	char* macosDirectory = strrchr(executablePath, '/');
	if (macosDirectory == NULL)
	{
		fprintf(stderr, "HelloOgre3D launcher path is invalid\n");
		return 1;
	}
	*macosDirectory = '\0';

	char binDirectory[PATH_MAX];
	const int written = snprintf(binDirectory, sizeof(binDirectory), "%s/../../../bin", executablePath);
	if (written < 0 || (size_t)written >= sizeof(binDirectory) || chdir(binDirectory) != 0)
	{
		fprintf(stderr, "HelloOgre3D launcher cannot enter %s: %s\n", binDirectory, strerror(errno));
		return 1;
	}

	if (getenv("HELLO_SANDBOX_SAMPLE") == NULL)
		setenv("HELLO_SANDBOX_SAMPLE", HELLO_DEFAULT_SAMPLE, 0);
	if (strcmp(HELLO_DEFAULT_SAMPLE, "Sandbox20") == 0 && getenv("HELLO_RENDER_FSAA") == NULL)
		setenv("HELLO_RENDER_FSAA", "4", 0);
	argv[0] = (char*)"./HelloOgre3D";
	execv(argv[0], argv);
	fprintf(stderr, "HelloOgre3D launcher cannot start the game: %s\n", strerror(errno));
	return 1;
}
