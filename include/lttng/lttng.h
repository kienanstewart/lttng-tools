/*
 * lttng.h
 *
 * Linux Trace Toolkit Control Library Header File
 *
 * Copyright (C) 2011 EfficiOS Inc.
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 *
 */

#ifndef LTTNG_H
#define LTTNG_H

/* Error codes that can be returned by API calls */
#include <lttng/lttng-error.h>

/* Include every LTTng ABI/API available. */
#include <lttng/action/action.h>
#include <lttng/action/list.h>
#include <lttng/action/notify.h>
#include <lttng/action/path.h>
#include <lttng/action/rate-policy.h>
#include <lttng/action/rotate-session.h>
#include <lttng/action/snapshot-session.h>
#include <lttng/action/start-session.h>
#include <lttng/action/stop-session.h>
#include <lttng/channel.h>
#include <lttng/clear-handle.h>
#include <lttng/clear.h>
#include <lttng/condition/buffer-usage.h>
#include <lttng/condition/condition.h>
#include <lttng/condition/evaluation.h>
#include <lttng/condition/event-rule-matches.h>
#include <lttng/condition/session-consumed-size.h>
#include <lttng/condition/session-rotation.h>
#include <lttng/constant.h>
#include <lttng/destruction-handle.h>
#include <lttng/domain.h>
#include <lttng/endpoint.h>
#include <lttng/error-query.h>
#include <lttng/event-expr.h>
#include <lttng/event-field-value.h>
#include <lttng/event-rule/event-rule.h>
#include <lttng/event-rule/jul-logging.h>
#include <lttng/event-rule/kernel-kprobe.h>
#include <lttng/event-rule/kernel-syscall.h>
#include <lttng/event-rule/kernel-tracepoint.h>
#include <lttng/event-rule/kernel-uprobe.h>
#include <lttng/event-rule/log4j-logging.h>
#include <lttng/event-rule/python-logging.h>
#include <lttng/event-rule/user-tracepoint.h>
#include <lttng/event.h>
#include <lttng/handle.h>
#include <lttng/health.h>
#include <lttng/kernel-probe.h>
#include <lttng/load.h>
#include <lttng/location.h>
#include <lttng/log-level-rule.h>
#include <lttng/lttng-error.h>
#include <lttng/lttng-export.h>
#include <lttng/notification/channel.h>
#include <lttng/notification/notification.h>
#include <lttng/rotation.h>
#include <lttng/save.h>
#include <lttng/session-descriptor.h>
#include <lttng/session.h>
#include <lttng/snapshot.h>
#include <lttng/tracker.h>
#include <lttng/trigger/trigger.h>
#include <lttng/userspace-probe.h>

#ifdef __cplusplus
extern "C" {
#endif

enum lttng_calibrate_type {
	LTTNG_CALIBRATE_FUNCTION = 0,
};

/* Machine interface output type */
enum lttng_mi_output_type {
	LTTNG_MI_XML = 1 /* XML output */
};

#define LTTNG_CALIBRATE_PADDING1 16
struct lttng_calibrate {
	enum lttng_calibrate_type type;

	char padding[LTTNG_CALIBRATE_PADDING1];
};

/*
 * Check if a session daemon is alive.
 *
 * Return 1 if alive or 0 if not. On error, returns a negative negative LTTng
 * error code.
 */
LTTNG_EXPORT extern int lttng_session_daemon_alive(void);

/*
 * Set the tracing group for the *current* flow of execution.
 *
 * On success, returns 0 else a negative LTTng error code.
 */
LTTNG_EXPORT extern int lttng_set_tracing_group(const char *name);

/*
 * This call registers an "outside consumer" for a session and an lttng domain.
 * No consumer will be spawned and all fds/commands will go through the socket
 * path given (socket_path).
 *
 * NOTE that this is not recommended unless you absolutely know what you are
 * doing.
 *
 * Return 0 on success else a negative LTTng error code.
 */
LTTNG_EXPORT extern int lttng_register_consumer(struct lttng_handle *handle,
						const char *socket_path);

/*
 * Start tracing for *all* domain(s) in the session.
 *
 * Return 0 on success else a negative LTTng error code.
 */
LTTNG_EXPORT extern int lttng_start_tracing(const char *session_name);

/*
 * Stop tracing for *all* domain(s) in the session.
 *
 * This call will wait for data availability for each domain of the session so
 * this can take an abritrary amount of time. However, when returning you have
 * the guarantee that the data is ready to be read and analyze. Use the
 * _no_wait call below to avoid this behavior.
 *
 * The session_name can't be NULL.
 *
 * Return 0 on success else a negative LTTng error code.
 */
LTTNG_EXPORT extern int lttng_stop_tracing(const char *session_name);

/*
 * Behave exactly like lttng_stop_tracing but does not wait for data
 * availability.
 */
LTTNG_EXPORT extern int lttng_stop_tracing_no_wait(const char *session_name);

/*
 * Deprecated: As of LTTng 2.9, this function always returns
 * -LTTNG_ERR_UND.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wshadow"
LTTNG_EXPORT extern int lttng_calibrate(struct lttng_handle *handle,
					struct lttng_calibrate *calibrate);
#pragma GCC diagnostic pop

/*
 * Set URL for a consumer for a session and domain.
 *
 * Both data and control URL must be defined. If both URLs are the same, only
 * the control URL is used even for network streaming.
 *
 * Default port are 5342 and 5343 respectively for control and data which uses
 * the TCP protocol.
 *
 * URL format: proto://[HOST|IP][:PORT1[:PORT2]][/TRACE_PATH]
 *
 * Possible protocols are:
 * > file://...
 *   Local filesystem full path.
 *
 * > net[6]://...
 *   This will use the default network transport layer which is TCP for both
 *   control (PORT1) and data port (PORT2).
 *
 * > tcp[6]://...
 *   TCP only streaming. For this one, both data and control URL must be given.
 *
 * Return 0 on success else a negative LTTng error code.
 */
LTTNG_EXPORT extern int
lttng_set_consumer_url(struct lttng_handle *handle, const char *control_url, const char *data_url);

/*
 * For a given session name, this call checks if the data is ready to be read
 * or is still being extracted by the consumer(s) (pending) hence not ready to
 * be used by any readers.
 *
 * Return 0 if there is _no_ data pending in the buffers thus having a
 * guarantee that the data can be read safely. Else, return 1 if there is still
 * traced data is pending. On error, a negative value is returned and readable
 * by lttng_strerror().
 */
LTTNG_EXPORT extern int lttng_data_pending(const char *session_name);

/*
 * Deprecated, replaced by lttng_regenerate_metadata.
 */
LTTNG_DEPRECATED("Use lttng_regenerate_metadata")
LTTNG_EXPORT extern int lttng_metadata_regenerate(const char *session_name);

/*
 * Trigger the regeneration of the metadata for a session.
 * The new metadata overwrite the previous one locally or remotely (through
 * the lttng-relayd). Only kernel, per-uid and non-live sessions are supported.
 * Return 0 on success, a negative LTTng error code on error.
 */
LTTNG_EXPORT extern int lttng_regenerate_metadata(const char *session_name);

/*
 * Trigger the regeneration of the statedump for a session. The new statedump
 * information is appended to the currently active trace, the session needs to
 * be active.
 *
 * Return 0 on success, a negative LTTng error code on error.
 */
LTTNG_EXPORT extern int lttng_regenerate_statedump(const char *session_name);

#ifdef __cplusplus
}
#endif

#endif /* LTTNG_H */
