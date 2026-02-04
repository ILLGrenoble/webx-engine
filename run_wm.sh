#!/bin/bash

if [ -r /etc/default/locale ]; then
    . /etc/default/locale
    export LANG LANGUAGE
fi

export DISPLAY=:20
export XORG_RUN_AS_USER_OK=1
# export XRDP_START_WIDTH=1920
# export XRDP_START_HEIGHT=1080
export XRDP_START_WIDTH=2560
export XRDP_START_HEIGHT=1220

FORCE_START=0
DO_STOP=0
X_PROCESS=Xorg

# Parse command line arguments
while [[ $# -gt 0 ]]; do
    key="$1"

    case $key in
        -d|--display)
        export DISPLAY="$2"
        shift
        shift
        ;;

        -f|--force)
        FORCE_START=1
        shift
        ;;

        -s|--stop)
        DO_STOP=1
        shift
        ;;

        -w|--width)
        XRDP_START_WIDTH="$2"
        shift
        shift
        ;;

        -h|--height)
        XRDP_START_HEIGHT="$2"
        shift
        shift
        ;;

        -x|--xserver)
        X_PROCESS="$2"
        shift
        shift
        ;;

        *)
        break
        ;;
    esac
done

cleanup_x() {
    echo "Cleaning up process $X_PROCESS on display $DISPLAY"

    x_proc=`ps aux | grep -v $0 | grep $X_PROCESS | grep $DISPLAY | grep -v grep | awk '{print $2}'`
    if [ "$x_proc" != "" ]; then
        if [ $DO_STOP == 1 ]; then
            echo "Stopping $X_PROCESS running on display $DISPLAY"

            kill -9 $x_proc
            exit 0

        elif [ $FORCE_START != 1 ]; then
            echo "$X_PROCESS is already running on display $DISPLAY"
            exit 1

        elif [ $FORCE_START == 1 ]; then
            echo "$X_PROCESS is already running on display $DISPLAY: stopping current process..."
            kill -9 $x_proc
            sleep 3

        fi
    fi
}

case "$X_PROCESS" in
    Xorg|Xvfb)
        ;;
    *)
        echo "Xserver $X_PROCESS is not known: must be either Xorg or Xvfb"
        exit 1
        ;;
esac


cleanup_x

if [ $DO_STOP != 1 ]; then
    echo "Starting $X_PROCESS on display $DISPLAY with resolution ${XRDP_START_WIDTH}x${XRDP_START_HEIGHT} in the background (logs available at /tmp/xorg.log)..."

    case "$X_PROCESS" in
        Xorg)
            nohup Xorg $DISPLAY -config /etc/X11/xrdp/xorg.conf -verbose > /tmp/xorg.log 2>&1 &
            ;;
        Xvfb)
            nohup Xvfb $DISPLAY -screen 0 ${XRDP_START_WIDTH}x${XRDP_START_HEIGHT}x24 > /tmp/xvfb.log 2>&1 &
            ;;
    esac

    sleep 3

    echo "... Starting xfce4"
    startxfce4

elif [ "$x_proc" == "" ]; then
    echo "$X_PROCESS is not running on display $DISPLAY"
fi

