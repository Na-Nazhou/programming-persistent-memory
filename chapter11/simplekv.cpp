/*
 * Copyright 2020, Intel Corporation
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
 * simplekv.cpp -- implementation of simple kv which uses vector to hold
 * values, string as a key and array to hold buckets
 */

#include <libpmemobj++/make_persistent_atomic.hpp>
#include <libpmemobj++/pool.hpp>
#include <stdexcept>

#include "simplekv.hpp"

enum simplekv_op {
	GET,
	PUT,
	EXIT,
	MAX_OPS,
};

const char *ops_str[MAX_OPS] = {"get", "put", "exit"};

simplekv_op parse_simplekv_ops(const std::string &ops)
{
	for (int i = 0; i < MAX_OPS; i++) {
		if (ops == ops_str[i]) {
			return (simplekv_op)i;
		}
	}
	return MAX_OPS;
}

using kv_type = simple_kv<int, 10>;

struct root {
	pmem::obj::persistent_ptr<kv_type> kv;
};

int
main(int argc, char *argv[])
{
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " path_to_pool" << std::endl;
		return 1;
	}

	const char *path = argv[1];
	pmem::obj::pool<root> pop;

	try {
		pop = pmem::obj::pool<root>::open(path, "simplekv");
	} catch (pmem::pool_error &e) {
		std::cerr << e.what() << std::endl;
		std::cerr << "To create pool run: pmempool create obj --layout=simplekv path_to_pool"
			<< std::endl;
	}

	auto r = pop.root();

	if (r->kv == nullptr) {
		pmem::obj::transaction::run(pop, [&] {
			r->kv = pmem::obj::make_persistent<kv_type>();
		});
	}

	try {
		while (1) {
			std::cout << "usage: [get key|put key value|exit]" << std::endl;

			std::string command;
			std::cin >> command;

			// parse string
			auto ops = parse_simplekv_ops(std::string(command));

			switch (ops) {
				case GET: {
					std::string key;
					std::cin >> key;

					std::cout << r->kv->get(key) << std::endl;
					
					break;
				}
				case PUT: {
					std::string key;
					int value;
					std::cin >> key;
					std::cin >> value;

					r->kv->put(key, value);

					break;
				}
				case EXIT: {
					pop.close();
					exit(0);
				}
				default: {
					std::cerr << "usage: [get key|put key value|exit]" << std::endl;
					pop.close();
					exit(0);
				}
			}
		}
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
	}
}
