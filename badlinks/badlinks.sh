#!/usr/bin/env bash

find -L "$1" -type l -mtime +7 -exec test ! -e {} \; -print
