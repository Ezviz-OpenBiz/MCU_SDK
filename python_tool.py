import json

str_pro_up_funcs = ""
str_event_up_funcs = ""
str_pro_handle_funcs = ""
str_atc_handle_funcs = ""
str_event_handle_funcs = ""
str_pro_cmd = ""
str_action_cmd = ""
str_event_cmd = ""
file_in = open('GEB4F721.json', encoding='utf-8')
ret = file_in.read()
total = json.loads(ret)
array_resources = total['resources']
obj_resources = array_resources[0]
array_domains = obj_resources['domains']
for i in range(len(array_domains)):
    data = array_domains[i]
    if(data['props'] != []):
        prop = data['props']
        for i in range(len(prop)):
            key = prop[i]
            schema = key['schema']
            type = ""
            if(schema['type'] == 'boolean'):
                type = "tsl_data_type_bool"
            elif(schema['type'] == 'integer'):
                type = "tsl_data_type_int"
            elif(schema['type'] == 'number'):
                type = "tsl_data_type_number"
            elif(schema['type'] == 'string'):
                type = "tsl_data_type_string"
            elif(schema['type'] == 'array'):
                type = "tsl_data_type_array"
            elif(schema['type'] == 'object'):
                type = "tsl_data_type_object"
            str_pro_handle_funcs = str_pro_handle_funcs+"static void fn_property_" + \
                key['identifier'] + \
                "(char *data,char *value)\n{\n\n\n" + \
                "    fn_property_"+key['identifier']+"_sync();\n}\n"
            str_pro_up_funcs = str_pro_up_funcs+"void fn_property_" + \
                key['identifier']+"_sync ()\n{\n    char szIdentifier[32]=\"" + \
                key['identifier'] + \
                "\";\n    char data[32];\n\n    property_up_handler_entry(szIdentifier,data);\n}\n"
            str_pro_cmd = str_pro_cmd+"    {"+"\""+data['identifier']+"\""+","+"\"" + \
                key['identifier']+"\""+","+type+"," + \
                "fn_property_"+key['identifier']+"},\n"
    if(data['actions'] != []):
        action = data['actions']
        for i in range(len(action)):
            key = action[i]
            str_atc_handle_funcs = str_atc_handle_funcs+"static void fn_action_" + \
                key['identifier']+"(char *data,char *value)\n{\n\n\n"+"\n}\n"
            str_action_cmd = str_action_cmd + \
                "    {"+"\""+data['identifier']+"\""+","+"\"" + \
                key['identifier']+"\""+"," + \
                "fn_action_"+key['identifier']+"},\n"
    if(data['events'] != []):
        event = data['events']
        for i in range(len(event)):
            key = event[i]
            str_event_up_funcs = str_event_up_funcs+"void fn_event_" + \
                key['identifier']+"_sync (char *data)\n{\n    char szIdentifier[32]="+"\"" + \
                key['identifier'] + \
                "\";\n    char data[32];\n\n    event_up_handler_entry(szIdentifier,data);\n}\n"
            str_event_cmd = str_event_cmd + \
                "    {"+"\""+data['identifier']+"\""+","+"\"" + \
                key['identifier']+"\""+","+"fn_event_"+key['identifier']+"},\n"
str_pro_cmd = str_pro_cmd+"    {NULL,NULL,NULL}"
str_action_cmd = str_action_cmd+"    {NULL,NULL,NULL}"
str_event_cmd = str_event_cmd+"    {NULL,NULL,NULL}"
file_out = open('./1.mcu_sdk/tsl_protocol.c', "w")
file_out.write("/*******************************************************************************\n * Copyright  2017-2022 Ezviz Inc.\n * @file:    tsl_protocol.c * @brief:   at????????????????????????????????????????????????\n *				1. ??????????????????????????????????????????????????????????????????????????????????????????????????????????????????\n *				2. ?????????????????????????????????????????????????????????????????????????????????????????????????????????????????????\n *				    ??????????????????????????????????????????????????????????????????API\n *          	3. ?????????????????????????????????????????????????????????????????????????????????????????????????????????mcu??????????????????\n * Change Logs:\n * Date           Author       Notes\n * 2022           Lee&Young    ????????????????????????,????????????????????????????????????????????????MCU_SDK\n *******************************************************************************/\n"
               +"#include \"at_protocol_tsl.h\"\n#include <stdio.h>\n#include \"mcu_at_system.h\"\n#include \"string.h\"\n#include \"stdlib.h\""
               + "\n//???????????????????????????????????????//"
               + "\n"+str_pro_up_funcs
               + "\n"+str_event_up_funcs
               + "\n//??????????????????????????????????????????//"
               + "\n"+str_pro_handle_funcs
               + "\n"+str_atc_handle_funcs
               + "\n//?????????????????????????????????//"
               + "\nAT_PROPERITY_CMD property_cmd[]=\n{\n"+str_pro_cmd+"\n};"
               + "\nAT_ACTION_CMD action_cmd[]=\n{\n"+str_action_cmd+"\n};"
               + "\nAT_EVENT_CMD event_cmd[]=\n{\n"+str_event_cmd+"\n};")
file_in.close()
file_out.close()
