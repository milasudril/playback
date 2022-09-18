# Playback

Playback is a Multi-track Multimedia Message Sequencing Format. Think of SMF, but for any sequenced
message, optionally containing its complete representation. Playback is a fully streamable format.
Thus, a producer can write it to a pipe, and a consumer may present its content.

## Supported features

Currently the following features are supported

* Sending a sequence of images. This feature can also be used for video display. Currently, only
linear data is supported, though sRGB is a planned feature.