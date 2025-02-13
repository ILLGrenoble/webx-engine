#!/bin/bash

# Expects DISPLAY to be set when running the script

calculate_transparency_stats() {
  local log_file=$1
  local count=0
  local sum=0

  # Extract lines containing "Transparency check" and get the timing value
  local times=$(grep "Transparency check" "$log_file" | awk '{gsub("us", "", $NF); print $NF}')

  # Loop through each extracted timing
  for time in $times; do
    sum=$(awk -v sum="$sum" -v time="$time" 'BEGIN {print sum + time}')
    count=$((count + 1))
  done

  # Calculate average
  if [ "$count" -gt 0 ]; then
      avg=$(awk -v sum="$sum" -v count="$count" 'BEGIN {print sum / count}')
      echo "Transparency checks: $count, total time: $sum us, average time: $avg us"
  else
      echo "No Transparency check entries found."
  fi
}

calculate_alphamap_creation_stats() {
  local log_file=$1
  local count=0
  local sum=0

  # Extract lines containing "Converted raw image data for alpha map" and get the timing value
  local times=$(grep "Converted raw image data for alpha map" "$log_file" | awk '{gsub("us", "", $NF); print $NF}')

  # Loop through each extracted timing
  for time in $times; do
    sum=$(awk -v sum="$sum" -v time="$time" 'BEGIN {print sum + time}')
    count=$((count + 1))
  done

  # Calculate average
  if [ "$count" -gt 0 ]; then
    avg=$(awk -v sum="$sum" -v count="$count" 'BEGIN {print sum / count}')
    echo "Alphamap creations: $count, total time: $sum us, average time: $avg us"
  else
    echo "No Alphamap creation entries found."
  fi
}

#!/bin/bash

calculate_grab_metrics() {
  local log_file="$1"
  local count=0
  local sum_width=0
  local sum_height=0
  local sum_grab_time=0
  local sum_encoding_time=0

  # Process each matching log line
  while IFS= read -r line; do
    # Extract width, height, grab time, and encoding time using awk
    width=$(echo "$line" | awk -F '[ ,x()=]+' '{print $(NF-10)}')
    height=$(echo "$line" | awk -F '[ ,x()=]+' '{print $(NF-9)}')
    grab_time=$(echo "$line" | awk -F '[ ,=(ms)]+' '{print $(NF-3)}')
    encoding_time=$(echo "$line" | awk -F '[ ,=(ms)]+' '{print $(NF-1)}')

    # Accumulate values
    sum_width=$((sum_width + width))
    sum_height=$((sum_height + height))
    sum_grab_time=$(awk -v sum="$sum_grab_time" -v time="$grab_time" 'BEGIN {print sum + time}')
    sum_encoding_time=$(awk -v sum="$sum_encoding_time" -v time="$encoding_time" 'BEGIN {print sum + time}')
    count=$((count + 1))
  done < <(grep "Grabbed WebXWindow" "$log_file")

  # Compute averages
  if [ "$count" -gt 0 ]; then
    avg_width=$((sum_width / count))
    avg_height=$((sum_height / count))
    avg_grab_time=$(awk -v sum="$sum_grab_time" -v count="$count" 'BEGIN {print sum / count}')
    avg_encoding_time=$(awk -v sum="$sum_encoding_time" -v count="$count" 'BEGIN {print sum / count}')

    # Print results
    echo "WebXWindow grabs: $count, avg size: ${avg_width} x ${avg_height}, total grab time: $sum_grab_time ms, total encoding time: $sum_grab_time ms, avg grab time: $avg_grab_time ms, avg encoding time: $avg_encoding_time ms"
  else
    echo "No 'Grabbed WebXWindow' entries found."
  fi
}

calculate_frame_metrics() {
  local log_file="$1"
  local count=0
  local sum_fps=0
  local sum_duration=0

  # Process each matching log line
  while IFS= read -r line; do
    # Extract width, height, grab time, and encoding time using awk
    duration=$(echo "$line" | awk -F '[ ,=()]+' '{gsub("ms", "", $NF); print $NF}')
    fps=$(echo "$line" | awk -F '[ ,=()]+' '{gsub("ms", "", $NF); print $(NF-4)}')

    # Accumulate values
    sum_duration=$(awk -v sum="$sum_duration" -v time="$duration" 'BEGIN {print sum + time}')
    sum_fps=$(awk -v sum="$sum_fps" -v fps="$fps" 'BEGIN {print sum + fps}')
    count=$((count + 1))
  done < <(grep "Average FPS" "$log_file")

  # Compute averages
  if [ "$count" -gt 0 ]; then
    avg_duration=$(awk -v sum="$sum_duration" -v count="$count" 'BEGIN {print sum / count}')
    avg_fps=$(awk -v sum="$sum_fps" -v count="$count" 'BEGIN {print sum / count}')

    # Print results
    echo "FPS calcs: $count, avg fps: ${avg_fps}, avg frame duration: $avg_duration ms"
  else
    echo "No FPS entries found."
  fi
}

echo "Starting webx-engine and sleep 2 seconds"

OUTPUT_LOG="test/output/web-engine.log"
WEBX_ENGINE_LOG=trace ./bin/webx-engine -s > "$OUTPUT_LOG" &
ENGINE_PID=$!
disown $ENGINE_PID

sleep 2

# setup terminator to be transparent
mkdir -p $HOME/.config/terminator
cat <<EOF > $HOME/.config/terminator/config
[global_config]
[keybindings]
[profiles]
  [[default]]
    background_type = transparent
[layouts]
  [[default]]
    [[[window0]]]
      type = Window
      parent = ""
    [[[child1]]]
      type = Terminal
      parent = window0
[plugins]
EOF

echo 'Running "top -d 0.01" in terminator for 5 seconds'
timeout 5 terminator -e "top -d 0.01" 2> /dev/null

echo "Stopping webx-engine"
kill -9 $ENGINE_PID

echo -e "\nResults:\n"

# Calculate metrics
calculate_transparency_stats "$OUTPUT_LOG"
calculate_alphamap_creation_stats "$OUTPUT_LOG"
calculate_grab_metrics "$OUTPUT_LOG"
calculate_frame_metrics "$OUTPUT_LOG"

echo -e "\n"

