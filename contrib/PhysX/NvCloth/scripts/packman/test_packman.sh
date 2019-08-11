#!/bin/bash

export PM_MODULE_EXT="$(dirname ${BASH_SOURCE})/../common/packman.py"

source "$(dirname ${BASH_SOURCE})/packman.sh" $*

unset PM_MODULE_EXT

