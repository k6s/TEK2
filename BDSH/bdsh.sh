#!/bin/sh

## TODO: ######################################################################
#
#	- newline in key/value ?
#	- Order by modification time aswell as insert time ?
#	- empty key
#	- r/w database access check
#
###############################################################################

db_file='sh.db'
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

# Shell special characters escape
# Concerned chars are * ? [  ] ' " \ $ ; & (  ) | ^ < > new-line space tab " '
escape_chars_sh()
{
	safe_sh=$(sed -e 's/\\/\\\\/g;s/\$/\\\$/g' \
		-e 's/\*/\\\*/g;s/\?/\\\?/g' \
	   	-e 's/\[/\\\[/g;s/\]/\\\]/g' \
		-e 's/"/\\"/g;' -e "s/'/\\'/" \
		-e 's/;/\\;/g;s/&/\\&/g' \
		-e 's/(/\\(/g;s/)/\\)/g' \
		-e 's/|/\\|/g;s/\^/\\\^/g' \
		-e 's/</\\\</g;s/>/\\>/g;s/-/\\-/g'\
	   	<<< "$1")
}

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
	key=${key:1}
	escape_chars_regexp
	key_val=$(grep "^[0-9][0-9]*$delim${safe_key}$delim" "${db_file}")
	if [ "k${key_val}" == "k" ]; then
		echo $EKNOENT ${key}
		exit 1
	fi
	key_val=$(grep "^[0-9][0-9]*$delim${safe_key}$delim" "${db_file}" \
		| cut -d $delim -f3)
}

select_val_var()
{
	ktmp=${key}
	key=${value}
	select_key_var
	value=${key_val}
	key=${ktmp}
}

select_key()
{
	n_args=$((ARGC-i))
	if [ $n_args -ge 2 ]; then
		echo $ESYNTAX $USAGE
		exit 1
	fi
	if [ $n_args == 0 ]; then
		safe_key='.*'
	else
		key=$((i+1))
		key=${!key}
	fi
	if [ "k${key}" != "k" ] && [ ${key:0:1} == '$' ]; then
		select_key_var
		key=${key_val}
		escape_chars_regexp

		## key must be a complete match vs grep result for 'regular' key
		select_regexp="${safe_key}$"
	else
		select_regexp=".*${key}"
	fi
	while read -r line; do
		len=$(cut -d $delim -f1 <<< "${line}")
		key=$(cut -d $delim -f2- <<< "${line}"| cut -b -${len} \
			| grep "^${select_regexp}")
		escape_chars_regexp
		if [ $v_key == 1 ]; then
			cut -d $delim -f 2- <<< "${line}" | grep "^${safe_key}$delim"
		else
			cut -d $delim -f 2- <<< "${line}" | grep "^${safe_key}$delim" \
				| cut -b $((len+2))-
		fi
	done < "${db_file}"
}

del_key()
{
	n_args=$((ARGC-i))
	if [ $n_args == 0 ] || [ $n_args -ge 3 ]; then
		echo $ESYNTAX $usage
		exit 1
	
	## get key value in command line arguments
	elif [ $n_args -ge 1 ]; then
		key=$((i+1))
		key=${!key}
		if [ "k${key}" != "k" ] && [ ${key:0:1} == '$' ]; then
			select_key_var
			key=${key_val}
		fi
		escape_chars_regexp

		## if no value specified, delete only key's content, not whole  entry
		if [ $n_args -eq 1 ]; then
			sed -i "s/\(^[0-9][0-9]*$delim${safe_key}$delim\).*$/\1/g" \
				"${db_file}"

		## if a value is specified, delete whole entry if provided value
		## matches database's one, else do nothing.
		else
			value=$((i+2))
			value=${!value}
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
	n_args=$((ARGC-i))
	if [[ $n_args -le 1 ]] || [ $n_args -ge 3 ]; then
		echo $ESYNTAX $usage
		exit 1
	fi

	## get key and value from command line arguments.
	key=$((i+1))
	value=$((i+2))
	key=${!key}
	value=${!value}
	if [ "k${key}" != "k"  ] && [ ${key:0:1} == '$' ]; then
		select_key_var
		key=${key_val}
	fi
	if [ "k${value}" != "k" ] && [ ${value:0:1} == '$' ]; then
		select_val_var
	fi
	escape_chars_regexp

	## if file is empty or does not exists then append new entry.
	if [ ! -f "$db_file" ]; then
		echo "${#key}$delim${key}$delim${value}" >> "${db_file}"
		return 0
	fi

	## if key is found in db replace entry else append it.
	line=$(grep "^[0-9][0-9]*$delim${safe_key}$delim" < "${db_file}")
	if [ 0 -eq $? ]; then
		escape_chars_repl
		sed -i "s/\(^[0-9][0-9]*$delim\)${safe_key}$delim.*$/\1${safe_value}/g"\
			"${db_file}"
	else
		echo "${#key}$delim${key}$delim${value}" >> "${db_file}"
	fi
	return 2
}

flush_db()
{
	if [ -f "$db_file" ]; then
		echo -n '' > "$db_file"
	else
		echo $EFNOENT $db_file
		exit 1
	fi
}

get_cmdline()
{
	i=$((OPTIND))
	case ${!i} in
		"put")
			put_key "$@"
			;;
		"flush")
			flush_db
			;;
		"select")
			if [ ! -f "${db_file}" ]; then
				echo $EFNOENT \'${db_file}\'
				exit 1
			fi
			select_key "$@"
			;;
		"del")
			if [ ! -f "$db_file" ]; then
				echo $EFNOENT \'${db_file}\'
				exit 1
			fi
			del_key "$@"
			;;
		*)
			echo $ESYNTAX $usage
			exit 1
	esac
}

get_options()
{
	OPTIND=1
	local OPTARG
	while getopts 'f:k' opt 2> /dev/null ; do
		case $opt in
			f)
				db_file=$OPTARG
				if [ "k${db_file}" != "k" ] && [ "${db_file:0:1}" == "-" ];
				then
					db_file="./${db_file}"
				fi
				;;
			k)
				v_key=1
				;;
			\?)
				echo $ESYNTAX "Invalid option in command arguments"
				exit 1
				;;
			:)
				echo $ESYNTAX "Option requires an argument"
				exit 1
				;;
		esac
	done
}

ARGC=$#
ARGV="$@"
get_options "$@"
get_cmdline "$@" $OPTIND
exit 0
