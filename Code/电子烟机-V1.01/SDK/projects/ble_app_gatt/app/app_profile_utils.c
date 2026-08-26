#include "app_profile_utils.h"

#include "app_task.h"
#include "gapm_task.h"
#include "ke_msg.h"

struct gapm_profile_task_add_cmd *app_profile_add_cmd_alloc(uint16_t prf_task_id,
                                                            uint16_t db_cfg_size)
{
    struct gapm_profile_task_add_cmd *req =
        KE_MSG_ALLOC_DYN(GAPM_PROFILE_TASK_ADD_CMD,
                         TASK_GAPM,
                         TASK_APP,
                         gapm_profile_task_add_cmd,
                         db_cfg_size);

    req->operation = GAPM_PROFILE_TASK_ADD;
    req->sec_lvl = 0;
    req->prf_task_id = prf_task_id;
    req->app_task = TASK_APP;
    req->start_hdl = 0;

    return req;
}
