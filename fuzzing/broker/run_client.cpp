/*
Copyright (c) 2023 Cedalo GmbH

All rights reserved. This program and the accompanying materials
are made available under the terms of the Eclipse Public License 2.0
and Eclipse Distribution License v1.0 which accompany this distribution.

The Eclipse Public License is available at
   https://www.eclipse.org/legal/epl-2.0/
and the Eclipse Distribution License is available at
  http://www.eclipse.org/org/documents/edl-v10.php.

SPDX-License-Identifier: EPL-2.0 OR BSD-3-Clause

Contributors:
   Roger Light - initial implementation and documentation.
*/

#include <stddef.h>
#include <stdint.h>
#include <sys/socket.h>
#include <unistd.h>

#include "broker_fuzz.h"

/* Feed one fuzz input to a running broker as a single client byte stream.
 * The broker (started on another thread by the harness) parses it through
 * the real network read path, so a fuzz input is an arbitrary sequence of
 * MQTT packets sent by one client.
 */
void run_client(struct fuzz_data *fuzz)
{
	uint8_t buf[1024];
	int sock;

	sock = connect_retrying(fuzz->port);

	if(send(sock, fuzz->data, fuzz->size, 0) > 0){
		/* Drain whatever the broker sends back so its write path and
		 * the client-facing state machine are exercised too. */
		recv_timeout(sock, buf, sizeof(buf), 20000);
	}

	close(sock);
}
