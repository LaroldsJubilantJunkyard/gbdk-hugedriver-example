# Playing Music in Game Boy Games with hUGE Driver

The game boy has 4 channels for playing noises. Each channel is controlled by it's own set of noise registers. These registers control things like the duration of the sound, the frequency, or the volume. The noises from these 4 channels can be combined to create music. Unfortunately, There is no native support for music with GBDK-2020. Third-Party drivers, like hUGEDriver, are usually used to add support for music.

## What is hUGEDriver?

hUGEDriver is a music driver for the Game Boy homebrew games. It supports GBDK, RGBDS, and is even built-in to the GB Studio music player. When added it to your project, It can play music created in hUGETracker, a visual application for creating Game Boy music.
