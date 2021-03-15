#include "FreeRTOS.h"
#include "task.h"
#include "cmd_parser.h"
#include "stdlib.h"
#include "version.h"
#include "reset_source.h"
#include "uart.h"
#include "task_manager.h"

#define CMD_DEBUG_EN            0
#define MAX_ITEM_COUNT          8
static const char DELIM = ',';

static const char moduleStr[] = "CMD";
#define PTS_dbg(fmt, ...) PTS_d(moduleStr, fmt, ##__VA_ARGS__)
#define PTS_dbg_f(fmt, ...) PTS_df(moduleStr, fmt, ##__VA_ARGS__)

void CmdHelp(void);
void CmdGetInfo(void);
void CmdGetVersion(void);
void CmdGetVref(void);
void CmdBlink(void);

const CommandStruct_t commands[] = {
    {"help",            &CmdHelp,               "Print all commands"},
    {"info",            &CmdGetInfo,            "Display device information"},
    {"version",         &CmdGetVersion,         "Display firmware version"},
    {"vref",            &CmdGetVref,            "Print Vref"},
    {"blink",           &CmdBlink,              "blink"},
    {NULL,              NULL,                   NULL}     // End of table indicator. MUST BE LAST!!!
};

void parse_cmd(uint8_t *cmd, uint16_t sz) {
    uint8_t i;
    for (i = 0; i < sizeof(commands) / sizeof(CommandStruct_t); i++) {
#if CMD_DEBUG_EN
        PTS_dbg_f("cmd[%d]:%s, cmd:%s, eq?:%d", i, commands[i].name, cmd, strcmp(commands[i].name, cmd));
#endif
        if (strcmp(commands[i].name, (char *)cmd) == 0) {
#if CMD_DEBUG_EN
            PTS_dbg_f("cmd: %s, %s", commands[i].name, commands[i].help);
#endif
            commands[i].execute();
            return;
        }
    }
    PTS_dbg("unknown cmd");
}

void CmdHelp(void) {
    PTS_f("Available commands:");
    uint8_t i;
    for (i = 0; i < sizeof(commands) / sizeof(CommandStruct_t); i++) {
        if (commands[i].name == NULL) break;
        PTS_f(" - %-12s %s", commands[i].name, commands[i].help);
    }
    PTS("");
}

void CmdGetInfo(void) {
    PTS("DEVICE INFO");
    print_mcu_id_code();
    print_reset_source();
    PTS_f("Up-time: %ld s", xTaskGetTickCount() / 1000 + 1);
    print_version();
    PTS("");
}

void CmdGetVersion(void) {
    print_version_str();
}

void CmdGetVref(void) {
    PTS_dbg("vref cmd");
}

void CmdBlink(void) {
    task_toggle();
}

void adv_parse_cmd(uint8_t *cmd, uint16_t sz) {
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
