/******************************************************************************
 * @brief        AT指令通信管理
 *
 * Copyright (c) 2020~2021, <morro_luo@163.com>
 *
 * SPDX-License-Identifier: Apathe-2.0
 *
 * Change Logs: 
 * Date           Author       Notes 
 * 2020-01-02     Morro        初版
 * 2021-01-20     Morro        增加debug调试接口, 解决链表未初始化导至段错误的问题
 *                             调通单行命令、多行命令、自定义命令等接口
 * 2021-03-03     Morro        解决未清除URC计数器导致频繁打印接收超时的问题
 ******************************************************************************/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "tsl_protocol.h"
#include "at_parse.h"
#include "mcu_at_system.h"
#include "download.h"
/***************测试示例代码***************/
#include "flash.h"
#include "usart1.h"
#include "usart3.h"
/*************************************/
#define AT_IS_TIMEOUT(start, time) (AT_GET_TICK() - (start) > (time))
/**AT作业类型(实际是4种类型的状态机轮询程序) -----------------------------------*/
#define AT_TYPE_WORK 0 /* 普通作业 ----------*/
#define AT_TYPE_CMD 1  /* 标准命令 ----------*/
#define AT_TYPE_SINGLLINE 3 /* 单行命令 ----------*/
#define min(a, b) ((a) < (b)) ? (a) : (b)
#ifndef AT_DEBUG
#define AT_DEBUG(...) \
    do                \
    {                 \
    } while (0)
#endif

typedef int (*base_work)(at_obj_t *at, ...);
at_obj_t at;
static void at_send_line(at_obj_t *at, const char *fmt, va_list args);
static uint8_t wifi_recvbuf[256];
static uint8_t wifi_urcbuf[128];
static volatile uint32_t tick; //滴答计时

/******************环形数组*********************/
static ring_buf_t rbsend, rbrecv;
static uint8_t rxbuf[TXBUF_SIZE];
static uint8_t txbuf[RXBUF_SIZE];

static int flag=0;
		
bool ring_buf_init(ring_buf_t *r, uint8_t *buf, uint32_t len)     //1.需要放在main函数里初始化
{
    r->buf = buf;
    r->size = len;
    r->front = r->rear = 0;
    return buf != NULL && (len & len - 1) == 0;
}

uint32_t ring_buf_put(ring_buf_t *r, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t left;
    left = r->size + r->front - r->rear;
    len = min(len, left);
    i = min(len, r->size - (r->rear & r->size - 1));
    memcpy(r->buf + (r->rear & r->size - 1), buf, i);
    memcpy(r->buf, buf + i, len - i);
    r->rear += len;
    return len;
}

uint32_t ring_buf_get(ring_buf_t *r, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    uint32_t left;
    left = r->rear - r->front;
    len = min(len, left);
    i = min(len, r->size - (r->front & r->size - 1));
    memcpy(buf, r->buf + (r->front & r->size - 1), i);
    memcpy(buf + i, r->buf, len - i);
    r->front += len;
    return len;
}

/******************环形数组*********************/

