# Arduino / ESP-IDF client library for mdash.net

See [Documentation](https://mdash.net/docs/) for the quick start guide.

In order to simulate an mDash device on your Mac/Linux workstation,
register a device on mDash and run the following command from the root of
this repo:

```sh
$ make -C posix ARGS="-pass DEVICE_MDASH_PASSWORD"
```

Firmware V1.2.17:

1. Resolved the bug on [mDashShadowUpdate()](https://github.com/cesanta/mDash/issues/22).
2. Added mDashNotify_shdw() for mDashShadowUpdate() with removal of MutexLock.
