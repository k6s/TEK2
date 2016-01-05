#!/bin/sh

## TODO: ######################################################################
#
#	- newline in key/value ?
#
###############################################################################

delim='='
v_key=0

EFNOENT='No base found : file '
EKNOENT='No such key : '
ESYNTAX='Syntax error : '

usage='Usage : bdsh.sh [-k] [-f db_file]
(put (<clef> | $<clef>) |
del (<clef> | $<clef>) |
select [<expr> | $<clef>] |
flush)'

## REGEXP special characters escape
## Concerned chars are delimiter (/) and \[]^$.*
escape_chars_regexp()
{
	safe_key=$(sed -e 's/\\/\\\\/g;s/\//\\\//g' \
		-e 's/\[/\\\[/g;s/\]/\\\]/g' \
	   	-e 's/\^/\\\^/g;s/\$/\\\$/g' \
		-e 's/\./\\\./g;s/\*/\\\*/g' <<< "${key}")
}

## Replacement text special characters escape for sed
## Concerned characters are delimiter (/) and &\
escape_chars_repl()
{
	safe_value=$(sed -e 's/\&/\\\&/g;s/\\/\\\\/g' \
		-e 's/\//\\\//g' <<< "${key}$delim${value}")
}

select_key_var()
{
	key="${key:1}"
	key_name="${key}"
	escape_chars_regexp
	select_regexp="^${safe_key}$"
	while read -r line; do
		len=$(cut -d $delim -f1 <<< "${line}")
		key=$(cut -d $delim -f2- <<< "${line}"| cut -b -${len} \
			| grep "${select_regexp}")
		if [ "k${key}" != "k" ]; then
			escape_chars_regexp
			key_val=$(cut -d $delim -f 2- <<< "${line}" \
				| grep "^${safe_key}$delim" | cut -b $((len+2))-)
			return 
		fi
	done < "${db_file}"
	echo $EKNOENT \'"${key_name}"\' 1>&2
	exit 1
}

select_val_var()
{
	ktmp="${key}"
	key="${value}"
	select_key_var
	value="${key_val}"
	key="${ktmp}"
}

select_key()
{
	if [ $n_args -ge 3 ]; then
		echo $ESYNTAX $USAGE 1>&2
		exit 1
	fi
	if [ $n_args == 0 ]; then
		safe_key='.*'
	else
		key="${cmd[1]}"
	fi
	if [ "k${key}" != "k" ] && [ ${key:0:1} == '$' ]; then
		select_key_var
		key=${key_val}
	fi
	if [ "${key:0:1}" == '^' ]; then
			select_regexp="${key}"
		else
			select_regexp="^.*${key}"
	fi
	while read -r line; do
		len=$(cut -sd $delim -f1 <<< "${line}")
		if [ ! -z $len ]; then
			if [ $len -le 0 ]; then
				len=0
				key=''
			else
				key=$(cut -d $delim -f2- <<< "${line}"| cut -b -${len} \
					| grep "${select_regexp}")
			fi
			escape_chars_regexp
			if [ $v_key == 1 ]; then
				cut -d $delim -f 2- <<< "${line}" | grep "^${safe_key}$delim"
			else
				cut -d $delim -f 2- <<< "${line}" | grep "^${safe_key}$delim" \
					| cut -b $((len+2))-
			fi
		fi
	done < "${db_file}"
}

del_key()
{
	if [ $n_args == 1 ] || [ $n_args -ge 4 ]; then
		echo $ESYNTAX $usage 1>&2
		exit 1
	
	## get key value in command line arguments
	elif [ $n_args -ge 1 ]; then
		key="${cmd[1]}"
		if [ "k${key}" != "k" ] && [ ${key:0:1} == '$' ]; then
			select_key_var
			key=${key_val}
		fi
		escape_chars_regexp

		## if no value specified, delete only key's content, not whole  entry
		if [ $n_args -eq 2 ]; then
			sed -i "s/\(^[0-9][0-9]*$delim${safe_key}$delim\).*$/\1/g" \
				"${db_file}"

		## if a value is specified, delete whole entry if provided value
		## matches database's one, else do nothing.
		else
			value="${cmd[2]}"
			if [ "k${value}" != "k" ] && [ ${value:0:1} == '$' ]; then
				select_val_var
			fi
			escape_chars_repl
			sed -i "/^[0-9][0-9]*$delim${safe_value}$/d" "$db_file"
		fi
	fi	
}