/******************单片机串口中断示例函数*********************/
void USART1_IRQHandler(void)
{
    uint8_t data;
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
    {
        data = USART_ReceiveData(USART1);
        ring_buf_put(&rbrecv, &data, 1);                 //一字节一字节收到接收环形数组里做个缓冲
    }
    if (USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
    {
        if (ring_buf_get(&rbsend, &data, 1))
            USART_SendData(USART1, data);               //一字节一字节从发送环形数组里读出来发送
        else
        {
            USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
        }
    }
}
/******************重写单片机串口中断示例函数*********************/

uint32_t wifi_uart_read(void *buf, uint32_t len)
{
    return ring_buf_get(&rbrecv, (uint8_t *)buf, len); //取一些数据到接收环形数组里
}

uint32_t wifi_uart_write(const void *buf, uint32_t len)
{
    uint32_t ret;
    ret = ring_buf_put(&rbsend, (uint8_t *)buf, len);  //把一堆数据推到发送环形数组里
    /***************测试代码***************/
    USART_ITConfig(USART1, USART_IT_TXE, ENABLE);            //开启TX中断使能
    /*************************************/
    return ret;
}


static const inline at_adapter_t *__get_adapter(at_obj_t *at)
{
    return &at->adap;
}

static bool at_is_timeout(at_obj_t *at, uint32_t ms)
{
    return AT_IS_TIMEOUT(at->timer, ms);
}
/**
 * @brief   发送数据
 */
static void send_data(at_obj_t *at, const void *buf, uint32_t len)
{
    at->adap.write(buf, len);
}
/**
 * @brief       格式化打印
 */
static void print(at_obj_t *at, const char *cmd, ...)
{
    va_list args;
    va_start(args, cmd);
    at_send_line(at, cmd, args);
    va_end(args);
}
/**
 * @brief   获取当前数据接收长度
 */
static uint32_t get_recv_count(at_obj_t *at)
{
    return at->recv_cnt;
}
/**
 * @brief   获取数据缓冲区
 */
static char *get_recv_buf(at_obj_t *at)
{
    return (char *)at->adap.recv_buf;
}
/**
 * @brief   清除数据缓冲区
 */
static void recv_buf_clear(at_obj_t *at)
{
    at->recv_cnt = 0;
}
/**前向查找字串*/
static char *search_string(at_obj_t *at, const char *str)
{
    return strstr(get_recv_buf(at), str);
}
/**
 * @brief   终止执行
 */
static bool at_isabort(at_obj_t *at)
{
    return at->cursor ? at->cursor->abort : 1;
}
/**
 * @brief  重置计时器
 */
static void at_reset_timer(at_obj_t *at)
{
    at->timer = AT_GET_TICK();
}
/**
 * @brief  AT执行回调
 */
static void do_at_callbatk(at_obj_t *a, at_item_t *i, at_callbatk_t cb, at_return ret)
{
    at_response_t r;
    if ((ret == AT_RET_ERROR || ret == AT_RET_TIMEOUT) && a->adap.error != NULL)
        a->adap.error();

    if (cb)
    {
        r.param = i->param;
        r.recvbuf = get_recv_buf(a);
        r.recvcnt = get_recv_count(a);
        r.ret = ret;
        cb(&r);
    }
}
/* 添加作业到队列*/
static bool add_work(at_obj_t *at, void *params, void *info, int type)
{
    at_item_t *i;
    if (list_empty(&at->ls_idle))
        return NULL;
    i = list_first_entry(&at->ls_idle, at_item_t, node); 
    i->info = (void *)info;
    i->param = (void *)params;
    i->state = AT_STATE_WAIT;
    i->type = type;
    i->abort = 0;
    list_move_tail(&i->node, &at->ls_ready);
    return i != 0;
}
/**
 * @brief  执行作业
 */
static int do_work_handler(at_obj_t *at)
{
    at_item_t *i = at->cursor;
    return ((int (*)(at_env_t * e)) i->info)(&at->env);
}
/**
 * @brief       通用命令处理
 * @param[in]   a - AT控制器
 * @return      true - 结束作业, false - 保持作业,
 */
static int do_cmd_handler(at_obj_t *a)
{
    at_item_t *i = a->cursor;
    at_env_t *e = &a->env;
    const at_cmd_t *c = (at_cmd_t *)i->info;
    switch (e->state)
    {
    case 0: 
        c->sender(e);
        e->state++;
        e->reset_timer(a);
        e->recvclr(a);
        break;
    case 1:
        if (search_string(a, c->matcher))
        { 
            AT_DEBUG("<-\r\n%s\r\n", get_recv_buf(a));
            do_at_callbatk(a, i, c->cb, AT_RET_OK);
            return true;
        }
        else if (search_string(a, "ERROR"))
        {
            AT_DEBUG("<-\r\n%s\r\n", get_recv_buf(a));
            if (++e->i >= c->retry)
            {
                do_at_callbatk(a, i, c->cb, AT_RET_ERROR);
                return true;
            }
            e->state = 2;      
            e->reset_timer(a); 
        }
        else if (e->is_timeout(a, c->timeout))
        { 
            if (++e->i >= c->retry)
            {
                do_at_callbatk(a, i, c->cb, AT_RET_TIMEOUT);
                return true;
            }
            e->state = 0; 
        }
        break;
    case 2:
        if (e->is_timeout(a, 500))
            e->state = 0; 
        break;
    default:
        e->state = 0;
    }
    return false;
}
/**
 * @brief       单行命令处理
 * @param[in]   a - AT控制器
 * @return      true - 结束作业, false - 保持作业,
 */
static int send_signlline_handler(at_obj_t *a)
{
    at_item_t *i = a->cursor;
    at_env_t *e = &a->env;
    const char *cmd = (const char *)i->param;
    at_callbatk_t cb = (at_callbatk_t)i->info;

    switch (e->state)
    {
    case 0: /*发送状态 ------------------------------------------------------*/
        e->printf(a, cmd);
        e->state++;
        e->reset_timer(a);
        e->recvclr(a);
        break;
    case 1:
        if (search_string(a, "OK"))
        {
            AT_DEBUG("<-\r\n%s\r\n", get_recv_buf(a));
            do_at_callbatk(a, i, cb, AT_RET_OK); //接收匹配
            return true;
        }
        else if (search_string(a, "ERROR"))
        {
            AT_DEBUG("<-\r\n%s\r\n", get_recv_buf(a));
            if (++e->i >= 3)
            {
                do_at_callbatk(a, i, cb, AT_RET_ERROR);
                return true;
            }
            e->state = 2; /*出错之后延时一段时间*/
            e->reset_timer(a); /*重置定时器*/
        }
        else if (e->is_timeout(a, 3000 + e->i * 2000))
        {
            if (++e->i >= 3)
            {
                do_at_callbatk(a, i, cb, AT_RET_TIMEOUT);
                return true;
            }
            e->state = 0; /*返回上一状态*/
        }
        break;
    case 2:
        if (e->is_timeout(a, 500))
            e->state = 0; /**返回初始状态*/
        break;
    default:
        e->state = 0;
    }
    return false;
}
/**
 * @brief       发送行
 * @param[in]   fmt    - 格式化输出
 * @param[in]   args   - 可变参数列表
 */
static void at_send_line(at_obj_t *at, const char *fmt, va_list args)
{
    char buf[MAX_AT_CMD_LEN];
    int len;
    const at_adapter_t *adt = __get_adapter(at);
    len = vsnprintf(buf, sizeof(buf), fmt, args);

    recv_buf_clear(at); 
    send_data(at, buf, len);
    send_data(at, "\r\n", 2);

    AT_DEBUG("->\r\n%s\r\n", buf);
}
/**
 * @brief       urc(unsolicited code) 处理总入口
 * @param[in]   urc    - 接收缓冲区
 * @param[in]   size   - 缓冲区大小
 * @return      none
 */
static void urc_handler_entry(at_obj_t *at, char *urc, uint32_t size)
{
    int i, n;
    utc_item_t *tbl = at->adap.utc_tbl;
    for (i = 0; i < at->adap.urc_tbl_count; i++, tbl++)
    {
        n = strlen(tbl->prefix);
        if (strncmp(urc, tbl->prefix, n) == 0)
        {                                        
            tbl->handler(urc, size); 
            break;
        }
    }
    if (at->cursor == NULL)
        AT_DEBUG("<=\r\n%s\r\n", urc);
}
/**
 * @brief       urc 接收处理
 * @param[in]   buf  - 数据缓冲区
 * @return      none
 */
static void urc_recv_process(at_obj_t *at, uint8_t *buf, uint32_t size)
{
    char *urc_buf;
    int ch;
    unsigned short urc_size;
    urc_buf = (char *)at->adap.urc_buf;
    urc_size = at->adap.urc_bufsize;
    if (size == 0 && at->urc_cnt > 0)
    {
        if (AT_IS_TIMEOUT(at->urc_timer, 2000))
        { 
            urc_handler_entry(at, urc_buf, at->urc_cnt);
            if (at->urc_cnt > 1)
                AT_DEBUG("Urc recv timeout.\r\n");
            at->urc_cnt = 0;
        }
    }
    else if (urc_buf != NULL)
    {
        at->urc_timer = AT_GET_TICK();
        while (size--)
        {
            ch = *buf++;
            urc_buf[at->urc_cnt++] = ch;
            if (ch == '\n' || ch == '\r' || ch == '\0')
            {
                urc_buf[at->urc_cnt] = '\0';
                if (at->urc_cnt > 2)
                    urc_handler_entry(at, urc_buf, at->urc_cnt);
                at->urc_cnt = 0;
            }
            else if (at->urc_cnt >= urc_size) 
                at->urc_cnt = 0;
        }
    }
}
/**
 * @brief       指令响应接收处理
 * @return      none
 */
static void resp_recv_process(at_obj_t *at, const uint8_t *buf, uint32_t size)
{
    char *recv_buf;
    unsigned short recv_size;

    recv_buf = (char *)at->adap.recv_buf;
    recv_size = at->adap.recv_bufsize;

    if (at->recv_cnt + size >= recv_size) 
        at->recv_cnt = 0;

    memcpy(recv_buf + at->recv_cnt, buf, size);
    at->recv_cnt += size;
    recv_buf[at->recv_cnt] = '\0';
}
/**
 * @brief       AT控制器初始化
 * @param[in]   at   - AT控制器
 * @param[in]   adap - AT适配器
 */
void at_obj_init(at_obj_t *at, const at_adapter_t *adap)
{
    int i;
    at_env_t *e;
    at->adap = *adap;
    e = &at->env;
    at->recv_cnt = 0;

    e->is_timeout = at_is_timeout;
    e->printf = print;
    e->recvbuf = get_recv_buf;
    e->recvclr = recv_buf_clear;
    e->recvlen = get_recv_count;
    e->find = search_string;
    e->abort = at_isabort;
    e->reset_timer = at_reset_timer;

    INIT_LIST_HEAD(&at->ls_idle);
    INIT_LIST_HEAD(&at->ls_ready);

    for (i = 0; i < sizeof(at->items) / sizeof(at_item_t); i++)
        list_add_tail(&at->items[i].node, &at->ls_idle);

    while (adap->recv_buf == NULL)
    {
    } 
}
/**
 * @brief       执行AT作业(自定义作业)
 * @param[in]   a      - AT控制器
 * @param[in]   work   - AT作业入口
 * @param[in]   params - 
 */
bool at_do_work(at_obj_t *at, int (*work)(at_env_t *e), void *params)
{
    return add_work(at, params, (void *)work, AT_TYPE_WORK);
}
/**
 * @brief       执行AT指令
 * @param[in]   a - AT控制器
 * @param[in]   cmd   - cmd命令
 */
bool at_do_cmd(at_obj_t *at, void *params, const at_cmd_t *cmd)
{
    return add_work(at, params, (void *)cmd, AT_TYPE_CMD);
}
/**
 * @brief       发送单行AT命令(默认等待OK返回, 3s超时)
 * @param[in]   at          - AT控制器
 * @param[in]   cb          - 执行回调
 * @param[in]   singlline   - 单行命令
 * @note        在命令执行完毕之前,singlline必须始终有效
 */
bool at_send_singlline(at_obj_t *at, at_callbatk_t cb, const char *singlline)
{
    return add_work(at, (void *)singlline, (void *)cb, AT_TYPE_SINGLLINE);
}
/**
 * @brief   强行中止AT作业
 */
void at_item_abort(at_item_t *i)
{
    i->abort = 1;
}
/**
 * @brief   AT忙判断
 * @return  true - 有AT指令或者任务正在执行中
 */
bool at_obj_busy(at_obj_t *at)
{
    return !list_empty(&at->ls_ready);
}
/**
 * @brief   AT作业管理
 */
static void at_work_manager(at_obj_t *at)
{
    at_env_t *e = &at->env;
    /*作业处理表 --------------------------------------------------------------*/
    static int (*const work_handler_table[])(at_obj_t *) = {
        do_work_handler,
        do_cmd_handler,
        send_signlline_handler};
    if (at->cursor == NULL)
    {
        if (list_empty(&at->ls_ready)) //就绪链为空
            return;
        e->i = 0;
        e->j = 0;
        e->state = 0;
        at->cursor = list_first_entry(&at->ls_ready, at_item_t, node);
        e->params = at->cursor->param;
        e->recvclr(at);
        e->reset_timer(at);
    }
    /* 工作执行完成,则将它放入到空闲工作链 -------------------------------------*/
    if (work_handler_table[at->cursor->type](at) || at->cursor->abort)
    {
        list_move_tail(&at->cursor->node, &at->ls_idle);
        at->cursor = NULL;
    }
}
/**
 * @brief  AT轮询任务
 */
void at_poll_task(at_obj_t *at)
{
    int read_size;
    read_size = __get_adapter(at)->read(at->adap.recv_buf, sizeof(at->adap.recv_buf));
    urc_recv_process(at, at->adap.recv_buf, read_size);
    resp_recv_process(at, at->adap.recv_buf, read_size);
    at_work_manager(at);
}
/* 
 * @brief   wifi URC
 */
static const utc_item_t urc_table[] = {
    "URC+ECTPG",
    fn_urc_property_get,
    "URC+ECTPS",
    fn_urc_property_set,
    "URC+ECTAS",
    fn_urc_action_set,
    "URC+OTAST",
    fn_urc_otast,
	  "+OTAST",
	  fn_otast,
    "URC+ECSTAC",
    fn_urc_ecstac,
    "URC+OTAAC",
    fn_urc_otaac,
	  "URC+TMC",
	  fn_urc_tmc,
	  "+TMG",
	  fn_tmg,
	  "+PTCONNECT",
	  fn_ptconnect,
	  "+PTGETRSSI",
		fn_ptgetrssi,
};//解析简单点可以解析前面接收环形数组，以\r\n为结束符匹配这个表的字段和处理函数就可以完成解析工作

static const at_adapter_t at_adapter = {
    .write = wifi_uart_write,
    .read = wifi_uart_read,
    .utc_tbl = (utc_item_t *)urc_table,
    .urc_buf = wifi_urcbuf,
    .recv_buf = wifi_recvbuf,
    .urc_tbl_count = sizeof(urc_table) / sizeof(urc_table[0]),
    .urc_bufsize = sizeof(wifi_urcbuf),
    .recv_bufsize = sizeof(wifi_recvbuf)
};

void wifi_init(void)
{
    at_obj_init(&at, &at_adapter);
}
driver_init("wifi", wifi_init);

void wifi_task(void)
{
    at_poll_task(&at);
}
task_register("wifi", wifi_task, 10);

/*
 * @brief   增加系统节拍数(定时器中断中调用,1ms 1次，1s1000次)
 */
void systick_increase(uint32_t ms)                     //2.需要放在定时器中断里
{
    tick += ms;
}

/*
 * @brief       获取系统滴答时钟值(通常单位是1ms)
 */
uint32_t get_tick(void)
{
    return tick;
}

/*
 * @brief       超时判断
 * @param[in]   start   - 起始时间
 * @param[in]   timeout - 超时时间(ms)
 */
bool is_timeout(uint32_t start, uint32_t timeout)
{
    return get_tick() - start > timeout;
}

/*
 * @brief       空处理,用于定位段入口
 */
static void nop_process(void)
{

}

//第一个初始化项
const init_item_t init_tbl_start SECTION("init.item.0") = {
    "", nop_process};
//最后个初始化项
const init_item_t init_tbl_end SECTION("init.item.4") = {
    "", nop_process};

//第一个任务项
const task_item_t task_tbl_start SECTION("task.item.0") = {
    "", nop_process};
//最后个任务项
const task_item_t task_tbl_end SECTION("task.item.2") = {
    "", nop_process};


/*
 * @brief       模块初始处理
 *              初始化模块优化级 system_init > driver_init > module_init
 * @param[in]   none
 * @return      none
 */
void module_task_init(void)                                   //3.需要在main函数里初始化
{
    ring_buf_init(&rbsend, txbuf, sizeof(txbuf));
	ring_buf_init(&rbrecv, rxbuf, sizeof(rxbuf));
    const init_item_t *it = &init_tbl_start;
    while (it < &init_tbl_end)
    {
        it++->init();
    }
}

/*
 * @brief       任务轮询处理
 * @param[in]   none
 * @return      none
 */
void module_task_process(void)                   
{
    const task_item_t *t;
    for (t = &task_tbl_start + 1; t < &task_tbl_end; t++)
    {
        if ((get_tick() - *t->timer) >= t->interval)
        {
            *t->timer = get_tick();
            t->handle();
        }
    }
}

void set_flag(int value)
{
    flag = value;
}

void do_poll_handler(void)
{
    switch(flag)
		{
			case 1:
			{	
        /***************测试示例代码***************/				
				NVIC_usart1_Configuration_Disable();
                //NVIC_usart3_Configuration_Disable();
                /*****************************************/
				SerialDownload();
				flag = 2;
			}
			break;
			case 2:
			{
				/***************测试示例代码***************/
				        NVIC_usart1_Configuration();                //命令通信串口，TX：PA9, RX：PA10
                //NVIC_usart3_Configuration();
                /******************************************/				
				flag = 0;
			}
			break;
			case 3:
			{			
				/***************测试示例代码***************/
				FlashErase(0x0800F800,0x0800F800+4);
				Writeflash(0x0800F800,0x55555555);      //写入升级成功标识
				FlashErase(0x08010004,0x08010004+4);    
				if(SCB->VTOR==0x08002000)
				{
					Writeflash(0x08010004,0x08008C00);    //重新启动后应用区APP2地址
				}
				if(SCB->VTOR==0x08008C00)
				{
					Writeflash(0x08010004,0x08002000);    //重新启动后应用区APP2地址
				}
				NVIC_SystemReset();
				/****************************************/
			}
			break;
			default:
			{
			  module_task_process();                   /*任务轮询*/	
			}
			break;
		}			
}
