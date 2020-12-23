###
# NOTE: This file is meant to be sourced by other scripts and isn't executable
#       on its own.
###

log()
{
    echo "${@}$(tput sgr0)" >&2
}

warning()
{
    log "$(tput setaf 3)$(tput bold)WARNING:$(tput sgr0)$(tput setaf 3)" ${@}
}

error()
{
    log "$(tput setaf 1)$(tput bold)ERROR:$(tput sgr0)$(tput setaf 1)" ${@}
}

die()
{
    error ${@}
    exit 1
}

exists()
{
    which ${1} &>/dev/null
    return $?
}

requires()
{
    local ret=0
    local app=""
    for app in ${@}; do
        if ! exists ${app}; then
            error "Unable to find required application '${app}'"
            ret=1
        fi
    done
    return ${ret}
}
