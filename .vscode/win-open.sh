#!/usr/bin/env bash

url="$1"
if [ -z "$url" ]; then
  exit 1
fi

log_file="/mnt/q/ASLWatchFace/.vscode/win-open.log"
printf '%s\n' "$(date '+%Y-%m-%d %H:%M:%S') raw: $url" >> "$log_file"

# Convert WSL file URLs to Windows-style file URLs.
if [[ "$url" =~ ^file:///mnt/([a-zA-Z])/(.*)$ ]]; then
  drive="${BASH_REMATCH[1]}"
  url="file:///${drive^^}:/${BASH_REMATCH[2]}"
fi

# Convert plain WSL paths to Windows-style file URLs.
if [[ "$url" =~ ^/mnt/([a-zA-Z])/(.*)$ ]]; then
  drive="${BASH_REMATCH[1]}"
  url="file:///${drive^^}:/${BASH_REMATCH[2]}"
fi

# Preserve URL params by moving query to hash for local file URLs.
if [[ "$url" =~ ^file:///.*\?.*$ ]]; then
  base="${url%%\?*}"
  query="${url#*\?}"
  url="${base}#${query}"
fi

# Windows browser must route callback traffic to WSL callback server.
url="${url//http:\/\/localhost:/http:\/\/wsl.localhost:}"
url="${url//http%3A%2F%2Flocalhost%3A/http%3A%2F%2Fwsl.localhost%3A}"

printf '%s\n' "$(date '+%Y-%m-%d %H:%M:%S') norm: $url" >> "$log_file"

# Invoke Edge via PowerShell helper.
printf '%s\n' "$(date '+%Y-%m-%d %H:%M:%S') launch: edge-ps1" >> "$log_file"
/mnt/c/Windows/System32/WindowsPowerShell/v1.0/powershell.exe -NoProfile -ExecutionPolicy Bypass -File "Q:\ASLWatchFace\.vscode\open-edge.ps1" "$url" >> "$log_file" 2>&1 &

exit 0
