#!/bin/bash

# Configuration
VIDEO="website/tek.mp4"
AUDIO="website/music.mp3"

# Vérifie si VLC est installé
if command -v vlc >/dev/null 2>&1; then
    # --fullscreen : plein écran
    # --no-video-title-show : cache le nom du fichier
    # --play-and-exit : ferme VLC après
    vlc --fullscreen --no-video-title-show --play-and-exit "$VIDEO" > /dev/null 2>&1 &
    # Pour le son, on peut utiliser cvlc (version sans interface)
    cvlc --play-and-exit "$AUDIO" > /dev/null 2>&1 &
fi