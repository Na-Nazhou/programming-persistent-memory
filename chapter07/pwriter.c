/*
 * Copyright 2015-2020, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *     * Neither the name of the copyright holder nor the names of its
 *       contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * pwriter.c - 	Write a string to a
 *		persistent memory pool
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpmemobj.h>

#define LAYOUT_NAME "rweg"
#define MAX_BUF_LEN 31

struct my_root {
	size_t len;
	char buf[MAX_BUF_LEN];
};

int
main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "usage: %s file-name\n", argv[0]);
		exit(1);
	}

	// Pool object pointer (POP) => reside in volatile memory
	// Keeps track of metadata related to the pool: offset to the root object
	PMEMobjpool *pop = pmemobj_create(argv[1], 
		LAYOUT_NAME, PMEMOBJ_MIN_POOL, 0666);

	if (pop == NULL) {
		perror("pmemobj_create");
		exit(1);
	}

	// Locate the root object
	PMEMoid root = pmemobj_root(pop, 
		sizeof(struct my_root));

	// Get a pointer to the root object
	struct my_root *rootp = pmemobj_direct(root);

	char buf[MAX_BUF_LEN] = "Hello PMEM World";

	rootp->len = strlen(buf);
	pmemobj_persist(pop, &rootp->len, 
		sizeof(rootp->len));

	pmemobj_memcpy_persist(pop, rootp->buf, buf, 
		rootp->len);

	pmemobj_close(pop);

	exit(0);
}
