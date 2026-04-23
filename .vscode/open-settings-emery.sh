#!/usr/bin/env bash
set -e

cd /mnt/q/ASLWatchFace
chmod +x .vscode/cleanup-emery.sh
chmod +x .vscode/win-open.sh

# Reset stale emulator helpers to avoid firmware boot hangs.
.vscode/cleanup-emery.sh

# Install the app in the foreground so emulator launch and install complete.
: > /mnt/q/ASLWatchFace/.vscode/emery-live.log
pebble install --emulator emery

# Start log capture after install so we can wait for PKJS readiness.
: > /mnt/q/ASLWatchFace/.vscode/emery-live.log
nohup pebble logs --emulator emery > /mnt/q/ASLWatchFace/.vscode/emery-live.log 2>&1 &

# Wait for PKJS readiness so config page can be served reliably.
for i in $(seq 1 60); do
  if grep -q "ASLWatchFace JS ready" /mnt/q/ASLWatchFace/.vscode/emery-live.log 2>/dev/null; then
    break
  fi
  sleep 1
done

# Launch official app config flow so changes are sent back via return_to callback.
# Retry a few times because emulator websocket startup can be racy.
for i in 1 2 3; do
  if BROWSER='/mnt/q/ASLWatchFace/.vscode/win-open.sh' pebble emu-app-config --emulator emery; then
    exit 0
  fi
  sleep 2
done

exit 1
