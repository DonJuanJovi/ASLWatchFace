#!/usr/bin/env bash
set -euo pipefail

cd /mnt/q/ASLWatchFace

pebble kill --force >/dev/null 2>&1 || true

kill_pattern() {
  local pattern="$1"
  local pid
  local -a pids=()

  mapfile -t pids < <(pgrep -A -f -- "$pattern" || true)

  for pid in "${pids[@]}"; do
    [ -n "$pid" ] || continue
    kill -9 "$pid" >/dev/null 2>&1 || true
  done
}

kill_pattern 'pebble install --emulator emery'
kill_pattern 'pebble logs --emulator emery'
kill_pattern 'pebble emu-app-config'
kill_pattern 'python -m pypkjs'
kill_pattern 'qemu-pebble'

exit 0