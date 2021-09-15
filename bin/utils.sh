###
# NOTE: This file is meant to be sourced by other scripts and isn't executable
#       on its own.
###

set -o errexit
set -o errtrace
set -o functrace
set -o nounset
set -o pipefail
set -u
shopt -s expand_aliases
shopt -s checkwinsize

trap abort SIGINT ERR

# Log a message to stderr
log()
{
    echo "${@}$(tput sgr0)" >&2
}

# Log a warning to stderr, colored yellow
warning()
{
    log "$(tput setaf 3)$(tput bold)WARNING:$(tput sgr0)$(tput setaf 3)" "${@}"
}

# Log an error to stderr, colored red
error()
{
    log "$(tput setaf 1)$(tput bold)ERROR:$(tput sgr0)$(tput setaf 1)" "${@}"
}

# Log an error, and exit with bad status
die()
{
    error "${@}"
    exit 1
}

# Print the current stack trace of the application, and then die.
abort()
{
    # Print stacktrace
    if [[ ${#FUNCNAME[@]} -gt 2 ]]; then
        echo -e "$(tput setaf 1)$(tput bold)    Stacktrace ::" >&2
        for (( frame=1; frame < ${#FUNCNAME[@]}-1; frame++ )); do
            echo -e "$(tput setaf 1)$(tput bold)    ${BASH_SOURCE[$frame+1]}:${BASH_LINENO[$frame]} ${FUNCNAME[$frame]}" >&2
        done
    fi
    die "${@}"
}

# Check if a command exists
exists()
{
    which "${1}" &>/dev/null
    return $?
}

# Specify a list of commands that must exist, or the program can't run. The
# entire list is checked, and each missing application is printed.
#
# NOTE: Doesn't automatically exit the script
requires()
{
    local ret=0
    local app=""
    for app in ${@}; do
        if ! exists "${app}"; then
            error "Unable to find required application '${app}'"
            ret=1
        fi
    done
    return ${ret}
}

# Runs a command in a ticker, capturing its output and hiding it behind a
# single line. If the command successfully runs, then a checkbox is printed,
# otherwise a red X is printed.
ticker()
{
    local cmd="${1}"
    shift

    (
        trap - ERR

        local start=${SECONDS}
        local current=${start}
        local delta=0
        local cols=
        local spinner=(— \\ \| /)
        local spin=

        while [[ 0 == 0 ]]; do
            cols=$(tput cols)
            current=${SECONDS}
            delta=$(( ${current} - ${start} ))
            spin=${spinner[$(( ${delta} % ${#spinner[@]} ))]}

            # NOTE: Leaving the cursor at the end of the line means that we
            #       don't have annoying flashing while also not having to
            #       disable the cursor from showing up. So it works nicer in
            #       error-cases where we may die (or be killed) without being
            #       able to re-enable the cursor first.
            printf "\r$(tput bold)[%s]$(tput sgr0) %-$(( ${cols} - 15 ))s $(tput bold)[%02d:%02d:%02d]$(tput sgr0)" "${spin}" "${@}" $(( ${delta} / 3600 )) $(( (${delta} % 3600) / 60 )) $(( ${delta} % 60 ))
            sleep 1
        done
    ) &
    local tickerPid=$!
    # Give the ticker a chance to complete its first cycle before continuing on
    sleep .003

    closeTicker()
    {
        local rc=${1:-1}
        kill ${tickerPid} 2>/dev/null || true
        wait ${tickerPid} 2>/dev/null || true

        if [[ ${rc} -eq 0 ]]; then
            echo -e "\r$(tput setaf 2)$(tput bold)[✓]$(tput sgr0)"
        else
            echo -e "\r$(tput setaf 1)$(tput bold)[✘]$(tput sgr0)"
            echo "    StdOut: ${outFile}"
            echo "    StdErr: ${errFile}"
        fi

    }
    trap closeTicker ERR

    local outFile="$(mktemp "/tmp/out-XXXXXX")"
    local errFile="$(mktemp "/tmp/err-XXXXXX")"
    local rc=

    if [[ "${VERBOSE:-0}" -eq 1 ]]; then
        (
            ${cmd}
        )
        rc=$?
    else
        (
            ${cmd} 1>"${outFile}" 2>"${errFile}"
        )
        rc=$?
    fi

    closeTicker ${rc}
    return ${rc}
}
