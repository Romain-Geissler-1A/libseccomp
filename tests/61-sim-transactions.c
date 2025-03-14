/**
 * Seccomp Library test program
 *
 * Copyright (c) 2023 Microsoft Corporation <paulmoore@microsoft.com>
 * Author: Paul Moore <paul@paul-moore.com>
 */

/*
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of version 2.1 of the GNU Lesser General Public License as
 * published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License
 * for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, see <http://www.gnu.org/licenses>.
 */

#include <errno.h>
#include <unistd.h>

#include <seccomp.h>

#include "util.h"

int main(int argc, char *argv[])
{
	int rc;
	int i, j;
	struct util_options opts;
	scmp_filter_ctx ctx = NULL;

	rc = util_getopt(argc, argv, &opts);
	if (rc < 0)
		goto out;

	ctx = seccomp_init(SCMP_ACT_ALLOW);
	if (ctx == NULL)
		return ENOMEM;

	rc = seccomp_transaction_start(ctx);
	if (rc != 0)
		goto out;
	rc = seccomp_rule_add_exact(ctx, SCMP_ACT_KILL, 1000, 0);
	if (rc != 0)
		goto out;
	rc = seccomp_transaction_commit(ctx);
	if (rc != 0)
		goto out;

	rc = seccomp_rule_add_exact(ctx, SCMP_ACT_KILL, 1001, 0);
	if (rc != 0)
		goto out;

	rc = seccomp_transaction_start(ctx);
	if (rc != 0)
		goto out;
	for (i = 1; i <= 10; i++) {
		for (j = 0; j <= i; j++) {
			rc = seccomp_transaction_start(ctx);
			if (rc != 0)
				goto out;
		}

		rc = seccomp_rule_add_exact(ctx, SCMP_ACT_KILL, 1100 + i, 0);
		if (rc != 0)
			goto out;

		if (i % 5) {
			for (j = 0; j <= i; j++) {
				rc = seccomp_transaction_commit(ctx);
				if (rc != 0)
					goto out;
			}
		} else {
			for (j = 0; j <= i; j++)
				seccomp_transaction_reject(ctx);
		}
	}
	rc = seccomp_transaction_commit(ctx);
	if (rc != 0)
		goto out;

	rc = seccomp_transaction_start(ctx);
	if (rc != 0)
		goto out;
	for (i = 1; i <= 10; i++) {
		for (j = 0; j <= i; j++) {
			rc = seccomp_transaction_start(ctx);
			if (rc != 0)
				goto out;
		}

		rc = seccomp_rule_add_exact(ctx, SCMP_ACT_KILL, 1200 + i, 0);
		if (rc != 0)
			goto out;

		if (i % 5) {
			for (j = 0; j <= i; j++) {
				rc = seccomp_transaction_commit(ctx);
				if (rc != 0)
					goto out;
			}
		} else {
			for (j = 0; j <= i; j++)
				seccomp_transaction_reject(ctx);
		}
	}
	seccomp_transaction_reject(ctx);

	rc = seccomp_transaction_start(ctx);
	if (rc != 0)
		goto out;
	rc = seccomp_rule_add_exact(ctx, SCMP_ACT_KILL, 1002, 0);
	if (rc != 0)
		goto out;
	rc = seccomp_transaction_commit(ctx);
	if (rc != 0)
		goto out;

	rc = seccomp_rule_add_exact(ctx, SCMP_ACT_KILL, 1003, 0);
	if (rc != 0)
		goto out;

	rc = util_filter_output(&opts, ctx);
	if (rc)
		goto out;

out:
	seccomp_release(ctx);
	return (rc < 0 ? -rc : rc);
}
