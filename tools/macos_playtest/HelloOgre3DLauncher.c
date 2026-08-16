#include <errno.h>
#include <limits.h>
#include <mach-o/dyld.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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

	argv[0] = (char*)"./HelloOgre3D";
	execv(argv[0], argv);
	fprintf(stderr, "HelloOgre3D launcher cannot start the game: %s\n", strerror(errno));
	return 1;
}
