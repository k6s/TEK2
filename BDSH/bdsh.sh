#!/bin/sh

## TODO: ######################################################################
#
#	- Escape newline character in replacement string
#	- DB file delimiter ?
#	- File order by modification time aswell as insert time ?
#	- ./bdsh.sh  -f -k (select | del) 4  
#
###############################################################################

db_file='sh.db'
delim='='
v_key=0

EFNOENT='No base found : file '
ESYNTAX='Syntax error : '

usage='usage bdsh.sh [-k] [-f db_file]\
\n\t\t(put (<clef> | $<clef>) |
\n\t\t del (<clef> | $<clef>) |
\n\t\t select [<expr> | $<clef>] |
\n\t\t flush)'

## REGEXP special characters escape
## Concerned chars are delimiter (/) and \[]^$.*
escape_chars_regexp()
{
	safe_key=$(echo $key | sed \
		-e 's/\\/\\\\/g;s/\//\\\//g' \
		-e 's/\[/\\\[/g;s/\]/\\\]/g' \
	   	-e 's/\^/\\\^/g;s/\$/\\$/g' \
		-e 's/\./\\\./g;s/\*/\\\*/g')
}

## Replacement text special characters escape for sed
## Concerned characters are delimiter (/) and &\
escape_chars_repl()
{
	safe_value=$(echo "$key$delim$value" | sed \
		-e 's/\&/\\\&/g;s/\\/\\\\/g' \
		-e 's/\//\\\//g')
}

select_key_var()
{
	key=${key:1}
	escape_chars_regexp
	key=$(grep "^.*$safe_key.*$delim" $db_file)
}

select_key()
{
	n_args=$((ARGC-i))
	if [ $n_args == 0 ]; then
		safe_key='.*'
	else
		key=$((i+1))
		key=${!key}
		escape_chars_regexp
	fi
	if [ ${key:0:1} == '$' ]; then
		select_key_var
	fi
	if [ $v_key == 1 ]; then
		grep "^.*$safe_key.*$delim" $db_file
	else
		grep "^.*$safe_key.*$delim" $db_file | cut -d $delim -f 2
	fi
}

del_key()
{
	n_args=$((ARGC-i))
	if [ $n_args == 0 ]; then
		echo $ESYNTAX $usage
		exit 1
	elif [ $n_args -ge 1 ]; then
		key=$((i+1))
		key=${!key}
		if [ $key -eq '$*']; then
			select_key_var
		fi
		escape_chars_regexp
		if [ $n_args -eq 1 ]; then
			sed -i "s/\(^$safe_key$delim\).*$/\1/g" $db_file
		else
			value=$((i+2))
			value=${!value}
			escape_chars_repl
			sed -i "/^$safe_value$/d" $db_file
		fi	
	fi
}

put_key()
{
	n_args=$((ARGC-i))
	if [[ n_args -le 1 ]]; then
		echo "Invalid put command parameters: missing key"
		exit 1
	fi

	## get key and value from command line arguments.
	key=$((i+1))
	value=$((i+2))
	key=${!key}
	value=${!value}
	escape_chars_regexp "$key"

	## if file is empty or does not exists then append new entry.
	if [ ! -f $db_file ]; then
		echo "$key$delim$value" >> $db_file
		return 0
	fi

	## if key is found in db replace entry else append it.
	line=$(grep "^$safe_key$delim" $db_file)
	if [ 0 -eq $? ]; then
		escape_chars_repl
		sed -i "s/^$safe_key$delim.*$/$safe_value/g" $db_file
	else
		echo "$key$delim$value" >> $db_file
	fi
	return 2
}

flush_db()
{
	if [ -f $db_file ]; then
		echo -n '' > $db_file
	else
		echo $EFNOENT $db_file
	fi
}

get_cmdline()
{
	i=0
	while [ $i -le $ARGC ]; do
		case ${!i} in
			"put")
				put_key $ARGV
				;;
			"flush")
				flush_db
				;;
			"select")
				select_key $ARGV
				;;
			"del")
				del_key $ARGV
		esac
		i=$((i+1))
	done
}

get_options()
{
	local OPTIND opt a
	while getopts 'f:k' opt; do
		case $opt in
			f)
				db_file=$OPTARG
				;;
			k)
				v_key=1
				;;
			\?)
				echo "Invalid option: -$OPTARG"
				exit 1
				;;
			:)
				echo "Option -$OPTARG requires an argument"
				exit 1
				;;
		esac
	done
}

ARGC=$#
ARGV=$@
get_options "$@"
get_cmdline $ARGV $OPTIND
exit 0
