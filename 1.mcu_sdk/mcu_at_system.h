#ifndef __MCU_AT_SYSTEM_H_
#define __MCU_AT_SYSTEM_H_

/**
 * @description: 配网函数，调用后发AT+WDST=0,5\r\n使模组进入配网状态
 * @return {*}
 */
void distribution_network(void);
/**
 * @description: 发送ATZ\r\n，清除用户数据，重置模组
 * @return {*}
 */
void reset(void);
/**
 * @description: 属性功能点查询处理函数，此函数是模组配上网后会下发URC+ECTPG属性查询命令，MCU需要上报属性功能点的值，好让APP面板上显示
 * @param {char} *identifier
 * @return {*}
 */
void property_get_handler_entry(char *identifier);
/**
 * @description: 属性功能点设置处理函数
 * @param {char} *identifier
 * @param {char} *value
 * @return {*}
 */
void property_set_handler_entry(char *identifier, char *value);
/**
 * @description: 操作功能点设置处理函数
 * @param {char} *identifier
 * @param {char} *value
 * @return {*}
 */
void action_set_handler_entry(char *identifier, char *value);
/**
 * @description: 属性功能点上报处理函数
 * @param {char} *identifier
 * @param {char} *value
 * @return {*}
 */
void property_up_handler_entry(char *identifier, char *value);
/**
 * @description: 事件功能点上报函数
 * @param {char} *identifier
 * @param {char} *value
 * @return {*}
 */
void event_up_handler_entry(char *identifier, char *value);
/**
 * @description: 收到属性设置URC命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_urc_property_set(char *recvbuf, int size);
/**
 * @description: 收到属性查询URC命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_urc_property_get(char *recvbuf, int size);
/**
 * @description: 收到操作设置URC命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_urc_action_set(char *recvbuf, int size);
/**
 * @description: 收到OTAST的URC命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_urc_otast(char *recvbuf, int size);
/**
 * @description: 收到+OTAST就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_otast(char *recvbuf, int size);
/**
 * @description: 收到ECSTAC的URC命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_urc_ecstac(char *recvbuf, int size);
/**
 * @description: 收到OTAAC的URC命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_urc_otaac(char *recvbuf, int size);
/**
 * @description: 收到TMC的URC命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_urc_tmc(char *recvbuf, int size);
/**
 * @description: 收到+TMG命令就会调用此函数
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_tmg(char *recvbuf, int size);
/**
 * @description: 发送产测第一步命令
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_ptconnect(char *recvbuf, int size);
/**
 * @description: 这个函数里会解析出RSSI
 * @param {char} *recvbuf
 * @param {int} size
 * @return {*}
 */
void fn_ptgetrssi(char *recvbuf, int size);
/**
 * @description: 串口发送数据
 * @param {unsigned char} *in
 * @param {unsigned short} len
 * @return {*}
 */
void uart_send_data(unsigned char *in, unsigned short len);
/**
 * @description: 串口发送一字节
 * @param {unsigned char} value
 * @return {*}
 */
void one_send_byte(unsigned char value);
/**
 * @description: 调用这个函数进行联网能力的产测
 * @return {*}
 */
void product_test(void);

#endif
