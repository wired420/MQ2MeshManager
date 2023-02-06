# MQ2MeshManager

A MacroQuest Next plugin for updating and/or downloading navigation meshes for MQ2Nav from https://mqmesh.com

## Getting Started

Getting started is very easy. Load the plugin. Confirm that you know it's going to connect to the internet. Then do an initial database update.

```txt
1) /plugin MQ2MeshManager
2) /mesh agree (follow further instructions on screen)
3) Wait for it to report "Plugin Activated!" This can take up to 30 seconds. If accepted in one window. All windows will get activated.
4) /mesh updatedb
5) /mesh help for more
```

### Commands

A list of available commands.

```txt
/mesh [help] - Help menu.
/mesh agree - View user agreement.
/mesh updatedb - Checks for updates to database.
/mesh updatezone [zone shortname] - Checks zone for updates. (Default: current)
/mesh updateall [confirm] [overwrite]- Checks all meshes for updates.
/mesh hash [zone shortname] - Displays hashes for current or designated zone.
/mesh tlos - Lists available TLOs available.
/mesh set - Displays settings menu.
/mesh settings - Displays current settings.

/mesh set maxdownloads <1-10>
/mesh set maxhashes <1-10>
/mesh set missing <on/off/1/0/true/false>
/mesh set autoupdate <on/off/1/0/true/false>
/mesh set progress <on/off/1/0/true/false>
/mesh set threadsafety <on/off/1/0/true/false>
```

### Configuration File

There is no configuration file for this plugin currently.

## Known Bug List & FAQ

If you get a download error number you can look it up here yourself [Libcurl Error codes](https://curl.se/libcurl/c/libcurl-errors.html) or ask wired420 in discord or forum private message what it means.

## Other Notes

Uses the [Curl](https://curl.se/libcurl/) and [OpenSSL](https://www.openssl.org/) libraries. Supports http2 compressed downloads and TLS1.3. Uses [nlohmann/json](https://github.com/nlohmann/json) for json parsing.

## Authors

* **Clayton Johnson** - *wired420* <wired420@gmail.com>


## Acknowledgments

* Wouldn't be needed without brainiac's MQ2Nav.
* Shoutout to dewey dannuic knightly and brainiac for answering sometimes dumb questions in MQ coders lounge!
* Also shoutout to all the core MacroQuest Developers and other plugin developers that answered questions..
