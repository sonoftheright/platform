#!/bin/bash
cd "${0%/*}"
cd ./run
./_example > ./run_log.txt
