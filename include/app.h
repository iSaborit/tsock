#ifndef TSOCK_APP_H
#define TSOCK_APP_H

#include "cli.h"

/* Runs the application from a validated `tsock_config`. */
int app_run(const struct tsock_config cfg);

#endif // !TSOCK_APP_H
