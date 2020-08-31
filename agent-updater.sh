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
    if [[ -z "$(retrieveNotificationMessage '.notifications["pre-reboot"].message')" ]]; then
        return 0 # May reboot
    fi
    return 1 # Won't reboot
}

prePatchNotification()
{
    local message=""

    log "Pre-patch notifications"
    if mayRequireReboot; then
        message="$(retrieveNotificationMessage '.notifications["pre-patch"].messageWithReboot' "Your system needs to patch, a reboot may be required afterwards.")"
    else
        message="$(retrieveNotificationMessage '.notifications["pre-patch"].message' "Your system needs to patch.")"
    fi

    log "Determine the notification mesage of '${message}'"

    local deferrals="$(retrieveNotificationMessage '.notifications["pre-patch"].maxDeferrals' 0)"
    local deferralTime="$(retrieveNotificationMessage '.notifications["pre-patch"].deferrals[0]' 60)"
    while [[ ${deferrals} -gt 0 ]]; do 
        log "About to run notification"
        result="$(pkexec --user jblades /home/jblades/Documents/cpp/notify/agent-notifier.sh software-update-available "Install Now" "Install Later" "${deferralTime}" "Updates are ready to install." "${message}")"
        log "Retrieved result from notification of '${result}'"

        case "${result}" in
            NOW)
                return 0
                ;;
            OK)
                sleep 900 # 15 minutes, it won't start exactly on time, but close enough for now
                return 0
                ;;
            *)
                sleep $(( ${result} * 60 ))
                ;;
        esac
    done
}

patch()
{
    log "We want to patch ${1}"
}

machineNeedsRestarting()
{
    log "Checking if we need to reboot."
    return 1
}

preRebootNotification()
{
    log "Pre-reboot notification"
}

rebootMachine()
{
    log "Reboot the machine here."
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
