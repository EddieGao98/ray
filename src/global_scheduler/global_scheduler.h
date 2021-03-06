#ifndef GLOBAL_SCHEDULER_H
#define GLOBAL_SCHEDULER_H

#include "task.h"

#include "state/db.h"
#include "state/local_scheduler_table.h"
#include "utarray.h"
#include "uthash.h"

/** Contains all information that is associated with a local scheduler. */
typedef struct {
  /** The ID of the local scheduler in Redis. */
  db_client_id id;
  /** The number of tasks sent from the global scheduler to this local
   *  scheduler. */
  int64_t num_tasks_sent;
  /** The number of tasks sent from the global scheduler to this local scheduler
   *  since the last heartbeat arrived. */
  int64_t num_recent_tasks_sent;
  /** The latest information about the local scheduler capacity. This is updated
   *  every time a new local scheduler heartbeat arrives. */
  local_scheduler_info info;
} local_scheduler;

typedef struct global_scheduler_policy_state global_scheduler_policy_state;

/**
 * This defines a hash table used to cache information about different objects.
 */
typedef struct {
  /** The object ID in question. */
  object_id object_id;
  /** The size in bytes of the object. */
  int64_t data_size;
  /** An array of object locations for this object. */
  UT_array *object_locations;
  /** Handle for the uthash table. */
  UT_hash_handle hh;
} scheduler_object_info;

typedef struct {
  char *aux_address; /* Key */
  db_client_id photon_db_client_id;
  UT_hash_handle hh;
} aux_address_entry;

typedef struct {
  /** The global scheduler event loop. */
  event_loop *loop;
  /** The global state store database. */
  db_handle *db;
  /** The local schedulers that are connected to Redis. TODO(rkn): This probably
   *  needs to be a hashtable since we often look up the local_scheduler struct
   *  based on its db_client_id. */
  UT_array *local_schedulers;
  /** The state managed by the scheduling policy. */
  global_scheduler_policy_state *policy_state;
  aux_address_entry *plasma_photon_map;
  /** Objects cached by this global scheduler instance. */
  scheduler_object_info *scheduler_object_info_table;
} global_scheduler_state;

/**
 * This is a helper method to look up the local scheduler struct that
 * corresponds to a particular photon_id.
 *
 * @param state The state of the global scheduler.
 * @param The photon_id of the local scheduler.
 * @return The corresponding local scheduler struct. If the global scheduler is
 *         not aware of the local scheduler, then this will be NULL.
 */
local_scheduler *get_local_scheduler(global_scheduler_state *state,
                                     db_client_id photon_id);

void assign_task_to_local_scheduler(global_scheduler_state *state,
                                    task *task,
                                    db_client_id local_scheduler_id);

#endif /* GLOBAL_SCHEDULER_H */
