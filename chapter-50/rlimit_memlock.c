#include <sys/resource.h>
#include <sys/mman.h>
#include <unistd.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[]) {
	struct rlimit lim;
	char *addr;
	size_t len;
	long pageSize;

	if (getrlimit(RLIMIT_MEMLOCK, &lim) == -1)
		errExit("getrlimit");
	
	pageSize = sysconf(_SC_PAGESIZE);
	
	printf("soft = %d, hard = %d\n", (int) lim.rlim_cur, (int) lim.rlim_max);

	len = (size_t) lim.rlim_max;
	addr = mmap(NULL, len + pageSize, PROT_READ, MAP_PRIVATE | MAP_ANON, -1, 0);
	if (addr == MAP_FAILED)
		errExit("mmap");

	if (mlock(addr, len + pageSize) == -1)
		errExit("mlock");

	exit(EXIT_SUCCESS);
}
