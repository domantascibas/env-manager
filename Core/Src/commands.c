#include "commands.h"
#include "stdlib.h"
#include "version.h"
#include "reset_source.h"
#include "uart.h"

#define MAX_ITEM_COUNT          8
static const char DELIM = ',';

static const char moduleStr[] = "CMD";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)


void device_info(void) {
    PTS_f("DEVICE INFO\r\nDevice reset source: %02X", get_reset_source());//\r\nDevice UpTime: %d s", reset_source_ret(), getDeviceUpTime());
    print_version();
}

void parse_cmd(uint8_t *cmd, uint16_t sz) {
    uint8_t pos = 0;
    // uint16_t shift_len = 0;
    uint8_t items_return = 0;
    // int32_t value = 0;

    char cmd_cache[MAX_MESSAGE_LENGTH];
    memset(cmd_cache, '\0', MAX_MESSAGE_LENGTH);
    memcpy(cmd_cache, cmd, sz);

    // PTS_f("[%s] sz:%d, cache:[%s]", cmd, sz, cmd_cache);

    char *item = strtok(cmd_cache, &DELIM);
    uint8_t item_len = 0;

    while (item != NULL) {
        item_len = (uint8_t)strlen(item);
        PTS_dbg_f("%d [%s] len:%d", pos, item, item_len);

        if (*item >= '0' && *item <= '9') {
            uint8_t i = atoi(item);
            PTS_dbg_f("number, %d", i);
            //  param[pos]=atoi(item);
             items_return++;
        } else {
            // PTS_dbg("text");

            // if (find_dictionary(item, item_len, pos , &value) > (-1)) {
                // param[pos] = value;
                //printf("dict param[%d] %d\r\n",pos,param[pos]);
                items_return++;
            // } else {
                // printf("dict param[%d] [%s] NOT FOUND\r\n",pos,item);
                // return (-1);
            // }
        }
        
        item = strtok(NULL, &DELIM);
        pos++;
    }
    // for (pos = 0; pos < MAX_ITEM_COUNT; pos++) {
    //     uint8_t item_len = 0;

    //     if (item == NULL) {
    //         break;
    //     }

    //     item_len = (uint8_t)strlen(item);
    //     PTS_dbg_f("%d [%s] len:%d", pos, item, item_len);

    //     if (*item >= '0' && *item <= '9') {
    //         PTS_dbg_f("number");
    //         //  param[pos]=atoi(item);
    //         // printf("atoi param[%d] %d\r\n",pos,param[pos]);
    //         //  items_return++;
    //     } else {
    //        PTS_dbg("text");

    //         // if (find_dictionary(item, item_len, pos , &value) > (-1)) {
    //             // param[pos] = value;
    //             //printf("dict param[%d] %d\r\n",pos,param[pos]);
    //             // items_return++;
    //         // } else {
    //             // printf("dict param[%d] [%s] NOT FOUND\r\n",pos,item);
    //             // return (-1);
    //         // }
    //     }


    //     shift_len += item_len + 1;
    //     PTS_dbg_f("%d L[%d]", pos, shift_len);

    //     // if (shift_len >= sz) {
    //     //     PTS_dbg("shift_len > sz");
    //     //     break;
    //     // }

    //     // memset(cmd_cache, 0, MAX_MESSAGE_LENGTH);
    //     // memcpy(cmd_cache, cmd + shift_len, sz - shift_len);

    //     // PTS_dbg_f("%d cmd_cache[%s][%s]", pos, cmd_cache, cmd);
    //     item = strtok(NULL, &DELIM);
    //     // if (pos >= 1) {
    //     //     break;
    //     // }

    // }
    PTS_dbg_f("items ret:%d", items_return);
    PTS_dbg("------------------");
}
}
