#!/bin/sh

GATEWAY_HOME_DIR='/home/pi/gateway'
ENV_GATEWAY=${GATEWAY_HOME_DIR}/.env

COUNT_PROCESS=$(ps -ef|grep main.py|grep -v grep|wc -l)

if [ ${COUNT_PROCESS} -eq 0 ]; then
    nohup ${ENV_GATEWAY}/bin/python ${GATEWAY_HOME_DIR}/main.py 2>&1 > ${GATEWAY_HOME_DIR}/gateway.log &
fi
exit 0