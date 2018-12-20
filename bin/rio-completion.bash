#/usr/bin/env bash

_rioSubCommands()
{

    local cur=${COMP_WORDS[COMP_CWORD]}
    local profile=io

    if [ -z ${RIO_PROFILE} ]; 
    then
      profile=default
    else
      profile=${RIO_PROFILE}
    fi

    local profileIdx=0
    local aliasIdx=0
    local linkIdx=0
    local lsIdx=0
    local catIdx=0
    local inspectIdx=0
    local rmIdx=0
    local addIdx=0
    local aliasFileIdx=0
    local linkFileIdx=0
    local createIdx=0
    local idx=1


    for (( i=0; i < ${#COMP_WORDS[@]}; i++)); 
    do
      local o=${COMP_WORDS[$i]}
      case $o in
          "rio") ;;
          "profile") profileIdx=$idx; ((idx++)) ;;
          "alias") aliasIdx=$idx; ((idx++)) ;;
          "link") linkIdx=$idx; ((idx++)) ;;
          "ls") lsIdx=$idx; ((idx++)) ;;
          "cat") catIdx=$idx; ((idx++)) ;;
          "inspect") inspectIdx=$idx; ((idx++)) ;;
          "rm") rmIdx=$idx; ((idx++)) ;;
          "add") addIdx=$idx; ((idx++)) ;;
          "create") createIdx=$idx; ((idx++)) ;;
          -i=*|--id=*) profile="${o#*=}" ;;
          "") ;;
          *)
              # check if alias
              if [ -f /var/run/rikerio/${profile}/alias/"${o}" ]
              then
                aliasFileIdx=$idx
                ((idx++)) 
              fi

              # check if link
              if [ -f /var/run/rikerio/${profile}/links/"${o}" ] && [ $linkFileIdx -eq "0" ];
              then
                linkFileIdx=$idx
                ((idx++)) 
              fi    
         ;;
       esac
    done


    local alias=""
    alias+=$aliasIdx
    alias+=$lsIdx
    alias+=$catIdx
    alias+=$inspectIdx
    alias+=$rmIdx
    alias+=$addIdx
    alias+=$aliasFileIdx
    alias+=$linkFileIdx

    local link=""
    link+=$linkIdx
    link+=$lsIdx
    link+=$catIdx
    link+=$linkFileIdx

    local pro=""
    pro+=$profileIdx
    pro+=$lsIdx
    pro+=$createIdx

    COMPREPLY=()

    # rio ?
    if [ $alias -eq "00000000" ] && [ $link -eq "0000" ] && [ $pro -eq "000" ];
    then
      local IFS=$' \t\n'
      local opts="profile link alias help version"
      COMPREPLY=($(compgen -W "$opts" -- ${cur}))
    fi

    # rio profile ?
    if [ $pro -eq "100" ];
    then
      local IFS=$' \t\n'
      COMPREPLY=($(compgen -W "ls create help" -- ${cur}))
    fi

    # rio link ?
    if [ $link -eq "1000" ];
    then
      local IFS=$' \t\n'
      COMPREPLY=($(compgen -W "ls cat help" -- ${cur}))
    fi

    # rio link inspect
    if [ $link -eq "1020" ];
    then
      links=$(ls /var/run/rikerio/${profile}/links/ 2> /dev/null)
      local IFS=$'\n'
      files=()
      files+="$links"
      COMPREPLY+=($(compgen -W "$files" -- ${cur}))
    fi

    # rio alias ?
    if [ $alias -eq "10000000" ];
    then
      local IFS=$' \t\n'
      COMPREPLY=($(compgen -W "ls cat inspect rm add help" -- ${cur}))
    fi

    # rio alias cat ? || rio alias inspect ? || rio alias rm ? || rio alias add ?
    if [ $alias -eq "10200000" ] || [ $alias -eq "10020000" ] || [ $alias -eq "10002000" ] || [ $alias -eq "10000200" ];
    then
      as=$(ls /var/run/rikerio/${profile}/alias/ 2> /dev/null)
      local IFS=$'\n'
      files=()
      files+="$as"
      COMPREPLY+=($(compgen -W "$files" -- ${cur}))
    fi

    # rio alias rm aliasFile ? || rio alias add aliasFile ?
    if [ $alias -eq "10002030" ] || [ $alias -eq "10002034" ] || [ $alias -eq "10000230" ] || [ $alias -eq "10000234" ];
    then
      links=$(ls /var/run/rikerio/${profile}/links/ 2> /dev/null)
      local IFS=$'\n'
      files=()
      files+="$links"
      COMPREPLY+=($(compgen -W "$files" -- ${cur}))
    fi

}

COMP_WORDBREAKS=$'\"\'><;|&(: '

complete -o nospace -F _rioSubCommands rio

