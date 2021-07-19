#!/usr/bin/env bash
cppcheck --std=c99 --enable=all --suppress=unusedFunction --suppress=variableScope -I./include .
