#ifndef APP_PROFILE_UTILS_H_
#define APP_PROFILE_UTILS_H_

#include <stdint.h>

struct gapm_profile_task_add_cmd;

struct gapm_profile_task_add_cmd *app_profile_add_cmd_alloc(uint16_t prf_task_id,
                                                            uint16_t db_cfg_size);

#endif
