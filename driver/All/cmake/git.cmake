# SPDX-FileCopyrightText: 2025 AJ Walter
# SPDX-License-Identifier: GPL-3.0-or-later OR MIT

# Output variables
set(GIT_BRANCH "unknown")
set(GIT_HASH "unknown")
set(GIT_DATE "unknown")
set(GIT_CHANGES "unknown")
set(RUNNER_INFORMATION "Run: Local")

find_program(GIT_AVAILABLE "git")
if(GIT_AVAILABLE)
    execute_process(
        COMMAND git rev-parse --abbrev-ref HEAD
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_BRANCH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND git log -1 --format=%H
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_HASH
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND git --no-pager log -1 --format="%ai"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_DATE
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    execute_process(
        COMMAND git status --porcelain
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_CHANGES_OUT
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    if("${GIT_CHANGES_OUT}" STREQUAL "")
        set(GIT_CHANGES "No")
    else()
        set(GIT_CHANGES "Yes")
    endif()
endif()

# Append some runner information if GITHUB_ACTIONS is set
if(DEFINED ENV{GITHUB_ACTIONS})
    set(GIT_BRANCH $ENV{BRANCH_NAME})
    set(GIT_HASH $ENV{GITHUB_SHA})
    list(APPEND RUNNER_INFORMATION_LIST "Run: $ENV{GITHUB_RUN_ID}")
    list(APPEND RUNNER_INFORMATION_LIST "Event: $ENV{GITHUB_EVENT_NAME}")
    list(APPEND RUNNER_INFORMATION_LIST "Job: $ENV{GITHUB_JOB}")
    list(APPEND RUNNER_INFORMATION_LIST "Workflow: $ENV{GITHUB_WORKFLOW}")
    list(APPEND RUNNER_INFORMATION_LIST "Repo: $ENV{GITHUB_SERVER_URL}/$ENV{GITHUB_REPOSITORY}")
    list(APPEND RUNNER_INFORMATION_LIST "Action: $ENV{GITHUB_SERVER_URL}/$ENV{GITHUB_REPOSITORY}/actions/runs/$ENV{GITHUB_RUN_ID}")
    list(APPEND RUNNER_INFORMATION_LIST "Tree: $ENV{GITHUB_SERVER_URL}/$ENV{GITHUB_REPOSITORY}/tree/$ENV{GITHUB_SHA}")
    string (REPLACE ";" "\n" RUNNER_INFORMATION "${RUNNER_INFORMATION_LIST}")
endif()
