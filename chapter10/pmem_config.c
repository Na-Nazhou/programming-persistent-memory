/*
 * Copyright (c) 2020 Intel Corporation
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
 *     * Neither the name of Intel Corporation nor the names of its
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
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY LOG OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * pmem_config.c - demonstrate configuration functions
 */

#include <memkind.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#define PMEM_MAX_SIZE (1024 * 1024 * 32)
#define PATH "/optane/nazhou"

void memkind_fatal(int err)
{
	char error_message[MEMKIND_ERROR_MESSAGE_SIZE];

	memkind_error_message(err, error_message,
		MEMKIND_ERROR_MESSAGE_SIZE);
	fprintf(stderr, "%s\n", error_message);
	exit(1);
}

int main(int argc, char *argv[])
{
	struct memkind *pmem_kind;
	int err;

	struct memkind_config *test_cfg =
		memkind_config_new();
	if (test_cfg == NULL) {
		fprintf(stderr,
			"memkind_config_new: out of memory\n");
		exit(1);
	}

	memkind_config_set_path(test_cfg, PATH);
	memkind_config_set_size(test_cfg, PMEM_MAX_SIZE);
	memkind_config_set_memory_usage_policy(test_cfg,
		MEMKIND_MEM_USAGE_POLICY_CONSERVATIVE);

	// Create PMEM partition with the configuration
	err =  memkind_create_pmem_with_config(test_cfg,
		&pmem_kind);
	if (err) {
		memkind_fatal(err);
	}

	err = memkind_destroy_kind(pmem_kind);
	if (err) {
		memkind_fatal(err);
	}

	memkind_config_delete(test_cfg);

	exit(0);
}