put_key()
{
	if [[ $n_args -le 2 ]] || [ $n_args -ge 4 ]; then
		echo $ESYNTAX $usage 1>&2
		exit 1
	fi

	## get key and value from command line arguments.
	key=${cmd[1]}
	value=${cmd[2]}
	if [ "k${key}" != "k"  ] && [ ${key:0:1} == '$' ]; then
		select_key_var
		key=${key_val}
	fi
	if [ "k${value}" != "k" ] && [ ${value:0:1} == '$' ]; then
		select_val_var
	fi
	escape_chars_regexp
	value=$(sed 's/%n/\%n/g' <<< "${value}");

	## if file is empty or does not exists then append new entry.
	if [ ! -f "$db_file" ]; then
		echo "${#key}$delim${key}$delim${value}" >> "${db_file}"
		return 0
	fi

	## if key is found in db delete it and append new one.
	sed -i "/^[0-9][0-9]*$delim${safe_key}$delim/d" "${db_file}"
	echo "${#key}$delim${key}$delim${value}" >> "${db_file}"
}

flush_db()
{
	echo -n '' > "$db_file" 1>&2
}

check_db_file()
{
	if [ $1 == 'x' ] && [ ! -f "${db_file}" ]; then
		echo $EFNOENT \'${db_file}\' not found or is not a regular file 1>&2
		exit 1
	fi
	if [ $2 == 'r' ] && [ ! -r "${db_file}" ]; then
		echo $EFNOENT \'${db_file}\' permission denied 1>&2
		exit 1
	fi
	if [ $3 == 'w' ] && [ ! -w "${db_file}" ]; then
		echo $EFNOENT \'${db_file}\' permission denied 1>&2
		exit 1
	fi
}

eval_cmdline()
{
	case "${cmd[0]}" in
		"put")
			check_db_file 0 r w
			put_key "$@"
			;;
		"flush")
			check_db_file x w 0
			flush_db
			;;
		"select")
			check_db_file x r 0
			select_key "$@"
			;;
		"del")
			check_db_file x r w
			if [ ! -f "$db_file" ]; then
				echo $EFNOENT \'${db_file}\' 1>&2
				exit 1
			fi
			del_key "$@"
			;;
		*)
			echo $ESYNTAX $usage 1>&2
			exit 1
	esac
}

get_cmd()
{
	cmd[0]="${1}"
	case $1 in
		"put")
			if [ $# -le 2 ]; then
				echo $ESYNTAX $usage
			fi
			cmd[1]=$2
			cmd[2]=$3
			n_args=3
			;;
		"flush")
			n_args=1
			;;
		"select")
			if [ $# -le 1 ]; then
				echo $ESYNTAX $usage
				exit 1
			fi
			cmd[1]=$2
			n_args=2
			;;
		"del")
			if [ $# -le 1 ]; then
				echo $ESYNTAX $usage
				exit 1
			fi
			cmd[1]=$2
			n_args=1
			if [ ! -z $1 ]; then
				cmd[2]=$1
				n_args=2
			fi
			;;
		*)
			echo $ESYNTAX $usage
			exit 1
			;;
	esac
}

get_options()
{
	OPTIND=1
   	local OPTARG
	unset cmd
	i=1
   	while getopts 'f:k' opt 2> /dev/null ; do
		i=$((i+1))
	   	case $opt in
		   	f)
			   	db_file=$OPTARG
			   	if [ "k${db_file}" != "k"  ] && [ "${db_file:0:1}" == "-"  ];
			   	then
				   	db_file="./${db_file}"
			   	fi
				i=$((i+1))
			   	;;
		   	k)
			   	v_key=1
			   	;;
		   	\?)
			   	echo $ESYNTAX "Invalid option in command arguments" 1>&2
			   	exit 1
			   	;;
		   	:)
			   	echo $ESYNTAX "Option requires an argument" 1>&2
				exit 1
			   	;;
	   	esac
	done
	while [ $i -le $ARGC ]; do
		cmd[n_args]="${!i}"
		n_args=$((n_args+1))
		i=$((i+1))
	done
}

ARGC=$#
ARGV="$@"
unset db_file
get_options "$@"
n_args=0
if [ -z "${db_file}" ]; then
	db_file="./sh.db"
fi
if [ $n_args -ge 1 ]; then
	eval_cmdline "$cmd"
else
	echo $usage 1>&2
fi
exit 0
