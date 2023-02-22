#!/bin/bash

# Setup
export NVM_DIR="$HOME/.nvm"
[ -s "$NVM_DIR/nvm.sh" ] && \. "$NVM_DIR/nvm.sh"  # This loads nvm
[ -s "$NVM_DIR/bash_completion" ] && \. "$NVM_DIR/bash_completion"  # This loads

# Run server
sleep  1
gnome-terminal --working-directory /home/dune/Desktop/DUNE_Web/81_DEV_UI -- bash -c "npm run dev && pause"
sleep 5

# Config touchscreen
xinput set-prop 'pointer:WaveShare WS170120' 'Coordinate Transformation Matrix' .2857 0 0 0 .741 0 0 0 1

# Open firefox:
env $MOZ_USE_XINPUT2=1 firefox &
sleep 1
xdotool keydown Super key Up Up keyup Super
xdotool mousemove 2000 5
sleep 1
xdotool mousedown 1
xdotool mousemove 200 5
xdotool mouseup 1
sleep 1
xdotool key F11

# Open DUNE
xdotool mousemove 2000 5
xdotool click 1
gnome-terminal --working-directory /home/dune/Desktop/DUNE_Web/90_BUILD/ -- ./DepthCamera
