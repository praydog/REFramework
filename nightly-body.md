## **Ignore the date shown for this release. These are the latest builds.**

Extract the zip file into your game folder.

If you're **NOT USING VR**, do **NOT** extract any file other than dinput8.dll into your game folder, or your game may become unstable, and you will lose any form of anti-aliasing.

`REFramework.zip` is the monolithic DLL — it works across every supported RE Engine game (DMC5, RE2/3/4/7/8/9, MHRise, MHWilds, MHStories3, SF6, DD2, Pragmata, Star Force Legacy Collection). The same `dinput8.dll` detects the game at runtime and dispatches to the correct TDB layout.

`csharp-api.zip` is the C# plugin framework. Extract it only if you want to run C# plugins; regular Lua scripts do not need it.

Old (non-RT) builds of RE2, RE3, and RE7 are not currently supported in the monolithic build.

### Migration note
Older releases published per-game zips named `DMC5.zip`, `RE2.zip`, `RE3.zip`, `RE4.zip`, `RE7.zip`, `RE8.zip`, `RE9.zip`, `MHRISE.zip`, `SF6.zip`, `DD2.zip`, `MHWILDS.zip`, `PRAGMATA.zip`, `MHSTORIES3.zip`, `STARFORCE.zip`, and legacy TDB-suffixed variants (`RE2_TDB66.zip`, `RE3_TDB67.zip`, `RE7_TDB49.zip`). These are replaced by the single `REFramework.zip`. Legacy TDB66/67/49 variants are no longer produced; use the last pre-monolithic nightly if you need them.

# Links
[Main repository](https://github.com/praydog/REFramework)

[Historical releases](https://github.com/praydog/REFramework/actions)

[Documentation](https://cursey.github.io/reframework-book/)

[Haven's Night](https://discord.gg/9Vr2SJ3) (General RE Engine modding)

[Infernal Warks](https://discord.com/invite/nX5EzVU) (DMC5 modding)

[Monster Hunter Modding Discord](https://discord.gg/gJwMdhK)

[Flatscreen to VR Modding Discord](http://flat2vr.com)

[Donations](https://www.patreon.com/praydog)