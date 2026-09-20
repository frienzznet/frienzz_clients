#ifndef WEBIO_H
#define WEBIO_H

#include "dataobjects.h"
#include "utils.h"

bool webio_jsonrpc_send_status(int available_type, int available_time);

bool loop_webio(bool force_update = false);

#endif // WEBIO_H
