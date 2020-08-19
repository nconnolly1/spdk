#!/usr/bin/env bash

find . -type d -print |
while read d
do
	[ -f $d/meson.build ] && continue
	(
		cd $d
		echo ==$d==
		sources=
		for i in *.c *.cc
		do
			[ "$i" == "*.c" ] && continue
			[ "$i" == "*.cc" ] && continue
			[ -n "$sources" ] && sources="$sources, "
			sources="$sources'$i'"
		done

		headers=
		for i in *.h
		do
			[ "$i" == "*.h" ] && continue
			[ -n "$headers" ] && headers="$headers, "
			headers="$headers'$i'"
		done

		if [ -n "$sources$headers" ]
		then
			cat <<! > meson.build
# SPDX-License-Identifier: BSD-3-Clause
# Copyright(c) 2017 Intel Corporation

includes = [global_inc]

!
			[ -n "$sources" ] && echo "sources = files($sources)" >> meson.build
			[ -n "$headers" ] && echo "headers = files($headers)" >> meson.build
		fi
	)	
done
