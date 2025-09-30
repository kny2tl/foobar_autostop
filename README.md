Autostop
Autostop is a small foobar2000 component that can automatically set the player to stop after the current track finishes. The toggle state is persisted per-user.
Why? Imagine learning a new language and you have dozens of short tracks and with this you can focus on instructions, pronounciation.<br>
<img width="234" height="78" alt="image" src="https://github.com/user-attachments/assets/4f74e2b4-046e-46fd-9247-3be34e510fdd" />

Disclaimer
- Plugin is provided as-is, tested
- Started as fun project, with minimal coding skills - almost everything is produced by GPT-5 (Copilot)

License
This project is licensed under the MIT License — see the included LICENSE file for details.
Attribution
Original author: kny2tl.
If you fork, modify, or redistribute this project, please keep the original copyright notice and this license in the distributed source files and binary packages.
A short attribution line to use in redistribution or derivative works: "Autostop — original by kny2tl (MIT License)"

Build and dependencies
- Requires the foobar2000 SDK headers and link libraries to build.
- Place SDK headers and lib files where your build system expects them (see CMake configuration).
- Recommended project layout:
- /src — C/C++ source files
- /resources — .rc and .ico files
- /build — generated build artifacts (ignored in VCS)
- /third_party — SDK references (do not commit SDK binaries unless permitted)

Installation
- Build the component (produces foo_autostop.dll).
- Copy the resulting DLL to foobar2000's components folder (usually %PROGRAMFILES%\foobar2000\components).
- Restart foobar2000.
- Add the "Autostop" command to your toolbar or assign a keyboard shortcut via Preferences → Keyboard Shortcuts.
- See resources\autostop.ico

Contributing
- Fork the repository, make changes on a named branch, and submit a pull request.
- Keep the original LICENSE and attribution in modified files.
- Include tests or a short manual test plan for non-trivial changes.
Reporting Issues
Open issues on the project repository with a clear description, steps to reproduce, and logs or console output when relevant.
Contact
For questions or contribution coordination, reference the project repository or open an issue/pull request.
