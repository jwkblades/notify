#!/bin/bash

##
#
# NOTE: This script requires and expects that you are running as root, if that
#       isn't the case, we will abort.
#
##

log()
{
    echo ${@} >&2
}

if [[ ${UID} -ne 0 ]]; then
    log "Must be run as root."
    exit 1
fi

#### Expected format for policy information:
# {
#     "policy": {
#         "type": "patch-all", // or "patch-all", "patch-all-except", "patch-only", "manual-approval", etc.
#         "enabled": ["package 1", "2", "etc."], // The packages that are enabled for the policy - only relevant for patch-only, manual-approval, severity, etc.
#         "disabled": [] // The packages that are not allowed to patch with this policy - only relevant for patch-all-except, manual-approval...
#     },
#     "notifications": {
#         "pre-patch": {
#             "message": "Your system needs to install patches.",
#             "messageWithReboot": "Your system needs to install patches and may require a reboot."
#             "maxDeferrals": 3, // 0 - no deferrals, >0 <=3 for the range
#             "deferralDurations": [60, 120, 480] // Array of numbers (minutes) to defer; max length: 3
#         },
#         "pre-reboot": {
#             "message": "Your system needs to install patches.",
#             "maxDeferrals": 2, // 0 - no deferrals, >0 <=3 for the range
#             "deferralDurations": [60, 120, 480] // Array of numbers (minutes) to defer; max length: 3
#         }
#     }
# }
policyInfo="${1}"

hasUpdates()
{
    local includes="${1:-}"
    local excludes="${2:-}"

    dnf check-update --quiet --excludepkgs "${excludes}" ${includes} | awk '{print $1}'
}

getUpdates()
{
    local type="${1}"
    shift

    log "Checking policy compliance for policy of type ${type}"

    case "${type}" in
        patch-all)
            hasUpdates
            ;;
        patch-only)
            hasUpdates "$(echo ${policyInfo} | jq -r '.policy.enabled[]' | tr '\n' ' ')"
            ;;
        patch-all-except)
            hasUpdates "" "$(echo ${policyInfo} | jq -r '.policy.disabled[]' | tr '\n' ' ')"
            ;;
        manual-approval)
            hasUpdates "$(echo ${policyInfo} | jq -r '.policy.enabled[]' | tr '\n' ' ')" "$(echo ${policyInfo} | jq -r '.policy.disabled[]' | tr '\n' ' ')"
            ;;
        *)
            log "Unknown policy type ${type}"
            return 1
            ;;
    esac
}

isCompliant()
{
    if [[ -z "${1:-}" ]]; then
        return 0
    fi
    return 1
}

retrieveNotificationMessage()
{
    local path="${1}"
    local fallback="${2:-}"
    local message="$(echo "${policyInfo}" | jq -r "${path}")"
    if [[ "${message}" == "null" ]]; then
        message="${fallback}"
    fi

    echo -n "${message}"
}

mayRequireReboot()
{
    if [[ -z "$(retrieveNotificationMessage '.policy.autoReboot')" ]]; then
        return 0 # May reboot
    fi
    return 1 # Won't reboot
}

getUsername()
{
    who | head -1 | awk '{print $1}'
}

getDisplay()
{
    who | head -1 | awk '{print $2}'
}

getUID()
{
    id -u "${1}"
}

timestamp()
{
    local year="$(date +"%Y")"
    local dayOfYear="$(date +"%j")"
    local hour="$(date +"%_H")"
    local minute="$(date +"%_M")"
    local second="$(date +"%_S")"

    echo -n "$(( (${year} * 31557600) + (${dayOfYear} * 86400) + (${hour} * 3600) + (${minute} * 60) + ${second} ))"
}

runNotifications()
{
    local path="${1}"
    local icon="${2}"
    local now="${3}"
    local later="${4}"
    local title="${5}"
    local message="${6}"

    local deferrals="$(retrieveNotificationMessage "${path}.maxDeferrals" 0)"
    local deferralTime="$(retrieveNotificationMessage "${path}.deferralDurations[0]" 60)"
    local user="$(getUsername)"
    local display="$(getDisplay)"
    local uid="$(getUID "${user}")"

    while [[ ${deferrals} -gt 0 ]]; do 
        deferrals=$(( ${deferrals} - 1 ))
        log "About to run notification, ${deferrals} notifications remaining."
        local start="$(timestamp)"
        if [[ ${deferrals} -eq 1 ]]; then
            result="$(su -c "DISPLAY=${display} XDG_RUNTIME_DIR=/run/user/${uid} ./notify --icon '${icon}' -o '${now}' -v 'NOW' '${title}' '${message}'" ${user})"
        else
            result="$(su -c "DISPLAY=${display} XDG_RUNTIME_DIR=/run/user/${uid} ./notify --icon '${icon}' -o '${later}' -v '${deferralTime}' -o '${now}' -v 'NOW' '${title}' '${message}'" ${user})"
        fi
        log "Retrieved result from notification of '${result}'"

        case "${result}" in
            NOW)
                return 0
                ;;
            OK)
                # We want to know how long the notification was up before being closed. By default, the notification
                # will timeout after 15 minutes, which is 900 seconds, so take 900 and subtract the duration of time
                # (in seconds) that the notification was open. If the result is positive, then we still have time
                # before the 15-minute timeout, and the notification was dismissed early by the user, but we still want
                # to patch after that 15 minute mark. So, sleep for the remainder and then continue.
                local sleepDuration="$(( 900 - ($(timestamp) - ${start}) ))"
                if [[ ${sleepDuration} -gt 0 ]]; then
                    log "Sleeping for ${sleepDuration} seconds before continuing."
                    sleep "${sleepDuration}"
                fi
                return 0
                ;;
            *)
                sleep $(( ${result} * 60 ))
                ;;
        esac
    done
}

prePatchNotification()
{
    log "Pre-patch notifications"
    local message=""
    if mayRequireReboot; then
        message="$(retrieveNotificationMessage '.notifications["pre-patch"].messageWithReboot' "Your system needs to patch, a reboot may be required afterwards.")"
    else
        message="$(retrieveNotificationMessage '.notifications["pre-patch"].message' "Your system needs to patch.")"
    fi

    log "Determine the notification mesage of '${message}'"
    runNotifications '.notifications["pre-patch"]' "software-update-available" "Install Now" "Install Later" "Updates are ready to install" "${message}"
}

patch()
{
    log "Patching ${1}"
    dnf update -y ${1}
}

machineNeedsRestarting()
{
    log "Checking if we need to reboot."
    dnf needs-restarting -r &>/dev/null
    local ret="${?}"
    log "Returned '${ret}'"
    if [[ ${ret} -eq 1 ]]; then
        log "Reboot required"
        return 0
    fi

    return 1
}

preRebootNotification()
{
    log "Pre-reboot notification"
    local message="$(retrieveNotificationMessage '.notifications["pre-reboot"].message' "Your system needs to reboot to complete patching.")"
    runNotifications '.notifications["pre-reboot"]' "system-reboot" "Reboot Now" "Reboot Later" "Updates pending reboot" "${message}"
}

rebootMachine()
{
    log "Reboot the machine"
    reboot
}

main()
{
    local policyType="$(echo ${policyInfo} | jq -r '.policy.type')"

    log "Policy type: '${policyType}'"

    local updates="$(getUpdates "${policyType}")"

    if isCompliant "${updates}"; then
        log "Policy compliant"
        return 0
    else
        log "Not compliant"
    fi

    prePatchNotification

    patch "${updates}"

    if machineNeedsRestarting; then
        preRebootNotification

        rebootMachine
    fi
}
main
