#/usr/bin/env bash

_rioSubCommands()
{

    local cur=${COMP_WORDS[COMP_CWORD]}
    local profile=default

    if [ -z ${RIO_PROFILE} ]; 
    then
      profile=default
    else
      profile=${RIO_PROFILE}
    fi

    local configIdx=0
    local memoryIdx=0
    local dataIdx=0
    local linkIdx=0
    local allocIdx=0
    local deallocIdx=0
    local lsIdx=0
    local addIdx=0
    local rmIdx=0
    local dataFileIdx=0
    local linkFileIdx=0
    local idx=1

    for (( i=0; i < ${#COMP_WORDS[@]}; i++)); 
    do
      local o=${COMP_WORDS[$i]}
      case $o in
          "rio") ;;
          "config") configIdx=$idx; ((idx++)) ;;
          "memory") memoryIdx=$idx; ((idx++)) ;;
          "data") dataIdx=$idx; ((idx++)) ;;
          "link") linkIdx=$idx; ((idx++)) ;;
          "alloc") allocIdx=$idx; ((idx++)) ;;
          "dealloc") allocIdx=$idx; ((idx++)) ;;
          "ls") lsIdx=$idx; ((idx++)) ;;
          "rm") rmIdx=$idx; ((idx++)) ;;
          "add") addIdx=$idx; ((idx++)) ;;
          -p=*|--profile=*) profile="${o#*=}" ;;
          "") ;;
          *)

	      if [ $linkIdx -ne "0" ] && [ $catIdx -ne "0" ];
	      then
	        if [ $linkFileIdx -eq "0" ];
	        then
		  linkFileIdx=$idx
		  ((idx++))
		fi
	      fi
	      
         ;;
       esac
    done

    local memory=""
    memory+=$memoryIdx
    memory+=$allocIdx
    memory+=$deallocIdx
    memory+=$lsIdx

    local data=""
    data+=$dataIdx
    data+=$lsIdx
    data+=$rmIdx
    data+=$addIdx
    data+=$dataFileIdx

    local link=""
    link+=$linkIdx
    link+=$lsIdx
    link+=$addIdx
    link+=$rmIdx
    link+=$linkFileIdx

    COMPREPLY=()

    # rio ?
    if [ $data -eq "00000" ] && [ $link -eq "00000" ];
    then
      local IFS=$' \t\n'
      local opts="config memory data link help version"
      COMPREPLY=($(compgen -W "$opts" -- ${cur}))
    fi

    # rio memory ?
    if [ $memory -eq "1000" ];
    then
      local IFS=$' \t\n'
      COMPREPLY=($(compgen -W "alloc dealloc ls" -- ${cur}))
    fi

    # rio data ?
    if [ $data -eq "10000" ];
    then
      local IFS=$' \t\n'
      COMPREPLY=($(compgen -W "add rm ls" -- ${cur}))
    fi

    # rio data add ? || rio data rm ? || rio data ls ?
    if [ $data -eq "10200" ] || [ $data -eq "10020" ] || [ $data -eq "12000" ];
    then
      strings=$(rio data ls 2> /dev/null)
      local IFS=$'\n'
      files=()
      files+="$strings"
      COMPREPLY=($(compgen -W "$files" -- ${cur}))
    fi

    # rio link inspect
    #if [ $link -eq "1020" ] || [ $link -eq "1023" ];
    #then
    #  links=$(ls -N /var/run/rikerio/${profile}/links/ 2> /dev/null)
    #  local IFS=$'\n'
    #  files=()
    #  files+="$links"
    #  COMPREPLY+=($(compgen -W "$files" -- ${cur}))
    #fi

    # rio alias ?
    #if [ $alias -eq "10000000" ];
    #then
    #  local IFS=$' \t\n'
    #  COMPREPLY=($(compgen -W "ls cat inspect rm add help" -- ${cur}))
    #fi

    # rio alias cat ? || rio alias inspect ? || rio alias rm ? || rio alias add ?
    #if [ $alias -eq "10200000" ] || [ $alias -eq "10020000" ] || [ $alias -eq "10002000" ] || [ $alias -eq "10000200" ] || [ $alias -eq "10200030" ] || [ $alias -eq "10020030" ] || [ $alias -eq "10002030" ] || [ $alias -eq "10000230" ]; 
    #then
    #  as=$(ls -N /var/run/rikerio/${profile}/alias/ 2> /dev/null)
    #  local IFS=$'\n'
    #  files=()
    #  files+="$as"
    #  COMPREPLY+=($(compgen -W "$files" -- ${cur}))
    #fi

    # rio alias rm aliasFile ? || rio alias add aliasFile ?
    #if [ $alias -eq "10002030" ] || [ $alias -eq "10002034" ] || [ $alias -eq "10000230" ] || [ $alias -eq "10000234" ];
    #then
    #  links=$(ls -N /var/run/rikerio/${profile}/links/ 2> /dev/null)
    #  local IFS=$'\n'
    #  files=()
    #  files+="$links"
    #  COMPREPLY+=($(compgen -W "$files" -- ${cur}))
    #fi

}

COMP_WORDBREAKS=$'\"\'><;|&(: '

complete -o nospace -F _rioSubCommands rio

